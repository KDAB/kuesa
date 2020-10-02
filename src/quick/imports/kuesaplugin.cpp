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
#include <Kuesa/KTXTexture>

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
    qmlRegisterUncreatableType<Kuesa::TransformCollection>(uri, 1, 0, "TransformCollection", QStringLiteral("You are not supposed to create an TransformCollection"));

    // FrameGraphs
    Qt3DCore::Quick::registerExtendedType<Kuesa::ReflectionPlane, Kuesa::ReflectionPlaneExtension>("ReflectionPlane", "Kuesa/ReflectionPlane", uri, 1, 0, "ReflectionPlane");
    Qt3DCore::Quick::registerExtendedType<Kuesa::View, Kuesa::ViewExtension>("View", "Kuesa/View", uri, 1, 0, "View");
    Qt3DCore::Quick::registerExtendedType<Kuesa::ForwardRenderer, Kuesa::ForwardRendererExtension>("ForwardRenderer", "Kuesa/ForwardRenderer", uri, 1, 0, "ForwardRenderer");

    // Lights
    qmlRegisterType<Kuesa::DirectionalLight>(uri, 1, 0, "DirectionalLight");
    qmlRegisterType<Kuesa::PointLight>(uri, 1, 0, "PointLight");
    qmlRegisterType<Kuesa::SpotLight>(uri, 1, 0, "SpotLight");

    // Misc
    qmlRegisterUncreatableType<Kuesa::KuesaNode>(uri, 1, 0, "KuesaNode", QStringLiteral("KuesaNode should only be subclassed"));
    qmlRegisterType<Kuesa::GLTF2Importer>(uri, 1, 0, "GLTF2Importer");
    qmlRegisterType<Kuesa::SceneEntity>(uri, 1, 0, "SceneEntity");

    qmlRegisterUncreatableType<Kuesa::GLTF2Material>(uri, 1, 0, "GLTF2Material", QStringLiteral("GLTF2Material is abstract"));
    qmlRegisterUncreatableType<Kuesa::GLTF2MaterialProperties>(uri, 1, 0, "GLTF2MaterialProperties", QStringLiteral("GLTF2MaterialProperties is abstract"));
    qmlRegisterUncreatableType<Kuesa::GLTF2MaterialEffect>(uri, 1, 0, "GLTF2MaterialEffect", QStringLiteral("GLTF2MaterialEffect is abstract"));

    qmlRegisterType<Kuesa::MetallicRoughnessMaterial>(uri, 1, 0, "MetallicRoughnessMaterial");
    qmlRegisterType<Kuesa::MetallicRoughnessProperties>(uri, 1, 0, "MetallicRoughnessProperties");
    qmlRegisterType<Kuesa::MetallicRoughnessEffect>(uri, 1, 0, "MetallicRoughnessEffect");

    qmlRegisterType<Kuesa::UnlitMaterial>(uri, 1, 0, "UnlitMaterial");
    qmlRegisterType<Kuesa::UnlitProperties>(uri, 1, 0, "UnlitProperties");
    qmlRegisterType<Kuesa::UnlitEffect>(uri, 1, 0, "UnlitEffect");

    qmlRegisterType<Kuesa::Skybox>(uri, 1, 0, "Skybox");
    qmlRegisterType<Kuesa::Asset>(uri, 1, 0, "Asset");

    qmlRegisterExtendedType<Kuesa::AnimationPlayer, Kuesa::AnimationPlayerItem>(uri, 1, 0, "AnimationPlayer");
    qmlRegisterUncreatableType<Kuesa::GLTF2Import::GLTF2Options>(uri, 1, 0, "GLTF2Options", QStringLiteral("You are not supposed to create a GLTF2Options instance"));
    qmlRegisterUncreatableType<Kuesa::EffectProperties>(uri, 1, 0, "EffectProperties", QStringLiteral("Cannot be instantiated"));
    qRegisterMetaType<Kuesa::EffectProperties::Properties>("EffectProperties::Properties");
    qmlRegisterType<Kuesa::TransformTracker>(uri, 1, 0, "TransformTracker");

    // Custom Simple Materials
    qmlRegisterType<Kuesa::IroDiffuseMaterial>("Kuesa.Iro", 1, 0, "IroDiffuseMaterial");
    qmlRegisterType<Kuesa::IroDiffuseProperties>("Kuesa.Iro", 1, 0, "IroDiffuseProperties");
    qmlRegisterType<Kuesa::IroDiffuseEffect>("Kuesa.Iro", 1, 0, "IroDiffuseEffect");

    qmlRegisterType<Kuesa::IroDiffuseHemiMaterial>("Kuesa.Iro", 1, 0, "IroDiffuseHemiMaterial");
    qmlRegisterType<Kuesa::IroDiffuseHemiProperties>("Kuesa.Iro", 1, 0, "IroDiffuseHemiProperties");
    qmlRegisterType<Kuesa::IroDiffuseHemiEffect>("Kuesa.Iro", 1, 0, "IroDiffuseHemiEffect");

    qmlRegisterType<Kuesa::IroGlassAddMaterial>("Kuesa.Iro", 1, 0, "IroGlassAddMaterial");
    qmlRegisterType<Kuesa::IroGlassAddProperties>("Kuesa.Iro", 1, 0, "IroGlassAddProperties");
    qmlRegisterType<Kuesa::IroGlassAddEffect>("Kuesa.Iro", 1, 0, "IroGlassAddEffect");

    qmlRegisterType<Kuesa::IroMatteOpaqueMaterial>("Kuesa.Iro", 1, 0, "IroMatteOpaqueMaterial");
    qmlRegisterType<Kuesa::IroMatteOpaqueProperties>("Kuesa.Iro", 1, 0, "IroMatteOpaqueProperties");
    qmlRegisterType<Kuesa::IroMatteOpaqueEffect>("Kuesa.Iro", 1, 0, "IroMatteOpaqueEffect");

    qmlRegisterType<Kuesa::IroMatteAlphaMaterial>("Kuesa.Iro", 1, 0, "IroMatteAlphaMaterial");
    qmlRegisterType<Kuesa::IroMatteAlphaProperties>("Kuesa.Iro", 1, 0, "IroMatteAlphaProperties");
    qmlRegisterType<Kuesa::IroMatteAlphaEffect>("Kuesa.Iro", 1, 0, "IroMatteAlphaEffect");

    qmlRegisterType<Kuesa::IroMatteSkyboxMaterial>("Kuesa.Iro", 1, 0, "IroMatteSkyboxMaterial");
    qmlRegisterType<Kuesa::IroMatteSkyboxProperties>("Kuesa.Iro", 1, 0, "IroMatteSkyboxProperties");
    qmlRegisterType<Kuesa::IroMatteSkyboxEffect>("Kuesa.Iro", 1, 0, "IroMatteSkyboxEffect");

    qmlRegisterType<Kuesa::IroMatteBackgroundMaterial>("Kuesa.Iro", 1, 0, "IroMatteBackgroundMaterial");
    qmlRegisterType<Kuesa::IroMatteBackgroundProperties>("Kuesa.Iro", 1, 0, "IroMatteBackgroundProperties");
    qmlRegisterType<Kuesa::IroMatteBackgroundEffect>("Kuesa.Iro", 1, 0, "IroMatteBackgroundEffect");

    qmlRegisterType<Kuesa::IroMatteAddMaterial>("Kuesa.Iro", 1, 0, "IroMatteAddMaterial");
    qmlRegisterType<Kuesa::IroMatteAddProperties>("Kuesa.Iro", 1, 0, "IroMatteAddProperties");
    qmlRegisterType<Kuesa::IroMatteAddEffect>("Kuesa.Iro", 1, 0, "IroMatteAddEffect");

    qmlRegisterType<Kuesa::IroMatteMultMaterial>("Kuesa.Iro", 1, 0, "IroMatteMultMaterial");
    qmlRegisterType<Kuesa::IroMatteMultProperties>("Kuesa.Iro", 1, 0, "IroMatteMultProperties");
    qmlRegisterType<Kuesa::IroMatteMultEffect>("Kuesa.Iro", 1, 0, "IroMatteMultEffect");

    qmlRegisterType<Kuesa::IroDiffuseAlphaMaterial>("Kuesa.Iro", 1, 0, "IroDiffuseAlphaMaterial");
    qmlRegisterType<Kuesa::IroDiffuseAlphaProperties>("Kuesa.Iro", 1, 0, "IroDiffuseAlphaProperties");
    qmlRegisterType<Kuesa::IroDiffuseAlphaEffect>("Kuesa.Iro", 1, 0, "IroDiffuseAlphaEffect");

    qmlRegisterType<Kuesa::KTXTexture>("Kuesa", 1, 0, "KTXTexture");

    // Post FX
    qmlRegisterUncreatableType<Kuesa::AbstractPostProcessingEffect>("Kuesa.Effects", 1, 0, "AbstractPostProcessingEffect", QStringLiteral("AbstractPostProcessingEffect is abstract"));
    qmlRegisterType<Kuesa::BloomEffect>("Kuesa.Effects", 1, 0, "BloomEffect");
    qmlRegisterType<Kuesa::DepthOfFieldEffect>("Kuesa.Effects", 1, 0, "DepthOfFieldEffect");
    qmlRegisterType<Kuesa::ThresholdEffect>("Kuesa.Effects", 1, 0, "ThresholdEffect");
    qmlRegisterType<Kuesa::GaussianBlurEffect>("Kuesa.Effects", 1, 0, "BlurEffect");
    qmlRegisterType<Kuesa::OpacityMask>("Kuesa.Effects", 1, 0, "OpacityMask");

    qRegisterMetaType<Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping>("ToneMappingAndGammaCorrectionEffect::ToneMapping");
    qmlRegisterType<Kuesa::ToneMappingAndGammaCorrectionEffect>("Kuesa.Effects", 1, 0, "ToneMappingAndGammaCorrectionEffect");

    // Utils
    qmlRegisterType<KuesaUtils::BoundingVolumeRenderer>("Kuesa.Utils", 1, 0, "BoundingVolumeRenderer");
    qmlRegisterType<KuesaUtils::OrbitCameraController>("Kuesa.Utils", 1, 0, "OrbitCameraController");
    qmlRegisterExtendedType<KuesaUtils::View3DScene, KuesaUtils::View3DSceneItem>("Kuesa.Utils", 1, 0, "View3DScene");

    // Particles
    qmlRegisterType<Kuesa::Particles>(uri, 1, 0, "Particles");
}

QT_END_NAMESPACE
