/*
    view.h

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

#ifndef KUESA_VIEW_H
#define KUESA_VIEW_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/tonemappingandgammacorrectioneffect.h>
#include <Kuesa/shadowmap.h>
#include <QFlags>
#include <QRectF>
#include <QVector4D>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QAbstractTexture>

class tst_View;
class tst_ForwardRenderer;

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QRenderTargetSelector;
class QNoDraw;
class QBlitFramebuffer;
class QRenderTarget;
class QLayerFilter;
class QCameraSelector;
class QRenderSurfaceSelector;
class QViewport;
class QClearBuffers;
}

namespace Kuesa {

class AbstractPostProcessingEffect;
class SceneStages;
class ReflectionStages;
class EffectsStages;
class ReflectionPlane;
class FBOResolver;
class ShadowMapStages;

using SceneStagesPtr = QSharedPointer<SceneStages>;
using ShadowMapStagesPtr = QSharedPointer<ShadowMapStages>;
using ReflectionStagesPtr = QSharedPointer<ReflectionStages>;
using EffectsStagesPtr = QSharedPointer<EffectsStages>;
using ShadowMapPtr = ShadowMapPtr;

class KUESASHARED_EXPORT View : public Qt3DRender::QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(Qt3DCore::QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(bool frustumCulling READ frustumCulling WRITE setFrustumCulling NOTIFY frustumCullingChanged)
    Q_PROPERTY(bool skinning READ skinning WRITE setSkinning NOTIFY skinningChanged)
    Q_PROPERTY(bool backToFrontSorting READ backToFrontSorting WRITE setBackToFrontSorting NOTIFY backToFrontSortingChanged)
    Q_PROPERTY(bool zFilling READ zFilling WRITE setZFilling NOTIFY zFillingChanged)
    Q_PROPERTY(bool particlesEnabled READ particlesEnabled WRITE setParticlesEnabled NOTIFY particlesEnabledChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *reflectionTexture READ reflectionTexture NOTIFY reflectionTextureChanged)
    Q_PROPERTY(QSize reflectionTextureSize READ reflectionTextureSize WRITE setReflectionTextureSize NOTIFY reflectionTextureSizeChanged)
    Q_PROPERTY(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm READ toneMappingAlgorithm WRITE setToneMappingAlgorithm NOTIFY toneMappingAlgorithmChanged)
    Q_PROPERTY(bool usesStencilMask READ usesStencilMask WRITE setUsesStencilMask NOTIFY usesStencilMaskChanged)
    Q_PROPERTY(float exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
    Q_PROPERTY(QColor clearColor READ clearColor WRITE setClearColor NOTIFY clearColorChanged)

public:
    explicit View(Qt3DCore::QNode *parent = nullptr);
    ~View();

    QRectF viewportRect() const;
    Qt3DCore::QEntity *camera() const;
    bool frustumCulling() const;
    bool skinning() const;
    bool backToFrontSorting() const;
    bool zFilling() const;
    bool particlesEnabled() const;
    Qt3DRender::QAbstractTexture *reflectionTexture() const;
    QSize reflectionTextureSize() const;
    QColor clearColor() const;
    float exposure() const;
    float gamma() const;
    ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm() const;
    bool usesStencilMask() const;

    const std::vector<AbstractPostProcessingEffect *> &postProcessingEffects() const;
    const std::vector<Qt3DRender::QLayer *> &layers() const;
    const std::vector<ReflectionPlane *> &reflectionPlanes() const;

    QVector<ShadowMapPtr> shadowMaps() const;

public Q_SLOTS:
    void setViewportRect(const QRectF &viewportRect);
    void setCamera(Qt3DCore::QEntity *camera);
    void setFrustumCulling(bool frustumCulling);
    void setSkinning(bool frustumCulling);
    void setBackToFrontSorting(bool backToFrontSorting);
    void setZFilling(bool zfilling);
    void setParticlesEnabled(bool enabled);
    void setReflectionTextureSize(const QSize &reflectionTextureSize);
    void setClearColor(const QColor &clearColor);
    void setGamma(float gamma);
    void setExposure(float exposure);
    void setToneMappingAlgorithm(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm);
    void setUsesStencilMask(bool usesStencilMask);

    void addPostProcessingEffect(AbstractPostProcessingEffect *effect);
    void removePostProcessingEffect(AbstractPostProcessingEffect *effect);

    void addLayer(Qt3DRender::QLayer *layer);
    void removeLayer(Qt3DRender::QLayer *layer);

    void addReflectionPlane(ReflectionPlane *plane);
    void removeReflectionPlane(ReflectionPlane *plane);

    void setShadowMaps(const QVector<ShadowMapPtr> &activeShadowMaps);

    void dump();

Q_SIGNALS:
    void viewportRectChanged(const QRectF &viewportRect);
    void cameraChanged(Qt3DCore::QEntity *camera);
    void frustumCullingChanged(bool frustumCulling);
    void skinningChanged(bool skinning);
    void backToFrontSortingChanged(bool backToFrontSorting);
    void zFillingChanged(bool zFilling);
    void particlesEnabledChanged(bool enabled);
    void reflectionTextureChanged(Qt3DRender::QAbstractTexture *reflectionTexture);
    void reflectionTextureSizeChanged(const QSize &reflectionTextureSize);
    void clearColorChanged(const QColor &clearColor);
    void gammaChanged(float gamma);
    void shadowMapsChanged(const QVector<ShadowMapPtr> &shadowMaps);
    void exposureChanged(float exposure);
    void toneMappingAlgorithmChanged(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm);
    void usesStencilMaskChanged(bool usesStencilMask);
    void frameGraphTreeReconfigured();

protected:
    void scheduleFGTreeRebuild();
    void rebuildFGTree();

    virtual void reconfigureStages();
    virtual void reconfigureFrameGraph();

    View *rootView() const;

    AbstractPostProcessingEffect::FrameGraphNodePtr frameGraphSubtreeForPostProcessingEffect(AbstractPostProcessingEffect *effect) const;

    enum Feature {
        BackToFrontSorting = (1 << 0),
        Skinning           = (1 << 1),
        FrustumCulling     = (1 << 2),
        ZFilling           = (1 << 3),
        Particles          = (1 << 4),
    };
    Q_DECLARE_FLAGS(Features, Feature)

private:
    void reconfigureFrameGraphHelper(Qt3DRender::QFrameGraphNode *fgRoot);

    void setSurfaceSize(const QSize &size);
    QSize surfaceSize() const;
    QSize currentTargetSize() const;

    SceneStagesPtr m_sceneStages;
    ShadowMapStagesPtr m_shadowMapStages;
    ReflectionStagesPtr m_reflectionStages;
    EffectsStagesPtr m_fxStages; // User Specified FX
    EffectsStagesPtr m_internalFXStages; // Mandatory FX (ToneMapping)

    Qt3DCore::QEntity *m_camera = nullptr;
    QRectF m_viewport = QRectF(0.0f, 0.0f, 1.0f, 1.0f);

    std::vector<Qt3DRender::QLayer *> m_layers;
    std::vector<AbstractPostProcessingEffect *> m_fxs;

    std::vector<ReflectionPlane *> m_reflectionPlanes;

    Features m_features = Features(FrustumCulling);
    bool m_fgTreeRebuiltScheduled = false;
    bool m_usesStencilMask = false;

    // GammaCorrection
    ToneMappingAndGammaCorrectionEffect *m_gammaCorrectionFX;
    QSize m_surfaceSize;

    struct ViewForward {
        // Interface
        explicit ViewForward(View *v);
        void setClearColor(const QColor &color);
        QColor clearColor();
        void reconfigure(Qt3DRender::QFrameGraphNode *fgRoot);

        // Implementation Details (kept public for now)
        void setupRenderTargets(Qt3DRender::QRenderTargetSelector *sceneTargetSelector,
                                size_t fxCount);
        void updateTextureSizes();

        Qt3DRender::QRenderTargetSelector *m_renderToTextureRootNode = nullptr;
        Qt3DRender::QClearBuffers *m_clearRT0 = nullptr;
        Qt3DRender::QLayerFilter *m_mainSceneLayerFilter = nullptr;
        Qt3DRender::QRenderTarget *m_renderTargets[2] = {nullptr, nullptr};
        Qt3DRender::QRenderTarget *m_multisampleTarget = nullptr;
        Qt3DRender::QBlitFramebuffer *m_blitFramebufferNodeFromMSToFBO0 = nullptr;
        Qt3DRender::QBlitFramebuffer *m_blitFramebufferNodeFromFBO0ToFBO1 = nullptr;
        FBOResolver *m_msaaResolver = nullptr;
        FBOResolver *m_rt0rt1Resolver = nullptr;

        View *m_view = nullptr;
        const bool m_usesRHI = false;
    };
    ViewForward *m_fg = nullptr;

    friend class ::tst_View;
    friend class ::tst_ForwardRenderer;
    friend class ForwardRenderer;
    friend class ViewResolver;
};

} // Kuesa

QT_END_NAMESPACE

#endif // KUESA_VIEW_H
