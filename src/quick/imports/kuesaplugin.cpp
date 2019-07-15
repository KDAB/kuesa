/*
    kuesaplugin.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Kuesa/SceneEntity>
#include <Kuesa/GLTF2Importer>
#include <Kuesa/ForwardRenderer>
#include <Kuesa/MetallicRoughnessMaterial>
#include <Kuesa/UnlitMaterial>
#include <Kuesa/BloomEffect>
#include <Kuesa/DepthOfFieldEffect>
#include <Kuesa/GaussianBlurEffect>
#include <Kuesa/ThresholdEffect>
#include <Kuesa/OpacityMask>
#include <Kuesa/Skybox>
#include <Kuesa/DirectionalLight>
#include <Kuesa/SpotLight>
#include <Kuesa/PointLight>
#include "postfxlistextension.h"

#include <QtQml/qqml.h>

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

    // FrameGraphs
    qmlRegisterExtendedType<Kuesa::ForwardRenderer, Kuesa::PostFXListExtension>(uri, 1, 0, "ForwardRenderer");

    // Lights
    qmlRegisterType<Kuesa::DirectionalLight>(uri, 1, 1, "DirectionalLight");
    qmlRegisterType<Kuesa::PointLight>(uri, 1, 1, "PointLight");
    qmlRegisterType<Kuesa::SpotLight>(uri, 1, 1, "SpotLight");

    // Misc
    qmlRegisterType<Kuesa::GLTF2Importer>(uri, 1, 0, "GLTF2Importer");
    qmlRegisterType<Kuesa::SceneEntity>(uri, 1, 0, "SceneEntity");
    qmlRegisterUncreatableType<Kuesa::GLTF2Material>(uri, 1, 1, "GLTF2Material", QStringLiteral("GLTF2Material is abstract"));
    qmlRegisterType<Kuesa::MetallicRoughnessMaterial>(uri, 1, 0, "MetallicRoughnessMaterial");
    qmlRegisterType<Kuesa::MetallicRoughnessMaterial, 1>(uri, 1, 1, "MetallicRoughnessMaterial");
    qmlRegisterType<Kuesa::MetallicRoughnessEffect>(uri, 1, 0, "MetallicRoughnessEffect");
    qmlRegisterType<Kuesa::MetallicRoughnessEffect, 1>(uri, 1, 1, "MetallicRoughnessEffect");
    qmlRegisterType<Kuesa::UnlitMaterial, 1>(uri, 1, 1, "UnlitMaterial");
    qmlRegisterType<Kuesa::UnlitEffect, 1>(uri, 1, 1, "UnlitEffect");
    qmlRegisterType<Kuesa::Skybox>(uri, 1, 0, "Skybox");
    qmlRegisterType<Kuesa::Asset>(uri, 1, 0, "Asset");
    qmlRegisterExtendedType<Kuesa::AnimationPlayer, Kuesa::AnimationPlayerItem>(uri, 1, 0, "AnimationPlayer");
    qmlRegisterUncreatableType<Kuesa::GLTF2Import::GLTF2Options>(uri, 1, 1, "GLTF2Options",  QStringLiteral("You are not supposed to create a GLTF2Options instance"));

    // Post FX
    qmlRegisterUncreatableType<Kuesa::AbstractPostProcessingEffect>("Kuesa.Effects", 1, 0, "AbstractPostProcessingEffect", QStringLiteral("AbstractPostProcessingEffect is abstract"));
    qmlRegisterType<Kuesa::BloomEffect>("Kuesa.Effects", 1, 0, "BloomEffect");
    qmlRegisterType<Kuesa::DepthOfFieldEffect>("Kuesa.Effects", 1, 0, "DepthOfFieldEffect");
    qmlRegisterType<Kuesa::ThresholdEffect>("Kuesa.Effects", 1, 0, "ThresholdEffect");
    qmlRegisterType<Kuesa::GaussianBlurEffect>("Kuesa.Effects", 1, 0, "BlurEffect");
    qmlRegisterType<Kuesa::OpacityMask>("Kuesa.Effects", 1, 0, "OpacityMask");
}

QT_END_NAMESPACE
