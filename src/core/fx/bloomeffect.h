/*
    bloomeffect.h

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_BLOOMEFFECT_H
#define KUESA_BLOOMEFFECT_H

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

class ThresholdEffect;
class GaussianBlurEffect;
class BloomMaterial;

class KUESASHARED_EXPORT BloomEffect : public AbstractPostProcessingEffect
{
    Q_OBJECT

    Q_PROPERTY(float threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)
    Q_PROPERTY(int blurPassCount READ blurPassCount WRITE setBlurPassCount NOTIFY blurPassCountChanged)

public:
    BloomEffect(Qt3DCore::QNode *parent = nullptr);
    ~BloomEffect() override;
    FrameGraphNodePtr frameGraphSubTree() const override;
    QVector<Qt3DRender::QLayer *> layers() const override;

    void setSceneSize(const QSize &size) override;
    void setInputTexture(Qt3DRender::QAbstractTexture *texture) override;

    float threshold() const;
    int blurPassCount() const;

public Q_SLOTS:
    void setThreshold(float threshold);
    void setBlurPassCount(int blurPassCount);

Q_SIGNALS:
    void thresholdChanged(float threshold);
    void blurPassCountChanged(int blurPassCount);

private:
    Qt3DRender::QRenderTarget *createRenderTarget(Qt3DRender::QAbstractTexture *texture);

    FrameGraphNodePtr m_rootFrameGraphNode;

    Qt3DRender::QAbstractTexture *m_brightTexture;
    Qt3DRender::QAbstractTexture *m_blurredBrightTexture;

    ThresholdEffect *m_thresholdEffect;
    GaussianBlurEffect *m_blurEffect;

    QVector<Qt3DRender::QLayer *> m_layers;

    Qt3DRender::QParameter *m_sceneTextureParam;
    Qt3DRender::QParameter *m_blurredBrightTextureParam;
};
} // namespace Kuesa
QT_END_NAMESPACE

#endif // KUESA_BLOOMEFFECT_H
