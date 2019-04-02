TEMPLATE = app

TARGET = music-box

QT += 3dcore 3drender 3dinput 3dextras 3dquick qml quick 3dquickextras 3danimation kuesa
SOURCES += \
    main.cpp \
    sampler.cpp

HEADERS += \
    sampler.h

RESOURCES += \
    qml/qml.qrc \

RCC_BINARY_SOURCES += \
    ../assets/models/music-box/music-box.qrc \
    ../assets/envmaps/pink_sunrise/envmap-pink-sunrise.qrc

asset_builder.commands = $$[QT_HOST_BINS]/rcc -binary ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT} -no-compress
asset_builder.depend_command = $$[QT_HOST_BINS]/rcc -list $$QMAKE_RESOURCE_FLAGS ${QMAKE_FILE_IN}
asset_builder.input = RCC_BINARY_SOURCES
asset_builder.output = $$OUT_PWD/${QMAKE_FILE_IN_BASE}.qrb
asset_builder.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += asset_builder

target.path = $$[QT_INSTALL_EXAMPLES]/kuesa/$$TARGET
INSTALLS += target

install_music_samples.path = $$[QT_INSTALL_EXAMPLES]/kuesa/$$TARGET/samples
install_music_samples.files = $$PWD/assets/samples/*

INSTALLS += install_music_samples


win32 {
QMAKE_POST_LINK +=$$quote(cmd /c copy /y $$PWD/assets/samples $$OUT_PWD/ $$escape_expand(\n\t))
}
else {
QMAKE_POST_LINK += $$quote(cp -rf $$PWD/assets/samples $$OUT_PWD/ $$escape_expand(\n\t))
}

packagesExist(portaudio-2.0) {
  CONFIG += link_pkgconfig
  PKGCONFIG += portaudio-2.0
  DEFINES += KUESA_HAS_AUDIO=1
}
