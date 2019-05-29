/*
    gltf2context_p.cpp

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

#include "gltf2context_p.h"
#include "kuesa_p.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

GLTF2Context::GLTF2Context()
    : m_options(GLTF2OptionsPtr::create())
{
}

GLTF2Context::~GLTF2Context()
{
}

int GLTF2Context::bufferCount() const
{
    return m_buffers.size();
}

const QByteArray GLTF2Context::buffer(qint32 id) const
{
    if (id >= 0 && id < m_buffers.size())
        return m_buffers.at(id);

    qCWarning(kuesa, "Invalid buffer id");
    return QByteArray();
}

void GLTF2Context::addBuffer(const QByteArray &buffer)
{
    m_buffers.push_back(buffer);
}

int GLTF2Context::bufferViewCount() const
{
    return m_bufferViews.size();
}

const BufferView GLTF2Context::bufferView(qint32 id) const
{
    if (id >= 0 && id < m_bufferViews.size())
        return m_bufferViews.at(id);

    qCWarning(kuesa, "Invalid buffer view id");
    return BufferView();
}

void GLTF2Context::addBufferView(const BufferView &bufferView)
{
    m_bufferViews.push_back(bufferView);
}

int GLTF2Context::cameraCount() const
{
    return m_cameras.size();
}

const Camera GLTF2Context::camera(qint32 id) const
{
    if (id >= 0 && id < m_cameras.size())
        return m_cameras.at(id);

    qCWarning(kuesa, "Invalid camera id");
    return {};
}

void GLTF2Context::addCamera(const Camera &camera)
{
    m_cameras.push_back(camera);
}

int GLTF2Context::treeNodeCount() const
{
    return m_treeNodes.size();
}

const TreeNode GLTF2Context::treeNode(qint32 id) const
{
    if (id >= 0 && id < m_treeNodes.size())
        return m_treeNodes.at(id);

    qCWarning(kuesa, "Invalid tree node id");
    return {};
}

void GLTF2Context::addTreeNode(const TreeNode &treeNode)
{
    m_treeNodes.push_back(treeNode);
}

int GLTF2Context::imagesCount() const
{
    return m_images.size();
}

void GLTF2Context::addImage(const Image &image)
{
    m_images.push_back(image);
}

const Image GLTF2Context::image(qint32 id) const
{
    if (id >= 0 && id < m_images.size())
        return m_images.at(id);

    qCWarning(kuesa, "Invalid image id");
    return Image();
}

int GLTF2Context::textureSamplersCount() const
{
    return m_textureSamplers.size();
}

void GLTF2Context::addTextureSampler(const TextureSampler &textureSampler)
{
    m_textureSamplers.push_back(textureSampler);
}

const TextureSampler GLTF2Context::textureSampler(qint32 id) const
{
    if (id >= 0 && id < m_textureSamplers.size())
        return m_textureSamplers.at(id);

    qCWarning(kuesa, "Invalid texture sampler id");
    return TextureSampler();
}

int GLTF2Context::texturesCount() const
{
    return m_textures.size();
}

void GLTF2Context::addTexture(const Texture &texture)
{
    m_textures.push_back(texture);
}

const Texture GLTF2Context::texture(qint32 id) const
{
    if (id >= 0 && id < m_textures.size())
        return m_textures.at(id);

    qCWarning(kuesa, "Invalid texture id");
    return Texture();
}

int GLTF2Context::animationsCount() const
{
    return m_animations.size();
}

void GLTF2Context::addAnimation(const Animation &animation)
{
    m_animations.push_back(animation);
}

const Animation GLTF2Context::animation(qint32 id) const
{
    if (id >= 0 && id < m_animations.size())
        return m_animations.at(id);

    qCWarning(kuesa, "Invalid animation id");
    return Animation();
}

int GLTF2Context::materialsCount() const
{
    return m_materials.size();
}

void GLTF2Context::addMaterial(const Material &material)
{
    m_materials.push_back(material);
}

const Material GLTF2Context::material(qint32 id) const
{

    if (id >= 0 && id < m_materials.size())
        return m_materials.at(id);

    qCWarning(kuesa, "Invalid material id");
    return Material();
}

Material &GLTF2Context::material(qint32 id)
{
    return m_materials[id];
}

int GLTF2Context::accessorCount() const
{
    return m_accessors.size();
}

const Accessor GLTF2Context::accessor(qint32 id) const
{
    if (id >= 0 && id < m_accessors.size())
        return m_accessors.at(id);

    qCWarning(kuesa, "Invalid accessor id");
    return Accessor();
}

void GLTF2Context::addAccessor(const Accessor &accessor)
{
    m_accessors.push_back(accessor);
}

int GLTF2Context::meshesCount() const
{
    return m_meshes.size();
}

void GLTF2Context::addMesh(const Mesh &mesh)
{
    m_meshes.push_back(mesh);
}

const Mesh GLTF2Context::mesh(qint32 id) const
{
    if (id >= 0 && id < m_meshes.size())
        return m_meshes.at(id);

    qCWarning(kuesa, "Invalid mesh id");
    return Mesh();
}

int GLTF2Context::layersCount() const
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
    if (id >= 0 && id < m_layers.size())
        return m_layers.at(id);

    qCWarning(kuesa, "Invalid layer id");
    return {};
}

int GLTF2Context::scenesCount() const
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

    qCWarning(kuesa, "Invalid scene id");
    return {};
}

int GLTF2Context::skinsCount() const
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

    qCWarning(kuesa, "Invalid skin id");
    return Skin();
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
int GLTF2Context::count<Mesh>() const
{
    return m_meshes.count();
}

template<>
Mesh GLTF2Context::assetAt<Mesh>(qint32 i) const
{
    return mesh(i);
}

template<>
int GLTF2Context::count<Layer>() const
{
    return m_layers.count();
}

template<>
Layer GLTF2Context::assetAt<Layer>(qint32 i) const
{
    return layer(i);
}

template<>
int GLTF2Context::count<TreeNode>() const
{
    return m_treeNodes.count();
}

template<>
TreeNode GLTF2Context::assetAt<TreeNode>(qint32 i) const
{
    return treeNode(i);
}

template<>
int GLTF2Context::count<Texture>() const
{
    return m_textures.count();
}

template<>
Texture GLTF2Context::assetAt<Texture>(qint32 i) const
{
    return texture(i);
}

template<>
int GLTF2Context::count<Animation>() const
{
    return m_animations.count();
}

template<>
Animation GLTF2Context::assetAt<Animation>(qint32 i) const
{
    return animation(i);
}

template<>
int GLTF2Context::count<Material>() const
{
    return m_materials.count();
}

template<>
Material GLTF2Context::assetAt<Material>(qint32 i) const
{
    return material(i);
}

template<>
int GLTF2Context::count<Skin>() const
{
    return m_skins.count();
}

template<>
Skin GLTF2Context::assetAt<Skin>(qint32 i) const
{
    return skin(i);
}



GLTF2OptionsPtr GLTF2Context::options() const
{
    return m_options;
}

void GLTF2Context::setOptions(GLTF2OptionsPtr options)
{
    m_options = options;
}

QT_END_NAMESPACE
