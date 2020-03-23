# core.pri
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/sceneentity.cpp \
    $$PWD/animationplayer.cpp \
    $$PWD/skybox.cpp \
    $$PWD/morphcontroller.cpp \
    $$PWD/particlemesh.cpp \
    $$PWD/particlegeometry.cpp \
    $$PWD/particlematerial.cpp \
    $$PWD/noisetextureimage.cpp \
    $$PWD/particles.cpp

HEADERS += \
    $$PWD/sceneentity.h \
    $$PWD/factory.h \
    $$PWD/kuesa_p.h \
    $$PWD/kuesa_global.h \
    $$PWD/kuesa_global_p.h \
    $$PWD/kuesa_utils_p.h \
    $$PWD/animationplayer.h \
    $$PWD/skybox.h \
    $$PWD/morphcontroller.h \
    $$PWD/particlemesh_p.h \
    $$PWD/particlegeometry_p.h \
    $$PWD/particlematerial_p.h \
    $$PWD/noisetextureimage_p.h \
    $$PWD/particles.h

qtConfig(draco) {
    HEADERS += $$PWD/draco_prefix_p.h
}
