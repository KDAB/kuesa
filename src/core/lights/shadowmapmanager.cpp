/*
    shadowmapmanager.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont (jim.albamont@kdab.com)

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

#include "shadowmapmanager_p.h"
#include "directionallight.h"
#include "spotlight.h"
#include "pointlight.h"
#include <Kuesa/private/framegraphutils_p.h>
#include <Kuesa/private/kuesa_utils_p.h>
#include <Qt3DRender/private/entity_p.h>

#include <QCamera>
#include <QFrameAction>
#include <QTexture>
#include <Qt3DRender/private/sphere_p.h>

QT_BEGIN_NAMESPACE

/*!
 * \internal
 * \class Kuesa::ShadowMapManager
 * \brief Holds the information necessary for rendering shadows in Kuesa
 * \inmodule Kuesa
 * \since Kuesa 1.3
 * \inherits QObject
 *
 * The ShadowMapManager keeps track of all ShadowCastingLights in the application
 * and creates/destroys ShadowMap objects for each light that casts a shadow.
 *
 * It's responsible for maintaining the 2 shadowmap depth textures (a cubemap
 * texture for point lights and normal depth texture for directional and spot lights).
 * These are array textures, with one layer per ShadowCastingLight and its
 * corresponding ShadowMap. An array texture requires that all layer be the same size,
 * so this class keeps track of the requested texture size for  all active
 * ShadowCastingLights and updates the array texture to be the size of the
 * largest. The ShadowMapManager also updates each ShadowMap to indicate which array
 * texture index to use.
*/

using namespace Kuesa;
ShadowMapManager::ShadowMapManager(QObject *parent)
    : QObject(parent)
{
    m_shadowMapEntity = new ShadowMapLightDataEntity;
    connect(m_shadowMapEntity, &ShadowMapLightDataEntity::worldBoundsChanged, this, &ShadowMapManager::updateSceneBounds);

    m_hasCubeMapArrayTextures = FrameGraphUtils::hasCubeMapArrayTextures();
    auto createTexture = [this](bool createCubeMap, const QSize &textureSize) {
        Qt3DRender::QAbstractTexture *depthTexture;
        if (createCubeMap) {
            if (m_hasCubeMapArrayTextures)
                depthTexture = new Qt3DRender::QTextureCubeMapArray(m_shadowMapEntity);
            else
                depthTexture = new Qt3DRender::QTextureCubeMap(m_shadowMapEntity);
        } else
            depthTexture = new Qt3DRender::QTexture2DArray(m_shadowMapEntity);

        depthTexture->setFormat(Qt3DRender::QAbstractTexture::D24);
        depthTexture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
        depthTexture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);

        // set comparison so that sampling depth texture results in shadow coverage, not a depth value
        depthTexture->setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);
        depthTexture->setComparisonFunction(Qt3DRender::QAbstractTexture::CompareLessEqual);
        depthTexture->setSize(textureSize.width(), textureSize.height());
        return depthTexture;
    };
    const QSize textureSize{ 512, 512 };
    m_depthTexture = createTexture(false, textureSize);
    m_depthCubeTexture = createTexture(true, textureSize);
}

void ShadowMapManager::addLight(ShadowCastingLight *light)
{
    if (m_lights.contains(light))
        return;

    m_lights.insert(light);
    connect(light, &ShadowCastingLight::castsShadowsChanged, this, &ShadowMapManager::castsShadowChanged);
    connect(light, &ShadowCastingLight::enabledChanged, this, &ShadowMapManager::castsShadowChanged);

    if (light->isEnabled() && light->castsShadows())
        createShadowMap(light);
}

void ShadowMapManager::removeLight(ShadowCastingLight *light)
{
    if (m_lights.contains(light))
        return;

    m_lights.remove(light);
    disconnect(light, &ShadowCastingLight::castsShadowsChanged, this, &ShadowMapManager::castsShadowChanged);
    destroyShadowMap(light);
}

void ShadowMapManager::setLights(const QVector<ShadowCastingLight *> &lights)
{
    QVector<ShadowCastingLight *> lightsToRemove;
    std::copy_if(m_lights.cbegin(), m_lights.cend(), std::back_inserter(lightsToRemove), [&lights](ShadowCastingLight *light) {
        return !lights.contains(light);
    });

    for (auto light : lightsToRemove)
        removeLight(light);

    for (auto light : lights)
        addLight(light);
}

void ShadowMapManager::setSceneEntity(Qt3DCore::QEntity *sceneEntity)
{
    m_shadowMapEntity->setParent(sceneEntity);
}

QVector<QSharedPointer<ShadowMap>> ShadowMapManager::activeShadowMaps() const
{
    QVector<QSharedPointer<ShadowMap>> shadowMaps;
    shadowMaps.reserve(m_shadowMaps.size());
    std::copy(m_shadowMaps.cbegin(), m_shadowMaps.cend(), std::back_inserter(shadowMaps));
    return shadowMaps;
}

QVector3D ShadowMapManager::sceneCenter() const
{
    return m_shadowMapEntity->sceneCenter();
}

float ShadowMapManager::sceneRadius() const
{
    return m_shadowMapEntity->sceneRadius();
}

Qt3DRender::QAbstractTexture *ShadowMapManager::depthTexture() const
{
    return m_depthTexture;
}

Qt3DRender::QAbstractTexture *ShadowMapManager::cubeDepthTexture() const
{
    return m_depthCubeTexture;
}

void ShadowMapManager::updateSceneBounds(const QVector3D &sceneCenter, float sceneRadius)
{
    emit sceneCenterChanged(sceneCenter);
    emit sceneRadiusChanged(sceneRadius);

    for (auto &shadowMap : m_shadowMaps)
        shadowMap->updateSceneBounds(sceneCenter, sceneRadius);
}

void ShadowMapManager::castsShadowChanged()
{
    auto light = qobject_cast<ShadowCastingLight *>(sender());
    Q_ASSERT(light);

    const bool castsShadows = light->isEnabled() && light->castsShadows();
    if (castsShadows && !m_shadowMaps.contains(light))
        createShadowMap(light);
    else if (!castsShadows && m_shadowMaps.contains(light))
        destroyShadowMap(light);
}

void ShadowMapManager::renumberShadowMaps()
{
    int cubeMapDepthTextureLayer = 0;
    int depthTextureLayer = 0;
    for (auto &shadowMap : m_shadowMaps) {
        auto &index = shadowMap->usesCubeMap() ? cubeMapDepthTextureLayer : depthTextureLayer;
        shadowMap->setDepthTextureLayer(index);
        ++index;
    }
    m_depthTexture->setLayers(depthTextureLayer);
    m_depthCubeTexture->setLayers(cubeMapDepthTextureLayer);

    if (!m_hasCubeMapArrayTextures && cubeMapDepthTextureLayer > 1)
        qWarning("Kuesa only supports shadows for a single point light");
}

void ShadowMapManager::resizeShadowMaps()
{
    int maxTextureWidth = 0;
    int maxTextureHeight = 0;
    int maxCubeMapTexureSize = 0;

    for (auto &shadowMap : m_shadowMaps) {
        auto &width = shadowMap->usesCubeMap() ? maxCubeMapTexureSize : maxTextureWidth;
        auto &height = shadowMap->usesCubeMap() ? maxCubeMapTexureSize : maxTextureHeight;
        const auto textureSize = shadowMap->light()->textureSize();
        width = std::max(width, textureSize.width());
        height = std::max(height, textureSize.height());
    }
    m_depthTexture->setSize(maxTextureWidth, maxTextureHeight);
    m_depthCubeTexture->setSize(maxCubeMapTexureSize, maxCubeMapTexureSize);
}

void ShadowMapManager::createShadowMap(ShadowCastingLight *light)
{
    auto shadowMap = QSharedPointer<ShadowMap>::create(light, light->type() == Qt3DRender::QAbstractLight::PointLight ? m_depthCubeTexture : m_depthTexture);
    shadowMap->updateSceneBounds(m_shadowMapEntity->sceneCenter(), m_shadowMapEntity->sceneRadius());
    m_shadowMaps.insert(light, shadowMap);
    renumberShadowMaps();
    resizeShadowMaps();
    emit shadowMapsChanged(activeShadowMaps());
}

void ShadowMapManager::destroyShadowMap(ShadowCastingLight *light)
{
    auto sm = m_shadowMaps.take(light);
    renumberShadowMaps();
    resizeShadowMaps();
    emit shadowMapsChanged(activeShadowMaps());
}

/*!
 * \internal
 * \class Kuesa::ShadowMapLightDataEntity
 * \brief Entity to get transformed world data needed by ShadowMaps
 * \inmodule Kuesa
 * \since Kuesa 1.3
 * \inherits QObject
 *
 * The ShadowMapLightDataEntity gets added to the scene when shadow-casting lights are present
 * to get transformed world data.
*/

ShadowMapLightDataEntity::ShadowMapLightDataEntity(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
{
    m_frameAction = new Qt3DLogic::QFrameAction(this);
    addComponent(m_frameAction);
    connect(m_frameAction, &Qt3DLogic::QFrameAction::triggered, this, &ShadowMapLightDataEntity::updateSceneInfo);
}

QVector3D ShadowMapLightDataEntity::sceneCenter() const
{
    return m_sceneCenter;
}

float ShadowMapLightDataEntity::sceneRadius() const
{
    return m_sceneRadius;
}

void ShadowMapLightDataEntity::updateSceneInfo()
{
    auto rootEntity = Utils::findBackendRootEntity(this);
    if (!rootEntity)
        return;

    auto toQVector3D = [](const auto &v) { return QVector3D(v.x(), v.y(), v.z()); };

    const auto boundingSphere = rootEntity->worldBoundingVolumeWithChildren();
    const auto sceneCenter = toQVector3D(boundingSphere->center());
    const auto sceneRadius = boundingSphere->radius();
    if (m_sceneRadius != sceneRadius || m_sceneCenter != sceneCenter) {
        m_sceneCenter = sceneCenter;
        m_sceneRadius = sceneRadius;
        emit worldBoundsChanged(m_sceneCenter, m_sceneRadius);
    }
}

QT_END_NAMESPACE
