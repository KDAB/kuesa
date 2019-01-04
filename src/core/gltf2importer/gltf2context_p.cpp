/*
    gltf2context_p.cpp

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

#include "gltf2context.h"
#include "gltf2context_p.h"
#include "kuesa_p.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

GLTF2ContextPrivate::GLTF2ContextPrivate()
{
}

GLTF2ContextPrivate::~GLTF2ContextPrivate()
{
}

GLTF2ContextPrivate *GLTF2ContextPrivate::get(GLTF2Context* ctx)
{
  Q_ASSERT_X(ctx, "GLTF2ContextPrivate::get", "Requires a valid GLTF2Context");
  return ctx->m_impl;
}

int GLTF2ContextPrivate::bufferCount() const
{
    return m_buffers.size();
}

const QByteArray GLTF2ContextPrivate::buffer(int id) const
{
    if (id >= 0 && id < m_buffers.size())
        return m_buffers.at(id);

    qCWarning(kuesa, "Invalid buffer id");
    return QByteArray();
}

void GLTF2ContextPrivate::addBuffer(const QByteArray &buffer)
{
    m_buffers.push_back(buffer);
}

int GLTF2ContextPrivate::bufferViewCount() const
{
    return m_bufferViews.size();
}

const BufferView GLTF2ContextPrivate::bufferView(int id) const
{
    if (id >= 0 && id < m_bufferViews.size())
        return m_bufferViews.at(id);

    qCWarning(kuesa, "Invalid buffer view id");
    return BufferView();
}

void GLTF2ContextPrivate::addBufferView(const BufferView &bufferView)
{
    m_bufferViews.push_back(bufferView);
}

int GLTF2ContextPrivate::cameraCount() const
{
    return m_cameras.size();
}

const Camera GLTF2ContextPrivate::camera(int id) const
{
    if (id >= 0 && id < m_cameras.size())
        return m_cameras.at(id);

    qCWarning(kuesa, "Invalid camera id");
    return {};
}

void GLTF2ContextPrivate::addCamera(const Camera &camera)
{
    m_cameras.push_back(camera);
}

int GLTF2ContextPrivate::treeNodeCount() const
{
    return m_treeNodes.size();
}

const TreeNode GLTF2ContextPrivate::treeNode(int id) const
{
    if (id >= 0 && id < m_treeNodes.size())
        return m_treeNodes.at(id);

    qCWarning(kuesa, "Invalid tree node id");
    return {};
}

void GLTF2ContextPrivate::addTreeNode(const TreeNode &treeNode)
{
    m_treeNodes.push_back(treeNode);
}

int GLTF2ContextPrivate::imagesCount() const
{
    return m_images.size();
}

void GLTF2ContextPrivate::addImage(const Image &image)
{
    m_images.push_back(image);
}

const Image GLTF2ContextPrivate::image(int id) const
{
    if (id >= 0 && id < m_images.size())
        return m_images.at(id);

    qCWarning(kuesa, "Invalid image id");
    return Image();
}

int GLTF2ContextPrivate::textureSamplersCount() const
{
    return m_textureSamplers.size();
}

void GLTF2ContextPrivate::addTextureSampler(const TextureSampler &textureSampler)
{
    m_textureSamplers.push_back(textureSampler);
}

const TextureSampler GLTF2ContextPrivate::textureSampler(int id) const
{
    if (id >= 0 && id < m_textureSamplers.size())
        return m_textureSamplers.at(id);

    qCWarning(kuesa, "Invalid texture sampler id");
    return TextureSampler();
}

int GLTF2ContextPrivate::texturesCount() const
{
    return m_textures.size();
}

void GLTF2ContextPrivate::addTexture(const Texture &texture)
{
    m_textures.push_back(texture);
}

const Texture GLTF2ContextPrivate::texture(int id) const
{
    if (id >= 0 && id < m_textures.size())
        return m_textures.at(id);

    qCWarning(kuesa, "Invalid texture id");
    return Texture();
}

int GLTF2ContextPrivate::animationsCount() const
{
    return m_animations.size();
}

void GLTF2ContextPrivate::addAnimation(const Animation &animation)
{
    m_animations.push_back(animation);
}

const Animation GLTF2ContextPrivate::animation(int id) const
{
    if (id >= 0 && id < m_animations.size())
        return m_animations.at(id);

    qCWarning(kuesa, "Invalid animation id");
    return Animation();
}

int GLTF2ContextPrivate::materialsCount() const
{
    return m_materials.size();
}

void GLTF2ContextPrivate::addMaterial(const Material &material)
{
    m_materials.push_back(material);
}

const Material GLTF2ContextPrivate::material(int id) const
{

    if (id >= 0 && id < m_materials.size())
        return m_materials.at(id);

    qCWarning(kuesa, "Invalid material id");
    return Material();
}

Material &GLTF2ContextPrivate::material(int id)
{
    return m_materials[id];
}

int GLTF2ContextPrivate::accessorCount() const
{
    return m_accessors.size();
}

const Accessor GLTF2ContextPrivate::accessor(int id) const
{
    if (id >= 0 && id < m_accessors.size())
        return m_accessors.at(id);

    qCWarning(kuesa, "Invalid accessor id");
    return Accessor();
}

void GLTF2ContextPrivate::addAccessor(const Accessor &accessor)
{
    m_accessors.push_back(accessor);
}

int GLTF2ContextPrivate::meshesCount() const
{
    return m_meshes.size();
}

void GLTF2ContextPrivate::addMesh(const Mesh &mesh)
{
    m_meshes.push_back(mesh);
}

const Mesh GLTF2ContextPrivate::mesh(int id) const
{
    if (id >= 0 && id < m_meshes.size())
        return m_meshes.at(id);

    qCWarning(kuesa, "Invalid mesh id");
    return Mesh();
}

int GLTF2ContextPrivate::layersCount() const
{
    return m_layers.size();
}

/*!
 * \internal
 * We assume the GLTF2ContextPrivate is only used a short amount of time during the
 * loading of a scene and destroyed immediately afterward. There is therefore
 * no risk of a QLayer being destroyed while still being referenced by the
 * GLTF2ContextPrivate.
 */
void GLTF2ContextPrivate::addLayer(const Layer &layer)
{
    m_layers.push_back(layer);
}

Layer GLTF2ContextPrivate::layer(int id) const
{
    if (id >= 0 && id < m_layers.size())
        return m_layers.at(id);

    qCWarning(kuesa, "Invalid layer id");
    return {};
}

int GLTF2ContextPrivate::scenesCount() const
{
    return m_scenes.size();
}

void GLTF2ContextPrivate::addScene(const Scene &scene)
{
    m_scenes.push_back(scene);
}

const Scene GLTF2ContextPrivate::scene(int id) const
{
    if (id >= 0 && id < m_scenes.size())
        return m_scenes.at(id);

    qCWarning(kuesa, "Invalid scene id");
    return {};
}

int GLTF2ContextPrivate::skinsCount() const
{
    return m_skins.size();
}

void GLTF2ContextPrivate::addSkin(const Skin &skin)
{
    m_skins.push_back(skin);
}

const Skin GLTF2ContextPrivate::skin(int id) const
{
    if (id >= 0 && id < m_skins.size())
        return m_skins.at(id);

    qCWarning(kuesa, "Invalid skin id");
    return Skin();
}

QStringList GLTF2ContextPrivate::usedExtension() const
{
    return m_usedExtensions;
}

void GLTF2ContextPrivate::setUsedExtensions(const QStringList &usedExtensions)
{
    m_usedExtensions = usedExtensions;
}

QStringList GLTF2ContextPrivate::requiredExtensions() const
{
    return m_requiredExtensions;
}

void GLTF2ContextPrivate::setRequiredExtensions(const QStringList &requiredExtensions)
{
    m_requiredExtensions = requiredExtensions;
}

template<>
int GLTF2ContextPrivate::count<Mesh>() const
{
    return m_meshes.count();
}

template<>
Mesh GLTF2ContextPrivate::assetAt<Mesh>(int i) const
{
    return mesh(i);
}

template<>
int GLTF2ContextPrivate::count<Layer>() const
{
    return m_layers.count();
}

template<>
Layer GLTF2ContextPrivate::assetAt<Layer>(int i) const
{
    return layer(i);
}

template<>
int GLTF2ContextPrivate::count<TreeNode>() const
{
    return m_treeNodes.count();
}

template<>
TreeNode GLTF2ContextPrivate::assetAt<TreeNode>(int i) const
{
    return treeNode(i);
}

template<>
int GLTF2ContextPrivate::count<Texture>() const
{
    return m_textures.count();
}

template<>
Texture GLTF2ContextPrivate::assetAt<Texture>(int i) const
{
    return texture(i);
}

template<>
int GLTF2ContextPrivate::count<Animation>() const
{
    return m_animations.count();
}

template<>
Animation GLTF2ContextPrivate::assetAt<Animation>(int i) const
{
    return animation(i);
}

template<>
int GLTF2ContextPrivate::count<Material>() const
{
    return m_materials.count();
}

template<>
Material GLTF2ContextPrivate::assetAt<Material>(int i) const
{
    return material(i);
}

template<>
int GLTF2ContextPrivate::count<Skin>() const
{
    return m_skins.count();
}

template<>
Skin GLTF2ContextPrivate::assetAt<Skin>(int i) const
{
    return skin(i);
}

QT_END_NAMESPACE
