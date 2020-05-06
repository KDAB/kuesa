/*
    sceneentity.h

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

#ifndef KUESA_SCENEENTITY_H
#define KUESA_SCENEENTITY_H

#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Kuesa/animationclipcollection.h>
#include <Kuesa/armaturecollection.h>
#include <Kuesa/effectcollection.h>
#include <Kuesa/layercollection.h>
#include <Kuesa/lightcollection.h>
#include <Kuesa/materialcollection.h>
#include <Kuesa/meshcollection.h>
#include <Kuesa/skeletoncollection.h>
#include <Kuesa/texturecollection.h>
#include <Kuesa/cameracollection.h>
#include <Kuesa/entitycollection.h>
#include <Kuesa/transformcollection.h>
#include <Kuesa/textureimagecollection.h>
#include <Kuesa/animationmappingcollection.h>
#include <Kuesa/kuesa_global.h>
#include <Kuesa/effectproperties.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QTextureLoader;
}

namespace Kuesa {

namespace GLTF2Import {
class GLTF2Parser;
}

class EffectsLibrary;
class KUESASHARED_EXPORT SceneEntity : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(Kuesa::AnimationClipCollection *animationClips READ animationClips NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::ArmatureCollection *armatures READ armatures NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::LayerCollection *layers READ layers NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::MaterialCollection *materials READ materials NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::MeshCollection *meshes READ meshes NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::SkeletonCollection *skeletons READ skeletons NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::TextureCollection *textures READ textures NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::CameraCollection *cameras READ cameras NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::EntityCollection *entities READ entities NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::TextureImageCollection *textureImages READ textureImages NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::AnimationMappingCollection *animationMappings READ animationMappings NOTIFY loadingDone)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *brdfLut READ brdfLut CONSTANT)
    Q_PROPERTY(Kuesa::LightCollection *lights READ lights NOTIFY loadingDone)
    Q_PROPERTY(Kuesa::TransformCollection *transforms READ transforms NOTIFY loadingDone REVISION 2)

public:
    SceneEntity(Qt3DCore::QNode *parent = nullptr);
    ~SceneEntity();

    AnimationClipCollection *animationClips() const;
    Q_INVOKABLE Qt3DAnimation::QAbstractAnimationClip *animationClip(const QString &name) const;

    ArmatureCollection *armatures() const;
    Q_INVOKABLE Qt3DCore::QArmature *armature(const QString &name) const;

    EffectCollection *effects() const;
    Q_INVOKABLE Qt3DRender::QEffect *effect(const QString &name) const;
    Q_INVOKABLE Qt3DRender::QEffect *effect(int effectPropertyFlags) const;

    LayerCollection *layers() const;
    Q_INVOKABLE Qt3DRender::QLayer *layer(const QString &name) const;

    MaterialCollection *materials() const;
    Q_INVOKABLE Kuesa::GLTF2MaterialProperties *material(const QString &name) const;

    MeshCollection *meshes() const;
    Q_INVOKABLE Qt3DRender::QGeometryRenderer *mesh(const QString &name) const;

    SkeletonCollection *skeletons() const;
    Q_INVOKABLE Qt3DCore::QAbstractSkeleton *skeleton(const QString &name) const;

    TextureCollection *textures() const;
    Q_INVOKABLE Qt3DRender::QAbstractTexture *texture(const QString &name) const;

    CameraCollection *cameras() const;
    Q_INVOKABLE Qt3DRender::QCamera *camera(const QString &name) const;

    EntityCollection *entities() const;
    Q_INVOKABLE Qt3DCore::QEntity *entity(const QString &name) const;

    Q_REVISION(2) TransformCollection *transforms() const;
    Q_REVISION(2) Q_INVOKABLE Qt3DCore::QTransform *transform(const QString &name) const;

    TextureImageCollection *textureImages() const;
    Q_INVOKABLE Qt3DRender::QAbstractTextureImage *textureImage(const QString &name);

    AnimationMappingCollection *animationMappings() const;
    Q_INVOKABLE Qt3DAnimation::QChannelMapper *animationMapping(const QString &name);

    Q_INVOKABLE void clearCollections();

    Q_INVOKABLE Qt3DCore::QNode *transformForEntity(const QString &name);

    Q_INVOKABLE Qt3DRender::QAbstractTexture *brdfLut() const;

    Q_INVOKABLE static Kuesa::SceneEntity *findParentSceneEntity(Qt3DCore::QEntity *entity);

    Kuesa::LightCollection *lights() const;
    Q_INVOKABLE Qt3DRender::QAbstractLight *light(const QString &name) const;

Q_SIGNALS:
    void loadingDone();

private:
    AnimationClipCollection *m_clips;
    ArmatureCollection *m_armatures;
    EffectCollection *m_effects;
    LayerCollection *m_layers;
    LightCollection *m_lights;
    MaterialCollection *m_materials;
    MeshCollection *m_meshes;
    SkeletonCollection *m_skeletons;
    TextureCollection *m_textures;
    CameraCollection *m_cameras;
    EntityCollection *m_entities;
    TransformCollection *m_transforms;
    TextureImageCollection *m_textureImages;
    AnimationMappingCollection *m_animationMappings;

    Qt3DRender::QTextureLoader *m_brdfLUT;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_SCENEENTITY_H
