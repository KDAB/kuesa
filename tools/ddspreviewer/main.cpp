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
