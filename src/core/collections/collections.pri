# collections.pri
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
    $$PWD/layercollection.cpp \
    $$PWD/lightcollection.cpp \
    $$PWD/meshcollection.cpp \
    $$PWD/reflectionplanecollection.cpp \
    $$PWD/texturecollection.cpp \
    $$PWD/effectcollection.cpp \
    $$PWD/materialcollection.cpp \
    $$PWD/skeletoncollection.cpp \
    $$PWD/armaturecollection.cpp \
    $$PWD/animationclipcollection.cpp \
    $$PWD/abstractassetcollection.cpp \
    $$PWD/cameracollection.cpp \
    $$PWD/entitycollection.cpp \
    $$PWD/textureimagecollection.cpp \
    $$PWD/animationmappingcollection.cpp \
    $$PWD/transformcollection.cpp \
    $$PWD/placeholdercollection.cpp

HEADERS += \
    $$PWD/layercollection.h \
    $$PWD/lightcollection.h \
    $$PWD/meshcollection.h \
    $$PWD/reflectionplanecollection.h \
    $$PWD/texturecollection.h \
    $$PWD/effectcollection.h \
    $$PWD/materialcollection.h \
    $$PWD/skeletoncollection.h \
    $$PWD/armaturecollection.h \
    $$PWD/animationclipcollection.h \
    $$PWD/abstractassetcollection.h \
    $$PWD/cameracollection.h \
    $$PWD/entitycollection.h \
    $$PWD/textureimagecollection.h \
    $$PWD/animationmappingcollection.h \
    $$PWD/transformcollection.h \
    $$PWD/placeholdercollection.h
