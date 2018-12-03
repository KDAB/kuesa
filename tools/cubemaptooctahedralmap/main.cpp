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
