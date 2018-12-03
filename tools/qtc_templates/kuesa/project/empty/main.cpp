#include <QGuiApplication>
#include <Qt3DAnimation/QAnimationAspect>
#include <Qt3DQuickExtras/Qt3DQuickWindow>

int main(int argc, char *argv[])
{
    // Manages the GUI application's control flow and main settings.
    QGuiApplication app(argc, argv);

    // Setup new instance of Qt3DWindow
    Qt3DExtras::Quick::Qt3DQuickWindow view;
    view.registerAspect(new Qt3DAnimation::QAnimationAspect());

    // Set QML scene source
    view.setSource(QUrl("qrc:/main.qml"));

    // Set the window size and then show it.
    view.resize(1920, 1080);
    view.show();

    // Enter the even loop
    return app.exec();
}
