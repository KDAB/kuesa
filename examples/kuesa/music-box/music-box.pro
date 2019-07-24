# music-box.pro
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Mauro Persano <mauro.persano@kdab.com>
#
# Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
# accordance with the Kuesa Enterprise License Agreement provided with the Software in the
# LICENSE.KUESA.ENTERPRISE file.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
    ../assets/models/music-box/music-box.qrc

ios|macos|android {
    RCC_BINARY_SOURCES += \
        ../assets/envmaps/pink_sunrise/envmap-pink-sunrise-16f.qrc
} else {
    RCC_BINARY_SOURCES += \
        ../assets/envmaps/pink_sunrise/envmap-pink-sunrise.qrc
}

macos: APP_PWD=$$OUT_PWD/$${TARGET}.app/Contents
else: APP_PWD=$$OUT_PWD
RES_PWD=$$APP_PWD/resources

asset_builder.commands = $$[QT_HOST_BINS]/rcc -binary ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT} -no-compress
asset_builder.depend_command = $$[QT_HOST_BINS]/rcc -list $$QMAKE_RESOURCE_FLAGS ${QMAKE_FILE_IN}
asset_builder.input = RCC_BINARY_SOURCES
asset_builder.output = $$RES_PWD/${QMAKE_FILE_IN_BASE}.qrb
asset_builder.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += asset_builder

target.path = $$[QT_INSTALL_EXAMPLES]/kuesa/$$TARGET
INSTALLS += target

install_music_samples.path = $$[QT_INSTALL_EXAMPLES]/kuesa/$$TARGET/samples
install_music_samples.files = $$PWD/assets/samples/*

INSTALLS += install_music_samples

include($$KUESA_ROOT/kuesa-global.pri)

win32 {
    QMAKE_POST_LINK += $$quote(cmd /c copy /y $$PWD\assets\samples $$RES_PWD\\ $$escape_expand(\n\t))
} else:ios {
    envmaps_dir = ../assets/envmaps
    resfiles.files = \
        $$RES_PWD/music-box.qrb \
        $$RES_PWD/envmap-pink-sunrise-16f.qrb
    resfiles.path = "/Library/Application Support"

    QMAKE_BUNDLE_DATA += resfiles

    OTHER_FILES += Info-ios.plist
    QMAKE_INFO_PLIST = Info-ios.plist

    OBJECTIVE_SOURCES += ios/iosutils.mm
    QMAKE_ASSET_CATALOGS += ios/Images.xcassets
} else:macos {
    ICON = ../../../resources/kuesa.icns
    OTHER_FILES += Info-macos.plist
    QMAKE_INFO_PLIST = Info-macos.plist
} else {
    QMAKE_POST_LINK += $$quote(cp -rf $$PWD/assets/samples $$RES_PWD $$escape_expand(\n\t))
}

packagesExist(portaudio-2.0) {
    CONFIG += link_pkgconfig
    PKGCONFIG += portaudio-2.0
    DEFINES += KUESA_HAS_AUDIO=1
}

OTHER_FILES += doc/src/* \
    ../assets/models/music-box/music-box.qrc \
    ../assets/envmaps/pink_sunrise/envmap-pink-sunrise.qrc
