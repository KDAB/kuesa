# 9-pbr.py
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Jean-Michaël Celerier <jean-michael.celerier@kdab.com>
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

import sys
import os
import platform
import random
from PySide2.QtCore import(Property, QObject, QPropertyAnimation, Signal, Slot)
from PySide2.QtGui import (QGuiApplication, QMatrix4x4, QQuaternion, QVector3D, QWindow)
from PySide2.Qt3DCore import (Qt3DCore)
from PySide2.Qt3DRender import (Qt3DRender)
from PySide2.Qt3DExtras import (Qt3DExtras)
from Kuesa import (Kuesa)
from KuesaUtils.DefaultEnvMap import (DefaultEnvMap)
from KuesaUtils.Assets import (assetsUrl)

class Window(Qt3DExtras.Qt3DWindow):
    def __init__(self):
        super(Window, self).__init__()

        # Default set-up
        self.rootEntity = Kuesa.SceneEntity()
        self.rootEntity.addComponent(DefaultEnvMap(self.rootEntity))

        self.camera().setPosition(QVector3D(50, 0, 50))
        self.camera().setViewCenter(QVector3D(10, 10, 0))
        self.camera().setUpVector(QVector3D(0, 1, 0))
        self.camera().setAspectRatio(16. / 9.)

        self.camController = Qt3DExtras.QOrbitCameraController(self.rootEntity)
        self.camController.setCamera(self.camera())

        self.fg = Kuesa.ForwardRenderer()
        self.fg.setCamera(self.camera())
        self.fg.setClearColor("white")
        self.setActiveFrameGraph(self.fg)

        # Load a glTF model
        self.gltfImporter = Kuesa.GLTF2Importer(self.rootEntity)
        self.gltfImporter.setSceneEntity(self.rootEntity)
        self.gltfImporter.setSource(assetsUrl() + "/models/duck/Duck.glb")
        self.gltfImporter.statusChanged.connect(self.importerLoaded)

        # Skybox creation
        envmap_root = assetsUrl() + "/envmaps/pink_sunrise"
        envmap_name = "pink_sunrise" + ("_16f" if platform.system() == "Darwin" else "") + "_radiance"
        self.skybox = Kuesa.Skybox(self.rootEntity)
        self.skybox.setBaseName(envmap_root  + "/" + envmap_name)
        self.skybox.setExtension(".dds")

        self.setRootEntity(self.rootEntity)

    def cloneMaterial(self, material, parent):
        orig_props = material.metallicRoughnessProperties()

        new_material = Kuesa.MetallicRoughnessMaterial(parent)

        new_props = Kuesa.MetallicRoughnessProperties(new_material)

        # Base GLTF2 material properties (see Kuesa::GLTF2MaterialProperties)
        new_props.setBaseColorUsesTexCoord1(orig_props.isBaseColorUsingTexCoord1())
        new_props.setBaseColorFactor(orig_props.baseColorFactor())
        new_props.setBaseColorMap(orig_props.baseColorMap())
        new_props.setAlphaCutoff(orig_props.alphaCutoff())
        new_props.setTextureTransform(orig_props.textureTransform())

        # PBR material properties (see Kuesa::MetallicRoughnessProperties)
        new_props.setMetallicRoughnessUsesTexCoord1(orig_props.isMetallicRoughnessUsingTexCoord1())
        new_props.setNormalUsesTexCoord1(orig_props.isNormalUsingTexCoord1())
        new_props.setAOUsesTexCoord1(orig_props.isAOUsingTexCoord1())
        new_props.setEmissiveUsesTexCoord1(orig_props.isEmissiveUsingTexCoord1())
        new_props.setMetallicFactor(orig_props.metallicFactor())
        new_props.setRoughnessFactor(orig_props.roughnessFactor())
        new_props.setMetalRoughMap(orig_props.metalRoughMap())
        new_props.setNormalScale(orig_props.normalScale())
        new_props.setNormalMap(orig_props.normalMap())
        new_props.setAmbientOcclusionMap(orig_props.ambientOcclusionMap())
        new_props.setEmissiveFactor(orig_props.emissiveFactor())
        new_props.setEmissiveMap(orig_props.emissiveMap())
        
        new_material.setMetallicRoughnessProperties(new_props)
        
        return new_material

    # Wait until all our ducks are loaded
    def importerLoaded(self, status):
        if status != Kuesa.GLTF2Importer.Status.Ready:
            return

        # First let's take the components that we are going to use to create our clones
        parent = self.rootEntity.entity("KuesaEntity_0")

        parent.setObjectName("KuesaEntity_0")
        orig_entity = self.rootEntity.entity("KuesaEntity_2").childNodes()[1]
        orig_geometry = orig_entity.childNodes()[0]
        orig_material = orig_entity.childNodes()[1]

        row = 10
        col = 10
        distance = 200
        r = 500
        # Then create clones by giving them a custom transform, and the same components than before
        for i in range(0, col):
            for j in range(0, row):
                new_entity = Qt3DCore.QEntity(parent)
    
                # Note : there is an inconsistency in the Python bindings - 
                # In C++ it is not necessary to assign a parent to the transform.
                new_transform = Qt3DCore.QTransform(new_entity)

                new_material = self.cloneMaterial(orig_material, new_entity)
                new_material.setEffect(orig_material.effect())
                new_props = new_material.metallicRoughnessProperties()

                new_props.setMetallicFactor(i / col)
                new_props.setRoughnessFactor(j / row)    
                
                dt = QVector3D(i * distance, j * distance, 0)
                new_transform.setTranslation(dt)
                
                # Add the custom transform to the entity
                new_entity.addComponent(new_transform)

                # Add the material we created
                new_entity.addComponent(new_material)

                # These two components will be shared across all the ducks.
                new_entity.addComponent(orig_geometry)


if __name__ == '__main__':
    app = QGuiApplication(sys.argv)
    view = Window()
    view.show()
    sys.exit(app.exec_())
