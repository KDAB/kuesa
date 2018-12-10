# assetpipelineeditor.pro
#
# This file is part of Kuesa.
#
# Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    camerawidget.cpp \
    collectionbrowser.cpp \
    collectionmodel.cpp \
    animationwidget.cpp \
    assetinspector.cpp \
    assetinspectorwidget.cpp \
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
    exportdialog.cpp

HEADERS += \
    mainwindow.h \
    camerawidget.h \
    collectionbrowser.h \
    collectionmodel.h \
    animationwidget.h \
    assetinspector.h \
    assetinspectorwidget.h \
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
    exportdialog.h

FORMS += \
    animationwidget.ui \
    vectorwidget.ui \
    camerawidget.ui \
    mainwindow.ui \
    materialwidget.ui \
    meshwidget.ui \
    texturewidget.ui \
    settingsdialog.ui \
    exportdialog.ui

RESOURCES += \
    qml.qrc \
    ../../resources/resources.qrc \
    ../../examples/kuesa/assets/envmaps/wobbly_bridge/envmap-wobbly-bridge.qrc

OTHER_FILES += \
    main.qml
    RenderPreviewGraph.qml

macos {
    TARGET = AssetPipelineEditor
    OTHER_FILES += Info.plist
    QMAKE_INFO_PLIST = Info.plist
    ICON = ../../resources/kuesa.icns

    OBJECTIVE_SOURCES += macUtils.mm
    LIBS += -framework AppKit
}

windows {
    RC_ICONS = ../../resources/kuesa.ico
}

qtConfig(kuesa-draco) : DEFINES += KUESA_DRACO_COMPRESSION

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
