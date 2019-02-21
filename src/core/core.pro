# core.pro
#
# This file is part of Kuesa.
#
# Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Paul Lemire <paul.lemire@kdab.com>
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

TARGET     = Kuesa
MODULE     = kuesa

# Kuesa is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH

DEFINES += QT_BUILD_KUESA_LIB

QT += qml 3dcore 3dcore-private 3drender 3dquickextras 3danimation

include($$OUT_PWD/qtkuesa-config.pri)
qtConfig(draco) {
    win32:CONFIG -= precompile_header
    include(../3rdparty/draco/draco_dependency.pri)
}

include(core.pri)
include(collections/collections.pri)
include(gltf2importer/gltf2importer.pri)
include(gltf2exporter/gltf2exporter.pri)
include(framegraphes/framegraphes.pri)
include(fx/fx.pri)
include(../3rdparty/mikktspace/mikktspace.pri)

RESOURCES += \
    shaders.qrc

OTHER_FILES += \
    shaders/gl3/simple.vert \
    shaders/graphs/metallicroughness.qt3d \
    configure.pri \
    configure.json

load(qt_module)
