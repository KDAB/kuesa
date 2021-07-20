/*
    window.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#include "window.h"
#include "view3dscene.h"

#include <Qt3DCore/qaspectengine.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/qrendersettings_p.h>
#include <Qt3DInput/qinputaspect.h>
#include <Qt3DInput/qinputsettings.h>
#include <Qt3DLogic/qlogicaspect.h>
#include <Qt3DAnimation/qanimationaspect.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/qcoreaspect.h>
#endif

#include <QOpenGLContext>
#include <QtGui/private/qwindow_p.h>
#include <Qt3DRender/private/vulkaninstance_p.h>

QT_USE_NAMESPACE

using namespace KuesaUtils;

/*!
    \class KuesaUtils::Window
    \brief Window is a convenience wrapper aimed at simplifying the creation of a window ready
    to be used with Kuesa's \l {KuesaUtils::View3DScene}.

    \inmodule KuesaUtils
    \since Kuesa 1.3
    \inherits QWindow

*/

namespace {

void setupWindowSurface(QWindow *window, Qt3DRender::API api) noexcept
{
    // If the user pass an API through the environment, we use that over the one passed as argument.
    const auto userRequestedApi = qgetenv("QSG_RHI_BACKEND").toLower();
    if (!userRequestedApi.isEmpty()) {
        if (userRequestedApi == QByteArrayLiteral("opengl") ||
            userRequestedApi == QByteArrayLiteral("gl") ||
            userRequestedApi == QByteArrayLiteral("gles2")) {
            api = Qt3DRender::API::OpenGL;
        } else if (userRequestedApi == QByteArrayLiteral("vulkan")) {
            api = Qt3DRender::API::Vulkan;
        } else if (userRequestedApi == QByteArrayLiteral("metal")) {
            api = Qt3DRender::API::Metal;
        } else if (userRequestedApi == QByteArrayLiteral("d3d11")) {
            api = Qt3DRender::API::DirectX;
        } else if (userRequestedApi == QByteArrayLiteral("null")) {
            api = Qt3DRender::API::Null;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        } else if (userRequestedApi == QByteArrayLiteral("auto")) {
            api = Qt3DRender::API::RHI;
#endif
        }
    }

    // We have to set the environment so that the backend is able to read it.
    switch (api) {
    case Qt3DRender::API::OpenGL:
        qputenv("QSG_RHI_BACKEND", "opengl");
        window->setSurfaceType(QSurface::OpenGLSurface);
        break;
    case Qt3DRender::API::DirectX:
        qputenv("QSG_RHI_BACKEND", "d3d11");
        window->setSurfaceType(QSurface::OpenGLSurface);
        break;
    case Qt3DRender::API::Null:
        qputenv("QSG_RHI_BACKEND", "null");
        window->setSurfaceType(QSurface::OpenGLSurface);
        break;
    case Qt3DRender::API::Metal:
        qputenv("QSG_RHI_BACKEND", "metal");
        window->setSurfaceType(QSurface::MetalSurface);
        break;
    case Qt3DRender::API::Vulkan: {
        qputenv("QSG_RHI_BACKEND", "vulkan");
        window->setSurfaceType(QSurface::VulkanSurface);
#if QT_VERSION >= QT_VERSION_CHECK(6, 1, 0)
    #if QT_CONFIG(qt3d_vulkan)
            window->setVulkanInstance(&Qt3DRender::staticVulkanInstance());
    #endif
#endif
        break;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    case Qt3DRender::API::RHI:
#endif
    default:
        break;
    }

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
#if QT_CONFIG(opengles2)
    format.setRenderableType(QSurfaceFormat::OpenGLES);
#else
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    } else
#endif
    if (!userRequestedApi.isEmpty()) {
        // This is used for RHI
        format.setVersion(1, 0);
    }

    format.setDepthBufferSize(24);
    format.setSamples(4);
    format.setStencilBufferSize(8);
    window->setFormat(format);
    QSurfaceFormat::setDefaultFormat(format);
}

} // namespace

Window::Window(QScreen *screen, Qt3DRender::API api)
    : QWindow(screen)
    , m_aspectEngine(new Qt3DCore::QAspectEngine)
    , m_initialized(false)
    , m_scene(new View3DScene)
{
    setupWindowSurface(this, api);
    resize(1024, 768);

    auto d = static_cast<QWindowPrivate *>(QObjectPrivate::get(this));
    if (!d->parentWindow)
        d->connectToScreen(screen ? screen : d->topLevelScreen.data());

    m_scene->frameGraph()->setRenderSurface(this);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_aspectEngine->registerAspect(new Qt3DCore::QCoreAspect);
#endif
    m_aspectEngine->registerAspect(new Qt3DRender::QRenderAspect);
    m_aspectEngine->registerAspect(new Qt3DLogic::QLogicAspect);
    m_aspectEngine->registerAspect(new Qt3DInput::QInputAspect);
    m_aspectEngine->registerAspect(new Qt3DAnimation::QAnimationAspect);

    Q_ASSERT(!m_scene->componentsOfType<Qt3DRender::QRenderSettings>().empty());
    m_renderSettings = m_scene->componentsOfType<Qt3DRender::QRenderSettings>().front();

    Q_ASSERT(!m_scene->componentsOfType<Qt3DInput::QInputSettings>().empty());
    auto inputSetting = m_scene->componentsOfType<Qt3DInput::QInputSettings>().front();
    inputSetting->setEventSource(this);

    connect(m_scene, &View3DScene::loadedChanged, this, [this](bool loaded) {
        if (loaded)
            updateAspectRatio();
    });
}

Window::~Window()
{
    delete m_aspectEngine;
}

View3DScene *Window::scene() const
{
    return m_scene;
}

void Window::showEvent(QShowEvent *e)
{
    if (!m_initialized) {
        m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(m_scene));
        m_initialized = true;
    }

    QWindow::showEvent(e);
}

void Window::resizeEvent(QResizeEvent *e)
{
    QWindow::resizeEvent(e);
    updateAspectRatio();
}

bool Window::event(QEvent *e)
{
    const bool needsRedraw = (e->type() == QEvent::Expose || e->type() == QEvent::UpdateRequest);
    if (needsRedraw && m_renderSettings->renderPolicy() == Qt3DRender::QRenderSettings::OnDemand) {
        Qt3DRender::QRenderSettingsPrivate *p = static_cast<Qt3DRender::QRenderSettingsPrivate *>(
                Qt3DCore::QNodePrivate::get(m_renderSettings));
        p->invalidateFrame();
    }

    return QWindow::event(e);
}

void Window::updateAspectRatio()
{
    Qt3DRender::QCamera *camera = qobject_cast<decltype(camera)>(m_scene->frameGraph()->camera());
    if (camera)
        camera->setAspectRatio(float(width()) / float(height()));
}
