/*
    main.cpp

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

#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "screencontroller.h"

namespace {

void registerQMLTypes()
{
    qmlRegisterUncreatableType<AbstractScreenController>("Drill", 1, 0, "AbstractScreenController", QStringLiteral("AbstractScreenController is abstract"));
    qmlRegisterUncreatableType<PartLabel>("Drill", 1, 0, "PartLabel", QStringLiteral("PartLabel is not instantiable"));
    qmlRegisterType<ScreenController>("Drill", 1, 0, "ScreenController");
}

} // namespace

int main(int ac, char **av)
{
    // Set OpenGL requirements
    Kuesa::setupDefaultSurfaceFormat();

    // Set QtQuick Controls Material Style
    QQuickStyle::setStyle("Material");

    // Set flag for High DPI support on Qt 5.15
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(ac, av);

    // Register QML Types
    registerQMLTypes();

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
