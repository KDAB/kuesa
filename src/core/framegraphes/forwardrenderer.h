/*
    forwardrenderer.h

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

#ifndef KUESA_FORWARDRENDERER_H
#define KUESA_FORWARDRENDERER_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/view.h>
#include <Kuesa/tonemappingandgammacorrectioneffect.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/QLayer>
#include <QVector>
#include <QVector4D>

class tst_ForwardRenderer;

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QTechniqueFilter;
class QRenderPassFilter;
class QViewport;
class QCameraSelector;
class QRenderSurfaceSelector;
class QRenderTargetSelector;
class QFrustumCulling;
class QNoDraw;
class QSortPolicy;
class QRenderTarget;
class QBlitFramebuffer;
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
class QDebugOverlay;
#endif
class QLayerFilter;
} // namespace Qt3DRender

namespace Kuesa {

class ParticleRenderStage;
class FBOResolver;
class SceneStages;
class ReflectionStages;
class View;

using SceneStagesPtr = QSharedPointer<SceneStages>;
using ReflectionStagesPtr = QSharedPointer<ReflectionStages>;

class KUESASHARED_EXPORT ForwardRenderer : public View
{
    Q_OBJECT
    Q_PROPERTY(QObject *renderSurface READ renderSurface WRITE setRenderSurface NOTIFY renderSurfaceChanged)
    Q_PROPERTY(QColor clearColor READ clearColor WRITE setClearColor NOTIFY clearColorChanged)
    Q_PROPERTY(Qt3DRender::QClearBuffers::BufferType clearBuffers READ clearBuffers WRITE setClearBuffers NOTIFY clearBuffersChanged)
    Q_PROPERTY(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm READ toneMappingAlgorithm WRITE setToneMappingAlgorithm NOTIFY toneMappingAlgorithmChanged)
    Q_PROPERTY(float exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
    Q_PROPERTY(bool showDebugOverlay READ showDebugOverlay WRITE setShowDebugOverlay NOTIFY showDebugOverlayChanged)
    Q_PROPERTY(bool usesStencilMask READ usesStencilMask WRITE setUsesStencilMask NOTIFY usesStencilMaskChanged)

public:
    explicit ForwardRenderer(Qt3DCore::QNode *parent = nullptr);
    ~ForwardRenderer();

    QObject *renderSurface() const;
    QColor clearColor() const;
    Qt3DRender::QClearBuffers::BufferType clearBuffers() const;
    float exposure() const;
    float gamma() const;
    ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm() const;
    bool showDebugOverlay() const;
    bool usesStencilMask() const;

    const std::vector<View *> &views() const;

public Q_SLOTS:
    void setRenderSurface(QObject *renderSurface);
    void setClearColor(const QColor &clearColor);
    void setClearBuffers(Qt3DRender::QClearBuffers::BufferType clearBuffers);
    void setGamma(float gamma);
    void setExposure(float exposure);
    void setToneMappingAlgorithm(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm);
    void setShowDebugOverlay(bool showDebugOverlay);
    void setUsesStencilMask(bool usesStencilMask);

    void addView(View *view);
    void removeView(View *view);

    void dump();

Q_SIGNALS:
    void renderSurfaceChanged(QObject *renderSurface);
    void clearColorChanged(const QColor &clearColor);
    void clearBuffersChanged(Qt3DRender::QClearBuffers::BufferType clearBuffers);
    void frameGraphTreeReconfigured();
    void gammaChanged(float gamma);
    void exposureChanged(float exposure);
    void toneMappingAlgorithmChanged(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm);
    void showDebugOverlayChanged(bool showDebugOverlay);
    void particlesEnabledChanged(bool enabled);
    void usesStencilMaskChanged(bool usesStencilMask);
    void externalRenderTargetSizeChanged(const QSize &renderTargetSize);

private:
    void updateTextureSizes();
    void handleSurfaceChange();
    QSize currentSurfaceSize() const;

    void reconfigureFrameGraph() override;

    void setupRenderTargets(Qt3DRender::QRenderTargetSelector *sceneTargetSelector,
                            size_t fxCount);

    Qt3DRender::QRenderSurfaceSelector *m_surfaceSelector;
    Qt3DRender::QViewport *m_effectsViewport;
    Qt3DRender::QFrameGraphNode *m_stagesRoot;
    Qt3DCore::QEntity *m_camera;
    Qt3DRender::QClearBuffers *m_clearBuffers;
    Qt3DRender::QClearBuffers *m_clearRT0;
    EffectsStagesPtr m_internalFXStages;

    QVector<QMetaObject::Connection> m_resizeConnections;

    //For rendering scene when useing post-processing effects
    Qt3DRender::QRenderTargetSelector *m_renderToTextureRootNode;
    Qt3DRender::QLayerFilter *m_mainSceneLayerFilter;
    Qt3DRender::QRenderTarget *m_renderTargets[2];
    Qt3DRender::QRenderTarget *m_multisampleTarget;
    Qt3DRender::QBlitFramebuffer *m_blitFramebufferNodeFromMSToFBO0;
    Qt3DRender::QBlitFramebuffer *m_blitFramebufferNodeFromFBO0ToFBO1;

    FBOResolver *m_msaaResolver;
    FBOResolver *m_rt0rt1Resolver;
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    Qt3DRender::QDebugOverlay *m_debugOverlay;
#endif
    bool m_fgTreeRebuiltScheduled;

    std::vector<View *> m_views;

    bool m_usesStencilMask;

    // GammaCorrection
    ToneMappingAndGammaCorrectionEffect *m_gammaCorrectionFX;

    friend class ::tst_ForwardRenderer;
};
} // namespace Kuesa
QT_END_NAMESPACE

#endif // FORWARDRENDERER_H
