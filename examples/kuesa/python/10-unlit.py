# 10-unlit.py
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

        self.camera().setPosition(QVector3D(5, 1.5, 5))
        self.camera().setViewCenter(QVector3D(0, 0.5, 0))
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

    def importerLoaded(self, status):
        if status != Kuesa.GLTF2Importer.Status.Ready:
            return

        parent = self.rootEntity.entity("KuesaEntity_0")

        parent.setObjectName("KuesaEntity_0")
        orig_entity = self.rootEntity.entity("KuesaEntity_2").childNodes()[1]
        orig_geometry = orig_entity.childNodes()[0]
        orig_material = orig_entity.childNodes()[1]
        orig_entity.removeComponent(orig_material)

        new_material = Kuesa.UnlitMaterial(orig_material.parent())

        # Materials need both an effect and material properties.
        # First create the Effect
        new_effect = Kuesa.UnlitEffect(new_material)

        new_effect.setBaseColorMapEnabled(False)
        new_effect.setUsingColorAttribute(True)
        new_effect.setDoubleSided(True)
        new_effect.setUseSkinning(False)
        new_effect.setOpaque(True)
        new_effect.setAlphaCutoffEnabled(True)

        new_material.setEffect(new_effect)

        # Then the properties
        new_props = Kuesa.UnlitProperties(new_material)

        new_props.setBaseColorUsesTexCoord1(True)
        new_props.setBaseColorFactor("white")

        # Load a texture
        new_tex = Qt3DRender.QTextureLoader(self.rootEntity)
        new_tex.setSource(assetsUrl() + "/models/car/KDAB.png")
        new_tex.setMinificationFilter(Qt3DRender.QAbstractTexture.LinearMipMapLinear)
        new_tex.setMagnificationFilter(Qt3DRender.QAbstractTexture.Linear)
        new_tex.wrapMode().setX(Qt3DRender.QTextureWrapMode.Repeat)
        new_tex.wrapMode().setY(Qt3DRender.QTextureWrapMode.Repeat)
        new_tex.setGenerateMipMaps(1)

        new_props.setBaseColorMap(new_tex)
        
        new_material.setUnlitProperties(new_props)

        orig_entity.addComponent(new_material)
        

if __name__ == '__main__':
    app = QGuiApplication(sys.argv)
    view = Window()
    view.show()
    sys.exit(app.exec_())
