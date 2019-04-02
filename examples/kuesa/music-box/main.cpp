#include <QGuiApplication>
#include <QQuickView>
#include <QResource>
#include <QTimer>
#include "sampler.h"
namespace {
bool initializeAssetResources(const QVector<QString> &fileNames)
{
    bool b = true;
    for (const auto &fileName : fileNames) {
        b &= QResource::registerResource(fileName);
        if (!b)
            qDebug() << "Failed to load assets from" << fileName;
    }
    return b;
}
}

int main(int argc, char *argv[])
{
    {
        // Set OpenGL requirements
        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
#ifndef QT_OPENGL_ES_2
        format.setVersion(4, 1);
        format.setProfile(QSurfaceFormat::CoreProfile);
#else
        format.setVersion(3, 0);
        format.setProfile(QSurfaceFormat::NoProfile);
        format.setRenderableType(QSurfaceFormat::OpenGLES);
#endif
        QSurfaceFormat::setDefaultFormat(format);
    }

    QGuiApplication app(argc, argv);

    initializeAssetResources({ QStringLiteral("music-box.qrb"), QStringLiteral("envmap-pink-sunrise.qrb") });
    qmlRegisterType<Sampler>("MusicBox", 1, 0, "Sampler");

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    view.resize(960, 540);
    view.show();

    return app.exec();
}
