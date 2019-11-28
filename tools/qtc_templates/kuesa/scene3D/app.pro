QT += quick
CONFIG += resources_big

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    %{CppFileName}

RESOURCES += qml.qrc

OTHER_FILES += \
    main.qml \
    SceneEntity.qml

QML_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
