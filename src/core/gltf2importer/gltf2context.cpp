/*
    gltf2context.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "gltf2context_p.h"
#include "kuesa_p.h"
#include "gltf2importer.h"
#include "effectslibrary_p.h"

#include "metallicroughnessproperties.h"
#include "unlitproperties.h"
#include "embeddedtextureimage_p.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

GLTF2Context::GLTF2Context()
    : m_options()
    , m_defaultScene(-1)
    , m_effectLibrary(new EffectsLibrary())
    , m_primitiveBuilder(new PrimitiveBuilder(this))
{
}

GLTF2Context::~GLTF2Context()
{
}

size_t GLTF2Context::bufferCount() const
{
    return m_buffers.size();
}

const QByteArray GLTF2Context::buffer(qint32 id) const
{
    if (id >= 0 && id < qint32(m_buffers.size()))
        return m_buffers.at(id);

    qCWarning(Kuesa::kuesa, "Invalid buffer id");
    return QByteArray();
}

void GLTF2Context::addBuffer(const QByteArray &buffer)
{
    m_buffers.push_back(buffer);
}

size_t GLTF2Context::bufferViewCount() const
{
    return m_bufferViews.size();
}

const BufferView GLTF2Context::bufferView(qint32 id) const
{
    if (id >= 0 && id < qint32(m_bufferViews.size()))
        return m_bufferViews.at(id);

    qCWarning(Kuesa::kuesa, "Invalid buffer view id");
    return BufferView();
}

void GLTF2Context::addBufferView(const BufferView &bufferView)
{
    m_bufferViews.push_back(bufferView);
}

BufferView &GLTF2Context::bufferView(qint32 id)
{
    return m_bufferViews[id];
}

Qt3DGeometry::QBuffer *GLTF2Context::getOrAllocateBuffer(BufferView &view)
{
    Qt3DGeometry::QBuffer *buffer = m_sharedBufferViews.getResourceFromCache(view);
    if (buffer) {
        qCDebug(Kuesa::kuesa) << "Reusing cached buffer";
        return buffer;
    }
    buffer = new Qt3DGeometry::QBuffer();
    buffer->setData(view.bufferData);
    m_sharedBufferViews.addResourceToCache(view, buffer);
    return buffer;
}

size_t GLTF2Context::cameraCount() const
{
    return m_cameras.size();
}

const Camera GLTF2Context::camera(qint32 id) const
{
    if (id >= 0 && id < qint32(m_cameras.size()))
        return m_cameras.at(id);

    qCWarning(Kuesa::kuesa, "Invalid camera id");
    return {};
}

void GLTF2Context::addCamera(const Camera &camera)
{
    m_cameras.push_back(camera);
}

Camera &GLTF2Context::camera(qint32 id)
{
    return m_cameras[id];
}

size_t GLTF2Context::treeNodeCount() const
{
    return m_treeNodes.size();
}

const TreeNode GLTF2Context::treeNode(qint32 id) const
{
    if (id >= 0 && id < qint32(m_treeNodes.size()))
        return m_treeNodes.at(id);

    qCWarning(Kuesa::kuesa, "Invalid tree node id");
    return {};
}

TreeNode &GLTF2Context::treeNode(qint32 id)
{
    Q_ASSERT(id >= 0 && id < qint32(m_treeNodes.size()));
    return m_treeNodes[id];
}

const std::vector<TreeNode> &GLTF2Context::treeNodes() const
{
    return m_treeNodes;
}

void GLTF2Context::addTreeNode(const TreeNode &treeNode)
{
    m_treeNodes.push_back(treeNode);
}

size_t GLTF2Context::imagesCount() const
{
    return m_images.size();
}

void GLTF2Context::addImage(const Image &image)
{
    m_images.push_back(image);
}

const Image GLTF2Context::image(qint32 id) const
{
    if (id >= 0 && id < qint32(m_images.size()))
        return m_images.at(id);

    qCWarning(Kuesa::kuesa, "Invalid image id");
    return Image();
}

size_t GLTF2Context::textureSamplersCount() const
{
    return m_textureSamplers.size();
}

void GLTF2Context::addTextureSampler(const TextureSampler &textureSampler)
{
    m_textureSamplers.push_back(textureSampler);
}

const TextureSampler GLTF2Context::textureSampler(qint32 id) const
{
    if (id >= 0 && id < qint32(m_textureSamplers.size()))
        return m_textureSamplers.at(id);

    qCWarning(Kuesa::kuesa, "Invalid texture sampler id");
    return TextureSampler();
}

size_t GLTF2Context::texturesCount() const
{
    return m_textures.size();
}

void GLTF2Context::addTexture(const Texture &texture)
{
    m_textures.push_back(texture);
}

const Texture GLTF2Context::texture(qint32 id) const
{
    if (id >= 0 && id < qint32(m_textures.size()))
        return m_textures.at(id);

    qCWarning(Kuesa::kuesa, "Invalid texture id");
    return Texture();
}

size_t GLTF2Context::animationsCount() const
{
    return m_animations.size();
}

void GLTF2Context::addAnimation(const Animation &animation)
{
    m_animations.push_back(animation);
}

const Animation GLTF2Context::animation(qint32 id) const
{
    if (id >= 0 && id < qint32(m_animations.size()))
        return m_animations.at(id);

    qCWarning(Kuesa::kuesa, "Invalid animation id");
    return Animation();
}

Animation &GLTF2Context::animation(qint32 id)
{
    Q_ASSERT(id >= 0 && id < qint32(m_animations.size()));
    return m_animations[id];
}

size_t GLTF2Context::materialsCount() const
{
    return m_materials.size();
}

void GLTF2Context::addMaterial(const Material &material)
{
    m_materials.push_back(material);
}

const Material GLTF2Context::material(qint32 id) const
{

    if (id >= 0 && id < qint32(m_materials.size()))
        return m_materials.at(id);

    qCWarning(Kuesa::kuesa, "Invalid material id");
    return Material();
}

Material &GLTF2Context::material(qint32 id)
{
    return m_materials[id];
}

size_t GLTF2Context::accessorCount() const
{
    return m_accessors.size();
}

const Accessor GLTF2Context::accessor(qint32 id) const
{
    if (id >= 0 && id < qint32(m_accessors.size()))
        return m_accessors.at(id);

    qCWarning(Kuesa::kuesa, "Invalid accessor id");
    return Accessor();
}

void GLTF2Context::addAccessor(const Accessor &accessor)
{
    m_accessors.push_back(accessor);
}

size_t GLTF2Context::meshesCount() const
{
    return m_meshes.size();
}

void GLTF2Context::addMesh(const Mesh &mesh)
{
    m_meshes.push_back(mesh);
}

const Mesh GLTF2Context::mesh(qint32 id) const
{
    if (id >= 0 && id < qint32(m_meshes.size()))
        return m_meshes.at(id);

    qCWarning(Kuesa::kuesa, "Invalid mesh id");
    return Mesh();
}

Mesh &GLTF2Context::mesh(qint32 id)
{
    return m_meshes[id];
}

Qt3DRender::QGeometryRenderer *GLTF2Context::getOrAllocateGeometryRenderer(Primitive &primitive)
{
    if (primitive.primitiveRenderer)
        return primitive.primitiveRenderer;

    Qt3DRender::QGeometryRenderer *renderer = m_sharedPrimitives.getResourceFromCache(primitive);
    // Use Resource from Cache
    if (renderer) {
        primitive.primitiveRenderer = renderer;
        qCDebug(Kuesa::kuesa) << "Reusing cached geometry renderer";
        return primitive.primitiveRenderer;
    }

    // Create the geometry renderer and store it in the cache if successful
    if (m_primitiveBuilder->generateGeometryRendererForPrimitive(primitive)) {
        m_sharedPrimitives.addResourceToCache(primitive, primitive.primitiveRenderer);
        return primitive.primitiveRenderer;
    }

    return nullptr;
}

size_t GLTF2Context::layersCount() const
{
    return m_layers.size();
}

/*!
 * \internal
 * We assume the GLTF2Context is only used a short amount of time during the
 * loading of a scene and destroyed immediately afterward. There is therefore
 * no risk of a QLayer being destroyed while still being referenced by the
 * GLTF2Context.
 */
void GLTF2Context::addLayer(const Layer &layer)
{
    m_layers.push_back(layer);
}

Layer GLTF2Context::layer(qint32 id) const
{
    if (id >= 0 && id < qint32(m_layers.size()))
        return m_layers.at(id);

    qCWarning(Kuesa::kuesa, "Invalid layer id");
    return {};
}

Layer &GLTF2Context::layer(qint32 id)
{
    return m_layers[id];
}

size_t GLTF2Context::scenesCount() const
{
    return m_scenes.size();
}

void GLTF2Context::addScene(const Scene &scene)
{
    m_scenes.push_back(scene);
}

const Scene GLTF2Context::scene(qint32 id) const
{
    if (id >= 0 && id < m_scenes.size())
        return m_scenes.at(id);

    qCWarning(Kuesa::kuesa, "Invalid scene id");
    return {};
}

void GLTF2Context::setDefaultScene(qint32 id)
{
    m_defaultScene = id;
}

qint32 GLTF2Context::defaultScene() const
{
    return m_defaultScene;
}

size_t GLTF2Context::skinsCount() const
{
    return m_skins.size();
}

void GLTF2Context::addSkin(const Skin &skin)
{
    m_skins.push_back(skin);
}

const Skin GLTF2Context::skin(qint32 id) const
{
    if (id >= 0 && id < m_skins.size())
        return m_skins.at(id);

    qCWarning(Kuesa::kuesa, "Invalid skin id");
    return Skin();
}

Skin &GLTF2Context::skin(qint32 id)
{
    Q_ASSERT(id >= 0 && id < qint32(m_skins.size()));
    return m_skins[id];
}

size_t GLTF2Context::lightCount() const
{
    return m_lights.size();
}

void GLTF2Context::addLight(const Light &light)
{
    m_lights.push_back(light);
}

const Light GLTF2Context::light(qint32 id) const
{
    if (id >= 0 && id < qint32(m_lights.size()))
        return m_lights.at(id);

    qCWarning(Kuesa::kuesa, "Invalid light id");
    return Light();
}

Light &GLTF2Context::light(qint32 id)
{
    Q_ASSERT(id >= 0 && id < qint32(m_lights.size()));
    return m_lights[id];
}

QStringList GLTF2Context::usedExtension() const
{
    return m_usedExtensions;
}

void GLTF2Context::setUsedExtensions(const QStringList &usedExtensions)
{
    m_usedExtensions = usedExtensions;
}

QStringList GLTF2Context::requiredExtensions() const
{
    return m_requiredExtensions;
}

void GLTF2Context::setRequiredExtensions(const QStringList &requiredExtensions)
{
    m_requiredExtensions = requiredExtensions;
}

const QString &GLTF2Context::filename() const
{
    return m_filename;
}

void GLTF2Context::setFilename(const QString &name)
{
    m_filename = name;
}

const QString &GLTF2Context::basePath() const
{
    return m_basePath;
}

void GLTF2Context::setBasePath(const QString &path)
{
    m_basePath = path;
}

const QJsonDocument &GLTF2Context::json() const
{
    return m_json;
}

void GLTF2Context::setJson(const QJsonDocument &doc)
{
    m_json = doc;
}

const QStringList &GLTF2Context::localFiles() const
{
    return m_localFiles;
}

void GLTF2Context::addLocalFile(const QString &file)
{
    m_localFiles.push_back(file);
}

QByteArray GLTF2Context::bufferChunk() const
{
    return m_bufferChunk;
}

void GLTF2Context::setBufferChunk(const QByteArray &bufferChunk)
{
    m_bufferChunk = bufferChunk;
}

template<>
size_t GLTF2Context::count<Mesh>() const
{
    return m_meshes.size();
}

template<>
Mesh GLTF2Context::assetAt<Mesh>(qint32 i) const
{
    return mesh(i);
}

template<>
size_t GLTF2Context::count<Layer>() const
{
    return m_layers.size();
}

template<>
Layer GLTF2Context::assetAt<Layer>(qint32 i) const
{
    return layer(i);
}

template<>
size_t GLTF2Context::count<TreeNode>() const
{
    return m_treeNodes.size();
}

template<>
TreeNode GLTF2Context::assetAt<TreeNode>(qint32 i) const
{
    return treeNode(i);
}

template<>
size_t GLTF2Context::count<Texture>() const
{
    return m_textures.size();
}

template<>
Texture GLTF2Context::assetAt<Texture>(qint32 i) const
{
    return texture(i);
}

template<>
size_t GLTF2Context::count<Animation>() const
{
    return m_animations.size();
}

template<>
Animation GLTF2Context::assetAt<Animation>(qint32 i) const
{
    return animation(i);
}

template<>
size_t GLTF2Context::count<Material>() const
{
    return m_materials.size();
}

template<>
Material GLTF2Context::assetAt<Material>(qint32 i) const
{
    return material(i);
}

template<>
size_t GLTF2Context::count<Skin>() const
{
    return m_skins.size();
}

template<>
Skin GLTF2Context::assetAt<Skin>(qint32 i) const
{
    return skin(i);
}

Kuesa::GLTF2Import::GLTF2Options *GLTF2Context::options()
{
    return &m_options;
}

const Kuesa::GLTF2Import::GLTF2Options *GLTF2Context::options() const
{
    return &m_options;
}

EffectsLibrary *GLTF2Context::effectLibrary() const
{
    return m_effectLibrary.data();
}

QVector<Qt3DCore::QEntity *> GLTF2Context::primitiveEntitiesForEntity(Qt3DCore::QEntity *e) const
{
    return m_treeNodeIdToPrimitiveEntities.value(e);
}

void GLTF2Context::addPrimitiveEntityToEntity(Qt3DCore::QEntity *e, Qt3DCore::QEntity *primitive)
{
    auto mapVectorIt = m_treeNodeIdToPrimitiveEntities.find(e);
    if (mapVectorIt == m_treeNodeIdToPrimitiveEntities.end())
        mapVectorIt = m_treeNodeIdToPrimitiveEntities.insert(e, {});
    mapVectorIt->push_back(primitive);
}

void GLTF2Context::reset(SceneEntity *sceneEntity)
{
    // Resets everything but the options;
    m_sharedImages.setSceneEntity(sceneEntity); // so that they don't get deleted with the scene graph
    m_sharedTextures.setSceneEntity(sceneEntity);
    m_sharedPrimitives.setSceneEntity(sceneEntity);
    m_sharedBufferViews.setSceneEntity(sceneEntity);
    m_effectLibrary->reset();

    // Reset Primitive Builder
    m_primitiveBuilder.reset(new PrimitiveBuilder(this));

    m_accessors.clear();
    m_buffers.clear();
    m_bufferViews.clear();
    m_cameras.clear();
    m_meshes.clear();
    m_treeNodes.clear();
    m_layers.clear();
    m_lights.clear();
    m_images.clear();
    m_textureSamplers.clear();
    m_textures.clear();
    m_animations.clear();
    m_scenes.clear();
    m_materials.clear();
    m_skins.clear();
    m_usedExtensions.clear();
    m_requiredExtensions.clear();
    m_filename.clear();
    m_json = {};
    m_localFiles.clear();
    m_bufferChunk.clear();
    m_treeNodes.clear();
    m_treeNodeIdToPrimitiveEntities.clear();
}

GLTF2Context *GLTF2Context::fromImporter(GLTF2Importer *importer)
{
    return importer->m_context;
}

Qt3DRender::QAbstractTexture *GLTF2Context::getOrAllocateTexture(qint32 id)
{
    if (id < 0 || id >= qint32(m_textures.size()))
        return nullptr;
    return getOrAllocateTexture(m_textures[id]);
}

Qt3DRender::QAbstractTexture *GLTF2Context::getOrAllocateTexture(Texture &texture)
{
    if (texture.texture)
        return texture.texture;

    const auto image = this->image(texture.sourceImage);
    if (image.url.isEmpty() && image.data.isEmpty())
        return nullptr;

    auto texture2d = std::unique_ptr<Qt3DRender::QAbstractTexture>(m_sharedTextures.getResourceFromCache(texture));

    // Use cached resource if it existed
    if (texture2d) {
        qCDebug(Kuesa::kuesa) << "Reusing cached texture";
        texture.texture = texture2d.release();
        return texture.texture;
    }

    if (texture.isDDSTexture) {
        if (image.data.isEmpty()) {
            auto textureLoader = new Qt3DRender::QTextureLoader;
            texture2d.reset(textureLoader);
            textureLoader->setSource(image.url);
        }
    } else {
        texture2d.reset(new Qt3DRender::QTexture2D);
        auto *textureImage = m_sharedImages.getResourceFromCache(image);

        if (textureImage)
            qCDebug(Kuesa::kuesa) << "Reusing cached texture image";

        if (textureImage == nullptr) {
            if (image.data.isEmpty()) {
                auto ti = new Qt3DRender::QTextureImage();
                ti->setSource(image.url);
                ti->setMirrored(false);
                textureImage = ti;
            } else {
                QImage qimage;
                if (!qimage.loadFromData(image.data)) {
                    qCWarning(Kuesa::kuesa) << "Failed to decode image" << texture.sourceImage << "from buffer";
                    return nullptr;
                }
                textureImage = new EmbeddedTextureImage(qimage);
            }
            m_sharedImages.addResourceToCache(image, textureImage);
        }

        // Add Image to Texture if compatible
        if (TextureParser::ensureImageIsCompatibleWithTexture(textureImage, texture2d.get()))
            texture2d->addTextureImage(textureImage);
        else
            qCWarning(Kuesa::kuesa) << "Image with source" << image.url << "is incompatbile with texture" << texture2d->objectName();
    }

    texture2d->setObjectName(texture.name);

    if (texture.sampler == -1) {
        // Repeat wrappring and auto filtering should be used
        texture2d->setWrapMode(Qt3DRender::QTextureWrapMode(Qt3DRender::QTextureWrapMode::Repeat));
        texture2d->setGenerateMipMaps(true);
        texture2d->setMinificationFilter(Qt3DRender::QAbstractTexture::LinearMipMapLinear);
        texture2d->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    } else {
        const auto sampler = textureSampler(texture.sampler);
        if (!sampler.textureWrapMode)
            return nullptr; // We could use default wrapping, but the file is wrong, so we reject it

        texture2d->setWrapMode(*sampler.textureWrapMode.get());
        texture2d->setMinificationFilter(sampler.minificationFilter);
        texture2d->setMagnificationFilter(sampler.magnificationFilter);
    }

    texture.texture = texture2d.release();

    m_sharedTextures.addResourceToCache(texture, texture.texture);
    return texture.texture;
}

GLTF2MaterialProperties *GLTF2Context::getOrAllocateMaterial(qint32 id)
{
    if (id < 0 || id >= qint32(m_materials.size()))
        return nullptr;
    return getOrAllocateMaterial(m_materials[id]);
}

GLTF2MaterialProperties *GLTF2Context::getOrAllocateMaterial(Material &material)
{
    material.getOrAllocateProperties(*this);
    return material.m_materialProperties;
}

QT_END_NAMESPACE
