QT += 3danimation 3dquickextras kuesa kuesa-private
CONFIG += resources_big
VERSION = 1.0.0

include($$KUESA_ROOT/kuesa-global.pri)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    orbitcameracontroller.cpp

HEADERS += \
    orbitcameracontroller.h

RESOURCES += qml.qrc \
    ../../resources/resources.qrc \
    ../../examples/kuesa/assets/envmaps/wobbly_bridge/envmap-wobbly-bridge-16f.qrc

OTHER_FILES += main.qml

macos {
    TARGET = gltfViewer
    OTHER_FILES += Info.plist.in
    QMAKE_SUBSTITUTES += Info.plist.in
    QMAKE_INFO_PLIST = $$OUT_PWD/Info.plist

    ICON_FILE.files = ../../resources/kuesa.icns
    ICON_FILE.path = Contents/Resources
    QMAKE_BUNDLE_DATA += ICON_FILE

}

windows {
    RC_ICONS = ../../resources/kuesa.ico
}

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

OTHER_FILES += README.md
