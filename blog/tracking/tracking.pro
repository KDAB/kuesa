TEMPLATE = app

QT += 3dcore 3drender 3dinput 3dextras 3dquick qml quick 3dquickextras 3danimation kuesa quickcontrols2

CONFIG += resources_big

SOURCES += main.cpp

RESOURCES += \
    assets/assets.qrc \
    qml/qml.qrc
