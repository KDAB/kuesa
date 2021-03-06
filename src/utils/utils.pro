# core.pro
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

TARGET     = KuesaUtils
MODULE     = kuesautils

# Kuesa is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH
DEFINES += QT_BUILD_KUESA_UTILS_LIB

# Avoid windows defines
win32:DEFINES += WIN32_LEAN_AND_MEAN

QT += core-private kuesa kuesa-private 3dcore-private 3dinput

HEADERS += \
    boundingvolumerenderer.h \
    kuesa_utils_global.h \
    orbitcameracontroller.h \
    sceneconfiguration.h \
    view3dscene.h \
    viewconfiguration.h \
    window.h

SOURCES += \
    boundingvolumerenderer.cpp \
    orbitcameracontroller.cpp \
    sceneconfiguration.cpp \
    view3dscene.cpp \
    viewconfiguration.cpp \
    window.cpp

RESOURCES += \
    utils.qrc

include($$KUESA_ROOT/kuesa-global.pri)

load(qt_module)
