/*
    sceneentity.cpp

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

#include "sceneentity.h"
#include "kuesa_utils_p.h"

#include <Qt3DCore/QTransform>

QT_BEGIN_NAMESPACE

static void initResources()
{
#ifdef QT_STATIC
    Q_INIT_RESOURCE(shaders);
#endif
}

using namespace Kuesa;

/*!
 * \class Kuesa::SceneEntity
 * \inheaderfile Kuesa/SceneEntity
 * \inmodule Kuesa
 * \since 1.0
 * \brief Root entity for Kuesa runtime
 *
 * SceneEntity provides support for the runtime behaviour of various
 * classes in Kuesa. It's main purpose is to create instances of
 * various asset collections which will be populated if the
 * scene entity is registered with the glTF importer.
 *
 * The AnimationPlayer classes will also use the SceneEntity
 * to access the collections and find assets.
 *
 * \sa AnimationPlayer, GLTF2Importer
*/

/*!
 * \qmltype SceneEntity
 * \instantiates Kuesa::SceneEntity
 * \inmodule Kuesa
 * \since 1.0
 * \brief Root entity for Kuesa runtime
 *
 * SceneEntity provides support for the runtime behaviour of various
 * classes in Kuesa. It's main purpose is to create instances of
 * various asset collections which will be populated if the
 * scene entity is registered with the glTF importer.
 *
 * The Asset and AnimationPlayer classes will also use the SceneEntity
 * to access the collections and find assets.
 *
 * \sa AnimationPlayer, Asset, GLTF2Importer
*/

/*!
  \fn SceneEntity::loadingDone()

  This signal is emitted when a glTF file is successfully loaded
  using a GLTFImporter instance which sceneEntity points to this
  entity.
*/

/*!
  \qmlsignal SceneEntity::loadingDone()

  This signal is emitted when a glTF file is successfully loaded
  using a GLTFImporter instance which sceneEntity points to this
  entity.
*/

// TODO document properties
SceneEntity::SceneEntity(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_clips(new AnimationClipCollection(this))
    , m_armatures(new ArmatureCollection(this))
    , m_effects(new EffectCollection(this))
    , m_layers(new LayerCollection(this))
    , m_materials(new MaterialCollection(this))
    , m_meshes(new MeshCollection(this))
    , m_skeletons(new SkeletonCollection(this))
    , m_textures(new TextureCollection(this))
    , m_cameras(new CameraCollection(this))
    , m_entities(new EntityCollection(this))
    , m_textureImages(new TextureImageCollection(this))
    , m_animationMappings(new AnimationMappingCollection(this))
{
    initResources();

    // TODO: Replace this with a nicer way for registering assets needed by various subsystems
    m_brdfLUT = new Qt3DRender::QTextureLoader(this);
    m_brdfLUT->setObjectName("_kuesa_brdfLUT");
    m_brdfLUT->setSource(QUrl(QLatin1String("qrc:/kuesa/shaders/brdfLUT.png")));
    m_brdfLUT->setWrapMode(Qt3DRender::QTextureWrapMode(Qt3DRender::QTextureWrapMode::ClampToEdge));
    m_textures->add(QLatin1String("_kuesa_brdfLUT"), m_brdfLUT);
}

SceneEntity::~SceneEntity() = default;

/*!
 * Returns instance of collection of Qt3DAnimation::QAbstractAnimationClip assets
 */
AnimationClipCollection *SceneEntity::animationClips() const
{
    return m_clips;
}

/*!
 * Utility method returning an instance of Qt3DAnimation::QAbstractAnimationClip matching \a name
 * (or nullptr if not found)
 */
Qt3DAnimation::QAbstractAnimationClip *SceneEntity::animationClip(const QString &name) const
{
    return m_clips->find(name);
}

/*!
 * Returns instance of collection of Qt3DCore::QArmature assets
 */
ArmatureCollection *SceneEntity::armatures() const
{
    return m_armatures;
}

/*!
 * Utility method returning an instance of Qt3DCore::QArmature matching \a name
 * (or nullptr if not found)
 */
Qt3DCore::QArmature *SceneEntity::armature(const QString &name) const
{
    return m_armatures->find(name);
}

EffectCollection *SceneEntity::effects() const
{
    return m_effects;
}

Qt3DRender::QEffect *SceneEntity::effect(const QString &name) const
{
    return m_effects->find(name);
}

/*!
 * Returns instance of collection of Qt3DRender::QLayer assets
 */
LayerCollection *SceneEntity::layers() const
{
    return m_layers;
}

/*!
 * Utility method returning an instance of Qt3DRender::QLayer matching \a name
 * (or nullptr if not found)
 */
Qt3DRender::QLayer *SceneEntity::layer(const QString &name) const
{
    return m_layers->find(name);
}

/*!
 * Returns instance of collection of Qt3DRender::QMaterial assets
 */
MaterialCollection *SceneEntity::materials() const
{
    return m_materials;
}

/*!
 * Utility method returning an instance of Qt3DRender::QMaterial matching \a name
 * (or nullptr if not found)
 */
Qt3DRender::QMaterial *SceneEntity::material(const QString &name) const
{
    return m_materials->find(name);
}

/*!
 * Returns instance of collection of Qt3DRender::QGeometryRenderer assets
 */
MeshCollection *SceneEntity::meshes() const
{
    return m_meshes;
}

/*!
 * Utility method returning an instance of Qt3DRender::QGeometryRenderer matching \a name
 * (or nullptr if not found)
 */
Qt3DRender::QGeometryRenderer *SceneEntity::mesh(const QString &name) const
{
    return m_meshes->find(name);
}

/*!
 * Returns instance of collection of Qt3DCore::QAbstractSkeleton assets
 */
SkeletonCollection *SceneEntity::skeletons() const
{
    return m_skeletons;
}

/*!
 * Utility method returning an instance of Qt3DCore::QAbstractSkeleton matching \a name
 * (or nullptr if not found)
 */
Qt3DCore::QAbstractSkeleton *SceneEntity::skeleton(const QString &name) const
{
    return m_skeletons->find(name);
}

/*!
 * Returns instance of collection of Qt3DRender::QAbstractTexture assets
 */
TextureCollection *SceneEntity::textures() const
{
    return m_textures;
}

/*!
 * Utility method returning an instance of Qt3DRender::QAbstractTexture matching \a name
 * (or nullptr if not found)
 */
Qt3DRender::QAbstractTexture *SceneEntity::texture(const QString &name) const
{
    return m_textures->find(name);
}

/*!
 * Returns instance of collection of Qt3DRender::QCamera assets
 */
CameraCollection *SceneEntity::cameras() const
{
    return m_cameras;
}

/*!
 * Utility method returning an instance of Qt3DRender::QCamera matching \a name
 * (or nullptr if not found)
 */
Qt3DRender::QCamera *SceneEntity::camera(const QString &name) const
{
    return m_cameras->find(name);
}

/*!
 * Returns instance of collection of Qt3DCore::QEntity assets
 */
EntityCollection *SceneEntity::entities() const
{
    return m_entities;
}

/*!
 * Utility method returning an instance of Qt3DCore::QEntity matching \a name
 * (or nullptr if not found)
 */
Qt3DCore::QEntity *SceneEntity::entity(const QString &name) const
{
    return m_entities->find(name);
}

/*!
 * Returns instance of collection of Qt3DRender::QAbstractTextureImage assets
 */
TextureImageCollection *SceneEntity::textureImages() const
{
    return m_textureImages;
}

/*!
 * Utility method returning an instance of Qt3DRender::QAbstractTextureImage matching \a name
 * (or nullptr if not found)
 */
Qt3DRender::QAbstractTextureImage *SceneEntity::textureImage(const QString &name)
{
    return m_textureImages->find(name);
}

/*!
 * Returns instance of collection of Qt3DAnimation::QChannelMapper assets
 */
AnimationMappingCollection *SceneEntity::animationMappings() const
{
    return m_animationMappings;
}

/*!
 * Utility method returning an instance of Qt3DAnimation::QChannelMapper matching \a name
 * (or nullptr if not found)
 */
Qt3DAnimation::QChannelMapper *SceneEntity::animationMapping(const QString &name)
{
    return m_animationMappings->find(name);
}

/*!
 * \brief Removes all assets from all the collections.
 *
 * \note Assets which are parented with the collection will be deleted.
 */
void SceneEntity::clearCollections()
{
    m_clips->clear();
    m_armatures->clear();
    m_effects->clear();
    m_layers->clear();
    m_materials->clear();
    m_meshes->clear();
    m_skeletons->clear();
    m_textures->clear();
    m_cameras->clear();
    m_entities->clear();
    m_textureImages->clear();
    m_animationMappings->clear();

    // TODO: Re-add required assets
    m_textures->add(QLatin1String("_kuesa_brdfLUT"), m_brdfLUT);
}

/*!
 * \brief SceneEntity::transformForEntity returns the transform component of the specified entity
 *
 * This is a utility method which will look for an entity in the SceneEntity's
 * EntityCollection and return the Qt3DCore::QTransform component if it exists.
 */
Qt3DCore::QNode *SceneEntity::transformForEntity(const QString &name)
{
    if (name.isEmpty())
        return nullptr;

    Qt3DCore::QEntity *e = entity(name);
    if (!e)
        return nullptr;

    return componentFromEntity<Qt3DCore::QTransform>(e);
}

QT_END_NAMESPACE
