/*
    main.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#include <Kuesa/bloomeffect.h>
#include <Kuesa/gaussianblureffect.h>
#include <Kuesa/thresholdeffect.h>

#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <QQmlAspectEngine>
#include <QApplication>
#include <QQmlContext>

#include "controllerwidget.h"
#include "scenecontroller.h"

using namespace Kuesa;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SceneController controller;
    ControllerWidget controllerWidget(&controller);
    controllerWidget.move(0, 0);
    controllerWidget.show();

    Qt3DExtras::Quick::Qt3DQuickWindow view;
    auto context = view.engine()->qmlEngine()->rootContext();
    context->setContextProperty("_controller", &controller);

    view.setSource(QUrl("qrc:/main.qml"));
    view.resize(1920, 1080);
    view.show();

    controllerWidget.raise();

    return app.exec();
}
