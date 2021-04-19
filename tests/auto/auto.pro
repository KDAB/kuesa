# auto.pro
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

TEMPLATE = subdirs

SUBDIRS = \
#    cmake \
    assetcollection \
    meshcollection \
    texturecollection \
    skeletoncollection \
    animationclipcollection \
    effectcollection \
    sceneentity \
    textureimagecollection \
    morphcontroller \
    gltf2importer \
    gltf2options \
    tonemappingandgammacorrectioneffect \
    kuesanode \
    fullscreenquad \
    reflectionplane \
    meshinstantiator \
    surfaceformat

#qtHaveModule(quick):lessThan(QT_MAJOR_VERSION, 6) {
#    SUBDIRS += qml

#    qtConfig(private_tests) {
#        SUBDIRS += \
#            assetitem \
#            forwardrendererextension
#    }
#}

#installed_cmake.depends = cmake

qtConfig(private_tests) {
    SUBDIRS += \
        bufferparser \
        bufferviewparser \
        bufferaccessorparser \
        cameraparser \
        meshparser \
        nodeparser \
        gltfparser \
#        gltfexporter \
        layerparser \
        lightparser \
        shadowparser \
        imageparser \
        tangentgenerator \
        texturesamplerparser \
        textureparser \
        animationparser \
        sceneparser \
        materialparser \
        skinparser \
        forwardrenderer \
        uri \
        metallicroughnesseffect \
        metallicroughnessproperties \
        parseroptions \
        gltf2materialproperties \
        unlitproperties \
        meshparser_utils \
        normalgenerator \
        gltf2material \
        effectslibrary \
        texturetransform \
        opaquestage \
        transparentstage \
        zfillstage \
        view \
        scenestages \
        framegraphutils \
        effectsstages \
        reflectionstages \
        particlerenderstage \
        empty2dtexture \
        transformtracker \
        fovadapter \
        assetcache  \
        assetkeyparser \
        view3dscene \
        sceneconfiguration \
        viewconfiguration \
        gaussianblureffect \
        placeholder \
        unliteffect \
        placeholdertracker \
        animationpulse \
        iro2alphaequirect \
        iro2alphasem \
        iro2diffuseequirect \
        iro2diffusesem \
        iro2glassequirect \
        iro2glasssem \
        iro2mattealpha \
        iro2planarreflectionequirect \
        iro2planarreflectionsem \
        irodiffuse \
        irodiffusealpha \
        irodiffusehemi \
        iroglassadd \
        iromatteadd \
        iromattealpha \
        iromattebackground \
        iromattemult \
        iromatteopaque \
        iromatteskybox \
        kuesaentity

    greaterThan(QT_MAJOR_VERSION, 5):
        SUBDIRS += fboresolver
}
