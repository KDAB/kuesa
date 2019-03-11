/*
    main.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QGuiApplication>
#include <QQuickView>
#include <QCommandLineParser>
#include <QQmlContext>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include "ddsheaderparser.h"

int main(int ac, char **av)
{
    QSurfaceFormat f = QSurfaceFormat::defaultFormat();
    f.setMajorVersion(4);
    f.setMinorVersion(0);
    f.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(f);

    QGuiApplication app(ac, av);
    QGuiApplication::setApplicationName(QStringLiteral("ddspreviewer"));
    QGuiApplication::setApplicationVersion(QStringLiteral("0.1"));
    QGuiApplication::setOrganizationName(QStringLiteral("KDAB"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("ddspreviewer"),
                                 QStringLiteral("The dds file to preview"),
                                 QStringLiteral("[dds_file]"));
    parser.process(app);

    qmlRegisterType<DDSHeaderParser>("DDSPreviewer", 1, 0, "DDSHeaderParser");

    QQuickStyle::setStyle("Material");
    QQuickView view;
    QQmlContext *ctx = view.rootContext();

    QString ddsFilePath;
    const QStringList args = parser.positionalArguments();
    if (args.size() == 1)
        ddsFilePath = args.at(0);

    ctx->setContextProperty(QStringLiteral("_ddsFilePath"), ddsFilePath);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.showMaximized();

    return app.exec();
}
