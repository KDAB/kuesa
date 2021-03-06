# core.pri
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    $$PWD/empty2dtexture.cpp \
    $$PWD/kuesa_global.cpp \
    $$PWD/kuesanode.cpp \
    $$PWD/kuesa_utils.cpp \
    $$PWD/logging.cpp \
    $$PWD/placeholder.cpp \
    $$PWD/meshinstantiator.cpp \
    $$PWD/placeholdertracker.cpp \
    $$PWD/sceneentity.cpp \
    $$PWD/animationplayer.cpp \
    $$PWD/skybox.cpp \
    $$PWD/morphcontroller.cpp \
    $$PWD/particlemesh.cpp \
    $$PWD/particlegeometry.cpp \
    $$PWD/particlematerial.cpp \
    $$PWD/noisetextureimage.cpp \
    $$PWD/particles.cpp \
    $$PWD/steppedanimationplayer.cpp \
    $$PWD/transformtracker.cpp \
    $$PWD/animationpulse.cpp \
    $$PWD/kuesaentity.cpp

HEADERS += \
    $$PWD/empty2dtexture_p.h \
    $$PWD/kuesanode.h \
    $$PWD/logging_p.h \
    $$PWD/placeholder.h \
    $$PWD/meshinstantiator.h \
    $$PWD/placeholdertracker.h \
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
    $$PWD/particles.h \
    $$PWD/steppedanimationplayer.h \
    $$PWD/transformtracker.h \
    $$PWD/animationpulse.h

# Don't add kuesaentity.h to HEADERS as we already implement the metaobject functions
#    $$PWD/kuesaentity.h

qtConfig(draco) {
    HEADERS += $$PWD/draco_prefix_p.h
}

qtConfig(ktx) {
    HEADERS += $$PWD/ktxtexture.h
    SOURCES += $$PWD/ktxtexture.cpp
}
