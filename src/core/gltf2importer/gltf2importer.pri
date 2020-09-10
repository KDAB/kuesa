# gltf2importer.pri
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

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/bufferparser.cpp \
    $$PWD/bufferviewsparser.cpp \
    $$PWD/cameraparser.cpp \
    $$PWD/fovadaptor.cpp \
    $$PWD/gltf2context.cpp \
    $$PWD/bufferaccessorparser.cpp \
    $$PWD/meshparser.cpp \
    $$PWD/nodeparser.cpp \
    $$PWD/gltf2parser.cpp \
    $$PWD/gltf2importer.cpp \
    $$PWD/layerparser.cpp \
    $$PWD/lightparser.cpp \
    $$PWD/imageparser.cpp \
    $$PWD/textureinfoparser.cpp \
    $$PWD/texturesamplerparser.cpp \
    $$PWD/textureparser.cpp \
    $$PWD/animationparser.cpp \
    $$PWD/sceneparser.cpp \
    $$PWD/materialparser.cpp \
    $$PWD/skinparser.cpp \
    $$PWD/gltf2uri.cpp \
    $$PWD/meshparser_utils.cpp \
    $$PWD/gltf2options.cpp

HEADERS += \
    $$PWD/bufferparser_p.h \
    $$PWD/bufferviewsparser_p.h \
    $$PWD/cameraparser_p.h \
    $$PWD/fovadaptor_p.h \
    $$PWD/gltf2context_p.h \
    $$PWD/bufferaccessorparser_p.h \
    $$PWD/meshparser_p.h \
    $$PWD/nodeparser_p.h \
    $$PWD/gltf2parser_p.h \
    $$PWD/gltf2importer.h \
    $$PWD/layerparser_p.h \
    $$PWD/lightparser_p.h \
    $$PWD/imageparser_p.h \
    $$PWD/textureinfoparser_p.h \
    $$PWD/texturesamplerparser_p.h \
    $$PWD/textureparser_p.h \
    $$PWD/animationparser_p.h \
    $$PWD/sceneparser_p.h \
    $$PWD/materialparser_p.h \
    $$PWD/skinparser_p.h \
    $$PWD/gltf2keys_p.h \
    $$PWD/gltf2uri_p.h \
    $$PWD/meshparser_utils_p.h \
    $$PWD/gltf2options.h
