QT += 3danimation 3dquickextras
CONFIG += resources_big

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \\
        %{CppFileName}

RESOURCES += qml.qrc

OTHER_FILES += \
    main.qml

QML_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
