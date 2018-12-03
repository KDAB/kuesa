QT += qml 3dcore 3drender 3dinput 3dlogic 3dextras 3dquickextras

HEADERS += \
    ssbobuffer.h

SOURCES += \
    main.cpp \
    ssbobuffer.cpp

RESOURCES += \
    qml.qrc

INCLUDEPATH += \
    $$PWD/3rdparty/gli-0.8.2.0/gli \
    $$PWD/3rdparty/gli-0.8.2.0/external/glm
