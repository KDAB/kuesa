# manual.pro
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Kevin Ottens <kevin.ottens@kdab.com>
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

TEMPLATE = subdirs

QT_FOR_CONFIG += kuesa

qtHaveModule(widgets) {
    SUBDIRS += \
        forwardrenderer_scene \
        metallicroughnessmaterial
}

qtHaveModule(quick) {
    SUBDIRS += \
        gltf2importer \
        multiple_animations \
        mapped_animations \
        KDAB_custom_material \
        IroDiffuseMaterial \
        IroDiffuseEquiRectangularMaterial \
        IroDiffuseHemiMaterial \
        IroGlassAddMaterial \
        IroMatteOpaqueMaterial \
        IroMatteAlphaMaterial \
        IroMatteSkyboxMaterial \
        IroMatteBackgroundMaterial \
        IroMatteAddMaterial \
        IroMatteMultMaterial \
        IroDiffuseAlphaMaterial \
        layered_framegraph \
        lights \
        particles \
        stencil \
        PlanarReflections \
        multi-views \
        multi-views-with-effects \
        KDAB_placeholder \
        instanced-rocks \
        extras
}

qtConfig(ktx) {
    SUBDIRS += KTXTextures
}

