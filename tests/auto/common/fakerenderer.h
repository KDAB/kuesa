/*
    fakerenderer.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_TESTS_FAKERENDERER_H
#define KUESA_TESTS_FAKERENDERER_H

#include <Qt3DRender/private/abstractrenderer_p.h>

class FakeRenderer : public Qt3DRender::Render::AbstractRenderer
{
public:
    void dumpInfo() const override { }
    Qt3DRender::API api() const override { return Qt3DRender::API::OpenGL; }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void setRenderDriver(AbstractRenderer::RenderDriver) override{};
    AbstractRenderer::RenderDriver renderDriver() const override { return Qt3D; }
    void render(bool swapBuffers) override { Q_UNUSED(swapBuffers); }
    std::vector<Qt3DCore::QAspectJobPtr> preRenderingJobs() override { return {}; }
    std::vector<Qt3DCore::QAspectJobPtr> renderBinJobs() override { return {}; }
    bool processMouseEvent(QObject *, QMouseEvent *) override { return false; }
    bool processKeyEvent(QObject *, QKeyEvent *) override { return false; }
    void setRHIContext(QRhi *) override { }
    void setDefaultRHIRenderTarget(QRhiRenderTarget *) override{};
    void setRHICommandBuffer(QRhiCommandBuffer *) override{};
#else
    void doRender(bool swapBuffers) override { Q_UNUSED(swapBuffers); };
    void setPendingEvents(const QList<QPair<QObject *, QMouseEvent> > &, const QList<QKeyEvent> &) override {}
    void loadShader(Qt3DRender::Render::Shader *, Qt3DRender::Render::HShader ) override {};
    void render() override { }
    QVector<Qt3DCore::QAspectJobPtr> preRenderingJobs() override { return {}; }
    QVector<Qt3DCore::QAspectJobPtr> renderBinJobs() override { return {}; }
#endif
    qint64 time() const override { return 0; }
    void setTime(qint64 time) override { Q_UNUSED(time); }
    void setAspect(Qt3DRender::QRenderAspect *aspect) override { m_aspect = aspect; }
    void setNodeManagers(Qt3DRender::Render::NodeManagers *m) override { m_managers = m; }
    void setServices(Qt3DCore::QServiceLocator *services) override { Q_UNUSED(services); }
    void setSurfaceExposed(bool exposed) override { Q_UNUSED(exposed); }
    void setJobsInLastFrame(int jobsInLastFrame) override { Q_UNUSED(jobsInLastFrame); }
    Qt3DRender::QRenderAspect *aspect() const override { return m_aspect; }
    Qt3DRender::Render::NodeManagers *nodeManagers() const override { return m_managers; }
    Qt3DCore::QServiceLocator *services() const override { return nullptr; }
    void initialize() override { }
    void shutdown() override { }
    void releaseGraphicsResources() override { }
    void cleanGraphicsResources() override { }
    bool isRunning() const override { return true; }
    bool shouldRender() const override { return true; }
    void skipNextFrame() override { }
    void jobsDone(Qt3DCore::QAspectManager *manager) override { Q_UNUSED(manager); }
    void setSceneRoot(Qt3DRender::Render::Entity *root) override { Q_UNUSED(root); }
    Qt3DRender::Render::Entity *sceneRoot() const override { return nullptr; }
    Qt3DRender::Render::FrameGraphNode *frameGraphRoot() const override { return nullptr; }
    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override { return nullptr; }
    void setSettings(Qt3DRender::Render::RenderSettings *settings) override { m_settings = settings; }
    Qt3DRender::Render::RenderSettings *settings() const override { return m_settings; }

    void markDirty(Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet changes, Qt3DRender::Render::BackendNode *) override { m_changes |= changes; }
    Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet dirtyBits() override { return m_changes; }
#if defined(QT_BUILD_INTERNAL)
    void clearDirtyBits(Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet changes) override
    {
        m_changes &= ~changes;
    }
#endif
    QVariant executeCommand(const QStringList &) override
    {
        return {};
    }
    QOpenGLContext *shareContext() const override { return nullptr; }
    const Qt3DRender::GraphicsApiFilterData *contextInfo() const override { return nullptr; }

    void setOffscreenSurfaceHelper(Qt3DRender::Render::OffscreenSurfaceHelper *) override{};
    QSurfaceFormat format() override { return {}; }

    void setOpenGLContext(QOpenGLContext *) override { }
    bool accessOpenGLTexture(Qt3DCore::QNodeId, QOpenGLTexture **, QMutex **, bool) override { return false; }
    QSharedPointer<Qt3DRender::Render::RenderBackendResourceAccessor> resourceAccessor() const override { return {}; }

protected:
    Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet m_changes;
    Qt3DRender::QRenderAspect *m_aspect = nullptr;
    Qt3DRender::Render::NodeManagers *m_managers = nullptr;
    Qt3DRender::Render::RenderSettings *m_settings = nullptr;
};

#endif // KUESA_TESTS_FAKERENDERER_H
