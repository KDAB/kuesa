/*
    shadowmapstages_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#ifndef KUESA_SHADOWMAPSTAGES_P_H
#define KUESA_SHADOWMAPSTAGES_P_H

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

#include <Kuesa/shadowmap.h>
#include <Kuesa/private/kuesa_global_p.h>
#include <private/scenestages_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QCamera;
class QCameraSelector;
class QRenderTargetOutput;
} // namespace Qt3DRender

namespace Kuesa {

class KUESA_PRIVATE_EXPORT ShadowMapRenderPass : public Qt3DRender::QFrameGraphNode
{
    Q_OBJECT
public:
    ShadowMapRenderPass(Qt3DRender::QFrameGraphNode *parent = nullptr);
    ~ShadowMapRenderPass();

    void setShadowMap(ShadowMapPtr shadowMap);
    void setFrustumCulling(bool frustumCulling);
    void setSkinning(bool skinning);
    void addParameter(Qt3DRender::QParameter *parameter);
    void removeParameter(Qt3DRender::QParameter *parameter);

private:
    void updateLightViewProjection();

    ShadowMapPtr m_shadowMap;
    ScenePass *m_shadowMapRenderPass;

    Qt3DRender::QCamera *m_lightCamera;
    Qt3DRender::QCameraSelector *m_cameraSelector;
    Qt3DRender::QRenderTargetOutput *m_depthOutput;
    Qt3DRender::QClearBuffers *m_clearBuffers;

    Qt3DRender::QParameter *m_cubeMapMatrixParam;
    Qt3DRender::QParameter *m_lightIndexParam;
};

class KUESA_PRIVATE_EXPORT ShadowMapStages : public SceneFeaturedRenderStageBase
{
    Q_OBJECT
public:
    explicit ShadowMapStages(Qt3DRender::QFrameGraphNode *parent = nullptr);
    ~ShadowMapStages();

    QVector<Qt3DRender::QLayer *> layers() const;
    void setShadowMaps(const QVector<ShadowMapPtr> &activeShadowMaps);
    QVector<ShadowMapPtr> shadowMaps() const;

public Q_SLOTS:
    void addLayer(Qt3DRender::QLayer *layer);
    void removeLayer(Qt3DRender::QLayer *layer);

Q_SIGNALS:
    void shadowMapsChanged(const QVector<ShadowMapPtr> &shadowMaps);

protected:
    void reconfigure(const Features features) override;

    Qt3DRender::QLayerFilter *m_layerFilter = nullptr;

    Qt3DRender::QParameter *m_reflectiveEnabledParameter = nullptr;
    Qt3DRender::QParameter *m_reflectivePlaneParameter = nullptr;

    using ShadowMapNodePtr = QSharedPointer<ShadowMapRenderPass>;
    QVector<ShadowMapNodePtr> m_shadowMapPasses;
    QVector<ShadowMapPtr> m_shadowMaps;
};

} // namespace Kuesa
QT_END_NAMESPACE

#endif // KUESA_SCENESTAGES_P_H
