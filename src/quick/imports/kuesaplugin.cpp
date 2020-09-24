/*
    kuesaplugin.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#include "kuesaplugin.h"
#include "animationplayeritem.h"
#include "asset.h"
#include "view3dsceneitem.h"
#include "forwardrendererextension.h"
#include "viewextension.h"
#include "reflectionplaneextension.h"

#include <Kuesa/SceneEntity>
#include <Kuesa/TransformTracker>
#include <Kuesa/GLTF2Importer>
#include <Kuesa/ForwardRenderer>
#include <Kuesa/MetallicRoughnessMaterial>
#include <Kuesa/UnlitMaterial>
#include <Kuesa/BloomEffect>
#include <Kuesa/DepthOfFieldEffect>
#include <Kuesa/GaussianBlurEffect>
#include <Kuesa/ThresholdEffect>
#include <Kuesa/OpacityMask>
#include <Kuesa/ToneMappingAndGammaCorrectionEffect>
#include <Kuesa/Skybox>
#include <Kuesa/DirectionalLight>
#include <Kuesa/SpotLight>
#include <Kuesa/PointLight>
#include <Kuesa/MetallicRoughnessProperties>
#include <Kuesa/UnlitProperties>
#include <Kuesa/EffectProperties>
#include <Kuesa/IroDiffuseMaterial>
#include <Kuesa/IroDiffuseEffect>
#include <Kuesa/IroDiffuseProperties>
#include <Kuesa/IroDiffuseHemiMaterial>
#include <Kuesa/IroDiffuseHemiEffect>
#include <Kuesa/IroDiffuseHemiProperties>
#include <Kuesa/IroGlassAddMaterial>
#include <Kuesa/IroGlassAddEffect>
#include <Kuesa/IroGlassAddProperties>
#include <Kuesa/IroMatteOpaqueMaterial>
#include <Kuesa/IroMatteOpaqueEffect>
#include <Kuesa/IroMatteOpaqueProperties>
#include <Kuesa/IroMatteAlphaMaterial>
#include <Kuesa/IroMatteAlphaEffect>
#include <Kuesa/IroMatteAlphaProperties>
#include <Kuesa/IroMatteSkyboxMaterial>
#include <Kuesa/IroMatteSkyboxEffect>
#include <Kuesa/IroMatteSkyboxProperties>
#include <Kuesa/IroMatteBackgroundMaterial>
#include <Kuesa/IroMatteBackgroundEffect>
#include <Kuesa/IroMatteBackgroundProperties>
#include <Kuesa/IroMatteAddMaterial>
#include <Kuesa/IroMatteAddEffect>
#include <Kuesa/IroMatteAddProperties>
#include <Kuesa/IroMatteMultMaterial>
#include <Kuesa/IroMatteMultEffect>
#include <Kuesa/IroMatteMultProperties>
#include <Kuesa/IroDiffuseAlphaMaterial>
#include <Kuesa/IroDiffuseAlphaEffect>
#include <Kuesa/IroDiffuseAlphaProperties>
#include <KuesaUtils/orbitcameracontroller.h>
#include <KuesaUtils/boundingvolumerenderer.h>
#include <KuesaUtils/view3dscene.h>
#include <Kuesa/Particles>
#include <Kuesa/KuesaNode>
#include <Kuesa/View>
#include <Kuesa/ReflectionPlane>

#include <QtQml/qqml.h>

#include <Qt3DQuick/private/qt3dquick_global_p.h>

QT_BEGIN_NAMESPACE

KuesaPlugin::KuesaPlugin(QObject *parent)
    : QQmlExtensionPlugin(parent)
{
}

KuesaPlugin::~KuesaPlugin()
{
}

void KuesaPlugin::registerTypes(const char *uri)
{
    // Make latest Kuesa import available (even if no new types are registered)
    qmlRegisterModule(uri, 1, 3);
    qmlRegisterModule("Kuesa.Effects", 1, 3);
    qmlRegisterModule("Kuesa.Iro", 1, 3);
    qmlRegisterModule("Kuesa.Utils", 1, 3);

    // Collections
    qmlRegisterUncreatableType<Kuesa::AbstractAssetCollection>(uri, 1, 0, "AbstractAssetCollection", QStringLiteral("You are not supposed to create an EntityCollection"));
    qmlRegisterUncreatableType<Kuesa::EntityCollection>(uri, 1, 0, "EntityCollection", QStringLiteral("You are not supposed to create an EntityCollection"));
    qmlRegisterUncreatableType<Kuesa::ArmatureCollection>(uri, 1, 0, "ArmatureCollection", QStringLiteral("You are not supposed to create an ArmatureCollection"));
    qmlRegisterUncreatableType<Kuesa::LayerCollection>(uri, 1, 0, "LayerCollection", QStringLiteral("You are not supposed to create a LayerCollection"));
    qmlRegisterUncreatableType<Kuesa::MaterialCollection>(uri, 1, 0, "MaterialCollection", QStringLiteral("You are not supposed to create a MaterialCollection"));
    qmlRegisterUncreatableType<Kuesa::MeshCollection>(uri, 1, 0, "MeshCollection", QStringLiteral("You are not supposed to create a MeshCollection"));
    qmlRegisterUncreatableType<Kuesa::SkeletonCollection>(uri, 1, 0, "SkeletonCollection", QStringLiteral("You are not supposed to create a SkeletonCollection"));
    qmlRegisterUncreatableType<Kuesa::TextureCollection>(uri, 1, 0, "TextureCollection", QStringLiteral("You are not supposed to create a TextureCollection"));
    qmlRegisterUncreatableType<Kuesa::CameraCollection>(uri, 1, 0, "CameraCollection", QStringLiteral("You are not supposed to create a CameraCollection"));
    qmlRegisterUncreatableType<Kuesa::TextureImageCollection>(uri, 1, 0, "TextureImageCollection", QStringLiteral("You are not supposed to create a TextureImageCollection"));
    qmlRegisterUncreatableType<Kuesa::AnimationMappingCollection>(uri, 1, 0, "AnimationMappingCollection", QStringLiteral("You are not supposed to create an AnimationMappingCollection"));
    qmlRegisterUncreatableType<Kuesa::AnimationClipCollection>(uri, 1, 0, "AnimationClipCollection", QStringLiteral("You are not supposed to create an AnimationClipCollection"));
    qmlRegisterUncreatableType<Kuesa::TransformCollection>(uri, 1, 3, "TransformCollection", QStringLiteral("You are not supposed to create an TransformCollection"));

    // FrameGraphs
    Qt3DCore::Quick::registerExtendedType<Kuesa::ReflectionPlane, Kuesa::ReflectionPlaneExtension>("ReflectionPlane", "Kuesa/ReflectionPlane", uri, 1, 3, "ReflectionPlane");
    Qt3DCore::Quick::registerExtendedType<Kuesa::View, Kuesa::ViewExtension>("View", "Kuesa/View", uri, 1, 3, "View");
    Qt3DCore::Quick::registerExtendedType<Kuesa::ForwardRenderer, Kuesa::ForwardRendererExtension>("ForwardRenderer", "Kuesa/ForwardRenderer", uri, 1, 0, "ForwardRenderer");
    qmlRegisterRevision<Kuesa::ForwardRenderer, 1>(uri, 1, 1);
    qmlRegisterRevision<Kuesa::ForwardRenderer, 2>(uri, 1, 2);
    qmlRegisterRevision<Kuesa::ForwardRenderer, 3>(uri, 1, 3);

    // Lights
    qmlRegisterType<Kuesa::DirectionalLight>(uri, 1, 1, "DirectionalLight");
    qmlRegisterType<Kuesa::PointLight>(uri, 1, 1, "PointLight");
    qmlRegisterType<Kuesa::SpotLight>(uri, 1, 1, "SpotLight");

    // Misc
    qmlRegisterUncreatableType<Kuesa::KuesaNode>(uri, 1, 0, "KuesaNode", QStringLiteral("KuesaNode should only be subclassed"));
    qmlRegisterType<Kuesa::GLTF2Importer>(uri, 1, 0, "GLTF2Importer");
    qmlRegisterType<Kuesa::SceneEntity>(uri, 1, 0, "SceneEntity");
    qmlRegisterRevision<Kuesa::SceneEntity, 2>(uri, 1, 2);
    qmlRegisterUncreatableType<Kuesa::GLTF2Material>(uri, 1, 1, "GLTF2Material", QStringLiteral("GLTF2Material is abstract"));
    qmlRegisterUncreatableType<Kuesa::GLTF2MaterialProperties>(uri, 1, 1, "GLTF2MaterialProperties", QStringLiteral("GLTF2MaterialProperties is abstract"));
    qmlRegisterType<Kuesa::MetallicRoughnessMaterial>(uri, 1, 0, "MetallicRoughnessMaterial");
    qmlRegisterType<Kuesa::MetallicRoughnessMaterial, 1>(uri, 1, 1, "MetallicRoughnessMaterial");
    qmlRegisterType<Kuesa::MetallicRoughnessProperties, 1>(uri, 1, 1, "MetallicRoughnessProperties");
    qmlRegisterType<Kuesa::MetallicRoughnessEffect>(uri, 1, 0, "MetallicRoughnessEffect");
    qmlRegisterType<Kuesa::MetallicRoughnessEffect, 1>(uri, 1, 1, "MetallicRoughnessEffect");
    qmlRegisterType<Kuesa::MetallicRoughnessProperties>(uri, 1, 1, "MetallicRoughnessProperties");
    qmlRegisterType<Kuesa::UnlitMaterial, 1>(uri, 1, 1, "UnlitMaterial");
    qmlRegisterType<Kuesa::UnlitProperties, 1>(uri, 1, 1, "UnlitProperties");
    qmlRegisterType<Kuesa::UnlitEffect, 1>(uri, 1, 1, "UnlitEffect");
    qmlRegisterType<Kuesa::UnlitProperties>(uri, 1, 1, "UnlitProperties");
    qmlRegisterType<Kuesa::Skybox>(uri, 1, 0, "Skybox");
    qmlRegisterType<Kuesa::Asset>(uri, 1, 0, "Asset");
    qmlRegisterExtendedType<Kuesa::AnimationPlayer, Kuesa::AnimationPlayerItem>(uri, 1, 0, "AnimationPlayer");
    qmlRegisterUncreatableType<Kuesa::GLTF2Import::GLTF2Options>(uri, 1, 1, "GLTF2Options", QStringLiteral("You are not supposed to create a GLTF2Options instance"));
    qmlRegisterUncreatableType<Kuesa::EffectProperties>(uri, 1, 1, "EffectProperties", QStringLiteral("Cannot be instantiated"));
    qRegisterMetaType<Kuesa::EffectProperties::Properties>("EffectProperties::Properties");
    qmlRegisterType<Kuesa::TransformTracker>(uri, 1, 3, "TransformTracker");

    // Custom Simple Materials
    qmlRegisterType<Kuesa::IroDiffuseMaterial>("Kuesa.Iro", 1, 2, "IroDiffuseMaterial");
    qmlRegisterType<Kuesa::IroDiffuseProperties>("Kuesa.Iro", 1, 2, "IroDiffuseProperties");
    qmlRegisterType<Kuesa::IroDiffuseEffect>("Kuesa.Iro", 1, 2, "IroDiffuseEffect");

    qmlRegisterType<Kuesa::IroDiffuseHemiMaterial>("Kuesa.Iro", 1, 2, "IroDiffuseHemiMaterial");
    qmlRegisterType<Kuesa::IroDiffuseHemiProperties>("Kuesa.Iro", 1, 2, "IroDiffuseHemiProperties");
    qmlRegisterType<Kuesa::IroDiffuseHemiEffect>("Kuesa.Iro", 1, 2, "IroDiffuseHemiEffect");

    qmlRegisterType<Kuesa::IroGlassAddMaterial>("Kuesa.Iro", 1, 2, "IroGlassAddMaterial");
    qmlRegisterType<Kuesa::IroGlassAddProperties>("Kuesa.Iro", 1, 2, "IroGlassAddProperties");
    qmlRegisterType<Kuesa::IroGlassAddEffect>("Kuesa.Iro", 1, 2, "IroGlassAddEffect");

    qmlRegisterType<Kuesa::IroMatteOpaqueMaterial>("Kuesa.Iro", 1, 2, "IroMatteOpaqueMaterial");
    qmlRegisterType<Kuesa::IroMatteOpaqueProperties>("Kuesa.Iro", 1, 2, "IroMatteOpaqueProperties");
    qmlRegisterType<Kuesa::IroMatteOpaqueEffect>("Kuesa.Iro", 1, 2, "IroMatteOpaqueEffect");

    qmlRegisterType<Kuesa::IroMatteAlphaMaterial>("Kuesa.Iro", 1, 2, "IroMatteAlphaMaterial");
    qmlRegisterType<Kuesa::IroMatteAlphaProperties>("Kuesa.Iro", 1, 2, "IroMatteAlphaProperties");
    qmlRegisterType<Kuesa::IroMatteAlphaEffect>("Kuesa.Iro", 1, 2, "IroMatteAlphaEffect");

    qmlRegisterType<Kuesa::IroMatteSkyboxMaterial>("Kuesa.Iro", 1, 2, "IroMatteSkyboxMaterial");
    qmlRegisterType<Kuesa::IroMatteSkyboxProperties>("Kuesa.Iro", 1, 2, "IroMatteSkyboxProperties");
    qmlRegisterType<Kuesa::IroMatteSkyboxEffect>("Kuesa.Iro", 1, 2, "IroMatteSkyboxEffect");

    qmlRegisterType<Kuesa::IroMatteBackgroundMaterial>("Kuesa.Iro", 1, 2, "IroMatteBackgroundMaterial");
    qmlRegisterType<Kuesa::IroMatteBackgroundProperties>("Kuesa.Iro", 1, 2, "IroMatteBackgroundProperties");
    qmlRegisterType<Kuesa::IroMatteBackgroundEffect>("Kuesa.Iro", 1, 2, "IroMatteBackgroundEffect");

    qmlRegisterType<Kuesa::IroMatteAddMaterial>("Kuesa.Iro", 1, 2, "IroMatteAddMaterial");
    qmlRegisterType<Kuesa::IroMatteAddProperties>("Kuesa.Iro", 1, 2, "IroMatteAddProperties");
    qmlRegisterType<Kuesa::IroMatteAddEffect>("Kuesa.Iro", 1, 2, "IroMatteAddEffect");

    qmlRegisterType<Kuesa::IroMatteMultMaterial>("Kuesa.Iro", 1, 2, "IroMatteMultMaterial");
    qmlRegisterType<Kuesa::IroMatteMultProperties>("Kuesa.Iro", 1, 2, "IroMatteMultProperties");
    qmlRegisterType<Kuesa::IroMatteMultEffect>("Kuesa.Iro", 1, 2, "IroMatteMultEffect");

    qmlRegisterType<Kuesa::IroDiffuseAlphaMaterial>("Kuesa.Iro", 1, 2, "IroDiffuseAlphaMaterial");
    qmlRegisterType<Kuesa::IroDiffuseAlphaProperties>("Kuesa.Iro", 1, 2, "IroDiffuseAlphaProperties");
    qmlRegisterType<Kuesa::IroDiffuseAlphaEffect>("Kuesa.Iro", 1, 2, "IroDiffuseAlphaEffect");

    // Post FX
    qmlRegisterUncreatableType<Kuesa::AbstractPostProcessingEffect>("Kuesa.Effects", 1, 0, "AbstractPostProcessingEffect", QStringLiteral("AbstractPostProcessingEffect is abstract"));
    qmlRegisterUncreatableType<Kuesa::AbstractPostProcessingEffect>("Kuesa.Effects", 1, 2, "AbstractPostProcessingEffect", QStringLiteral("AbstractPostProcessingEffect is abstract"));
    qmlRegisterType<Kuesa::BloomEffect>("Kuesa.Effects", 1, 0, "BloomEffect");
    qmlRegisterType<Kuesa::DepthOfFieldEffect>("Kuesa.Effects", 1, 0, "DepthOfFieldEffect");
    qmlRegisterType<Kuesa::ThresholdEffect>("Kuesa.Effects", 1, 0, "ThresholdEffect");
    qmlRegisterType<Kuesa::GaussianBlurEffect>("Kuesa.Effects", 1, 0, "BlurEffect");
    qmlRegisterType<Kuesa::OpacityMask>("Kuesa.Effects", 1, 0, "OpacityMask");

    qRegisterMetaType<Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping>("ToneMappingAndGammaCorrectionEffect::ToneMapping");
    qmlRegisterType<Kuesa::ToneMappingAndGammaCorrectionEffect>("Kuesa.Effects", 1, 1, "ToneMappingAndGammaCorrectionEffect");

    // Utils
    qmlRegisterType<KuesaUtils::BoundingVolumeRenderer>("Kuesa.Utils", 1, 3, "BoundingVolumeRenderer");
    qmlRegisterType<KuesaUtils::OrbitCameraController>("Kuesa.Utils", 1, 3, "OrbitCameraController");
    qmlRegisterExtendedType<KuesaUtils::View3DScene, KuesaUtils::View3DSceneItem>("Kuesa.Utils", 1, 3, "View3DScene");

    // Particles
    qmlRegisterType<Kuesa::Particles>(uri, 1, 3, "Particles");
}

QT_END_NAMESPACE
