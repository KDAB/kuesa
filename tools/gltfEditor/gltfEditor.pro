# gltfEditor.pro
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Mike Krus <mike.krus@kdab.com>
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
QT += 3dcore 3drender 3dinput 3danimation 3dextras 3dquick widgets kuesa kuesa-private
lessThan(QT_MINOR_VERSION, 14): QT += 3dcore-private 3drender-private

include($$KUESA_ROOT/kuesa-global.pri)

DEFINES += QT_DEPRECATED_WARNINGS
VERSION = 1.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    camerawidget.cpp \
    colorlabel.cpp \
    collectionbrowser.cpp \
    collectionmodel.cpp \
    animationwidget.cpp \
    assetinspector.cpp \
    assetinspectorwidget.cpp \
    lightinspector.cpp \
    lightwidget.cpp \
    materialinspector.cpp \
    materialwidget.cpp \
    meshinspector.cpp \
    meshwidget.cpp \
    renderedpreviewwidget.cpp \
    texturewidget.cpp \
    textureinspector.cpp \
    texturepreviewmaterial.cpp \
    settingsdialog.cpp \
    orbitcameracontroller.cpp \
    exportdialog.cpp \
    memoryusagewidget.cpp \
    utils.cpp

HEADERS += \
    mainwindow.h \
    camerawidget.h \
    colorlabel.h \
    collectionbrowser.h \
    collectionmodel.h \
    animationwidget.h \
    assetinspector.h \
    assetinspectorwidget.h \
    lightinspector.h \
    lightwidget.h \
    materialinspector.h \
    materialwidget.h \
    meshinspector.h \
    meshwidget.h \
    renderedpreviewwidget.h \
    texturewidget.h \
    textureinspector.h \
    texturepreviewmaterial.h \
    settingsdialog.h \
    orbitcameracontroller.h \
    exportdialog.h \
    memoryusagewidget.h \
    utils.h

FORMS += \
    animationwidget.ui \
    vectorwidget.ui \
    camerawidget.ui \
    mainwindow.ui \
    lightwidget.ui \
    materialwidget.ui \
    meshwidget.ui \
    texturewidget.ui \
    settingsdialog.ui \
    exportdialog.ui \
    memoryusagewidget.ui

RESOURCES += \
    qml.qrc \
    ../../resources/resources.qrc \
    ../../examples/kuesa/assets/envmaps/wobbly_bridge/envmap-wobbly-bridge-16f.qrc

OTHER_FILES += \
    main.qml
    RenderPreviewGraph.qml

macos {
    TARGET = gltfEditor
    QT_DEPLOY=$$[QT_INSTALL_LIBS]/../bin/macdeployqt
    APE_BUNDLE=$${TARGET}.app
    APE_SOURCE=$$PWD
    OTHER_FILES += Info.plist.in create_macos_installer.sh.in
    QMAKE_SUBSTITUTES += Info.plist.in create_macos_installer.sh.in
    QMAKE_INFO_PLIST = $$OUT_PWD/Info.plist

    ICON_FILE.files = ../../resources/kuesa.icns
    ICON_FILE.path = Contents/Resources
    QMAKE_BUNDLE_DATA += ICON_FILE

    OBJECTIVE_SOURCES += macUtils.mm
    LIBS += -framework AppKit
}

windows {
    RC_ICONS = ../../resources/kuesa.ico

    APE_SOURCE=$$shell_path($$PWD)
    APE_BUILD=$$shell_path($$OUT_PWD)
    KUESA_BUILD_WINPATH=$$shell_path($$KUESA_BUILD_ROOT)
    KUESA_SOURCE_WINPATH=$$shell_path($$KUESA_ROOT)
    QT_DEPLOY=$$shell_path($$[QT_INSTALL_LIBS])\..\bin\windeployqt
    QMAKE_SUBSTITUTES += create_windows_installer.bat.in installer.iss.in
    OTHER_FILES += create_windows_installer.bat.in installer.iss.in
}

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
