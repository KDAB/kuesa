/*
    dofeffect.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jean-Michaël Celerier <jean-michael.celerier@kdab.com>

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

#ifndef KUESA_DEPTHOFFIELDEFFECT_H
#define KUESA_DEPTHOFFIELDEFFECT_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/abstractpostprocessingeffect.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QRenderTarget;
class QRenderPass;
class QShaderProgram;
class QParameter;
class QFrameGraphNode;
class QRenderPassFilter;
class QAbstractTexture;
class QLayer;
class QCamera;
} // namespace Qt3DRender

namespace Kuesa {

class FullScreenQuad;

class KUESASHARED_EXPORT DepthOfFieldEffect : public AbstractPostProcessingEffect
{
    Q_OBJECT
    Q_PROPERTY(float focusRange READ focusRange WRITE setFocusRange NOTIFY focusRangeChanged)
    Q_PROPERTY(float focusDistance READ focusDistance WRITE setFocusDistance NOTIFY focusDistanceChanged)
    Q_PROPERTY(float radius READ radius WRITE setRadius NOTIFY radiusChanged)
public:
    DepthOfFieldEffect(Qt3DCore::QNode *parent = nullptr);
    ~DepthOfFieldEffect() override;

    float focusRange() const;
    float radius() const;
    float focusDistance() const;

public Q_SLOTS:
    void setFocusRange(float focusRange);
    void setRadius(float radius);
    void setFocusDistance(float focusDistance);

Q_SIGNALS:
    void focusRangeChanged(float focusRange);
    void radiusChanged(float radius);
    void focusDistanceChanged(float focusDistance);

private:
    FrameGraphNodePtr frameGraphSubTree() const override;
    QVector<Qt3DRender::QLayer *> layers() const override;

    void setInputTexture(Qt3DRender::QAbstractTexture *texture) override;
    void setDepthTexture(Qt3DRender::QAbstractTexture *texture) override;
    void setCamera(Qt3DCore::QEntity *camera) override;

    FrameGraphNodePtr m_rootFrameGraphNode;
    Qt3DRender::QLayer *m_layer;

    Qt3DRender::QParameter *m_textureParam;
    Qt3DRender::QParameter *m_textureSizeParam;
    Qt3DRender::QParameter *m_depthParam;
    Qt3DRender::QParameter *m_nearPlaneParam;
    Qt3DRender::QParameter *m_farPlaneParam;
    Qt3DRender::QParameter *m_focusRangeParam;
    Qt3DRender::QParameter *m_focusDistanceParam;
    Qt3DRender::QParameter *m_radiusParam;
    void setWindowSize(const QSize &sceneSize) override;
    float m_focusRange;
    float m_radius;
    float m_focusDistance;

    Qt3DRender::QParameter *m_dofTextureParam;
    Qt3DRender::QAbstractTexture *m_dofTexture = nullptr;
    FullScreenQuad *m_fsQuad = nullptr;
};
} // namespace Kuesa
QT_END_NAMESPACE

#endif // KUESA_DEPTHOFFIELDEFFECT_H
