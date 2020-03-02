# ToneMapping.pro
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Robert Brock <robert.brock@kdab.com>
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

QT += 3dcore 3drender 3dinput 3dextras 3dquick qml quick 3dquickextras 3danimation kuesa
QT_FOR_CONFIG += kuesa

SOURCES += main.cpp \
    ../../../tools/gltfViewer/orbitcameracontroller.cpp

HEADERS += \
    ../../../tools/gltfViewer/orbitcameracontroller.h

include(../shared-utils/shared-utils.pri)
include($$KUESA_ROOT/kuesa-global.pri)

RESOURCES += \
    assets/assets.qrc \
    qml/qml.qrc \

envmaps_dir = ../assets/envmaps/pink_sunrise
envmaps.files = \
    $${envmaps_dir}/pink_sunrise_16f_irradiance.dds \
    $${envmaps_dir}/pink_sunrise_16f_radiance.dds \
    $${envmaps_dir}/pink_sunrise_16f_specular.dds \
    $${envmaps_dir}/pink_sunrise_skybox.dds

helmet_dir = ../assets/models/damagedhelmet/rotating_glTF
helmet.files = \
    $${helmet_dir}/Default_albedo.png \
    $${helmet_dir}/Default_emissive.png \
    $${helmet_dir}/Default_metalRoughness.png \
    $${helmet_dir}/Default_normal.png \
    $${helmet_dir}/RotatingHelmet.bin \
    $${helmet_dir}/RotatingHelmet.gltf

android {
    envmaps.path = /assets
    helmet.path = /assets

    INSTALLS += helmet envmaps

    DISTFILES += android/AndroidManifest.xml
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
} else:ios {
    envmaps.path = "/Library/Application Support"
    helmet.path = "/Library/Application Support"

    QMAKE_BUNDLE_DATA += helmet envmaps

    OTHER_FILES += Info-ios.plist
    QMAKE_INFO_PLIST = Info-ios.plist

    OBJECTIVE_SOURCES += ios/iosutils.mm
    QMAKE_ASSET_CATALOGS += ios/Images.xcassets
} else:osx {
    envmaps.path = "/Contents/Resources"
    helmet.path = "/Contents/Resources"

    QMAKE_BUNDLE_DATA += helmet envmaps

    ICON = ../shared-utils/kuesa.icns
    OTHER_FILES += Info-macos.plist
    QMAKE_INFO_PLIST = Info-macos.plist
} else {
    RCC_BINARY_SOURCES += \
        ../assets/models/damagedhelmet/rotating_glTF/helmet.qrc \
        ../assets/envmaps/pink_sunrise/envmap-pink-sunrise-16f.qrc

    windows {
        RC_ICONS = ../shared-utils/kuesa.ico
        DESTDIR = $$KUESA_BUILD_ROOT/examples/kuesa/$$TARGET
    }

    # Build resources as external files
    asset_builder.commands = $$[QT_HOST_BINS]/rcc -binary ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT} -no-compress
    asset_builder.depend_command = $$[QT_HOST_BINS]/rcc -list $$QMAKE_RESOURCE_FLAGS ${QMAKE_FILE_IN}
    asset_builder.input = RCC_BINARY_SOURCES
    asset_builder.output = $$OUT_PWD/resources/${QMAKE_FILE_IN_BASE}.qrb
    asset_builder.CONFIG += no_link target_predeps
    QMAKE_EXTRA_COMPILERS += asset_builder

    ext_resources.path = $$[QT_INSTALL_EXAMPLES]/kuesa/$$TARGET
    ext_resources.files = $$OUT_PWD/resources

    INSTALLS += ext_resources

    OTHER_FILES += \
        ../assets/models/damagedhelmet/rotating_glTF/helmet.qrc \
        ../assets/envmaps/pink_sunrise/envmap-pink-sunrise-16f.qrc
}

target.path = $$[QT_INSTALL_EXAMPLES]/kuesa/$$TARGET
target.files = $$PWD/*
INSTALLS += target

OTHER_FILES += doc/src/*

