/*
    bufferviewsparser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "gltf2parser_p.h"
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
#include "imageparser_p.h"
#include "texturesamplerparser_p.h"
#include "animationparser_p.h"
#include "sceneparser_p.h"
#include "skinparser_p.h"
#include "metallicroughnessmaterial.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>

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
const QLatin1String KEY_BUFFERS = QLatin1Literal("buffers");
const QLatin1String KEY_BUFFERVIEWS = QLatin1Literal("bufferViews");
const QLatin1String KEY_ACCESSORS = QLatin1Literal("accessors");
const QLatin1String KEY_NODES = QLatin1Literal("nodes");
const QLatin1String KEY_MESHES = QLatin1Literal("meshes");
const QLatin1String KEY_SCENE = QLatin1Literal("scene");
const QLatin1String KEY_SCENES = QLatin1Literal("scenes");
const QLatin1String KEY_KDAB_KUESA_LAYER_EXTENSION = QLatin1String("KDAB_Kuesa_Layers");
const QLatin1String KEY_MSFT_DDS_EXTENSION = QLatin1String("MSFT_texture_dds");
const QLatin1String KEY_KUESA_LAYERS = QLatin1Literal("layers");
const QLatin1String KEY_CAMERAS = QLatin1Literal("cameras");
const QLatin1String KEY_IMAGES = QLatin1Literal("images");
const QLatin1String KEY_TEXTURE_SAMPLERS = QLatin1Literal("samplers");
const QLatin1String KEY_TEXTURES = QLatin1String("textures");
const QLatin1String KEY_EXTENSIONS = QLatin1String("extensions");
const QLatin1String KEY_EXTENSIONS_USED = QLatin1String("extensionsUsed");
const QLatin1String KEY_EXTENSIONS_REQUIRED = QLatin1String("extensionsRequired");
const QLatin1String KEY_ANIMATIONS = QLatin1String("animations");
const QLatin1String KEY_MATERIALS = QLatin1String("materials");
const QLatin1String KEY_SKINS = QLatin1String("skins");
#if defined(KUESA_DRACO_COMPRESSION)
const QLatin1String KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION = QLatin1String("KHR_draco_mesh_compression");
#endif

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

void extractPositionViewDirAndUpVectorFromViewMatrix(const QMatrix4x4 viewMatrix,
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
    , m_defaultSceneIdx(-1)
    , m_assignNames(assignNames)
{
}

GLTF2Parser::~GLTF2Parser()
{
}

Qt3DCore::QEntity *GLTF2Parser::parse(const QString &filePath)
{
    QFile f(filePath);
    f.open(QIODevice::ReadOnly);
    if (!f.isOpen()) {
        qCWarning(kuesa()) << "Can't read file" << filePath;
        return nullptr;
    }

    QFileInfo finfo(filePath);
    const QByteArray jsonData = f.readAll();
    return parse(jsonData, finfo.absolutePath());
}

template<class T>
void GLTF2Parser::updateDataForJointsAttr(Qt3DRender::QAttribute *attr, int skinId)
{
    auto bufferData = attr->buffer()->data();
    auto data = bufferData.data() + attr->byteOffset();
    auto typedData = reinterpret_cast<const T *>(data);
    const size_t nbJoints = 4 * attr->count();
    QByteArray updatedData;

    updatedData.resize(nbJoints * sizeof(T));
    T *updatedJointIndices = reinterpret_cast<T *>(updatedData.data());
    for (size_t jointId = 0; jointId < nbJoints; ++jointId) {
        updatedJointIndices[jointId] = m_gltfJointIdxToSkeletonJointIdxPerSkeleton[skinId][typedData[jointId]];
    }
    attr->buffer()->updateData(attr->byteOffset(), updatedData);
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
                  } }
             };
             // Having no extensions is a valid use case
             if (value.isUndefined())
                 return true;
             return traverseGLTF(extensionParsers, value.toObject());
         } }
    };
}

Qt3DCore::QEntity *GLTF2Parser::parse(const QByteArray &jsonData, const QString &basePath)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
    if (jsonDocument.isNull() || !jsonDocument.isObject()) {
        qCWarning(kuesa()) << "File is not a valid json document";
        return nullptr;
    }

    Q_ASSERT(m_context);
    m_animators.clear();
    m_treeNodes.clear();
    m_skeletons.clear();
    m_gltfJointIdxToSkeletonJointIdxPerSkeleton.clear();

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
            KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION
#endif
        };
        for (auto e : qAsConst(extensions)) {
            if (supportedExtensions.contains(e))
                continue;
            allRequiredAreSupported = false;
            unsupportedExtensions << e;
        }

        if (!allRequiredAreSupported) {
            qCWarning(kuesa()) << "File contains unsupported extensions: " << unsupportedExtensions;
            return nullptr;
        }
    }

    const QVector<KeyParserFuncPair> topLevelParsers = prepareParsers();
    const bool parsingSucceeded = traverseGLTF(topLevelParsers, rootObject);

    if (!parsingSucceeded)
        return nullptr;

    m_defaultSceneIdx = rootObject.value(KEY_SCENE).toInt(-1);
    if (m_defaultSceneIdx < 0 || m_defaultSceneIdx > m_context->scenesCount()) {
        qCWarning(kuesa()) << "Invalid default scene reference";
        return nullptr;
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

    // Note: we only add resources into the collection after having set an
    // existing parent on the scene root This avoid sending a destroy + created
    // changes because sceneEntity exists and has a backend while the scene root
    // has no backend
    Qt3DCore::QEntity *gltfSceneEntity = scene(m_defaultSceneIdx);

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
                }
            }

            if (m_assignNames) {
                int j = 0;
                for (const TreeNode &treeNode : qAsConst(m_treeNodes)) {
                    if (treeNode.entity != nullptr && treeNode.name.isEmpty()) {
                        addToCollectionWithUniqueName(m_sceneEntity->entities(), QStringLiteral("KuesaEntity_%1").arg(j), treeNode.entity);

                        if (treeNode.cameraIdx >= 0)
                            addToCollectionWithUniqueName(m_sceneEntity->cameras(), QStringLiteral("KuesaCamera_%1").arg(j), qobject_cast<Qt3DRender::QCamera *>(treeNode.entity));
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
    return gltfSceneEntity;
}

void GLTF2Parser::setContext(GLTF2ContextPrivate *ctx)
{
    m_context = ctx;
}

const GLTF2ContextPrivate *GLTF2Parser::context() const
{
    return m_context;
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
        while (leaf) {
            TreeNode &node = m_treeNodes[leaf->nodeIdx];
            const bool entityAlreadyCreated = (node.entity != nullptr);
            if (!entityAlreadyCreated) {
                // If we are dealing with a Camera, we instantiate a QCamera
                // instead of a QEntity
                if (node.cameraIdx >= 0)
                    node.entity = new Qt3DRender::QCamera();
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
            leaf = leaf->parent;
        }
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

void GLTF2Parser::buildJointHierarchy(const HierarchyNode *node, int &jointAccessor, const Skin &skin, unsigned int skinIdx, Qt3DCore::QJoint *parentJoint)
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
    Qt3DCore::QComponent *defaultMaterial = nullptr;
    Qt3DCore::QComponent *defaultSkinnedMaterial = nullptr;
    m_sceneRootEntity = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DCore::QEntity>("QEntity");
    m_sceneRootEntity->setObjectName(QStringLiteral("GLTF2Scene"));

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

            // If the node has a mesh, add it
            const qint32 meshId = node.meshIdx;
            if (meshId >= 0 && meshId < m_context->meshesCount()) {
                const qint32 skinId = node.skinIdx;
                const Mesh &meshData = m_context->mesh(meshId);
                const bool isSkinned = skinId >= 0 && skinId < m_context->skinsCount();
                Qt3DCore::QArmature *armature = nullptr;
                Qt3DCore::QEntity *skinRootJointEntity = nullptr;

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
                            for (auto attr : geometry->attributes()) {
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

                // Generate one Entity per primitive (1 primitive == 1 geometry renderer)
                for (Primitive primitiveData : meshData.meshPrimitives) {
                    Qt3DCore::QEntity *primitiveEntity = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DCore::QEntity>("QEntity");
                    primitiveEntity->addComponent(primitiveData.primitiveRenderer);

                    // Add material for mesh
                    {
                        Qt3DCore::QComponent *material = nullptr;
                        const qint32 materialId = primitiveData.materialIdx;
                        if (materialId >= 0 && materialId < m_context->materialsCount()) {
                            Material &mat = m_context->material(materialId);
                            // Get or create Qt3D for material
                            material = mat.material(isSkinned, primitiveData.hasColorAttr, m_context);
                        } else {
                            // Only create defaultMaterial if we know we need it
                            // otherwise we might leak it
                            if (isSkinned) {
                                if (!defaultSkinnedMaterial) {
                                    MetallicRoughnessMaterial *material = new MetallicRoughnessMaterial;
                                    material->setUseSkinning(true);
                                    defaultSkinnedMaterial = material;
                                }
                                material = defaultSkinnedMaterial;
                            } else {
                                if (!defaultMaterial) {
                                    MetallicRoughnessMaterial *material = new MetallicRoughnessMaterial;
                                    material->setUseSkinning(false);
                                    defaultMaterial = material;
                                }
                                material = defaultMaterial;
                            }
                        }
                        primitiveEntity->addComponent(material);
                    }

                    // Add armature if it is not null
                    if (armature != nullptr) {
                        primitiveEntity->addComponent(armature);
                        // We set the parent to the skeleton root's transform
                        // (in other words, our parent is the entity that
                        // corresponds to the node to which the root joint is
                        // assigned)
                        primitiveEntity->setParent(skinRootJointEntity->parentEntity() ? skinRootJointEntity->parentEntity() : m_sceneRootEntity);
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

        auto *channelMapper = new Qt3DAnimation::QChannelMapper();
        auto *clip = new Qt3DAnimation::QAnimationClip();
        clip->setClipData(animation.clipData);
        m_animators.push_back({ clip, channelMapper });

        for (const auto &skeleton : qAsConst(m_skeletons)) {
            auto skeletonMapping = new Qt3DAnimation::QSkeletonMapping;
            skeletonMapping->setSkeleton(skeleton);
            channelMapper->addMapping(skeletonMapping);
        }

        for (const ChannelMapping &mapping : animation.mappings) {
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

            // Map channel to entity transform
            if (targetNode.entity != nullptr) {
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
            }
        }
    }
}

Qt3DCore::QEntity *GLTF2Parser::scene(const int id)
{
    if (id < 0 || id > m_context->scenesCount())
        return nullptr;

    const Scene scene = m_context->scene(id);

    // Get scene node
    const QVector<int> toRetrieveNodes = scene.rootNodeIndices;

    for (auto i = 0, m = toRetrieveNodes.size(); i < m; ++i) {
        const TreeNode node = m_treeNodes[toRetrieveNodes.at(i)];
        if (node.entity != nullptr)
            node.entity->setParent(m_sceneRootEntity);
        for (auto &joint : node.joints)
            if (joint != nullptr) // For ownership only (doesn't affect hierarchy)
                joint->setParent(m_sceneRootEntity);
    }
    return m_sceneRootEntity;
}

QT_END_NAMESPACE
