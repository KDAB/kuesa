/*
    effectsstages.cpp

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

#include "effectsstages_p.h"

#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QBlitFramebuffer>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QNoDepthMask>

#include <Qt3DCore/private/qnode_p.h>
#include <private/kuesa_utils_p.h>
#include <private/framegraphutils_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

EffectsStages::EffectsStages(Qt3DRender::QFrameGraphNode *parent)
    : AbstractRenderStage(parent)
{
    setObjectName(QLatin1String("ViewFXStage"));
    m_stateSet = new Qt3DRender::QRenderStateSet();

    m_stateSet->addRenderState(new Qt3DRender::QNoDepthMask());
    Qt3DRender::QDepthTest *depthTest = new Qt3DRender::QDepthTest();
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Always);
    m_stateSet->addRenderState(depthTest);

    m_viewport = new Qt3DRender::QViewport(m_stateSet);
    m_viewport->setObjectName(QLatin1String("KuesaFXViewport"));
}

EffectsStages::~EffectsStages()
{
    // unparent the effect subtrees or they'll be deleted twice
    for (auto &framegraph : qAsConst(m_effectFGSubtrees))
        framegraph->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
    delete m_stateSet;
}

void EffectsStages::setCamera(Qt3DCore::QEntity *camera)
{
    if (camera == m_camera)
        return;

    m_camera = camera;

    if (m_camera) {
        if (!m_camera->parent())
            m_camera->setParent(this);

        auto d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(m_camera, &EffectsStages::setCamera, m_camera);
    }

    reconfigure();
}

Qt3DCore::QEntity *EffectsStages::camera() const
{
    return m_camera;
}

void EffectsStages::setViewport(const QRectF &vp)
{
    m_viewport->setNormalizedRect(vp);
}

QRectF EffectsStages::viewport() const
{
    return m_viewport->normalizedRect();
}

void EffectsStages::addEffect(AbstractPostProcessingEffect *effect)
{
    if (Utils::contains(m_effects, effect))
        return;

    // Add effect to vector of registered effects
    m_effects.push_back(effect);

    // Handle destruction of effect
    auto d = Qt3DCore::QNodePrivate::get(this);
    d->registerDestructionHelper(effect, &EffectsStages::removeEffect, m_effects);

    // Add FrameGraph subtree to dedicated subtree of effects
    auto effectFGSubtree = effect->frameGraphSubTree();
    if (!effectFGSubtree.isNull()) {
        // Register FrameGraph Subtree
        m_effectFGSubtrees.insert(effect, effectFGSubtree);
        reconfigure();
    }
}

void EffectsStages::removeEffect(AbstractPostProcessingEffect *effect)
{
    // Remove effect entry
    if (Utils::removeAll(m_effects, effect) <= 0)
        return;

    // unparent FG subtree associated with Effect.
    m_effectFGSubtrees.take(effect)->setParent(Q_NODE_NULLPTR);

    // Stop watching for effect destruction
    auto d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(effect);

    reconfigure();
}

const std::vector<AbstractPostProcessingEffect *> EffectsStages::effects() const
{
    return m_effects;
}

void EffectsStages::clearEffects()
{
    const std::vector<AbstractPostProcessingEffect *> fXs = m_effects;

    for (AbstractPostProcessingEffect *fx: fXs) {
        Utils::removeAll(m_effects, fx);
        m_effectFGSubtrees.take(fx)->setParent(Q_NODE_NULLPTR);
        // Stop watching for effect destruction
        auto d = Qt3DCore::QNodePrivate::get(this);
        d->unregisterDestructionHelper(fx);
    }
    m_effects.clear();
}

std::vector<Qt3DRender::QLayer *> EffectsStages::layers() const
{
    std::vector<Qt3DRender::QLayer *> layers;

    for (AbstractPostProcessingEffect *fx : m_effects) {
        const QVector<Qt3DRender::QLayer *> &fLayers = fx->layers();
        std::copy(fLayers.cbegin(), fLayers.cend(), std::back_inserter(layers));
    }

    return layers;
}

Qt3DRender::QFrameGraphNode *EffectsStages::effectFGSubtree(AbstractPostProcessingEffect *fx) const
{
    return m_effectFGSubtrees.value(fx).data();
}

void EffectsStages::setWindowSize(const QSize &size)
{
    if (size == m_windowSize)
        return;
    m_windowSize = size;
    reconfigure();
}

QSize EffectsStages::windowSize() const
{
    return m_windowSize;
}

void EffectsStages::setRenderTargets(Qt3DRender::QRenderTarget *rtA, Qt3DRender::QRenderTarget *rtB)
{
    if (rtA == m_rt[0] && rtB == m_rt[1])
        return;

    disconnect(m_rt[0]);
    disconnect(m_rt[1]);

    m_rt[0] = rtA;
    m_rt[1] = rtB;

    if (m_rt[0])
        QObject::connect(m_rt[0], &Qt3DRender::QRenderTarget::nodeDestroyed, this, [this] { m_rt[0] = nullptr; reconfigure(); });

    if (m_rt[1])
        QObject::connect(m_rt[1], &Qt3DRender::QRenderTarget::nodeDestroyed, this, [this] { m_rt[1] = nullptr; reconfigure(); });

    reconfigure();
}

std::pair<Qt3DRender::QRenderTarget *, Qt3DRender::QRenderTarget *> EffectsStages::renderTargets() const
{
    return {m_rt[0], m_rt[1]};
}

void EffectsStages::setDepthTexture(Qt3DRender::QAbstractTexture *depthTexture)
{
    if (depthTexture == m_depthTexture)
        return;
    disconnect(m_depthTexture);
    m_depthTexture = depthTexture;
    if (m_depthTexture)
        QObject::connect(m_depthTexture, &Qt3DRender::QRenderTarget::nodeDestroyed, this, [this] { setDepthTexture(nullptr); });
    reconfigure();
}

Qt3DRender::QAbstractTexture *EffectsStages::depthTexture() const
{
    return m_depthTexture;
}

Qt3DRender::QAbstractTexture *EffectsStages::finalColorTexture() const
{
    return FrameGraphUtils::findRenderTargetTexture(m_rt[m_finalRTIndex],
                                                    Qt3DRender::QRenderTargetOutput::Color0);
}

void EffectsStages::setPresentToScreen(bool presentToScreen)
{
    if (presentToScreen == m_presentToScreen)
        return;
    m_presentToScreen = presentToScreen;
    reconfigure();
}

bool EffectsStages::presentToScreen() const
{
    return m_presentToScreen;
}

void EffectsStages::setBlitFinalRT(bool blit)
{
    if (blit == m_blitFinalRT)
        return;
    m_blitFinalRT = blit;
    reconfigure();
}

bool EffectsStages::blitFinalRT() const
{
    return m_blitFinalRT;
}

void EffectsStages::reconfigure()
{
    m_stateSet->setParent(Q_NODE_NULLPTR);

    for (AbstractPostProcessingEffect *fx : m_effects)
        m_effectFGSubtrees[fx]->setParent(Q_NODE_NULLPTR);

    for (Qt3DRender::QRenderTargetSelector *rtS : m_rtSelectors)
        rtS->setParent(Q_NODE_NULLPTR);

    if (m_blitRt) {
        m_blitRt->setParent(Q_NODE_NULLPTR);
        if (!m_blitFinalRT) {
            delete  m_blitRt;
            m_blitRt = nullptr;
        }
    }

    if (!m_rt[0])
        return;

    if (m_effects.size() && !m_presentToScreen && !m_rt[1])
        return;

    const int targetSelectorCount = std::max(int(m_effects.size() - int(m_presentToScreen)), 0);
    const int diff = m_rtSelectors.size() - targetSelectorCount;

    // Add or remove Target Selectors we will make use of
    if (diff > 0) {
        // We need to destroy RT selector we don't need
        for (int i = diff; i > 0; --i) {
            auto it = --(m_rtSelectors.end());
            delete *it;
            m_rtSelectors.erase(it);
        }
    } else if (diff < 0) {
        // We need to add new RT selectors
        for (int i = 0; i > diff; --i)
            m_rtSelectors.push_back(new Qt3DRender::QRenderTargetSelector());
    }

    int previousRenderTargetIndex = 0;
    for (size_t i = 0, m = m_effects.size(); i < m; ++i) {
        const int currentRenderTargetIndex = 1 - previousRenderTargetIndex;
        Qt3DRender::QRenderTarget *previousRenderTarget = m_rt[previousRenderTargetIndex];
        Qt3DRender::QRenderTarget *currentRenderTarget = m_rt[currentRenderTargetIndex];
        AbstractPostProcessingEffect *fx = m_effects[i];
        fx->setInputTexture(FrameGraphUtils::findRenderTargetTexture(previousRenderTarget, Qt3DRender::QRenderTargetOutput::Color0));
        fx->setDepthTexture(m_depthTexture);
        fx->setCamera(m_camera);
        fx->setWindowSize(m_windowSize);

        // Parent last effect to render to screen by default
        Qt3DRender::QFrameGraphNode *fxFGParent = m_viewport;

        // Use render target selectors for all but last effect (if m_presentToScreen set)
        if (i < (m - 1) || !m_presentToScreen) {
            Qt3DRender::QRenderTargetSelector *renderTargetSelector = m_rtSelectors[i];
            renderTargetSelector->setTarget(currentRenderTarget);
            renderTargetSelector->setObjectName(QStringLiteral("RenderToTexture %1").arg(currentRenderTargetIndex));
            renderTargetSelector->setParent(this);
            fxFGParent = renderTargetSelector;
        }

        // Blit currentRT into previousRt for the last FX. This is needed to handle
        // the case where we have different views with different FX
        if (i == (m - 1) && m_blitFinalRT && !m_presentToScreen) {
            if (m_blitRt == nullptr) {
                m_blitRt = new Qt3DRender::QBlitFramebuffer();
                auto noDraw = new Qt3DRender::QNoDraw(m_blitRt);
                Q_UNUSED(noDraw);
            }
            m_blitRt->setParent(this);
            m_blitRt->setSource(currentRenderTarget);
            m_blitRt->setDestination(previousRenderTarget);
            m_blitRt->setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
            m_blitRt->setDestinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

            const QRect blitRect(QPoint(), m_windowSize);
            m_blitRt->setSourceRect(blitRect);
            m_blitRt->setDestinationRect(blitRect);
        }

        m_effectFGSubtrees[fx]->setParent(fxFGParent);

        // Flip previousRenderTargetIndex
        previousRenderTargetIndex = currentRenderTargetIndex;
    }

    // Record final RT index to make it easier to retrieve the final color attachment
    m_finalRTIndex = previousRenderTargetIndex;

    if (m_presentToScreen)
        m_stateSet->setParent(this);
}

} // namespace Kuesa

QT_END_NAMESPACE
