# car-scene.pro
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

TEMPLATE = app

QT += 3dcore 3drender 3dinput 3dextras 3dquick qml quick 3dquickextras 3danimation kuesa quickcontrols2
QT_FOR_CONFIG += kuesa

SOURCES += main.cpp

RESOURCES += \
    assets/assets.qrc \
    qml/qml.qrc \
    ../assets/envmaps/pink_sunrise/envmap-pink-sunrise-16f.qrc \
    ../assets/envmaps/wobbly_bridge/envmap-wobbly-bridge-16f.qrc \
    ../assets/envmaps/neuer_zollhof/envmap-neuer-zollhof-16f.qrc \
    ../assets/envmaps/kdab-studiosky-small/envmap-kdab-studiosky-small-16f.qrc

qtConfig(draco) {
    RESOURCES += ../assets/models/car/car-draco.qrc
} else {
    RESOURCES += ../assets/models/car/car.qrc
}

target.path = $$[QT_INSTALL_EXAMPLES]/kuesa/$$TARGET
INSTALLS += target

OTHER_FILES += doc/src/*
