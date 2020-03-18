/*
    main.cpp

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

#include <QGuiApplication>
#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <QQmlAspectEngine>
#include <QCommandLineParser>
#include <QQmlContext>
#include "ssbobuffer.h"

int main(int ac, char **av)
{
    QGuiApplication app(ac, av);
    QGuiApplication::setApplicationName(QStringLiteral("cubemaptooctohedralmap"));
    QGuiApplication::setApplicationVersion(QStringLiteral("0.1"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("cubemaptexture"),
                                 QStringLiteral("The cubemap texture file to convert"),
                                 QStringLiteral("[cubemaptexture]"));

    QCommandLineOption interactiveOption(QStringList() << QStringLiteral("i") << QStringLiteral("interactive"),
                                         QStringLiteral("Does not exit once texture has been generated."));
    parser.addOption(interactiveOption);

    QCommandLineOption outputOption(QStringList() << QStringLiteral("o") << QStringLiteral("output"),
                                    QStringLiteral("Output file name"),
                                    QStringLiteral("outputFile"),
                                    QStringLiteral("output.dds"));
    parser.addOption(outputOption);

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 1) {
        qWarning() << "No cubemap specified";
        parser.showHelp(-1);
    }

    qmlRegisterType<SSBOBuffer>("Tools", 1, 0, "SSBOBuffer");
    Qt3DExtras::Quick::Qt3DQuickWindow view;
    QQmlContext *ctx = view.engine()->qmlEngine()->rootContext();
    ctx->setContextProperty(QStringLiteral("_cubeMapPath"), QUrl::fromLocalFile(args.at(0)));
    ctx->setContextProperty(QStringLiteral("_outputPath"), parser.value(outputOption));
    ctx->setContextProperty(QStringLiteral("_isInteractive"), parser.isSet(interactiveOption));

    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
