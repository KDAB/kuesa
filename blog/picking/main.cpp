/*
    main.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Andrew Hayzen <andrew.hayzen@kdab.com>

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
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <Kuesa/kuesa_global.h>

#ifdef Q_OS_ANDROID
#include <QOpenGLContext>
#endif

int main(int ac, char **av)
{
    Kuesa::setupDefaultSurfaceFormat();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(ac, av);

    QQuickView view;
    view.engine()->rootContext()->setContextProperty(QStringLiteral("_view"), &view);

#ifdef Q_OS_IOS
    view.setFlags(view.flags() | Qt::MaximizeUsingFullscreenGeometryHint);
#endif
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    view.resize(1920, 1080);
    view.show();

    return app.exec();
}
