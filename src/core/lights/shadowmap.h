/*
    shadowmap.h

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

#ifndef KUESA_SHADOWMAP_H
#define KUESA_SHADOWMAP_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/shadowcastinglight.h>
#include <Qt3DRender/qtexture.h>
#include <QVector3D>
#include <QMatrix4x4>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEntity;
}

namespace Qt3DRender {
class QAbstractTexture;
class QCamera;
} // namespace Qt3DRender

namespace Kuesa {

class KUESASHARED_EXPORT ShadowMap : public QObject
{
    Q_OBJECT
public:
    ShadowMap(ShadowCastingLight *light, Qt3DRender::QAbstractTexture *depthTexture, QObject *parent = nullptr);
    ~ShadowMap();

    ShadowCastingLight *light() const;

    Qt3DRender::QAbstractTexture *depthTexture() const;

    Qt3DRender::QCamera *lightCamera() const;

    QVariantList cubeMapProjectionMatrixList() const;

    bool usesCubeMap() const;

    void updateSceneBounds(const QVector3D &sceneCenter, float sceneRadius);

    void setDepthTextureLayer(int layer);
    int depthTextureLayer() const;

Q_SIGNALS:
    void lightViewProjectionChanged();

private:
    void updateLightCamera();
    void updateLightTransform(const QMatrix4x4 &worldMatrix);
    void updateLightMatrixUniforms();

    ShadowCastingLight *m_light = nullptr;
    Qt3DRender::QCamera *m_lightCamera = nullptr;

    QMatrix4x4 m_worldMatrix;
    QVector3D m_sceneCenter;
    float m_sceneRadius = 0.0f;
    QHash<int, Qt3DRender::QCamera *> m_cubeMapLightCameras;
    Qt3DRender::QAbstractTexture *m_depthTexture = nullptr;
    int m_depthTextureLayer = -1;
};

using ShadowMapPtr = QSharedPointer<ShadowMap>;

} // namespace Kuesa
QT_END_NAMESPACE
Q_DECLARE_METATYPE(Kuesa::ShadowMap *)

#endif // KUESA_SHADOWMAP_H
