/*
    gaussianblureffect.h

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

#ifndef KUESA_GAUSSIANBLUREFFECT_H
#define KUESA_GAUSSIANBLUREFFECT_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/abstractpostprocessingeffect.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QRenderPass;
class QParameter;
class QAbstractTexture;
class QRenderTarget;
class QRenderTargetOutput;
class QRenderPassFilter;
class QFrameGraphNode;
} // namespace Qt3DRender

namespace Kuesa {

class GaussianBlurMaterial;

class KUESASHARED_EXPORT GaussianBlurEffect : public AbstractPostProcessingEffect
{
    Q_OBJECT

    Q_PROPERTY(int blurPassCount READ blurPassCount WRITE setBlurPassCount NOTIFY blurPassCountChanged)

public:
    GaussianBlurEffect(Qt3DCore::QNode *parent = nullptr);
    FrameGraphNodePtr frameGraphSubTree() const override;
    QVector<Qt3DRender::QLayer *> layers() const override;

    void setInputTexture(Qt3DRender::QAbstractTexture *texture) override;
    void setSceneSize(const QSize &size) override;
    int blurPassCount() const;

public Q_SLOTS:
    void setBlurPassCount(int blurPassCount);

Q_SIGNALS:
    void blurPassCountChanged(int blurPassCount);

private:
    void createBlurPasses();
    QString passName() const;
    Qt3DRender::QRenderPassFilter *createRenderPassFilter(const QString &name, const QVariant &value = QVariant());
    Qt3DRender::QRenderPass *createBlurPass(int pass);

    FrameGraphNodePtr m_rootFrameGraphNode;
    Qt3DRender::QLayer *m_layer;

    int m_blurPassCount;

    //Textures and targets
    Qt3DRender::QRenderTargetOutput *m_blurTextureOutput1;
    Qt3DRender::QRenderTarget *m_blurTarget1;
    Qt3DRender::QRenderTarget *m_blurTarget2;

    Qt3DRender::QAbstractTexture *m_blurTexture1;
    Qt3DRender::QAbstractTexture *m_blurTexture2;

    Qt3DRender::QFrameGraphNode *m_blurPassRoot;

    Qt3DRender::QParameter *m_blurTextureParam1;
    Qt3DRender::QParameter *m_blurTextureParam2;
    Qt3DRender::QParameter *m_widthParameter;
    Qt3DRender::QParameter *m_heightParameter;
};
} // namespace Kuesa
QT_END_NAMESPACE

#endif // KUESA_GAUSSIANBLUREFFECT_H
