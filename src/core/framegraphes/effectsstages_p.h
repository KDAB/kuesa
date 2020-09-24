/*
    effectsstages_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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

#ifndef KUESA_EFFECTSSTAGES_P_H
#define KUESA_EFFECTSSTAGES_P_H

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
#include <private/abstractrenderstage_p.h>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <QSize>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QViewport;
class QCameraSelector;
class QRenderTarget;
class QRenderTargetSelector;
class QBlitFramebuffer;
} // Qt3DRender

namespace Kuesa {

class KUESA_PRIVATE_EXPORT EffectsStages : public AbstractRenderStage
{
    Q_OBJECT
public:
    explicit EffectsStages(Qt3DRender::QFrameGraphNode *parent = nullptr);

    void setCamera(Qt3DCore::QEntity *camera);
    Qt3DCore::QEntity *camera() const;

    void setViewport(const QRectF &vp);
    QRectF viewport() const;

    void addEffect(AbstractPostProcessingEffect *effect);
    void removeEffect(AbstractPostProcessingEffect *effect);
    const std::vector<AbstractPostProcessingEffect *> effects() const;

    std::vector<Qt3DRender::QLayer *> layers() const;

    Qt3DRender::QFrameGraphNode *effectFGSubtree(AbstractPostProcessingEffect *fx) const;

    void setWindowSize(const QSize &size);
    QSize windowSize() const;

    void setRenderTargets(Qt3DRender::QRenderTarget *rtA,
                          Qt3DRender::QRenderTarget *rtB);
    std::pair<Qt3DRender::QRenderTarget *,
              Qt3DRender::QRenderTarget *> renderTargets() const;

    void setDepthTexture(Qt3DRender::QAbstractTexture *depthTexture);
    Qt3DRender::QAbstractTexture *depthTexture() const;

    void setPresentToScreen(bool presentToScreen);
    bool presentToScreen() const;

    void setBlitFinalRT(bool blit);
    bool blitFinalRT() const;

private:
    void reconfigure();

    Qt3DRender::QViewport *m_viewport = nullptr;
    Qt3DCore::QEntity *m_camera = nullptr;

    std::vector<AbstractPostProcessingEffect *> m_effects;
    QHash<AbstractPostProcessingEffect *, AbstractPostProcessingEffect::FrameGraphNodePtr> m_effectFGSubtrees;
    std::vector<Qt3DRender::QRenderTargetSelector *> m_rtSelectors;

    Qt3DRender::QRenderTarget *m_rt[2] = {nullptr, nullptr};
    Qt3DRender::QAbstractTexture *m_depthTexture = nullptr;
    Qt3DRender::QBlitFramebuffer *m_blitRt = nullptr;

    QSize m_windowSize;
    bool m_presentToScreen = false;
    bool m_blitFinalRT = false;
};

using EffectsStagesPtr = QSharedPointer<EffectsStages>;

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_EFFECTSSTAGES_P_H
