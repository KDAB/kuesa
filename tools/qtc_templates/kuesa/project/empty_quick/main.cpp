#include <QGuiApplication>
#include <QQuickView>

int main(int argc, char *argv[])
{
    // Request highest OpenGL version available
    // Qt3D will resolve the highest version actually available at runtime
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setMajorVersion(4);
    format.setMinorVersion(6);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    // Manages the GUI application's control flow and main settings.
    QGuiApplication app(argc, argv);

    // Setup new QQuickView
    QQuickView view;

    // Set QML scene source
    view.setSource(QUrl("qrc:/main.qml"));

    // Set the window size and then show it.
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(1920, 1080);
    view.show();

    // Enter the even loop
    return app.exec();
}
