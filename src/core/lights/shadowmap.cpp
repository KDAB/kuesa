/*
    shadowmap.cpp

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "shadowmap.h"
#include "directionallight.h"
#include "spotlight.h"
#include "pointlight.h"
#include <QCamera>
#include <QTransform>
#include <Kuesa/private/kuesa_utils_p.h>
#include <cmath>

QT_BEGIN_NAMESPACE

/*!
 * \internal
 * \class Kuesa::ShadowMap
 * \brief Holds the information needed to render a shadowmap for a single light
 * \inmodule Kuesa
 * \since Kuesa 1.3
 * \inherits QObject
 *
 * ShadowMap keeps track of the information needed to render the shadowmap for a particular
 * ShadowCastingLight. It contains a camera positioned at the light to render the scene
 * from the light's point of view. It takes information about the scene and updates the
 * light parameters automatically.
*/

using namespace Kuesa;
using namespace Qt3DRender;

ShadowMap::ShadowMap(ShadowCastingLight *light, Qt3DRender::QAbstractTexture *depthTexture, QObject *parent)
    : QObject(parent)
    , m_light(light)
    , m_depthTexture(depthTexture)
{
    //parent light camera to light so it doesn't get deleted when being reparented to CameraSelector
    m_lightCamera = new QCamera;
    m_lightCamera->setProjectionType(light->type() == QAbstractLight::DirectionalLight ? QCameraLens::OrthographicProjection : QCameraLens::PerspectiveProjection);

    switch (m_light->type()) {
    case QAbstractLight::DirectionalLight: {
        auto directionalLight = qobject_cast<DirectionalLight *>(m_light);
        connect(directionalLight, &DirectionalLight::directionChanged, this, &ShadowMap::updateLightCamera);
        connect(directionalLight, &DirectionalLight::directionModeChanged, this, &ShadowMap::updateLightCamera);
        break;
    }
    case QAbstractLight::SpotLight: {
        auto spotLight = qobject_cast<SpotLight *>(m_light);
        connect(spotLight, &SpotLight::rangeChanged, this, &ShadowMap::updateLightCamera);
        connect(spotLight, &SpotLight::localDirectionChanged, this, &ShadowMap::updateLightCamera);
        connect(spotLight, &SpotLight::outerConeAngleChanged, this, &ShadowMap::updateLightCamera);
        break;
    }
    case QAbstractLight::PointLight:
        auto pointLight = qobject_cast<PointLight *>(m_light);
        connect(pointLight, &PointLight::rangeChanged, this, &ShadowMap::updateLightCamera);
        m_cubeMapLightCameras[QAbstractTexture::CubeMapPositiveX] = new QCamera;
        m_cubeMapLightCameras[QAbstractTexture::CubeMapNegativeX] = new QCamera;
        m_cubeMapLightCameras[QAbstractTexture::CubeMapPositiveY] = new QCamera;
        m_cubeMapLightCameras[QAbstractTexture::CubeMapNegativeY] = new QCamera;
        m_cubeMapLightCameras[QAbstractTexture::CubeMapPositiveZ] = new QCamera;
        m_cubeMapLightCameras[QAbstractTexture::CubeMapNegativeZ] = new QCamera;

        m_cubeMapLightCameras[QAbstractTexture::CubeMapPositiveX]->setUpVector({ 0, -1, 0 });
        m_cubeMapLightCameras[QAbstractTexture::CubeMapNegativeX]->setUpVector({ 0, -1, 0 });
        m_cubeMapLightCameras[QAbstractTexture::CubeMapPositiveY]->setUpVector({ 0, 0, 1 });
        m_cubeMapLightCameras[QAbstractTexture::CubeMapNegativeY]->setUpVector({ 0, 0, -1 });
        m_cubeMapLightCameras[QAbstractTexture::CubeMapPositiveZ]->setUpVector({ 0, -1, 1 });
        m_cubeMapLightCameras[QAbstractTexture::CubeMapNegativeZ]->setUpVector({ 0, -1, -1 });
        break;
    }

    Q_ASSERT(light->entities().size() > 0);

    auto transform = Kuesa::componentFromEntity<Qt3DCore::QTransform>(light->entities().first());
    if (transform) {
        m_worldMatrix = transform->worldMatrix();
        QObject::connect(transform, &Qt3DCore::QTransform::worldMatrixChanged,
                         this, &ShadowMap::updateLightTransform);
    }
    updateLightMatrixUniforms();
}

ShadowMap::~ShadowMap()
{
    delete m_lightCamera;
    for (auto &cam : m_cubeMapLightCameras)
        delete cam;
}

ShadowCastingLight *ShadowMap::light() const
{
    return m_light;
}

QAbstractTexture *ShadowMap::depthTexture() const
{
    return m_depthTexture;
}

QCamera *ShadowMap::lightCamera() const
{
    return m_lightCamera;
}

QVariantList ShadowMap::cubeMapProjectionMatrixList() const
{
    Q_ASSERT(m_cubeMapLightCameras.size() == 6);
    QVariantList matrixList;
    for (int face = QAbstractTexture::CubeMapFace::CubeMapPositiveX; face < QAbstractTexture::CubeMapFace::AllFaces; ++face) {
        const auto &camera = m_cubeMapLightCameras[face];
        matrixList << camera->projectionMatrix() * camera->viewMatrix();
    }
    return matrixList;
}

void ShadowMap::updateLightCamera()
{
    const auto worldPos = m_worldMatrix * QVector3D(0.0f, 0.0f, 0.0f);
    //    qDebug() << "scene bounds = " << m_sceneCenter << ", radius = " << m_sceneRadius;
    //    qDebug() << "Light position = " << worldPos;

    auto updateCameraUpVector = [this](const QVector3D &cameraDirection) {
        const auto dp = QVector3D::dotProduct(cameraDirection.normalized(), QVector3D{ 0, 1, 0 });
        if (std::fabs(dp) > .99f)
            m_lightCamera->setUpVector({ 0, 0, 1 });
    };

    const auto calcNearPlane = [this](float farPlane) { return m_light->nearPlane() > 0.0f ? m_light->nearPlane() : farPlane / 500.0f; };

    switch (m_light->type()) {
    case QAbstractLight::DirectionalLight: {
        // TODO: This renders the entire scene into the shadowmap. Make it so that we only
        // render what's visible to camera rendering the scene.
        auto directionalLight = qobject_cast<DirectionalLight *>(m_light);
        auto worldDir = directionalLight->direction();
        if (directionalLight->directionMode() == DirectionalLight::Local) {
            const auto lightDir = directionalLight->direction();
            worldDir = (m_worldMatrix * QVector4D(lightDir)).toVector3D();
        }

        m_lightCamera->setPosition(m_sceneCenter);
        m_lightCamera->setViewCenter(m_sceneCenter + worldDir);
        m_lightCamera->setLeft(-m_sceneRadius);
        m_lightCamera->setRight(m_sceneRadius);
        m_lightCamera->setBottom(-m_sceneRadius);
        m_lightCamera->setTop(m_sceneRadius);
        m_lightCamera->setNearPlane(-m_sceneRadius + m_light->nearPlane());
        m_lightCamera->setFarPlane(m_sceneRadius);
        updateCameraUpVector(worldDir);
        break;
    }
    case QAbstractLight::SpotLight: {
        auto spotLight = qobject_cast<SpotLight *>(m_light);
        const auto worldDir = (m_worldMatrix * QVector4D(spotLight->localDirection())).toVector3D().normalized();
        m_lightCamera->setPosition(worldPos);
        m_lightCamera->setViewCenter(worldPos + worldDir);
        m_lightCamera->setFieldOfView(spotLight->outerConeAngle() * 2);

        const auto farPlane = [this, &worldPos, &worldDir, spotRange = spotLight->range()]() {
            // For camera far plane, use plane which is tangent to bounding sphere and perpendicular
            // to the light direction. This will be sure to include entire scene in camera's frustum.
            // Need to find distance along light vector to this plane. Distance is scene center to edge
            // (i.e. sceneRadius), plus distance between scene center and light along the light vector
            const auto lightToCtrVec = m_sceneCenter - worldPos;
            // project light-to-center onto light vec
            const auto lightToCtrProjectedDistance = QVector3D::dotProduct(lightToCtrVec, worldDir);
            const auto farPlaneVec = worldDir * (lightToCtrProjectedDistance + m_sceneRadius);
            const auto calculatedDistance = farPlaneVec.length();
            // use lesser of spot light range (if defined) or calculated distance
            return spotRange > 0.0f ? qMin(calculatedDistance, spotRange) : calculatedDistance;
        }();

        m_lightCamera->setNearPlane(calcNearPlane(farPlane));
        m_lightCamera->setFarPlane(farPlane);
        updateCameraUpVector(worldDir);
        break;
    }
    case QAbstractLight::PointLight:
        auto pointLight = qobject_cast<PointLight *>(m_light);
        auto farPlane = pointLight->range();
        if (farPlane == 0.0f) {

            const auto sceneRadiusVec = QVector3D(m_sceneRadius, m_sceneRadius, m_sceneRadius);
            const auto bboxCorner1 = m_sceneCenter - sceneRadiusVec;
            const auto bboxCorner2 = m_sceneCenter + sceneRadiusVec;
            auto maxDimension = [](float currentMax, const QVector3D &vec) {
                return qMax(currentMax, qMax(qMax(vec.x(), vec.y()), vec.z()));
            };
            farPlane = maxDimension(maxDimension(0.0f, worldPos - bboxCorner1), bboxCorner2 - worldPos);
        }
        const auto nearPlane = calcNearPlane(farPlane);

        for (auto camera : m_cubeMapLightCameras) {
            camera->setPosition(worldPos);
            camera->setFarPlane(farPlane);
            camera->setNearPlane(nearPlane);
            camera->setFieldOfView(90);
        }
        m_cubeMapLightCameras[QAbstractTexture::CubeMapPositiveX]->setViewCenter(worldPos + QVector3D{ 1, 0, 0 });
        m_cubeMapLightCameras[QAbstractTexture::CubeMapNegativeX]->setViewCenter(worldPos + QVector3D{ -1, 0, 0 });
        m_cubeMapLightCameras[QAbstractTexture::CubeMapPositiveY]->setViewCenter(worldPos + QVector3D{ 0, 1, 0 });
        m_cubeMapLightCameras[QAbstractTexture::CubeMapNegativeY]->setViewCenter(worldPos + QVector3D{ 0, -1, 0 });
        m_cubeMapLightCameras[QAbstractTexture::CubeMapPositiveZ]->setViewCenter(worldPos + QVector3D{ 0, 0, 1 });
        m_cubeMapLightCameras[QAbstractTexture::CubeMapNegativeZ]->setViewCenter(worldPos + QVector3D{ 0, 0, -1 });
        break;
    }
    updateLightMatrixUniforms();
}

void ShadowMap::updateSceneBounds(const QVector3D &sceneCenter, float sceneRadius)
{
    if (sceneCenter == m_sceneCenter && sceneRadius == m_sceneRadius)
        return;

    m_sceneCenter = sceneCenter;
    m_sceneRadius = sceneRadius;
    updateLightCamera();
}

void ShadowMap::setDepthTextureLayer(int layer)
{
    m_depthTextureLayer = layer;
    m_light->setLightIndex(layer);
}

int ShadowMap::depthTextureLayer() const
{
    return m_depthTextureLayer;
}

void ShadowMap::updateLightTransform(const QMatrix4x4 &worldMatrix)
{
    if (m_worldMatrix == worldMatrix)
        return;

    m_worldMatrix = worldMatrix;
    updateLightCamera();
}

bool ShadowMap::usesCubeMap() const
{
    return m_light->type() == QAbstractLight::PointLight;
}

void ShadowMap::updateLightMatrixUniforms()
{
    const bool isPointLight = m_light->type() == QAbstractLight::PointLight;
    const auto &cam = isPointLight ? m_cubeMapLightCameras[QAbstractTexture::CubeMapPositiveX] : m_lightCamera;
    m_light->setNearFarPlanes(cam->nearPlane(), cam->farPlane());

    // don't need the projection matrix for point lights. Texture lookup done via
    // world direction vector.
    if (!isPointLight)
        m_light->setLightViewProjectionMatrix(m_lightCamera->projectionMatrix() * m_lightCamera->viewMatrix());

    emit lightViewProjectionChanged();
}

QT_END_NAMESPACE
