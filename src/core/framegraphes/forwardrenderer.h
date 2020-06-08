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
#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/tonemappingandgammacorrectioneffect.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/QLayer>
#include <QVector>

class tst_ForwardRenderer;

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QTechniqueFilter;
class QViewport;
class QCameraSelector;
class QRenderSurfaceSelector;
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

class AbstractRenderStage;
class OpaqueRenderStage;
class TransparentRenderStage;
class ZFillRenderStage;

class KUESASHARED_EXPORT ForwardRenderer : public Qt3DRender::QRenderSurfaceSelector
{
    Q_OBJECT
    Q_PROPERTY(QObject *renderSurface READ renderSurface WRITE setRenderSurface NOTIFY renderSurfaceChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(Qt3DCore::QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QColor clearColor READ clearColor WRITE setClearColor NOTIFY clearColorChanged)
    Q_PROPERTY(Qt3DRender::QClearBuffers::BufferType clearBuffers READ clearBuffers WRITE setClearBuffers NOTIFY clearBuffersChanged)
    Q_PROPERTY(bool frustumCulling READ frustumCulling WRITE setFrustumCulling NOTIFY frustumCullingChanged)
    Q_PROPERTY(bool backToFrontSorting READ backToFrontSorting WRITE setBackToFrontSorting NOTIFY backToFrontSortingChanged)
    Q_PROPERTY(bool zFilling READ zFilling WRITE setZFilling NOTIFY zFillingChanged)
    Q_PROPERTY(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm READ toneMappingAlgorithm WRITE setToneMappingAlgorithm NOTIFY toneMappingAlgorithmChanged REVISION 1)
    Q_PROPERTY(float exposure READ exposure WRITE setExposure NOTIFY exposureChanged REVISION 1)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged REVISION 1)
    Q_PROPERTY(bool showDebugOverlay READ showDebugOverlay WRITE setShowDebugOverlay NOTIFY showDebugOverlayChanged REVISION 2)
public:
    ForwardRenderer(Qt3DCore::QNode *parent = nullptr);
    ~ForwardRenderer();

    QObject *renderSurface() const;
    QRectF viewportRect() const;
    Qt3DCore::QEntity *camera() const;
    QColor clearColor() const;
    Qt3DRender::QClearBuffers::BufferType clearBuffers() const;
    bool frustumCulling() const;
    bool backToFrontSorting() const;
    bool zFilling() const;
    float exposure() const;
    float gamma() const;
    ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm() const;
    bool showDebugOverlay() const;

    Q_INVOKABLE void addPostProcessingEffect(Kuesa::AbstractPostProcessingEffect *effect);
    Q_INVOKABLE void removePostProcessingEffect(Kuesa::AbstractPostProcessingEffect *effect);
    QVector<AbstractPostProcessingEffect *> postProcessingEffects() const;

    static Qt3DRender::QAbstractTexture *findRenderTargetTexture(Qt3DRender::QRenderTarget *target, Qt3DRender::QRenderTargetOutput::AttachmentPoint attachmentPoint);
    static bool hasHalfFloatRenderable();

public Q_SLOTS:
    void setRenderSurface(QObject *renderSurface);
    void setViewportRect(const QRectF &viewportRect);
    void setCamera(Qt3DCore::QEntity *camera);
    void setClearColor(const QColor &clearColor);
    void setClearBuffers(Qt3DRender::QClearBuffers::BufferType clearBuffers);
    void setFrustumCulling(bool frustumCulling);
    void setBackToFrontSorting(bool backToFrontSorting);
    void setZFilling(bool zfilling);
    void setGamma(float gamma);
    void setExposure(float exposure);
    void setToneMappingAlgorithm(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm);
    void setShowDebugOverlay(bool showDebugOverlay);

    void addLayer(Qt3DRender::QLayer *layer);
    void removeLayer(Qt3DRender::QLayer *layer);
    QVector<Qt3DRender::QLayer *> layers() const;

Q_SIGNALS:
    void renderSurfaceChanged(QObject *renderSurface);
    void viewportRectChanged(const QRectF &viewportRect);
    void cameraChanged(Qt3DCore::QEntity *camera);
    void clearColorChanged(const QColor &clearColor);
    void clearBuffersChanged(Qt3DRender::QClearBuffers::BufferType clearBuffers);
    void frustumCullingChanged(bool frustumCulling);
    void backToFrontSortingChanged(bool backToFrontSorting);
    void zFillingChanged(bool zFilling);
    void frameGraphTreeReconfigured();
    void gammaChanged(float gamma);
    void exposureChanged(float exposure);
    void toneMappingAlgorithmChanged(ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm);
    void showDebugOverlayChanged(bool showDebugOverlay);

private:
    void updateTextureSizes();
    void handleSurfaceChange();
    QSize currentSurfaceSize() const;

    void scheduleFGTreeRebuild();
    void rebuildFGTree();

    void reconfigureFrameGraph();
    void reconfigureStages();

    AbstractPostProcessingEffect::FrameGraphNodePtr frameGraphSubtreeForPostProcessingEffect(AbstractPostProcessingEffect *effect) const;

    Qt3DRender::QTechniqueFilter *m_noFrustumCullingOpaqueTechniqueFilter;
    Qt3DRender::QTechniqueFilter *m_frustumCullingOpaqueTechniqueFilter;
    Qt3DRender::QTechniqueFilter *m_noFrustumCullingTransparentTechniqueFilter;
    Qt3DRender::QTechniqueFilter *m_frustumCullingTransparentTechniqueFilter;
    Qt3DRender::QViewport *m_viewport;
    Qt3DRender::QCameraSelector *m_cameraSelector;
    Qt3DRender::QClearBuffers *m_clearBuffers;
    Qt3DRender::QNoDraw *m_noDrawClearBuffer;
    Qt3DRender::QFrustumCulling *m_frustumCullingOpaque;
    Qt3DRender::QFrustumCulling *m_frustumCullingTransparent;
    bool m_backToFrontSorting;
    bool m_zfilling;
    QVector<AbstractPostProcessingEffect *> m_userPostProcessingEffects;
    QVector<AbstractPostProcessingEffect *> m_internalPostProcessingEffects;
    QHash<AbstractPostProcessingEffect *, AbstractPostProcessingEffect::FrameGraphNodePtr> m_effectFGSubtrees;

    QVector<QMetaObject::Connection> m_resizeConnections;

    //For rendering scene when useing post-processing effects
    Qt3DRender::QFrameGraphNode *m_renderToTextureRootNode;
    Qt3DRender::QFrameGraphNode *m_effectsRootNode;
    Qt3DRender::QRenderTarget *m_renderTargets[2];
    Qt3DRender::QRenderTarget *m_multisampleTarget;
    Qt3DRender::QBlitFramebuffer *m_blitFramebufferNode;
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    Qt3DRender::QDebugOverlay *m_debugOverlay;
#endif
    bool m_fgTreeRebuiltScheduled;
    QVector<Qt3DRender::QLayer *> m_layers;
    QVector<QMetaObject::Connection> m_layerDestructionConnections;

    class Q_AUTOTEST_EXPORT SceneStages {
    public:
        SceneStages();
        ~SceneStages();

        SceneStages(const SceneStages &) = delete;

        void init();
        void setZFilling(bool zFilling);
        void setBackToFrontSorting(bool backToFrontSorting);
        void setLayer(Qt3DRender::QLayer *layer);

        bool zFilling();
        bool backToFrontSorting();
        Qt3DRender::QLayer *layer() const;

        void unParent();
        void setParent(QNode *opaqueRoot, QNode *transparentRoot);

        Qt3DRender::QLayerFilter *opaqueStagesRoot() const;
        Qt3DRender::QLayerFilter *transparentStagesRoot() const;

        OpaqueRenderStage *opaqueStage() const;
        ZFillRenderStage *zFillStage() const;
        TransparentRenderStage *transparentStage() const;

    private:
        Qt3DRender::QLayerFilter *m_opaqueStagesRoot;
        Qt3DRender::QLayerFilter *m_transparentStagesRoot;
        OpaqueRenderStage *m_opaqueStage;
        TransparentRenderStage *m_transparentStage;
        ZFillRenderStage *m_zFillStage;
        QMetaObject::Connection m_zFillDestroyedConnection;
    };
    using SceneStagesPtr = QSharedPointer<SceneStages>;

    //For controlling render stages
    QVector<SceneStagesPtr> m_sceneStages;

    // GammaCorrection
    ToneMappingAndGammaCorrectionEffect *m_gammaCorrectionFX;

    friend class ::tst_ForwardRenderer;
};
} // namespace Kuesa
QT_END_NAMESPACE

#endif // FORWARDRENDERER_H
