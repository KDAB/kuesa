/*
    bufferviewsparser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <qtkuesa-config.h>
#include "gltf2parser_p.h"
#include "gltf2keys_p.h"
#include "kuesa_p.h"
#include "kuesa_utils_p.h"
#include "bufferparser_p.h"
#include "bufferviewsparser_p.h"
#include "bufferaccessorparser_p.h"
#include "cameraparser_p.h"
#include "meshparser_p.h"
#include "nodeparser_p.h"
#include "sceneentity.h"
#include "meshcollection.h"
#include "layerparser_p.h"
#include "lightparser_p.h"
#include "imageparser_p.h"
#include "texturesamplerparser_p.h"
#include "animationparser_p.h"
#include "sceneparser_p.h"
#include "skinparser_p.h"
#include "metallicroughnessmaterial.h"
#include "metallicroughnessproperties.h"
#include "unlitmaterial.h"
#include "unlitproperties.h"
#include "morphcontroller.h"
#include "gltf2material.h"
#include "directionallight.h"
#include "pointlight.h"
#include "spotlight.h"

#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>

#include <functional>

#include <Qt3DCore/private/qabstractnodefactory_p.h>
#include <Qt3DRender/private/qcamera_p.h>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QJoint>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QSkeleton>
#include <Qt3DCore/private/qmath3d_p.h>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLayer>
#include <Qt3DAnimation/QClipAnimator>
#include <Qt3DAnimation/QAnimationClip>
#include <Qt3DAnimation/QChannelMapping>
#include <Qt3DAnimation/QChannelMapper>
#include <Qt3DAnimation/QSkeletonMapping>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QAttribute>
#else
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#endif

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;
using namespace Qt3DGeometry;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using namespace Qt3DCore;
#else
using namespace Qt3DRender;
#endif

namespace {

const quint32 GLTF_BINARY_MAGIC = 0x46546C67;
const quint32 GLTF_CHUNCK_JSON = 0x4E4F534A;
const quint32 GLTF_CHUNCK_BIN = 0x004E4942;

struct GLBHeader {
    quint32 magic;
    quint32 version;
    quint32 length;
};

template<class CollectionType>
void addToCollectionWithUniqueName(CollectionType *collection, const QString &basename, typename CollectionType::ContentType *asset)
{
    Q_ASSERT(!basename.isEmpty());
    QString currentName = basename;
    for (int i = 1;; ++i) {
        if (!collection->contains(currentName))
            break;
        currentName = QString(QLatin1String("%1_%2")).arg(basename, QString::number(i));
    }

    collection->add(currentName, asset);

    if (asset->objectName().isEmpty())
        asset->setObjectName(currentName);
}

bool traverseGLTF(const QVector<KeyParserFuncPair> &parsers,
                  const QJsonObject &rootObject)
{
    auto parserIt = parsers.cbegin();
    const auto parserEnd = parsers.cend();

    QElapsedTimer t;
    t.start();
    qint64 elapsedSinceLastCall = 0;
    while (parserIt != parserEnd) {
        const QJsonValue value = rootObject.value((*parserIt).first);
        const bool success = value.isUndefined() || (*parserIt).second(value);
        if (!success) {
            qCWarning(Kuesa::kuesa) << "Failed to parse" << (*parserIt).first;
            return false;
        }
        qCDebug(gltf2_parser_profiling) << "GLTF2 Traversed" << parserIt->first << " in (" << t.elapsed() - elapsedSinceLastCall << "ms)";
        ++parserIt;
        elapsedSinceLastCall = t.elapsed();
    }
    return true;
}

void extractPositionViewDirAndUpVectorFromViewMatrix(const QMatrix4x4 &viewMatrix,
                                                     QVector3D &position,
                                                     QVector3D &viewDir,
                                                     QVector3D &up)
{
    const QVector3D upVector(viewMatrix.row(0)[1],
                             viewMatrix.row(1)[1],
                             viewMatrix.row(2)[1]);
    const QVector3D forward(viewMatrix.row(0)[2],
                            viewMatrix.row(1)[2],
                            viewMatrix.row(2)[2]);
    viewDir = -forward;
    up = QVector3D::crossProduct(QVector3D::crossProduct(forward, upVector), forward);
    position = QVector3D(viewMatrix.row(0)[3],
                         viewMatrix.row(1)[3],
                         viewMatrix.row(2)[3]);
}

using Path = QVector<const HierarchyNode *>;
const HierarchyNode *multiLCA(const Path &nodes)
{
    if (nodes.empty())
        return nullptr;

    // Create reverse path hierarchies from leaf to root
    QVector<Path> paths(nodes.size());
    for (int i = 0; i < nodes.size(); ++i) {
        auto n = nodes[i];
        paths[i].reserve(n->nodeIdx > 0 ? std::log(n->nodeIdx) : 1);
        paths[i].push_back(n);
        while ((n = n->parent) != nullptr)
            paths[i].push_back(n);
    }

    // Record length of shortest path
    QVector<Path::const_reverse_iterator> iterators;
    iterators.reserve(nodes.size());
    const HierarchyNode *firstNodeRoot = paths[0].back();
    int min_length = paths[0].length();
    for (const Path &path : paths) {
        // Sanity check : are we in the same tree
        if (path.back() != firstNodeRoot)
            return nullptr;

        min_length = std::min(min_length, int(path.length()));
        iterators.push_back(path.crbegin());
    }

    // Return lowest common ancestor
    for (int i = 0; i < min_length; ++i) {
        const HierarchyNode *firstNode = *iterators[0];

        // Check each branch's first node against firstNode
        // and advance all iterators if it's the same
        for (auto &it : iterators) {
            if ((*it) != firstNode)
                return firstNode->parent;
            ++it;
        }

        if (i == min_length - 1)
            return firstNode;
    }

    return nullptr;
}

} // namespace

GLTF2Parser::GLTF2Parser(SceneEntity *sceneEntity, bool assignNames)
    : m_context(nullptr)
    , m_sceneEntity(sceneEntity)
    , m_contentRootEntity(nullptr)
    , m_defaultSceneIdx(-1)
    , m_assignNames(assignNames)
{
}

GLTF2Parser::~GLTF2Parser()
{
}

bool GLTF2Parser::parse(const QString &filePath)
{
    QFile f(filePath);
    f.open(QIODevice::ReadOnly);
    if (!f.isOpen()) {
        qCWarning(Kuesa::kuesa) << "Can't read file" << filePath;
        return false;
    }

    QString cleanedFilePath = filePath;
    QString prefix;
#if defined(Q_OS_ANDROID)
    static const QLatin1String assetsPrefix("assets:");
    if (filePath.startsWith(assetsPrefix)) {
        cleanedFilePath = filePath.mid(assetsPrefix.size());
        prefix = assetsPrefix;
    }
#endif
    QFileInfo finfo(cleanedFilePath);
    QByteArray data = f.readAll();
    return parse(data, prefix + finfo.absolutePath(), finfo.fileName());
}

/*!
 * \internal
 *
 * Parsing operates in two steps:
 * First it reads the GLTF/GLB file and create Qt3D resources and store
 * hierarchies.
 * If that step is successful
 * Then it adds named resources to SceneEntity (if SceneEntity provided)
 * Parsing returns true if parsing was a success, false otherwise
 */
bool GLTF2Parser::parse(const QByteArray &data, const QString &basePath, const QString &filename)
{
    QElapsedTimer t;
    qCDebug(gltf2_parser_profiling) << "GLTF2 Parsing starting";
    t.start();
    const bool isValid = detectTypeAndParse(data, basePath, filename);
    const qint64 elapsedAfterParsing = t.elapsed();
    qCDebug(gltf2_parser_profiling) << "GLTF2 Parsing completed (" << elapsedAfterParsing << "ms)";
    if (isValid)
        addResourcesToSceneEntityCollections();
    qCDebug(gltf2_parser_profiling) << "GLTF2 Qt3D Resources added to collection (" << t.elapsed() - elapsedAfterParsing << "ms)";
    qCDebug(gltf2_parser_profiling) << "GLTF2 Parsing total (" << t.elapsed() << "ms)";
    return isValid;
}

bool GLTF2Parser::detectTypeAndParse(const QByteArray &data, const QString &basePath, const QString &filename)
{
    Q_ASSERT(!m_contentRootEntity);
    // Ensure we have never parsed a scene before (you have to use a new GLTF2Parser
    // instance to reload a scene)
    bool isValid = false;

    if (isBinaryGLTF(data, isValid)) {
        if (isValid)
            return parseBinary(data, basePath, filename);
    } else {
        if (isValid)
            return parseJSON(data, basePath, filename);
    }
    return false;
}

bool GLTF2Parser::isBinaryGLTF(const QByteArray &data, bool &isValid)
{
    bool isBinary = true;
    isValid = true;

    if (size_t(data.size()) < sizeof(GLBHeader))
        return false;
    const GLBHeader *glbHeader = reinterpret_cast<const GLBHeader *>(data.constData());
    if (glbHeader->magic != GLTF_BINARY_MAGIC) {
        isBinary = false;
    } else {
        if (glbHeader->version != 2) {
            qCWarning(Kuesa::kuesa) << "Unsupported glb version" << glbHeader->version;
            isValid = false;
        } else if (glbHeader->length != data.size()) {
            qCWarning(Kuesa::kuesa) << "Unexpected glb file size" << data.size() << ". Was expecting" << glbHeader->length;
            isValid = false;
        }
    }

    return isBinary;
}

template<class T>
void GLTF2Parser::updateDataForJointsAttr(QAttribute *attr, int skinId)
{
    auto bufferData = attr->buffer()->data();
    auto data = bufferData.data() + attr->byteOffset();
    auto typedData = reinterpret_cast<const T *>(data);
    const size_t nbJoints = 4 * attr->count();
    QByteArray updatedData;

    updatedData.resize(static_cast<int>(nbJoints * sizeof(T)));
    T *updatedJointIndices = reinterpret_cast<T *>(updatedData.data());
    for (size_t jointId = 0; jointId < nbJoints; ++jointId)
        updatedJointIndices[jointId] = static_cast<T>(m_gltfJointIdxToSkeletonJointIdxPerSkeleton[skinId][typedData[jointId]]);
    attr->buffer()->updateData(static_cast<int>(attr->byteOffset()), updatedData);
}

QVector<KeyParserFuncPair> GLTF2Parser::prepareParsers()
{
    return {
        { KEY_BUFFERS, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             BufferParser parser(m_basePath);
             return parser.parse(array, m_context);
         } },
        { KEY_BUFFERVIEWS, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             BufferViewsParser parser;
             return parser.parse(array, m_context);
         } },
        { KEY_ACCESSORS, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             BufferAccessorParser parser;
             return parser.parse(array, m_context);
         } },
        { KEY_MESHES, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             MeshParser parser;
             return parser.parse(array, m_context);
         } },
        { KEY_CAMERAS, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             CameraParser parser;
             return parser.parse(array, m_context);
         } },
        { KEY_NODES, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             NodeParser parser;
             return parser.parse(array, m_context);
         } },
        { KEY_SCENES, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             SceneParser parser;
             return parser.parse(array, m_context);
         } },
        { KEY_IMAGES, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             ImageParser parser(m_basePath);
             return parser.parse(array, m_context);
         } },
        { KEY_TEXTURE_SAMPLERS, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             TextureSamplerParser parser;
             return parser.parse(array, m_context);
         } },
        { KEY_TEXTURES, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             TextureParser parser;
             return parser.parse(array, m_context);
         } },
        { KEY_SKINS, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             SkinParser parser;
             return parser.parse(array, m_context);
         } },
        { KEY_MATERIALS, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             MaterialParser parser;
             return parser.parse(array, m_context);
         } },
        { KEY_EXTENSIONS, [this](const QJsonValue &value) {
             const QVector<KeyParserFuncPair> extensionParsers = {
                 { KEY_KDAB_KUESA_LAYER_EXTENSION, [this](const QJsonValue &value) {
                      const QJsonObject obj = value.toObject();
                      const QJsonArray layers = obj.value(KEY_KUESA_LAYERS).toArray();
                      if (layers.size() == 0)
                          return true;
                      LayerParser parser;
                      return parser.parse(layers, m_context);
                  } },
                 { KEY_KHR_LIGHTS_PUNCTUAL_EXTENSION, [this](const QJsonValue &value) {
                      const QJsonObject obj = value.toObject();
                      const QJsonArray lights = obj.value(KEY_KHR_PUNCTUAL_LIGHTS).toArray();
                      if (lights.isEmpty())
                          return true;
                      LightParser parser;
                      return parser.parse(lights, m_context);
                  } }
             };
             // Having no extensions is a valid use case
             if (value.isUndefined())
                 return true;
             return traverseGLTF(extensionParsers, value.toObject());
         } },
        { KEY_ANIMATIONS, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             AnimationParser parser;
             return parser.parse(array, m_context);
         } },
    };
}

bool GLTF2Parser::parseJSON(const QByteArray &jsonData, const QString &basePath, const QString &filename)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
    if (jsonDocument.isNull() || !jsonDocument.isObject()) {
        qCWarning(Kuesa::kuesa) << "File is not a valid json document";
        return false;
    }

    Q_ASSERT(m_context);
    m_context->setJson(jsonDocument);
    m_context->setFilename(filename);
    m_context->setBasePath(basePath);
    m_basePath = basePath;

    const QJsonObject rootObject = jsonDocument.object();

    if (rootObject.contains(KEY_EXTENSIONS_USED) && rootObject.value(KEY_EXTENSIONS_USED).isArray()) {
        const QJsonArray extensionObjects = rootObject.value(KEY_EXTENSIONS_USED).toArray();
        QStringList extensions;
        std::transform(extensionObjects.constBegin(), extensionObjects.constEnd(), std::back_inserter(extensions),
                       [](const QJsonValue &e) -> QString { return e.toString(); });
        m_context->setUsedExtensions(extensions);
    }

    if (rootObject.contains(KEY_EXTENSIONS_REQUIRED) && rootObject.value(KEY_EXTENSIONS_REQUIRED).isArray()) {
        const QJsonArray extensionObjects = rootObject.value(KEY_EXTENSIONS_REQUIRED).toArray();
        QStringList extensions;
        std::transform(extensionObjects.constBegin(), extensionObjects.constEnd(), std::back_inserter(extensions),
                       [](const QJsonValue &e) -> QString { return e.toString(); });
        m_context->setRequiredExtensions(extensions);

        bool allRequiredAreSupported = true;
        QStringList unsupportedExtensions;
        const QStringList supportedExtensions = {
            KEY_KDAB_KUESA_LAYER_EXTENSION,
            KEY_MSFT_DDS_EXTENSION,
#if defined(KUESA_DRACO_COMPRESSION)
            KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION,
#endif
            KEY_KHR_MATERIALS_UNLIT,
            KEY_KHR_LIGHTS_PUNCTUAL_EXTENSION,
            KEY_KDAB_CUSTOM_MATERIAL,
            KEY_EXT_PROPERTY_ANIMATION_EXTENSION,
            KEY_KHR_TEXTURE_TRANSFORM
        };
        for (const auto &e : qAsConst(extensions)) {
            if (supportedExtensions.contains(e))
                continue;
            allRequiredAreSupported = false;
            unsupportedExtensions << e;
        }

        if (!allRequiredAreSupported) {
            qCWarning(Kuesa::kuesa) << "File contains unsupported extensions: " << unsupportedExtensions;
            return false;
        }
    }

    const QVector<KeyParserFuncPair> topLevelParsers = prepareParsers();
    const bool parsingSucceeded = traverseGLTF(topLevelParsers, rootObject);

    if (!parsingSucceeded)
        return false;

    // It is valid for the default scene to not be specified
    // in that case m_defaultSceneIdx remains at -1 but doesn't
    // prevent parsing from continuing
    if (rootObject.contains(KEY_SCENE)) {
        m_defaultSceneIdx = rootObject.value(KEY_SCENE).toInt(0);
        m_context->setDefaultScene(m_defaultSceneIdx);

        if (m_defaultSceneIdx < 0 || m_defaultSceneIdx > m_context->scenesCount()) {
            qCWarning(Kuesa::kuesa) << "Invalid default scene reference";
            return false;
        }
    }

    QElapsedTimer t;
    qint64 elapsed = 0;
    qCDebug(gltf2_parser_profiling) << "GLTF2 Qt3D Scene contruction starting";
    t.start();

    // Build hierarchies for Entities and QJoints
    buildEntitiesAndJointsGraph();
    elapsed = t.elapsed();
    qCDebug(gltf2_parser_profiling) << "GLTF2 Qt3D Building Entities and Joints Graphes in (" << elapsed << "ms)";

    // Generate Qt3D content for skeletons
    generateSkeletonContent();
    qCDebug(gltf2_parser_profiling) << "GLTF2 Qt3D Building Skeleton in (" << t.elapsed() - elapsed << "ms)";
    elapsed = t.elapsed();

    // Generate Qt3D data for the nodes based on their type
    generateTreeNodeContent();
    qCDebug(gltf2_parser_profiling) << "GLTF2 Qt3D Building Entities Tree Content in (" << t.elapsed() - elapsed << "ms)";
    elapsed = t.elapsed();

    // Generate Qt3D content for animations
    generateAnimationContent();
    qCDebug(gltf2_parser_profiling) << "GLTF2 Qt3D Building Animation Content in (" << t.elapsed() - elapsed << "ms)";
    elapsed = t.elapsed();

    // Build Scene Roots
    buildSceneRootEntities();
    qCDebug(gltf2_parser_profiling) << "GLTF2 Qt3D Building Scene Roots in (" << t.elapsed() - elapsed << "ms)";

    qCDebug(gltf2_parser_profiling) << "GLTF2 Qt3D Content generated in (" << t.elapsed() << "ms)";
    return true;
}

void GLTF2Parser::moveToThread(QThread *targetThread)
{
    QObject::moveToThread(targetThread);
    if (m_contentRootEntity)
        m_contentRootEntity->moveToThread(targetThread);

    for (int i = 0, n = m_context->animationsCount(); i < n; ++i) {
        const Animation &anim = m_context->animation(i);
        anim.clip->moveToThread(targetThread);
        anim.mapper->moveToThread(targetThread);
    }

    for (int i = 0, n = m_context->scenesCount(); i < n; i++) {
        const Scene scene = m_context->scene(i);
        const QVector<int> toRetrieveNodes = scene.rootNodeIndices;

        for (int j = 0, m = toRetrieveNodes.size(); j < m; ++j) {
            const TreeNode node = m_context->treeNode(toRetrieveNodes.at(j));
            if (node.entity != nullptr)
                node.entity->moveToThread(targetThread);
            for (auto &joint : node.joints)
                if (joint != nullptr)
                    joint->moveToThread(targetThread);
        }
    }
}

void GLTF2Parser::setContext(GLTF2Context *ctx)
{
    m_context = ctx;
}

const GLTF2Context *GLTF2Parser::context() const
{
    return m_context;
}

void GLTF2Parser::addResourcesToSceneEntityCollections()
{
    // Note: we only add resources into the collection after having set an
    // existing parent on the scene root This avoid sending a destroy + created
    // changes because sceneEntity exists and has a backend while the scene root
    // has no backend

    if (m_sceneEntity) {
        if (m_sceneEntity->meshes()) {
            addAssetsIntoCollection<Mesh>(
                    [this](const Mesh &mesh, int) {
                        for (int j = 0, n = mesh.meshPrimitives.size(); j < n; ++j) {
                            const QString name = QStringLiteral("%1_%2").arg(mesh.name, QString::number(j));
                            addToCollectionWithUniqueName(m_sceneEntity->meshes(), name, mesh.meshPrimitives.at(j).primitiveRenderer);
                        }
                    },
                    [this](const Mesh &mesh, int) {
                        for (int j = 0, n = mesh.meshPrimitives.size(); j < n; ++j) {
                            const QString name = QStringLiteral("KeusaMesh_%1").arg(j);
                            addToCollectionWithUniqueName(m_sceneEntity->meshes(), name, mesh.meshPrimitives.at(j).primitiveRenderer);
                        }
                    });
        }

        if (m_sceneEntity->layers())
            addAssetsIntoCollection<Layer>(
                    [this](const Layer &layer, int) { addToCollectionWithUniqueName(m_sceneEntity->layers(), layer.name, layer.layer); },
                    [this](const Layer &layer, int i) { addToCollectionWithUniqueName(m_sceneEntity->layers(), QStringLiteral("KuesaLayer_%1").arg(i), layer.layer); });

        // For TreeNode we use our local copy and not the entries on the context
        if (m_sceneEntity->entities()) {
            const QVector<TreeNode> threeNodes = m_context->treeNodes();
            for (const TreeNode &treeNode : threeNodes) {
                if (treeNode.entity != nullptr && !treeNode.name.isEmpty()) {
                    addToCollectionWithUniqueName(m_sceneEntity->entities(), treeNode.name, treeNode.entity);

                    if (treeNode.cameraIdx >= 0)
                        addToCollectionWithUniqueName(m_sceneEntity->cameras(), treeNode.name, qobject_cast<Qt3DRender::QCamera *>(treeNode.entity));

                    auto light = componentFromEntity<Qt3DRender::QAbstractLight>(treeNode.entity);
                    if (light)
                        addToCollectionWithUniqueName(m_sceneEntity->lights(), light->objectName(), light);

                    if (m_sceneEntity->transforms()) {
                        Qt3DCore::QTransform *transform = qobject_cast<Qt3DCore::QTransform *>(m_sceneEntity->transformForEntity(treeNode.name));
                        Q_ASSERT(transform != nullptr);
                        addToCollectionWithUniqueName(m_sceneEntity->transforms(), treeNode.name, transform);
                    }
                }
            }

            if (m_assignNames) {
                int j = 0;
                const QVector<TreeNode> threeNodes = m_context->treeNodes();
                for (const TreeNode &treeNode : threeNodes) {
                    if (treeNode.entity != nullptr && treeNode.name.isEmpty()) {
                        addToCollectionWithUniqueName(m_sceneEntity->entities(), QStringLiteral("KuesaEntity_%1").arg(j), treeNode.entity);

                        if (treeNode.cameraIdx >= 0)
                            addToCollectionWithUniqueName(m_sceneEntity->cameras(), QStringLiteral("KuesaCamera_%1").arg(j), qobject_cast<Qt3DRender::QCamera *>(treeNode.entity));

                        auto light = componentFromEntity<Qt3DRender::QAbstractLight>(treeNode.entity);
                        if (light)
                            addToCollectionWithUniqueName(m_sceneEntity->lights(), QStringLiteral("KuesaLight_%1").arg(j), light);
                    }
                    j++;
                }
            }
        }

        // Currently Qt3D doesn't support texture image sharing
        // TO DO: Enable back once Qt3D supports texture image sharing
        //        if (m_sceneEntity->textureImages()) {
        //            for (int i = 0, m = m_context->imagesCount(); i < m; ++i) {
        //                const Image img = m_context->image(i);
        //                addToCollectionWithUniqueName(m_sceneEntity->textureImages(), img.name, img.textureImage);
        //            }
        //        }

        if (m_sceneEntity->textures())
            addAssetsIntoCollection<Texture>(
                    [this](const Texture &texture, int) {
                        if (texture.texture)
                            addToCollectionWithUniqueName(m_sceneEntity->textures(), texture.name, texture.texture);
                    },
                    [this](const Texture &texture, int i) {
                        if (texture.texture)
                            addToCollectionWithUniqueName(m_sceneEntity->textures(), QStringLiteral("KuesaTexture_%1").arg(i), texture.texture);
                    });

        if (m_sceneEntity->animationClips())
            addAssetsIntoCollection<Animation>(
                    [this](const Animation &animation, int) { addToCollectionWithUniqueName(m_sceneEntity->animationClips(), animation.name, animation.clip); },
                    [this](const Animation &animation, int i) { addToCollectionWithUniqueName(m_sceneEntity->animationClips(), QStringLiteral("KuesaAnimation_%1").arg(i), animation.clip); });

        if (m_sceneEntity->animationMappings())
            addAssetsIntoCollection<Animation>(
                    [this](const Animation &animation, int) { addToCollectionWithUniqueName(m_sceneEntity->animationMappings(), animation.name, animation.mapper); },
                    [this](const Animation &animation, int i) { addToCollectionWithUniqueName(m_sceneEntity->animationMappings(), QStringLiteral("KuesaAnimation_%1").arg(i), animation.mapper); });

        if (m_sceneEntity->materials())
            addAssetsIntoCollection<Material>(
                    [this](const Material &material, int) {
                        addToCollectionWithUniqueName(m_sceneEntity->materials(), material.name, material.materialProperties());
                    },
                    [this](const Material &material, int i) {
                        addToCollectionWithUniqueName(m_sceneEntity->materials(), QStringLiteral("KuesaMaterial_%1").arg(i), material.materialProperties());
                    });

        if (m_sceneEntity->skeletons())
            addAssetsIntoCollection<Skin>(
                    [this](const Skin &skin, int) { addToCollectionWithUniqueName(m_sceneEntity->skeletons(), skin.name, skin.skeleton); },
                    [this](const Skin &skin, int i) { addToCollectionWithUniqueName(m_sceneEntity->skeletons(), QStringLiteral("KuesaSkeleton_%1").arg(i), skin.skeleton); });

        if (m_sceneEntity->effects()) {
            const auto effectsHash = m_context->effectLibrary()->effects();
            auto it = effectsHash.cbegin();
            const auto end = effectsHash.cend();
            while (it != end) {
                const EffectProperties::Properties propertyFlags = it.key();
                const QString name = QStringLiteral("KuesaEffect_%1").arg(QString::number(static_cast<int>(propertyFlags), 2));
                addToCollectionWithUniqueName(m_sceneEntity->effects(), name, it.value());
                ++it;
            }
        }
    }
}

void GLTF2Parser::buildSceneRootEntities()
{
    for (int i = 0, m = m_context->scenesCount(); i < m; ++i) {
        const Scene scene = m_context->scene(i);

        // Technically nothing stops having 2 scenes referencing the same root
        // nodes. This can be an issue with Qt3D as a node can only have a
        // unique parent We therefore use a specific subclass of QEntity called
        // SceneRootEntity which records the list of root entities and has a
        // makeActive function

        // Get scene nodes
        const QVector<int> toRetrieveNodes = scene.rootNodeIndices;
        QVector<Qt3DCore::QEntity *> rootNodeEntities;

        for (int j = 0, m = toRetrieveNodes.size(); j < m; ++j) {
            const TreeNode node = m_context->treeNode(toRetrieveNodes.at(j));
            // Specs specify that scene nodes have to be root nodes
            Q_ASSERT(node.isRootNode);
            rootNodeEntities.push_back(node.entity);
            for (auto &joint : node.joints)
                if (joint != nullptr && joint->parent() == nullptr) // For ownership only (doesn't affect hierarchy)
                    joint->setParent(m_contentRootEntity);
        }

        // Record SceneRootEntity
        SceneRootEntity *sceneRootEntity = new SceneRootEntity(rootNodeEntities);
        m_sceneRootEntities.push_back(sceneRootEntity);
    }
}

bool GLTF2Parser::parseBinary(const QByteArray &data, const QString &basePath, const QString &filename)
{
    QByteArray jsonData;
    const char *current = data.constData();
    current += sizeof(GLBHeader);
    int currentOffset = sizeof(GLBHeader);
    const int endOffset = data.size();

    while (currentOffset < endOffset) {
        struct ChunkHeader {
            quint32 chunkLength;
            quint32 chunkType;
        };

        if (size_t(endOffset - currentOffset) < sizeof(ChunkHeader)) {
            qCWarning(Kuesa::kuesa) << "Malformed chunck in glb file";
            return false;
        }

        const ChunkHeader *chunkHeader = reinterpret_cast<const ChunkHeader *>(current);
        current += sizeof(ChunkHeader);
        currentOffset += sizeof(ChunkHeader);

        if (size_t(endOffset - currentOffset) < chunkHeader->chunkLength) {
            qCWarning(Kuesa::kuesa) << "Malformed chunck in glb file";
            return false;
        }

        if (chunkHeader->chunkType == GLTF_CHUNCK_JSON) {
            // JSON chunck
            if (jsonData.isEmpty()) {
                jsonData = QByteArray::fromRawData(current, chunkHeader->chunkLength);
            } else {
                qCWarning(Kuesa::kuesa) << "Multiple JSON chunks in glb file";
                return false;
            }
        } else if (chunkHeader->chunkType == GLTF_CHUNCK_BIN) {
            // BIN chunck
            if (m_context->bufferChunk().isEmpty()) {
                m_context->setBufferChunk(QByteArray::fromRawData(current, chunkHeader->chunkLength));
            } else {
                qCWarning(Kuesa::kuesa) << "Multiple BIN chunks in glb file";
                return false;
            }
        } else {
            // skip chunck
            qCWarning(Kuesa::kuesa) << "Ignoring unhandled chunck type in glb file:" << chunkHeader->chunkType;
        }

        current += chunkHeader->chunkLength;
        currentOffset += chunkHeader->chunkLength;
    }

    if (jsonData.isEmpty()) {
        qCWarning(Kuesa::kuesa) << "Missing JSON chunk in glb file";
        return false;
    }

    const bool parsingSucceeded = parseJSON(jsonData, basePath, filename);

    // make sure chunk doesn't outlive the original data
    m_context->setBufferChunk({});

    return parsingSucceeded;
}

void GLTF2Parser::buildEntitiesAndJointsGraph()
{
    const int nbNodes = m_context->treeNodeCount();

    // We need to only build an Entity subtree when we have identified a Mesh
    // or Camera.
    // Given a Mesh/Camera we need to build an Entity tree from the mesh node
    // to the scene root
    QVector<HierarchyNode> tree(nbNodes);
    QVector<HierarchyNode *> leafNodes;

    // Initialize Node tree hierarchy
    for (int nodeId = 0; nodeId < nbNodes; ++nodeId) {
        HierarchyNode *hierarchyNode = tree.data() + nodeId;
        hierarchyNode->nodeIdx = nodeId;

        const TreeNode treeNode = m_context->treeNode(nodeId);
        const QVector<int> childrenIndices = treeNode.childrenIndices;

        if (childrenIndices.size() > 0) {
            for (const int childId : childrenIndices) {
                if (childId < 0 || childId > nbNodes) {
                    qCWarning(Kuesa::kuesa) << "Encountered invalid child node reference while building hierarchy";
                    continue;
                }
                HierarchyNode *childHierarchyNode = tree.data() + childId;
                childHierarchyNode->parent = hierarchyNode;
                hierarchyNode->children.push_back(childHierarchyNode);
            }
        } else { // We have a leaf node
            leafNodes.push_back(hierarchyNode);
        }
    }

    // Traverse branches from leaves to roots and create QEntity
    for (HierarchyNode *leaf : leafNodes) {
        Qt3DCore::QEntity *lastChild = nullptr;
        HierarchyNode *root = leaf;
        while (leaf) {
            TreeNode &node = m_context->treeNode(leaf->nodeIdx);
            const bool entityAlreadyCreated = (node.entity != nullptr);
            if (!entityAlreadyCreated) {
                // If we are dealing with a Camera, we instantiate a QCamera
                // instead of a QEntity
                if (node.cameraIdx >= 0)
                    node.entity = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DRender::QCamera>("QCamera");
                else
                    node.entity = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DCore::QEntity>("QEntity");
            }
            // Set ourselves as parent of our last child
            if (lastChild != nullptr)
                lastChild->setParent(node.entity);

            // If the entity had already been created, the branck from node to
            // root was already traversed
            if (entityAlreadyCreated)
                break;

            lastChild = node.entity;
            root = leaf;
            leaf = leaf->parent;
        }
        // Root is at this point the root of the branch
        TreeNode &branchRoot = m_context->treeNode(root->nodeIdx);
        branchRoot.isRootNode = true;
    }

    for (HierarchyNode *leaf : leafNodes) {
        bool subtreeHasJoints = false;
        while (leaf) {
            // If branch already processed and hasJoints
            if (leaf->hasJoints)
                break;

            if (subtreeHasJoints) {
                leaf->hasJoints = subtreeHasJoints;
            } else {
                // Is this node a Joint
                for (int skinId = 0, m = m_context->skinsCount(); skinId < m && !subtreeHasJoints; ++skinId) {
                    const Skin &skin = m_context->skin(skinId);
                    leaf->hasJoints = skin.jointsIndices.contains(leaf->nodeIdx);
                    subtreeHasJoints |= leaf->hasJoints;
                }
            }
            leaf = leaf->parent;
        }
    }

    int skinsCount = m_context->skinsCount();
    for (int i = 0, m = m_context->treeNodeCount(); i < m; ++i) {
        TreeNode &treeNode = m_context->treeNode(i);
        treeNode.joints.resize(skinsCount);
    }

    // Assign QJoint to treenodes used as joints
    m_gltfJointIdxToSkeletonJointIdxPerSkeleton.resize(m_context->skinsCount());
    for (int skinId = 0, m = m_context->skinsCount(); skinId < m; ++skinId) {
        Skin &skin = m_context->skin(skinId);

        // Find the LCA of the joints
        QVector<const HierarchyNode *> jointNodes;
        jointNodes.reserve(skin.jointsIndices.size());
        for (auto jointId : skin.jointsIndices)
            jointNodes.push_back(&tree[jointId]);
        const HierarchyNode *lcaNode = ::multiLCA(jointNodes);

        // If the LCA exists we use that as the rootJoint for the skin
        if (lcaNode) {
            skin.rootJoint.jointNodeIdx = lcaNode->nodeIdx;
            int jointAccessor = 0;
            const HierarchyNode *skeletonRootHNode = tree.data() + skin.rootJoint.jointNodeIdx;
            skin.rootJoint.skeletonNode = m_context->treeNode(lcaNode->nodeIdx).entity->parentEntity();

            // Is the LCA a joint itself?
            if (skin.jointsIndices.contains(lcaNode->nodeIdx))
                m_gltfJointIdxToSkeletonJointIdxPerSkeleton[skinId][skin.jointsIndices.indexOf(skin.rootJoint.jointNodeIdx)] = jointAccessor;
            buildJointHierarchy(skeletonRootHNode, jointAccessor, skin, skinId);
        } else {
            // If we don't have a LCA, we have one or more joints that doesn't create a tree, but they create subtrees. Look for the roots of those subtrees
            QVector<int> rootJoints;
            for (const auto joint : qAsConst(skin.jointsIndices)) {
                const HierarchyNode &node = tree[joint];
                if (!node.parent)
                    rootJoints.push_back(joint);
            }

            // All this subtrees are going to be parented to a dummy QJoint that will act as the rootJoint
            Qt3DCore::QJoint *joint = new Qt3DCore::QJoint;
            skin.rootJoint.joint = joint;
            int jointAccessor = 1;
            for (const auto rootJointId : rootJoints) {
                const HierarchyNode *skeletonRootHNode = tree.data() + rootJointId;
                if (skin.jointsIndices.contains(rootJointId))
                    m_gltfJointIdxToSkeletonJointIdxPerSkeleton[skinId][skin.jointsIndices.indexOf(rootJointId)] = jointAccessor;
                buildJointHierarchy(skeletonRootHNode, jointAccessor, skin, skinId);
                joint->addChildJoint(m_context->treeNode(rootJointId).joints[skinId]);
            }
        }
    }
}

void GLTF2Parser::buildJointHierarchy(const HierarchyNode *node, int &jointAccessor, const Skin &skin, int skinIdx, Qt3DCore::QJoint *parentJoint)
{
    Qt3DCore::QJoint *joint = new Qt3DCore::QJoint;
    if (parentJoint != nullptr)
        parentJoint->addChildJoint(joint);
    TreeNode &treeNode = m_context->treeNode(node->nodeIdx);
    treeNode.joints[skinIdx] = joint;
    jointAccessor++;
    joint->setName(treeNode.name);
    joint->setInverseBindMatrix(QMatrix4x4());

    for (const HierarchyNode *childNode : node->children) {
        if (skin.jointsIndices.contains(childNode->nodeIdx))
            m_gltfJointIdxToSkeletonJointIdxPerSkeleton[skinIdx][skin.jointsIndices.indexOf(childNode->nodeIdx)] = jointAccessor;
        if (childNode->hasJoints)
            buildJointHierarchy(childNode, jointAccessor, skin, skinIdx, joint);
    }
}

void GLTF2Parser::generateTreeNodeContent()
{
    m_contentRootEntity = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DCore::QEntity>("QEntity");
    m_contentRootEntity->setObjectName(QStringLiteral("GLTF2Scene"));

    for (int i = 0, m = m_context->treeNodeCount(); i < m; ++i) {
        TreeNode &node = m_context->treeNode(i);
        // Build Entity Content
        if (node.entity) {
            Qt3DCore::QEntity *entity = node.entity;
            Qt3DRender::QCamera *camera = qobject_cast<Qt3DRender::QCamera *>(node.entity);
            Qt3DCore::QTransform *transform = camera ? camera->transform() : new Qt3DCore::QTransform();

            // Set transform properties
            if (node.transformInfo.bits & TreeNode::TransformInfo::MatrixSet) {
                transform->setMatrix(node.transformInfo.matrix);
            } else {
                if (node.transformInfo.bits & TreeNode::TransformInfo::ScaleSet)
                    transform->setScale3D(node.transformInfo.scale3D);
                if (node.transformInfo.bits & TreeNode::TransformInfo::RotationSet)
                    transform->setRotation(node.transformInfo.rotation);
                if (node.transformInfo.bits & TreeNode::TransformInfo::TranslationSet)
                    transform->setTranslation(node.transformInfo.translation);
            }

            // The QCamera node already contains a lens + transform components
            if (camera != nullptr) {
                const qint32 cameraId = node.cameraIdx;
                if (cameraId >= 0 && cameraId < m_context->cameraCount()) {
                    const Camera cam = m_context->camera(cameraId);
                    // Note: we need to keep the lens in cam around as that one will be added
                    // into the collection
                    auto lenses = componentsFromEntity<Qt3DRender::QCameraLens>(camera);
                    qDeleteAll(lenses);

                    auto cameraPrivate = static_cast<Qt3DRender::QCameraPrivate *>(Qt3DRender::QCameraPrivate::get(camera));
                    cameraPrivate->m_lens = cam.lens;

                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::projectionTypeChanged, camera, &Qt3DRender::QCamera::projectionTypeChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::nearPlaneChanged, camera, &Qt3DRender::QCamera::nearPlaneChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::farPlaneChanged, camera, &Qt3DRender::QCamera::farPlaneChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::fieldOfViewChanged, camera, &Qt3DRender::QCamera::fieldOfViewChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::aspectRatioChanged, camera, &Qt3DRender::QCamera::aspectRatioChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::leftChanged, camera, &Qt3DRender::QCamera::leftChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::rightChanged, camera, &Qt3DRender::QCamera::rightChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::bottomChanged, camera, &Qt3DRender::QCamera::bottomChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::topChanged, camera, &Qt3DRender::QCamera::topChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::projectionMatrixChanged, camera, &Qt3DRender::QCamera::projectionMatrixChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::exposureChanged, camera, &Qt3DRender::QCamera::exposureChanged);
                    QObject::connect(cam.lens, &Qt3DRender::QCameraLens::viewSphere, camera, &Qt3DRender::QCamera::viewSphere);

                    camera->addComponent(cam.lens);

                    QVector3D position, up, viewDir;
                    // Extract local position, direction and upVector from
                    // transform->matrix which is our local view matrix in this
                    // case So as to have sensible values set in the QCamera
                    // which can the be used with camera controllers
                    extractPositionViewDirAndUpVectorFromViewMatrix(transform->matrix(),
                                                                    position,
                                                                    viewDir,
                                                                    up);
                    camera->setPosition(position);
                    camera->setUpVector(up);
                    // Choose random center along viewDir line
                    camera->setViewCenter(position + viewDir * 25.0f);
                }
            } else {
                // Don't forget to add transform as a component
                // if we aren't a QCamera
                entity->addComponent(transform);
            }

            // If the node references Kuesa Layers, add them
            const QVector<int> layerIds = node.layerIndices;
            for (const qint32 layerId : layerIds) {
                if (layerId >= 0 && layerId < m_context->layersCount()) {
                    const Layer layer = m_context->layer(layerId);
                    if (layer.layer) {
                        // Make it recursive so it affects subentities and in particular primitive entities
                        layer.layer->setRecursive(true);
                        layer.layer->setParent(m_contentRootEntity);
                        entity->addComponent(layer.layer);
                    }
                }
            }

            // If the node references Light (KHR_lights_punctual extension), add them
            if (node.lightIdx != -1 && node.lightIdx < m_context->lightCount()) {
                const Light lightDefinition = m_context->light(node.lightIdx);
                Qt3DRender::QAbstractLight *light = lightDefinition.lightComponent;
                if (light == nullptr) {
                    switch (lightDefinition.type) {
                    case Qt3DRender::QAbstractLight::PointLight: {
                        auto pointLight = new PointLight(m_contentRootEntity);
                        pointLight->setRange(lightDefinition.range.toFloat());
                        light = pointLight;
                        break;
                    }
                    case Qt3DRender::QAbstractLight::SpotLight: {
                        auto spotLight = new SpotLight(m_contentRootEntity);
                        spotLight->setInnerConeAngle(qRadiansToDegrees(lightDefinition.innerConeAngleRadians));
                        spotLight->setOuterConeAngle(qRadiansToDegrees(lightDefinition.outerConeAngleRadians));
                        spotLight->setLocalDirection({ 0.0, 0.0, -1.0 });
                        spotLight->setRange(lightDefinition.range.toFloat());
                        light = spotLight;
                        break;
                    }
                    case Qt3DRender::QAbstractLight::DirectionalLight: {
                        auto directionalLight = new DirectionalLight(m_contentRootEntity);
                        directionalLight->setDirection({ 0.0, 0.0, -1.0 });
                        directionalLight->setDirectionMode(DirectionalLight::Local);
                        light = directionalLight;
                    }
                    }
                    light->setObjectName(lightDefinition.name);
                    light->setColor(lightDefinition.color);
                    light->setIntensity(lightDefinition.intensity);

                    // Record component into light
                    Light &l = m_context->light(node.lightIdx);
                    l.lightComponent = light;
                }
                entity->addComponent(light);
            }

            // If the node has a mesh, add it
            const qint32 meshId = node.meshIdx;
            if (meshId >= 0 && meshId < m_context->meshesCount()) {
                const qint32 skinId = node.skinIdx;
                const Mesh &meshData = m_context->mesh(meshId);
                const bool isSkinned = skinId >= 0 && skinId < m_context->skinsCount();
                const bool hasMorphTargets = meshData.morphTargetCount > 0;

                Qt3DCore::QArmature *armature = nullptr;
                Qt3DCore::QEntity *skinRootJointEntity = nullptr;
                MorphController *morphController = nullptr;

                // Create armature if node references a skin
                {
                    if (isSkinned) {
                        armature = new Qt3DCore::QArmature(m_contentRootEntity);
                        const Skin &skin = m_context->skin(skinId);
                        Qt3DCore::QSkeleton *skeleton = skin.skeleton;
                        armature->setSkeleton(skeleton);
                        if (!skin.rootJoint.joint && skin.rootJoint.skeletonNode)
                            skinRootJointEntity = skin.rootJoint.skeletonNode;
                        else
                            skinRootJointEntity = m_contentRootEntity;

                        // We need to get the skeleton index buffer and adapt the joints it refers to
                        for (const auto &primitive : qAsConst(meshData.meshPrimitives)) {
                            QGeometry *geometry = primitive.primitiveRenderer->geometry();
                            QAttribute *jointIndicesAttr = nullptr;
                            const auto attributes = geometry->attributes();
                            for (auto attr : attributes) {
                                if (attr->name() == QAttribute::defaultJointIndicesAttributeName()) {
                                    jointIndicesAttr = attr;
                                    break;
                                }
                            }

                            if (jointIndicesAttr == nullptr) {
                                qCWarning(Kuesa::kuesa) << "You are using a skinned mesh without joints buffer";
                                continue;
                            }

                            switch (jointIndicesAttr->vertexBaseType()) {
                            case QAttribute::UnsignedByte:
                                updateDataForJointsAttr<unsigned char>(jointIndicesAttr, skinId);
                                break;
                            case QAttribute::UnsignedShort:
                                updateDataForJointsAttr<unsigned short>(jointIndicesAttr, skinId);
                                break;
                            default:
                                qCWarning(Kuesa::kuesa, "Joint indices buffer component type should be UnsignedByte or UnsignedShort");
                                Q_UNREACHABLE();
                            }
                        }

                        Q_ASSERT(skinRootJointEntity);
                    }
                }

                // Create a Morph Controller if we have morph targets
                {
                    if (hasMorphTargets) {
                        // Initiliaze the Morph Controller with the default weights
                        const QVector<float> nodeDefaultWeight = node.morphTargetWeights;
                        const QVector<float> meshDefaultWeights = meshData.morphTargetWeights;
                        const quint8 morphTargetCount = std::min(meshData.morphTargetCount, quint8(8));

                        if (meshData.morphTargetCount > 8)
                            qCWarning(Kuesa::kuesa) << "Kuesa only supports up to 8 morph targets per mesh";

                        // node Default Weights have priority over the mesh ones
                        morphController = new MorphController(m_contentRootEntity);
                        morphController->setCount(morphTargetCount);

                        QVariantList defaultWeights;
                        defaultWeights.reserve(morphTargetCount);

                        for (int i = 0, m = morphTargetCount; i < m; ++i) {
                            const float defaultWeight = (i < nodeDefaultWeight.size()) ? nodeDefaultWeight.at(i) : meshDefaultWeights.at(i);
                            defaultWeights.push_back(defaultWeight);
                        }

                        morphController->setMorphWeights(defaultWeights);
                    }
                }

                // Generate one Entity per primitive (1 primitive == 1 geometry renderer)
                for (const Primitive &primitiveData : meshData.meshPrimitives) {
                    Qt3DCore::QEntity *primitiveEntity = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DCore::QEntity>("QEntity");

                    // If the mesh is skinned, the parent of the primitiveEntity wont be the treeNode
                    // Store this Entity to Entity map so we can use it later on the animation generation
                    primitiveData.primitiveRenderer->setParent(m_contentRootEntity);
                    m_context->addPrimitiveEntityToEntity(node.entity, primitiveEntity);
                    primitiveEntity->addComponent(primitiveData.primitiveRenderer);

                    // Add morph controller if it is not null
                    if (morphController != nullptr)
                        primitiveEntity->addComponent(morphController);

                    // Add material for mesh
                    {
                        // TO DO: generate proper morph target vertex shader based
                        // on meshData.morphTargetCount and primitiveData.morphTargets

                        auto setBrdfLutOnEffect = [this](Qt3DRender::QMaterial *m) {
                            auto effect = qobject_cast<MetallicRoughnessEffect *>(m->effect());
                            if (effect && m_sceneEntity)
                                effect->setBrdfLUT(m_sceneEntity->brdfLut());
                        };

                        auto checkMaterialIsCompatibleWithPrimitive = [](Qt3DRender::QMaterial *m, Qt3DRender::QGeometryRenderer *renderer,
                                                                         const QString &primitiveName, const QString &materialName) {
                            MetallicRoughnessMaterial *metalRoughMat = qobject_cast<MetallicRoughnessMaterial *>(m);
                            // When we have a normal map on a MetalRoughMetarial, make sure we have a tangent attribute
                            if (metalRoughMat != nullptr && metalRoughMat->materialProperties()->normalMap() != nullptr) {
                                Q_ASSERT(renderer->geometry());
                                const QVector<QAttribute *> attributes = renderer->geometry()->attributes();
                                bool hasTangentAttribute = std::find_if(attributes.cbegin(),
                                                                        attributes.cend(),
                                                                        [](QAttribute *attr) {
                                                                            return attr->name() == QAttribute::defaultTangentAttributeName();
                                                                        }) != attributes.cend();
                                if (!hasTangentAttribute)
                                    qWarning() << QStringLiteral("Primitive %1 is trying to use Material %2 which does normal mapping even though it defines no tangent attribute. This can result in incorrect rendering, please consider generating tangents.")
                                                          .arg(primitiveName, materialName);
                            }
                        };

                        const qint32 materialId = primitiveData.materialIdx;
                        Material mat;
                        // Check if the mesh references a material and fetch it
                        // If it doesn't reference a valid material, create a default shader data
                        if (materialId >= 0 && materialId < m_context->materialsCount())
                            mat = m_context->material(materialId);
                        else
                            mat.materialProperties(*m_context);
                        auto effectProperties = Material::effectPropertiesFromMaterial(mat);
                        if (isSkinned)
                            effectProperties |= EffectProperties::Skinning;
                        if (primitiveData.hasColorAttr)
                            effectProperties |= EffectProperties::VertexColor;
                        Qt3DRender::QEffect *effect = nullptr;

                        if (mat.extensions.KDAB_custom_material)
                            effect = m_context->effectLibrary()->getOrCreateCustomEffect({ mat.customMaterial.effectClassMetaObject, effectProperties },
                                                                                         m_contentRootEntity);
                        else
                            effect = m_context->effectLibrary()->getOrCreateEffect(effectProperties,
                                                                                   m_contentRootEntity);

                        Kuesa::GLTF2Material *material = nullptr;
                        auto materialName = mat.name;

                        if (mat.extensions.KDAB_custom_material) {
                            GLTF2Material *specificMaterial = qobject_cast<GLTF2Material *>(
                                    mat.customMaterial.materialClassMetaObject->newInstance());
                            Q_ASSERT(specificMaterial);
                            material = specificMaterial;
                            Kuesa::GLTF2MaterialProperties *materialProperties = mat.materialProperties();
                            materialProperties->setParent(m_contentRootEntity);
                            // Set material properties on material
                            QMetaObject::invokeMethod(specificMaterial,
                                                      "setMaterialProperties",
                                                      Qt::DirectConnection,
                                                      Q_ARG(Kuesa::GLTF2MaterialProperties *, materialProperties));
                        } else if (mat.extensions.KHR_materials_unlit) {
                            auto *specificMaterial = new Kuesa::UnlitMaterial;
                            material = specificMaterial;
                            auto materialProperties = static_cast<Kuesa::UnlitProperties *>(mat.materialProperties());
                            materialProperties->setParent(m_contentRootEntity);

                            specificMaterial->setMaterialProperties(materialProperties);
                        } else {
                            auto *specificMaterial = new MetallicRoughnessMaterial;
                            material = specificMaterial;
                            auto materialProperties = static_cast<MetallicRoughnessProperties *>(mat.materialProperties());
                            materialProperties->setParent(m_contentRootEntity);
                            specificMaterial->setMaterialProperties(materialProperties);

                            checkMaterialIsCompatibleWithPrimitive(specificMaterial, primitiveData.primitiveRenderer,
                                                                   meshData.name, materialName);
                        }
                        // TODO assign brdfLut on effect creation on the library
                        material->setParent(m_contentRootEntity);
                        material->setEffect(effect);
                        setBrdfLutOnEffect(material);
                        if (hasMorphTargets)
                            material->setMorphController(morphController);

                        primitiveEntity->addComponent(material);
                    }
                    // Add armature if it is not null
                    if (armature != nullptr) {
                        primitiveEntity->addComponent(armature);
                        // We set the parent to the skeleton root's transform
                        // (in other words, our parent is the entity that
                        // corresponds to the node to which the root joint is
                        // assigned)
                        primitiveEntity->setParent(skinRootJointEntity);
                    } else {
                        // We set the parent to entity so that transform is applied
                        primitiveEntity->setParent(entity);
                    }
                }
            }
        }

        // Build Joint Content
        for (auto &joint : node.joints) {
            if (joint != nullptr) {
                if (node.transformInfo.bits & TreeNode::TransformInfo::MatrixSet) {
                    QVector3D translation;
                    QQuaternion rotation;
                    QVector3D scale;
                    decomposeQMatrix4x4(node.transformInfo.matrix,
                                        translation,
                                        rotation,
                                        scale);
                    joint->setScale(scale);
                    joint->setRotation(rotation);
                    joint->setTranslation(translation);
                } else {
                    if (node.transformInfo.bits & TreeNode::TransformInfo::ScaleSet)
                        joint->setScale(node.transformInfo.scale3D);
                    if (node.transformInfo.bits & TreeNode::TransformInfo::RotationSet)
                        joint->setRotation(node.transformInfo.rotation);
                    if (node.transformInfo.bits & TreeNode::TransformInfo::TranslationSet)
                        joint->setTranslation(node.transformInfo.translation);
                }
            }
        }
    }
}

void GLTF2Parser::generateSkeletonContent()
{
    for (int skinId = 0, m = m_context->skinsCount(); skinId < m; ++skinId) {
        Skin &skin = m_context->skin(skinId);

        const bool bindMatrixDataSpecified = skin.inverseBindMatricesAccessorIdx >= 0;
        // Bind Matrix data is specified
        // (otherwise we assume each joint is the identity matrix == inverse bind matrix preapplied )
        if (bindMatrixDataSpecified) {
            // Update Joints inverseBindMatrix
            int matrixIdx = 0;
            for (int jointId : skin.jointsIndices) {
                TreeNode &jointNode = m_context->treeNode(jointId);
                jointNode.joints[skinId]->setInverseBindMatrix(skin.inverseBindMatrices[matrixIdx++]);
            }
        }

        // Set root joint on skeleton
        Qt3DCore::QJoint *rootJoint = skin.rootJoint.joint;
        if (!rootJoint)
            rootJoint = m_context->treeNode(skin.rootJoint.jointNodeIdx).joints[skinId];

        Qt3DCore::QSkeleton *skeleton = new Qt3DCore::QSkeleton();
        Q_ASSERT(rootJoint);
        skeleton->setRootJoint(rootJoint);
        skin.skeleton = skeleton;
    }
}

void GLTF2Parser::generateAnimationContent()
{
    for (int animationId = 0, m = m_context->animationsCount(); animationId < m; ++animationId) {
        Animation &animation = m_context->animation(animationId);

        auto *channelMapper = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DAnimation::QChannelMapper>("QChannelMapper");
        auto *clip = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DAnimation::QAnimationClip>("QAnimationClip");
        clip->setClipData(animation.clipData);
        animation.clip = clip;
        animation.mapper = channelMapper;

        for (const ChannelMapping &mapping : qAsConst(animation.mappings)) {
            const std::vector<Qt3DAnimation::QChannelMapping *> channelMappings = mapping.generator(m_context,
                                                                                                    mapping);
            for (Qt3DAnimation::QChannelMapping *m : channelMappings)
                channelMapper->addMapping(m);
        }
    }
}

// Root Entity to which Qt3D resources a parented
Qt3DCore::QEntity *GLTF2Parser::contentRoot() const
{
    return m_contentRootEntity;
}

// Array of QEntity subtrees for each glTF scene
QVector<SceneRootEntity *> GLTF2Parser::sceneRoots() const
{
    return m_sceneRootEntities;
}

ThreadedGLTF2Parser::ThreadedGLTF2Parser(
        GLTF2Context *context,
        SceneEntity *sceneEntity,
        bool assignNames)
    : m_parser{ sceneEntity, assignNames }
{
    m_parser.setContext(context);

    m_parser.moveToThread(&m_thread);
    m_thread.start();
}

ThreadedGLTF2Parser::~ThreadedGLTF2Parser()
{
    m_thread.exit(0);
    m_thread.wait();
}

void ThreadedGLTF2Parser::on_parsingFinished()
{
    Qt3DCore::QEntity *root = m_parser.m_contentRootEntity;

    if (root) {
        m_parser.addResourcesToSceneEntityCollections();
        root->setParent((Qt3DCore::QNode *)nullptr);
        root->moveToThread(this->thread());
    }
    emit parsingFinished(root);
}

void ThreadedGLTF2Parser::parse(const QString &path)
{
    QMetaObject::invokeMethod(&m_parser, [=] {
        QFile f(path);
        f.open(QIODevice::ReadOnly);
        if (!f.isOpen()) {
            qCWarning(Kuesa::kuesa) << "Can't read file" << path;
            return;
        }

        QFileInfo finfo(path);
        const QByteArray jsonData = f.readAll();
        bool isValid = m_parser.detectTypeAndParse(jsonData, finfo.absolutePath(), finfo.fileName());

        m_parser.moveToThread(this->thread());

        if (isValid)
            QMetaObject::invokeMethod(this, &ThreadedGLTF2Parser::on_parsingFinished);
        else
            emit parsingFinished(nullptr);
        return;
    });
}

void ThreadedGLTF2Parser::parse(const QByteArray &data, const QString &basePath, const QString &filename)
{
    QMetaObject::invokeMethod(&m_parser, [=] {
        bool isValid = m_parser.detectTypeAndParse(data, basePath, filename);

        m_parser.moveToThread(this->thread());

        if (isValid)
            QMetaObject::invokeMethod(this, &ThreadedGLTF2Parser::on_parsingFinished);
        else
            emit parsingFinished(nullptr);
    });
}

QT_END_NAMESPACE
