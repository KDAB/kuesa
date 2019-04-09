/*
    main.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <qtkuesa-config.h>
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>
#include <QResource>

#ifdef  Q_OS_ANDROID
#include <QOpenGLContext>
#endif

int main(int ac, char **av)
{
    bool isES2 = false;
    {
        // Set OpenGL requirements
        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
#ifndef QT_OPENGL_ES_2
        format.setVersion(4, 1);
        format.setProfile(QSurfaceFormat::CoreProfile);
#else
#ifndef QT_OPENGL_ES_3
        isES2 = true;
#endif
        format.setVersion(3, 0);
        format.setProfile(QSurfaceFormat::NoProfile);
        format.setRenderableType(QSurfaceFormat::OpenGLES);
#endif
        QSurfaceFormat::setDefaultFormat(format);
    }

    QGuiApplication app(ac, av);

    QDir resourceDir(app.applicationDirPath() + QStringLiteral("/resources"));
    QDirIterator it(resourceDir, QDirIterator::IteratorFlag::NoIteratorFlags);
    while (it.hasNext()) {
        QString path = it.next();
        if (!QResource::registerResource(path))
            qWarning() << "Failed to load binary resources: " << path;
        else
            qDebug() << "Loaded binary resources: " << path;
    }

    QCommandLineParser parser;
    parser.setApplicationDescription("KDAB Kuesa Demo");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption cursorDisableOption(QStringList() << "no-cursor"
                                                         << "cursor visibility",
                                           "Make sure no pointer is hidden.");
    parser.addOption(cursorDisableOption);

    QCommandLineOption fullscreenOption(QStringList() << "fullscreen"
                                                      << "Fullscreen option",
                                        "Open the demo in fullscreen");
    parser.addOption(fullscreenOption);

    QCommandLineOption heightScaleOption(QStringList() << "screenHeightScale"
                                                       << "Height Scale UI",
                                         "Scale UI based on screen height rather than DPI (default on iOS and Android)");
    parser.addOption(heightScaleOption);

    parser.process(app);

    if (parser.isSet(cursorDisableOption))
        app.setOverrideCursor(QCursor(Qt::BlankCursor));

    QQuickView view;

#ifdef Q_OS_ANDROID
    const QString assetsPrefix = QStringLiteral("assets:/");

    // Qt builds for android may not define QT_OPENGL_ES_3
    // Therefore we need a runtime check to see whether we can use ES 3.0 or not
    QOpenGLContext ctx;
    ctx.setFormat(QSurfaceFormat::defaultFormat());
    if (ctx.create()) {
        const QSurfaceFormat androidFormat = ctx.format();
        isES2 = (androidFormat.majorVersion() == 2);
    }

#elif defined(Q_OS_IOS)
    const QString assetsPrefix = QString(QStringLiteral("file://%1/Library/Application Support/")).arg(QGuiApplication::applicationDirPath());
#elif defined(Q_OS_OSX)
    const QString assetsPrefix = QString(QStringLiteral("file://%1/../Resources/")).arg(QGuiApplication::applicationDirPath());
#else
    const QString assetsPrefix = QStringLiteral("qrc:/");
#endif

#if defined(KUESA_DRACO_COMPRESSION)
    const QString modelSuffix = QStringLiteral("-draco");
#else
    const QString modelSuffix = QStringLiteral("");
#endif

#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    bool screenHeightScale = true;
#else
    bool screenHeightScale = parser.isSet(heightScaleOption);
#endif

    view.engine()->rootContext()->setContextProperty(QStringLiteral("_isES2"), isES2);
    view.engine()->rootContext()->setContextProperty(QStringLiteral("_view"), &view);
    view.engine()->rootContext()->setContextProperty(QStringLiteral("_screenHeightScale"), screenHeightScale);
    view.engine()->rootContext()->setContextProperty(QStringLiteral("_isFullScreen"), parser.isSet(fullscreenOption));
    view.engine()->rootContext()->setContextProperty(QStringLiteral("_assetsPrefix"), assetsPrefix);
    view.engine()->rootContext()->setContextProperty(QStringLiteral("_modelSuffix"), modelSuffix);

#ifdef Q_OS_IOS
    view.setFlags(view.flags() | Qt::MaximizeUsingFullscreenGeometryHint);
#endif
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    view.resize(1920, 1080);
    if (parser.isSet(fullscreenOption)) {
        view.showFullScreen();
    } else {
        view.show();
    }

    return app.exec();
}
