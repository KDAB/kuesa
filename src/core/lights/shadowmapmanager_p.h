/*
    shadowmapmanager_p.h

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

#ifndef KUESA_SHADOWMAPMANAGER_P_H
#define KUESA_SHADOWMAPMANAGER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Kuesa/private/kuesa_global_p.h>
#include <Kuesa/shadowmap.h>
#include <QEntity>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QCamera;

namespace Render {
class Light;
class Entity;
} // namespace Render
} // namespace Qt3DRender

namespace Qt3DLogic {
class QFrameAction;
}

namespace Kuesa {
class ShadowCastingLight;
class ShadowMapLightDataEntity;

class KUESA_PRIVATE_EXPORT ShadowMapManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D sceneCenter READ sceneCenter NOTIFY sceneCenterChanged)
    Q_PROPERTY(float sceneRadius READ sceneRadius NOTIFY sceneRadiusChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *depthTexture READ depthTexture CONSTANT)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *cubeDepthTexture READ cubeDepthTexture CONSTANT)

public:
    explicit ShadowMapManager(QObject *parent = nullptr);

    void addLight(ShadowCastingLight *light);
    void removeLight(ShadowCastingLight *light);
    void setLights(const QVector<ShadowCastingLight *> &lights);
    void setSceneEntity(Qt3DCore::QEntity *sceneEntity);

    QVector<QSharedPointer<ShadowMap>> activeShadowMaps() const;

    QVector3D sceneCenter() const;
    float sceneRadius() const;

    Qt3DRender::QAbstractTexture *depthTexture() const;
    Qt3DRender::QAbstractTexture *cubeDepthTexture() const;

Q_SIGNALS:
    void shadowMapsChanged(const QVector<QSharedPointer<ShadowMap>> &activeShadowMaps);
    void sceneCenterChanged(const QVector3D &sceneCenter);
    void sceneRadiusChanged(float sceneRadius);

private:
    void updateSceneBounds(const QVector3D &sceneCenter, float sceneRadius);
    void destroyShadowMap(ShadowCastingLight *light);
    void createShadowMap(ShadowCastingLight *light);
    void castsShadowChanged();
    void renumberShadowMaps();
    void resizeShadowMaps();

    QSet<ShadowCastingLight *> m_lights;
    QHash<ShadowCastingLight *, QSharedPointer<ShadowMap>> m_shadowMaps;
    ShadowMapLightDataEntity *m_shadowMapEntity = nullptr;

    Qt3DRender::QAbstractTexture *m_depthTexture = nullptr;
    Qt3DRender::QAbstractTexture *m_depthCubeTexture = nullptr;
    bool m_hasCubeMapArrayTextures = false;
};

class KUESASHARED_EXPORT ShadowMapLightDataEntity : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit ShadowMapLightDataEntity(Qt3DCore::QNode *parent = nullptr);
    QVector3D sceneCenter() const;
    float sceneRadius() const;

Q_SIGNALS:
    void worldBoundsChanged(const QVector3D &sceneCenter, float sceneRadius);

private:
    void updateSceneInfo();
    Qt3DLogic::QFrameAction *m_frameAction = nullptr;

    QVector3D m_sceneCenter;
    float m_sceneRadius = 0.0f;
};

} // namespace Kuesa
QT_END_NAMESPACE
Q_DECLARE_METATYPE(Kuesa::ShadowMapManager *)

#endif // KUESA_SHADOWMAPMANAGER_P_H
