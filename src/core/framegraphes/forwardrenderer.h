/*
    forwardrenderer.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Qt3DRender/qframegraphnode.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <QVector>

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
} // namespace Qt3DRender

namespace Kuesa {

class AbstractPostProcessingEffect;
class AbstractRenderStage;

class KUESASHARED_EXPORT ForwardRenderer : public Qt3DRender::QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(QObject *renderSurface READ renderSurface WRITE setRenderSurface NOTIFY renderSurfaceChanged)
    Q_PROPERTY(QSize externalRenderTargetSize READ externalRenderTargetSize WRITE setExternalRenderTargetSize NOTIFY externalRenderTargetSizeChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(Qt3DCore::QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QColor clearColor READ clearColor WRITE setClearColor NOTIFY clearColorChanged)
    Q_PROPERTY(Qt3DRender::QClearBuffers::BufferType clearBuffers READ clearBuffers WRITE setClearBuffers NOTIFY clearBuffersChanged)
    Q_PROPERTY(bool frustumCulling READ frustumCulling WRITE setFrustumCulling NOTIFY frustumCullingChanged)
    Q_PROPERTY(bool backToFrontSorting READ backToFrontSorting WRITE setBackToFrontSorting NOTIFY backToFrontSortingChanged)
    Q_PROPERTY(bool zFilling READ zFilling WRITE setZFilling NOTIFY zFillingChanged)

public:
    ForwardRenderer(Qt3DCore::QNode *parent = nullptr);
    ~ForwardRenderer();

    QObject *renderSurface() const;
    QSize externalRenderTargetSize() const;
    QRectF viewportRect() const;
    Qt3DCore::QEntity *camera() const;
    QColor clearColor() const;
    Qt3DRender::QClearBuffers::BufferType clearBuffers() const;
    bool frustumCulling() const;
    bool backToFrontSorting() const;
    bool zFilling() const;

    Q_INVOKABLE void addPostProcessingEffect(AbstractPostProcessingEffect *effect);
    Q_INVOKABLE void removePostProcessingEffect(AbstractPostProcessingEffect *effect);
    QVector<AbstractPostProcessingEffect *> postProcessingEffects() const;

    static Qt3DRender::QAbstractTexture *findRenderTargetTexture(Qt3DRender::QRenderTarget *target, Qt3DRender::QRenderTargetOutput::AttachmentPoint attachment);

    QVector<AbstractRenderStage *> renderStages() const;

public Q_SLOTS:
    void setRenderSurface(QObject *renderSurface);
    void setExternalRenderTargetSize(const QSize &externalRenderTargetSize);
    void setViewportRect(const QRectF &viewportRect);
    void setCamera(Qt3DCore::QEntity *camera);
    void setClearColor(const QColor &clearColor);
    void setClearBuffers(Qt3DRender::QClearBuffers::BufferType clearBuffers);
    void setFrustumCulling(bool frustumCulling);
    void setBackToFrontSorting(bool backToFrontSorting);
    void setZFilling(bool zfilling);

Q_SIGNALS:
    void renderSurfaceChanged(QObject *renderSurface);
    void externalRenderTargetSizeChanged(const QSize &externalRenderTargetSize);
    void viewportRectChanged(const QRectF &viewportRect);
    void cameraChanged(Qt3DCore::QEntity *camera);
    void clearColorChanged(const QColor &clearColor);
    void clearBuffersChanged(Qt3DRender::QClearBuffers::BufferType clearBuffers);
    void frustumCullingChanged(bool frustumCulling);
    void backToFrontSortingChanged(bool backToFrontSorting);
    void zFillingChanged(bool zFilling);
    void frameGraphTreeReconfigured();

private:
    void updateTextureSizes();
    void handleSurfaceChange();
    QSize currentSurfaceSize() const;
    void reconfigureFrameGraph();
    void reconfigureStages();
    Qt3DRender::QRenderTarget *createRenderTarget(bool includeDepth);
    AbstractPostProcessingEffect::FrameGraphNodePtr frameGraphSubtreeForPostProcessingEffect(AbstractPostProcessingEffect *effect) const;

    Qt3DRender::QTechniqueFilter *m_techniqueFilter;
    Qt3DRender::QViewport *m_viewport;
    Qt3DRender::QCameraSelector *m_cameraSelector;
    Qt3DRender::QClearBuffers *m_clearBuffers;
    Qt3DRender::QRenderSurfaceSelector *m_surfaceSelector;
    Qt3DRender::QNoDraw *m_noDrawClearBuffer;
    Qt3DRender::QFrustumCulling *m_frustumCulling;
    bool m_backToFrontSorting;
    bool m_zfilling;
    QVector<AbstractPostProcessingEffect *> m_postProcessingEffects;
    QHash<AbstractPostProcessingEffect *, AbstractPostProcessingEffect::FrameGraphNodePtr> m_effectFGSubtrees;

    QVector<QMetaObject::Connection> m_resizeConnections;

    //For rendering scene when useing post-processing effects
    Qt3DRender::QFrameGraphNode *m_renderToTextureRootNode;
    Qt3DRender::QFrameGraphNode *m_effectsRootNode;
    Qt3DRender::QFrameGraphNode *m_renderStageRootNode;
    Qt3DRender::QRenderTarget *m_renderTargets[2];

    //For controlling render stages
    QVector<AbstractRenderStage *> m_renderStages;
};
} // namespace Kuesa
QT_END_NAMESPACE

#endif // FORWARDRENDERER_H
