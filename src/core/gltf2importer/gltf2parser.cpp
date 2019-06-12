/*
    bufferviewsparser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "morphcontroller.h"
#include "gltf2material.h"
#include "directionallight.h"
#include "pointlight.h"
#include "spotlight.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>

#include <functional>

#include <Qt3DCore/private/qabstractnodefactory_p.h>
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

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

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
        currentName = QString(QLatin1Literal("%1_%2")).arg(basename, QString::number(i));
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

    while (parserIt != parserEnd) {
        const QJsonValue value = rootObject.value((*parserIt).first);
        const bool success = value.isUndefined() || (*parserIt).second(value);
        if (!success) {
            qCWarning(kuesa()) << "Failed to parse" << (*parserIt).first;
            return false;
        }
        ++parserIt;
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
        qCWarning(kuesa()) << "Can't read file" << filePath;
        return false;
    }

    QFileInfo finfo(filePath);
    QByteArray data = f.readAll();
    return parse(data, finfo.absolutePath(), finfo.fileName());
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
    const bool isValid = detectTypeAndParse(data, basePath, filename);
    if (isValid)
        addResourcesToSceneEntityCollections();
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
}

bool GLTF2Parser::isBinaryGLTF(const QByteArray &data, bool &isValid)
{
    bool isBinary = true;
    isValid = true;

    if (data.size() < sizeof(GLBHeader))
        return false;
    const GLBHeader *glbHeader = reinterpret_cast<const GLBHeader *>(data.constData());
    if (glbHeader->magic != GLTF_BINARY_MAGIC) {
        isBinary = false;
    } else {
        if (glbHeader->version != 2) {
            qCWarning(kuesa()) << "Unsupported glb version" << glbHeader->version;
            isValid = false;
        } else if (glbHeader->length != data.size()) {
            qCWarning(kuesa()) << "Unexpected glb file size" << data.size() << ". Was expecting" << glbHeader->length;
            isValid = false;
        }
    }

    return isBinary;
}

template<class T>
void GLTF2Parser::updateDataForJointsAttr(Qt3DRender::QAttribute *attr, int skinId)
{
    auto bufferData = attr->buffer()->data();
    auto data = bufferData.data() + attr->byteOffset();
    auto typedData = reinterpret_cast<const T *>(data);
    const size_t nbJoints = 4 * attr->count();
    QByteArray updatedData;

    updatedData.resize(static_cast<int>(nbJoints * sizeof(T)));
    T *updatedJointIndices = reinterpret_cast<T *>(updatedData.data());
    for (size_t jointId = 0; jointId < nbJoints; ++jointId) {
        updatedJointIndices[jointId] = static_cast<T>(m_gltfJointIdxToSkeletonJointIdxPerSkeleton[skinId][typedData[jointId]]);
    }
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
        { KEY_ANIMATIONS, [this](const QJsonValue &value) {
             const QJsonArray array = value.toArray();
             if (array.size() == 0)
                 return true;
             AnimationParser parser;
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
         } }
    };
}

bool GLTF2Parser::parseJSON(const QByteArray &jsonData, const QString &basePath, const QString &filename)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
    if (jsonDocument.isNull() || !jsonDocument.isObject()) {
        qCWarning(kuesa()) << "File is not a valid json document";
        return false;
    }

    Q_ASSERT(m_context);
    m_context->setJson(jsonDocument);
    m_context->setFilename(filename);
    m_context->setBasePath(basePath);
    m_basePath = basePath;

    m_animators.clear();
    m_treeNodes.clear();
    m_skeletons.clear();
    m_gltfJointIdxToSkeletonJointIdxPerSkeleton.clear();

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
            KEY_KHR_LIGHTS_PUNCTUAL_EXTENSION
        };
        for (const auto &e : qAsConst(extensions)) {
            if (supportedExtensions.contains(e))
                continue;
            allRequiredAreSupported = false;
            unsupportedExtensions << e;
        }

        if (!allRequiredAreSupported) {
            qCWarning(kuesa) << "File contains unsupported extensions: " << unsupportedExtensions;
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
            qCWarning(kuesa) << "Invalid default scene reference";
            return false;
        }
    }

    // Build vector of tree nodes
    for (int i = 0, m = m_context->treeNodeCount(); i < m; ++i)
        m_treeNodes.push_back(m_context->treeNode(i));

    // Build hierarchies for Entities and QJoints
    buildEntitiesAndJointsGraph();

    // Generate Qt3D content for skeletons
    generateSkeletonContent();

    // Generate Qt3D data for the nodes based on their type
    generateTreeNodeContent();

    // Generate Qt3D content for animations
    generateAnimationContent();

    // Build Scene Roots
    buildSceneRootEntities();

    return true;
}

void GLTF2Parser::moveToThread(QThread *targetThread)
{
    QObject::moveToThread(targetThread);
    if (m_contentRootEntity)
        m_contentRootEntity->moveToThread(targetThread);
    for (auto &anim : m_animators) {
        anim.clip->moveToThread(targetThread);
        anim.mapper->moveToThread(targetThread);
    }

    for (int i = 0, n = m_context->scenesCount(); i < n; i++) {
        const Scene scene = m_context->scene(i);
        const QVector<int> toRetrieveNodes = scene.rootNodeIndices;

        for (int j = 0, m = toRetrieveNodes.size(); j < m; ++j) {
            const TreeNode node = m_treeNodes[toRetrieveNodes.at(j)];
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
            for (const TreeNode &treeNode : qAsConst(m_treeNodes)) {
                if (treeNode.entity != nullptr && !treeNode.name.isEmpty()) {
                    addToCollectionWithUniqueName(m_sceneEntity->entities(), treeNode.name, treeNode.entity);

                    if (treeNode.cameraIdx >= 0)
                        addToCollectionWithUniqueName(m_sceneEntity->cameras(), treeNode.name, qobject_cast<Qt3DRender::QCamera *>(treeNode.entity));

                    auto light = componentFromEntity<Qt3DRender::QAbstractLight>(treeNode.entity);
                    if (light)
                        addToCollectionWithUniqueName(m_sceneEntity->lights(), light->objectName(), light);
                }
            }

            if (m_assignNames) {
                int j = 0;
                for (const TreeNode &treeNode : qAsConst(m_treeNodes)) {
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
                    [this](const Animation &animation, int i) { addToCollectionWithUniqueName(m_sceneEntity->animationClips(), animation.name, m_animators.at(i).clip); },
                    [this](const Animation &, int i) { addToCollectionWithUniqueName(m_sceneEntity->animationClips(), QStringLiteral("KuesaAnimation_%1").arg(i), m_animators.at(i).clip); });

        if (m_sceneEntity->animationMappings())
            addAssetsIntoCollection<Animation>(
                    [this](const Animation &animation, int i) { addToCollectionWithUniqueName(m_sceneEntity->animationMappings(), animation.name, m_animators.at(i).mapper); },
                    [this](const Animation &, int i) { addToCollectionWithUniqueName(m_sceneEntity->animationMappings(), QStringLiteral("KuesaAnimation_%1").arg(i), m_animators.at(i).mapper); });

        if (m_sceneEntity->materials())
            addAssetsIntoCollection<Material>(
                    [this](const Material &material, int) {
                        if (material.hasRegularMaterial())
                            addToCollectionWithUniqueName(m_sceneEntity->materials(), material.name, material.material(false));
                        if (material.hasSkinnedMaterial())
                            addToCollectionWithUniqueName(m_sceneEntity->materials(), material.name + QStringLiteral("_skinned"), material.material(true));
                    },
                    [this](const Material &material, int i) {
                        if (material.hasRegularMaterial())
                            addToCollectionWithUniqueName(m_sceneEntity->materials(), QStringLiteral("KuesaMaterial_%1").arg(i), material.material(false));
                        if (material.hasSkinnedMaterial())
                            addToCollectionWithUniqueName(m_sceneEntity->materials(), QStringLiteral("KuesaMaterial_%1_skinned").arg(i), material.material(true));
                    });

        if (m_sceneEntity->skeletons())
            addAssetsIntoCollection<Skin>(
                    [this](const Skin &skin, int i) { addToCollectionWithUniqueName(m_sceneEntity->skeletons(), skin.name, m_skeletons.at(i)); },
                    [this](const Skin &, int i) { addToCollectionWithUniqueName(m_sceneEntity->skeletons(), QStringLiteral("KuesaSkeleton_%1").arg(i), m_skeletons.at(i)); });
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
            const TreeNode node = m_treeNodes[toRetrieveNodes.at(j)];
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

        if ((endOffset - currentOffset) < sizeof(ChunkHeader)) {
            qCWarning(kuesa()) << "Malformed chunck in glb file";
            return false;
        }

        const ChunkHeader *chunkHeader = reinterpret_cast<const ChunkHeader *>(current);
        current += sizeof(ChunkHeader);
        currentOffset += sizeof(ChunkHeader);

        if ((endOffset - currentOffset) < chunkHeader->chunkLength) {
            qCWarning(kuesa()) << "Malformed chunck in glb file";
            return false;
        }

        if (chunkHeader->chunkType == GLTF_CHUNCK_JSON) {
            // JSON chunck
            if (jsonData.isEmpty()) {
                jsonData = QByteArray::fromRawData(current, chunkHeader->chunkLength);
            } else {
                qCWarning(kuesa()) << "Multiple JSON chunks in glb file";
                return false;
            }
        } else if (chunkHeader->chunkType == GLTF_CHUNCK_BIN) {
            // BIN chunck
            if (m_context->bufferChunk().isEmpty()) {
                m_context->setBufferChunk(QByteArray::fromRawData(current, chunkHeader->chunkLength));
            } else {
                qCWarning(kuesa()) << "Multiple BIN chunks in glb file";
                return false;
            }
        } else {
            // skip chunck
            qCWarning(kuesa()) << "Ignoring unhandled chunck type in glb file:" << chunkHeader->chunkType;
        }

        current += chunkHeader->chunkLength;
        currentOffset += chunkHeader->chunkLength;
    }

    if (jsonData.isEmpty()) {
        qCWarning(kuesa()) << "Missing JSON chunk in glb file";
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

        const TreeNode treeNode = m_treeNodes.at(nodeId);
        const QVector<int> childrenIndices = treeNode.childrenIndices;

        if (childrenIndices.size() > 0) {
            for (const int childId : childrenIndices) {
                if (childId < 0 || childId > nbNodes) {
                    qCWarning(kuesa) << "Encountered invalid child node reference while building hierarchy";
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
            TreeNode &node = m_treeNodes[leaf->nodeIdx];
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
        TreeNode &branchRoot = m_treeNodes[root->nodeIdx];
        branchRoot.isRootNode = true;
    }

    int skinsCount = m_context->skinsCount();
    for (auto &treeNode : m_treeNodes)
        treeNode.joints.resize(skinsCount);

    // Assign QJoint to treenodes used as joints
    m_gltfJointIdxToSkeletonJointIdxPerSkeleton.resize(m_context->skinsCount());
    for (int skinId = 0, m = m_context->skinsCount(); skinId < m; ++skinId) {
        const Skin skin = m_context->skin(skinId);

        int jointAccessor = 0;

        const HierarchyNode *skeletonRootHNode = tree.data() + skin.skeletonIdx;
        if (skin.jointsIndices.contains(skin.skeletonIdx))
            m_gltfJointIdxToSkeletonJointIdxPerSkeleton[skinId][skin.jointsIndices.indexOf(skin.skeletonIdx)] = jointAccessor;
        buildJointHierarchy(skeletonRootHNode, jointAccessor, skin, skinId);
    }
}

void GLTF2Parser::buildJointHierarchy(const HierarchyNode *node, int &jointAccessor, const Skin &skin, int skinIdx, Qt3DCore::QJoint *parentJoint)
{
    Qt3DCore::QJoint *joint = new Qt3DCore::QJoint;
    if (parentJoint != nullptr)
        parentJoint->addChildJoint(joint);
    m_treeNodes[node->nodeIdx].joints[skinIdx] = joint;
    jointAccessor++;
    joint->setName(m_treeNodes[node->nodeIdx].name);
    joint->setInverseBindMatrix(QMatrix4x4());

    for (const HierarchyNode *childNode : node->children) {
        if (skin.jointsIndices.contains(childNode->nodeIdx)) {
            m_gltfJointIdxToSkeletonJointIdxPerSkeleton[skinIdx][skin.jointsIndices.indexOf(childNode->nodeIdx)] = jointAccessor;
            buildJointHierarchy(childNode, jointAccessor, skin, skinIdx, joint);
        }
    }
}

void GLTF2Parser::generateTreeNodeContent()
{
    MetallicRoughnessMaterial *defaultMaterial = nullptr;
    MetallicRoughnessMaterial *defaultSkinnedMaterial = nullptr;
    m_contentRootEntity = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DCore::QEntity>("QEntity");
    m_contentRootEntity->setObjectName(QStringLiteral("GLTF2Scene"));

    for (TreeNode &node : m_treeNodes) {
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
                    camera->lens()->setProjectionType(cam.lens->projectionType());
                    if (cam.lens->projectionType() == Qt3DRender::QCameraLens::PerspectiveProjection) {
                        camera->lens()->setAspectRatio(cam.lens->aspectRatio());
                        camera->lens()->setNearPlane(cam.lens->nearPlane());
                        camera->lens()->setFarPlane(cam.lens->farPlane());
                        camera->lens()->setFieldOfView(cam.lens->fieldOfView());
                    } else {
                        // Custom Projection Matrix
                        camera->lens()->setProjectionMatrix(cam.lens->projectionMatrix());
                    }

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
                        entity->addComponent(layer.layer);
                    }
                }
            }

            // If the node references Light (KHR_lights_punctual extension), add them
            if (node.lightIdx != -1 && node.lightIdx < m_context->lightCount()) {
                const auto lightDefinition = m_context->light(node.lightIdx);
                Qt3DRender::QAbstractLight *light = nullptr;
                switch (lightDefinition.type) {
                case Qt3DRender::QAbstractLight::PointLight: {
                    auto pointLight = new PointLight;
                    pointLight->setRange(lightDefinition.range.toFloat());
                    light = pointLight;
                    break;
                }
                case Qt3DRender::QAbstractLight::SpotLight: {
                    auto spotLight = new SpotLight;
                    spotLight->setInnerConeAngle(qRadiansToDegrees(lightDefinition.innerConeAngleRadians));
                    spotLight->setOuterConeAngle(qRadiansToDegrees(lightDefinition.outerConeAngleRadians));
                    spotLight->setLocalDirection({ 0.0, 0.0, -1.0 });
                    spotLight->setRange(lightDefinition.range.toFloat());
                    light = spotLight;
                    break;
                }
                case Qt3DRender::QAbstractLight::DirectionalLight: {
                    auto directionalLight = new DirectionalLight;
                    directionalLight->setDirection({ 0.0, 0.0, -1.0 });
                    directionalLight->setDirectionMode(DirectionalLight::Local);
                    light = directionalLight;
                }
                }
                light->setObjectName(lightDefinition.name);
                light->setColor(lightDefinition.color);
                light->setIntensity(lightDefinition.intensity);
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
                        armature = new Qt3DCore::QArmature();
                        const Skin &skin = m_context->skin(skinId);
                        Qt3DCore::QSkeleton *skeleton = m_skeletons.at(skinId);
                        armature->setSkeleton(skeleton);
                        skinRootJointEntity = m_treeNodes[skin.skeletonIdx].entity->parentEntity();
                        if (!skinRootJointEntity)
                            skinRootJointEntity = m_treeNodes[skin.skeletonIdx].entity;

                        // We need to get the skeleton index buffer and adapt the joints it refers to
                        for (const auto &primitive : qAsConst(meshData.meshPrimitives)) {
                            Qt3DRender::QGeometry *geometry = primitive.primitiveRenderer->geometry();
                            Qt3DRender::QAttribute *jointIndicesAttr = nullptr;
                            const auto attributes = geometry->attributes();
                            for (auto attr : attributes) {
                                if (attr->name() == Qt3DRender::QAttribute::defaultJointIndicesAttributeName()) {
                                    jointIndicesAttr = attr;
                                    break;
                                }
                            }

                            if (jointIndicesAttr == nullptr) {
                                qCWarning(kuesa) << "You are using a skinned mesh without joints buffer";
                                continue;
                            }

                            switch (jointIndicesAttr->vertexBaseType()) {
                            case Qt3DRender::QAttribute::UnsignedByte:
                                updateDataForJointsAttr<unsigned char>(jointIndicesAttr, skinId);
                                break;
                            case Qt3DRender::QAttribute::UnsignedShort:
                                updateDataForJointsAttr<unsigned short>(jointIndicesAttr, skinId);
                                break;
                            default:
                                qCWarning(kuesa, "Joint indices buffer component type should be UnsignedByte or UnsignedShort");
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
                            qCWarning(kuesa) << "Kuesa only supports up to 8 morph targets per mesh";

                        // node Default Weights have priority over the mesh ones
                        morphController = new MorphController();
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
                    primitiveEntity->addComponent(primitiveData.primitiveRenderer);

                    // Add morph controller if it is not null
                    if (morphController != nullptr) {
                        primitiveEntity->addComponent(morphController);
                    }

                    // Add material for mesh
                    {
                        GLTF2Material *material = nullptr;

                        // TO DO: generate proper morph target vertex shader based
                        // on meshData.morphTargetCount and primitiveData.morphTargets

                        auto setBrdfLutOnEffect = [this](GLTF2Material *m) {
                            auto effect = qobject_cast<MetallicRoughnessEffect *>(m->effect());
                            if (effect && m_sceneEntity)
                                effect->setBrdfLUT(m_sceneEntity->brdfLut());
                        };

                        auto checkMaterialIsCompatibleWithPrimitive = [](GLTF2Material *m, Qt3DRender::QGeometryRenderer *renderer,
                                                                         const QString &primitiveName, const QString &materialName) {
                            MetallicRoughnessMaterial *metalRoughMat = qobject_cast<MetallicRoughnessMaterial *>(m);
                            // When we have a normal map on a MetalRoughMetarial, make sure we have a tangent attribute
                            if (metalRoughMat != nullptr && metalRoughMat->normalMap() != nullptr) {
                                Q_ASSERT(renderer->geometry());
                                const QVector<Qt3DRender::QAttribute *> attributes = renderer->geometry()->attributes();
                                bool hasTangentAttribute = std::find_if(attributes.cbegin(),
                                                                        attributes.cend(),
                                                                        [](Qt3DRender::QAttribute *attr) {
                                                                            return attr->name() == Qt3DRender::QAttribute::defaultTangentAttributeName();
                                                                        }) != attributes.cend();
                                if (!hasTangentAttribute)
                                    qWarning() << QStringLiteral("Primitive %1 is trying to use Material %2 which does normal mapping even though it defines no tangent attribute. This can result in incorrect rendering, please consider generating tangents.")
                                                          .arg(primitiveName)
                                                          .arg(materialName);
                            }
                        };

                        const qint32 materialId = primitiveData.materialIdx;
                        QString materialName;
                        if (materialId >= 0 && materialId < m_context->materialsCount()) {
                            Material &mat = m_context->material(materialId);
                            // Get or create Qt3D for material
                            material = mat.material(isSkinned, primitiveData.hasColorAttr, m_context);
                            materialName = mat.name;
                            setBrdfLutOnEffect(material);
                        } else {
                            // Only create defaultMaterial if we know we need it
                            // otherwise we might leak it
                            if (isSkinned) {
                                if (!defaultSkinnedMaterial) {
                                    MetallicRoughnessMaterial *material = new MetallicRoughnessMaterial;
                                    material->setUseSkinning(true);
                                    setBrdfLutOnEffect(material);
                                    defaultSkinnedMaterial = material;
                                }
                                materialName = QStringLiteral("defaultSkinnedMaterial");
                                material = defaultSkinnedMaterial;
                            } else {
                                if (!defaultMaterial) {
                                    MetallicRoughnessMaterial *material = new MetallicRoughnessMaterial;
                                    material->setUseSkinning(false);
                                    setBrdfLutOnEffect(material);
                                    defaultMaterial = material;
                                }
                                materialName = QStringLiteral("defaultNonSkinnedMaterial");
                                material = defaultMaterial;
                            }
                        }

                        if (hasMorphTargets)
                            material->setMorphController(morphController);

                        checkMaterialIsCompatibleWithPrimitive(material, primitiveData.primitiveRenderer,
                                                               meshData.name, materialName);
                        primitiveEntity->addComponent(material);
                    }

                    // Add armature if it is not null
                    if (armature != nullptr) {
                        primitiveEntity->addComponent(armature);
                        // We set the parent to the skeleton root's transform
                        // (in other words, our parent is the entity that
                        // corresponds to the node to which the root joint is
                        // assigned)
                        primitiveEntity->setParent(skinRootJointEntity->parentEntity() ? skinRootJointEntity->parentEntity() : m_contentRootEntity);
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
        const Skin skin = m_context->skin(skinId);

        const bool bindMatrixDataSpecified = skin.inverseBindMatricesAccessorIdx >= 0;
        // Bind Matrix data is specified
        // (otherwise we assume each joint is the identity matrix == inverse bind matrix preapplied )
        if (bindMatrixDataSpecified) {
            // Update Joints inverseBindMatrix
            int matrixIdx = 0;
            for (int jointId : skin.jointsIndices) {
                TreeNode &jointNode = m_treeNodes[jointId];
                jointNode.joints[skinId]->setInverseBindMatrix(skin.inverseBindMatrices[matrixIdx++]);
            }
        }

        // Set root joint on skeleton
        int rootJointId = skin.skeletonIdx;
        if (rootJointId < 0) {
            // Use the scene's root
            const Scene defaultScene = m_context->scene(m_defaultSceneIdx);
            // Find first root node of type Joint
            const QVector<int> rootNodeIndices = defaultScene.rootNodeIndices;
            rootJointId = rootNodeIndices.first();
        }

        Qt3DCore::QSkeleton *skeleton = new Qt3DCore::QSkeleton();
        Qt3DCore::QJoint *rootJoint = m_treeNodes.at(rootJointId).joints[skinId];
        Q_ASSERT(rootJoint);
        skeleton->setRootJoint(rootJoint);
        m_skeletons.push_back(skeleton);
    }
}

void GLTF2Parser::generateAnimationContent()
{
    for (int animationId = 0, m = m_context->animationsCount(); animationId < m; ++animationId) {
        Animation animation = m_context->animation(animationId);

        auto *channelMapper = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DAnimation::QChannelMapper>("QChannelMapper");
        auto *clip = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DAnimation::QAnimationClip>("QAnimationClip");
        clip->setClipData(animation.clipData);
        m_animators.push_back({ clip, channelMapper });

        for (const auto &skeleton : qAsConst(m_skeletons)) {
            auto skeletonMapping = new Qt3DAnimation::QSkeletonMapping;
            skeletonMapping->setSkeleton(skeleton);
            channelMapper->addMapping(skeletonMapping);
        }

        for (const ChannelMapping &mapping : qAsConst(animation.mappings)) {
            const TreeNode targetNode = m_treeNodes[mapping.targetNodeId];

            // Map channel to joint
            for (auto &joint : targetNode.joints) {
                if (joint) {
                    auto channelMapping = new Qt3DAnimation::QChannelMapping();
                    channelMapping->setTarget(joint);
                    channelMapping->setChannelName(mapping.name);
                    channelMapping->setProperty(mapping.property == QStringLiteral("scale3D") ? QStringLiteral("scale") : mapping.property);
                    channelMapper->addMapping(channelMapping);
                }
            }

            // Map channel to entity transform or morphcontroller
            if (targetNode.entity != nullptr) {
                const bool isMorphWeightProperty = (mapping.property == QStringLiteral("morphWeights"));

                if (!isMorphWeightProperty) {
                    auto transformComponent = componentFromEntity<Qt3DCore::QTransform>(targetNode.entity);
                    if (!transformComponent) {
                        qCWarning(kuesa, "Target node doesn't have a transform component");
                        continue;
                    }
                    auto channelMapping = new Qt3DAnimation::QChannelMapping();
                    channelMapping->setTarget(transformComponent);
                    channelMapping->setChannelName(mapping.name);
                    channelMapping->setProperty(mapping.property);
                    channelMapper->addMapping(channelMapping);
                } else {
                    // The actual entities to animate are those which render the
                    // primitive not the Node Entity
                    const QList<Qt3DCore::QEntity *> primitiveEntities =
                            targetNode.entity->findChildren<Qt3DCore::QEntity *>(QString(), Qt::FindDirectChildrenOnly);

                    for (Qt3DCore::QEntity *primitiveEntity : primitiveEntities) {
                        MorphController *morphControllerComponent =
                                componentFromEntity<Kuesa::MorphController>(primitiveEntity);
                        if (!morphControllerComponent) {
                            qCWarning(kuesa) << "Target node doesn't have a morph "
                                                "controller component to animate";
                            continue;
                        }
                        auto channelMapping = new Qt3DAnimation::QChannelMapping();
                        channelMapping->setTarget(morphControllerComponent);
                        channelMapping->setChannelName(mapping.name);
                        channelMapping->setProperty(mapping.property);
                        channelMapper->addMapping(channelMapping);
                    }
                }
            }
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
            qCWarning(kuesa()) << "Can't read file" << path;
            return;
        }

        QFileInfo finfo(path);
        const QByteArray jsonData = f.readAll();
        bool isValid = m_parser.detectTypeAndParse(jsonData, finfo.absolutePath(), finfo.fileName());

        m_parser.moveToThread(this->thread());

        if (isValid) {
            QMetaObject::invokeMethod(this, &ThreadedGLTF2Parser::on_parsingFinished);
        } else {
            emit parsingFinished(nullptr);
        }
        return;
    });
}

void ThreadedGLTF2Parser::parse(const QByteArray &data, const QString &basePath, const QString &filename)
{
    QMetaObject::invokeMethod(&m_parser, [=] {
        bool isValid = m_parser.detectTypeAndParse(data, basePath, filename);

        m_parser.moveToThread(this->thread());

        if (isValid) {
            QMetaObject::invokeMethod(this, &ThreadedGLTF2Parser::on_parsingFinished);
        } else {
            emit parsingFinished(nullptr);
        }
    });
}

QT_END_NAMESPACE
