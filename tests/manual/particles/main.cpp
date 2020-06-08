/*
    main.cpp

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mauro Persano <mauro.persano@kdab.com>

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

#include <QApplication>
#include <QQuickView>
#include <QSurfaceFormat>
#include <QQmlEngine>
#include <QQmlContext>

#include <KuesaUtils/orbitcameracontroller.h>

#include "controllerwidget.h"
#include "scenecontroller.h"

int main(int argc, char *argv[])
{
    QSurfaceFormat f;
    f.setVersion(4, 3);
    f.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(f);

    QApplication app(argc, argv);

    SceneController controller;
    ControllerWidget controllerWidget(&controller);
    controllerWidget.move(0, 0);
    controllerWidget.show();

    QQuickView view;
    auto context = view.engine()->rootContext();
    context->setContextProperty(QStringLiteral("_controller"), &controller);
    qmlRegisterType<KuesaUtils::OrbitCameraController>("Controllers", 1, 0, "OrbitCameraController");

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    view.setSource(QUrl("qrc:/main_qt6.qml"));
#else
    view.setSource(QUrl("qrc:/main_qt5.qml"));
#endif
    view.resize(1024, 512);
    view.show();

    controllerWidget.raise();

    return app.exec();
}
