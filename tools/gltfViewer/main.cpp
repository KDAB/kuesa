/*
    main.cpp

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

#include <QGuiApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QFileOpenEvent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QMimeData>
#include <Qt3DRender/QCamera>
#include <Qt3DQuick/QQmlAspectEngine>
#include <Qt3DAnimation/QAnimationAspect>
#include <Qt3DQuickExtras/Qt3DQuickWindow>
#include <Kuesa/GLTF2Importer>
#include <Kuesa/kuesaversion.h>

class ViewerApplication : public QGuiApplication
{
    Q_OBJECT
public:
    ViewerApplication(int &argc, char **argv, int aflags);
    ~ViewerApplication();

    void setContext(QQmlContext *context) { m_context = context; }
    bool event(QEvent *event) override;

    Q_INVOKABLE void viewAll(Qt3DRender::QCamera *camera);
    Q_INVOKABLE void reload();

signals:
    void nextCamera();
    void previousCamera();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QQmlContext *m_context;
};

ViewerApplication::ViewerApplication(int &argc, char **argv, int aflags = ApplicationFlags)
    : QGuiApplication(argc, argv, aflags)
    , m_context(nullptr)
{
}

ViewerApplication::~ViewerApplication() = default;

bool ViewerApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *fileEvent = static_cast<QFileOpenEvent *>(event);
        if (m_context)
            m_context->setContextProperty("_gltfFile", fileEvent->url());
    }

    return QGuiApplication::event(event);
}

bool ViewerApplication::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::Drop: {
        QDropEvent *de = static_cast<QDropEvent *>(event);
        if (de->mimeData()->urls().size() == 1) {
            QUrl u = de->mimeData()->urls().front();
            if (m_context)
                m_context->setContextProperty("_gltfFile", u);
            de->accept();
            return false;
        }
        break;
    }
    case QEvent::DragEnter: {
        QDragEnterEvent *de = static_cast<QDragEnterEvent *>(event);
        if (de->mimeData()->urls().size() == 1) {
            QUrl u = de->mimeData()->urls().front();
            if (u.scheme() != QLatin1String("file"))
                break;
            QFileInfo fi(u.toLocalFile());
            if (!fi.exists() || fi.suffix().toLower() != "gltf")
                break;
            de->accept();
            return false;
        }
        break;
    }
    case QEvent::Resize: {
        QResizeEvent *re = static_cast<QResizeEvent *>(event);
        if (!re->size().isEmpty() && m_context)
            m_context->setContextProperty("_winSize", re->size());
        break;
    }
    case QEvent::KeyPress: {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F && ke->modifiers() == Qt::ControlModifier) {
            QWindow *w = qobject_cast<QWindow *>(watched);
            if (w->windowState() & Qt::WindowFullScreen)
                w->showNormal();
            else
                w->showFullScreen();
        }
        break;
    }
    default:
        break;
    };

    return QGuiApplication::eventFilter(watched, event);
}

void ViewerApplication::viewAll(Qt3DRender::QCamera *camera)
{
    if (!camera)
        return;

    connect(camera, &Qt3DRender::QCamera::viewMatrixChanged, this, [camera, this]() {
        const QVector3D position = camera->position();
        const QVector3D viewCenter = camera->viewCenter();
        const float sceneExtent = 2.0f * (viewCenter - position).length();
        camera->setFarPlane(10.0f * sceneExtent); // add some headroom to be able to zoom out
        camera->setNearPlane(0.001f * sceneExtent);
        camera->disconnect(this);
    });
    camera->viewAll();
}

void ViewerApplication::reload()
{
    if (!m_context)
        return;
    QString current = m_context->contextProperty("_gltfFile").toString();
    m_context->setContextProperty("_gltfFile", "");
    m_context->setContextProperty("_gltfFile", current);
}

int main(int argc, char *argv[])
{
    {
        // Set OpenGL requirements
        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
#ifndef KUESA_OPENGL_ES_2
        format.setVersion(4, 1);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSamples(4);
#else
        format.setVersion(3, 0);
        format.setProfile(QSurfaceFormat::NoProfile);
        format.setRenderableType(QSurfaceFormat::OpenGLES);
#endif
        QSurfaceFormat::setDefaultFormat(format);
    }

    QCoreApplication::setApplicationName("gltfViewer");
    QCoreApplication::setOrganizationDomain("kdab.com");
    QCoreApplication::setOrganizationName("KDAB");
    QCoreApplication::setApplicationVersion(KUESA_VERSION_STR);

    ViewerApplication app(argc, argv);

    QCommandLineParser cmdline;
    cmdline.addHelpOption();
    cmdline.addVersionOption();
    cmdline.setApplicationDescription(
            QObject::tr("\nThis tool renders glTF 2.0 files.\n"
                        "Example : ./gltfViewer [-f] my_file.gltf"));
    cmdline.addPositionalArgument("source", QCoreApplication::translate("main", "gltf 2.0 file to render."));

    QCommandLineOption fullScreenOption({ "f", "fullScreen" }, QObject::tr("show full screen."));
    QCommandLineOption cameraOption({ "c", "camera" }, QObject::tr("use named camera"), QObject::tr("camera"), QString());
    QCommandLineOption animationsOption({ "p", "play" }, QObject::tr("play named animation"), QObject::tr("animations"), QString());
    QCommandLineOption loopOption({ "l", "loop" }, QObject::tr("loop animations"));
    QCommandLineOption sceneOption({ "s", "scene" }, QObject::tr("index of the glTF scene to load"),
                                   QObject::tr("scene"), QString::number(Kuesa::GLTF2Importer::DefaultScene));
    QCommandLineOption reflectionPlaneOption({ "r", "reflectionPlane" }, QObject::tr("use named reflection plane"),
                                             QObject::tr("plane name"), QString());
    cmdline.addOptions({ fullScreenOption, animationsOption, loopOption, cameraOption, sceneOption, reflectionPlaneOption });

    cmdline.process(app);

    QUrl inputFile;
    const QStringList sources = cmdline.positionalArguments();
    if (!sources.empty()) {
        QFileInfo fi(sources.front());
        if (!fi.exists()) {
            qCritical("Input file not found.\n%s", cmdline.helpText().toLatin1().constData());
            return 1;
        }
        inputFile = QUrl::fromLocalFile(sources.front());
#ifndef Q_OS_MACOS
    } else {
        cmdline.showHelp();
        return 1;
#endif
    }

    Qt3DExtras::Quick::Qt3DQuickWindow view;
    view.setFlags(view.flags() | Qt::WindowFullscreenButtonHint);
    view.registerAspect(new Qt3DAnimation::QAnimationAspect());
    view.installEventFilter(&app);
#ifdef KUESA_BUILD_ROOT
    view.engine()->qmlEngine()->addImportPath(QStringLiteral(KUESA_BUILD_ROOT "/qml"));
#endif

    QQmlContext *context = view.engine()->qmlEngine()->rootContext();
    app.setContext(context);
    context->setContextProperty("_app", &app);
    context->setContextProperty("_gltfFile", inputFile);
    context->setContextProperty("_winSize", QSize(1920, 1080));
    context->setContextProperty("_gltfCamera", cmdline.value(cameraOption));
    context->setContextProperty("_gltfAnimations", cmdline.values(animationsOption));
    context->setContextProperty("_gltfLoopAnimations", cmdline.isSet(loopOption));
    context->setContextProperty("_sceneIndex", cmdline.value(sceneOption));
    context->setContextProperty("_gltfReflectionPlane", cmdline.value(reflectionPlaneOption));
    view.setSource(QUrl("qrc:/main.qml"));

    view.resize(1920, 1080);
    if (cmdline.isSet(fullScreenOption))
        view.showFullScreen();
    else
        view.show();

    return app.exec();
}

#include "main.moc"
