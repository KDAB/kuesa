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
class SceneStages;
class ReflectionStages;
class View;
class ViewResolver;

using SceneStagesPtr = QSharedPointer<SceneStages>;
using ReflectionStagesPtr = QSharedPointer<ReflectionStages>;

class KUESASHARED_EXPORT ForwardRenderer : public View
{
    Q_OBJECT
    Q_PROPERTY(QObject *renderSurface READ renderSurface WRITE setRenderSurface NOTIFY renderSurfaceChanged)
    Q_PROPERTY(Qt3DRender::QClearBuffers::BufferType clearBuffers READ clearBuffers WRITE setClearBuffers NOTIFY clearBuffersChanged)
    Q_PROPERTY(bool showDebugOverlay READ showDebugOverlay WRITE setShowDebugOverlay NOTIFY showDebugOverlayChanged)

public:
    explicit ForwardRenderer(Qt3DCore::QNode *parent = nullptr);
    ~ForwardRenderer();

    QObject *renderSurface() const;
    Qt3DRender::QClearBuffers::BufferType clearBuffers() const;

    bool showDebugOverlay() const;
    const std::vector<View *> &views() const;

public Q_SLOTS:
    void setRenderSurface(QObject *renderSurface);
    void setClearBuffers(Qt3DRender::QClearBuffers::BufferType clearBuffers);
    void setShowDebugOverlay(bool showDebugOverlay);

    void addView(View *view);
    void removeView(View *view);

    void dump();

Q_SIGNALS:
    void renderSurfaceChanged(QObject *renderSurface);
    void clearBuffersChanged(Qt3DRender::QClearBuffers::BufferType clearBuffers);
    void showDebugOverlayChanged(bool showDebugOverlay);
    void externalRenderTargetSizeChanged(const QSize &renderTargetSize);

private:
    void handleSurfaceChange();
    QSize currentSurfaceSize() const;
    void updateTextureSizes();

    void reconfigureFrameGraph() override;

    Qt3DRender::QRenderSurfaceSelector *m_surfaceSelector;
    Qt3DRender::QClearBuffers *m_clearBuffers;

    QVector<QMetaObject::Connection> m_resizeConnections;

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    Qt3DRender::QDebugOverlay *m_debugOverlay;
#endif
    bool m_fgTreeRebuiltScheduled;

    std::vector<View *> m_views;
    std::vector<ViewResolver *> m_viewRenderers;

    friend class ::tst_ForwardRenderer;
};
} // namespace Kuesa
QT_END_NAMESPACE

#endif // FORWARDRENDERER_H
