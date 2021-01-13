# 2-access.py
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
from PySide2.QtCore import(Property, QObject, QPropertyAnimation, Signal, Slot, QTimer)
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

        self.camera().setPosition(QVector3D(25, 1.5, 25))
        self.camera().setViewCenter(QVector3D(0, 3, 0))
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
        self.gltfImporter.setSource(assetsUrl() + "/models/InterpolationTest.glb")

        # When opening this model in gltfEditor, we can see :
        # - Multiple entities :
        #   Cube, Cube.001, Cube.002, Light, Plane...
        # - Multiple meshes :
        #   Cube.001_0, Cube.002_0, ...
        # - Multiple materials :
        #   Material, Material.001, ...
        # etc.

        # Let's use Kuesa to change one of these entities on-the-fly.
        # We need to wait for the glTF file to be fully loaded in order to do that :
        self.gltfImporter.statusChanged.connect(self.on_sceneLoaded)

        self.setRootEntity(self.rootEntity)

    def on_sceneLoaded(self, status):
        if(status != Kuesa.GLTF2Importer.Status.Ready):
            return

        # The glTF file has now been loaded, we can start fiddling with the scene.
        # Cube.001 is at the bottom-left.

        # Here, we are going to change the transform of an entity.
        # It is also possible to change other properties - refer to the collection documentation
        # to get an overview of what is possible.
        self.cubeAsset = self.rootEntity.entities().find("Cube.001")

        for child in self.cubeAsset.children():
            if isinstance(child, Qt3DCore.QTransform):
                # We found the transform of the cube we want to change
                self.cubeTransform = child

                # It can be modified like this
                child.setScale(2)

                # Let's change one of its properties regularly with a timer
                # Note: the next tutorial, 3-animation.py, will show how to use embedded glTF animations.
                self.cubeScaleDirection = 1

                timer = QTimer(self)
                timer.setInterval(16)
                timer.timeout.connect(self.changeCubeScale)
                timer.start()

    def changeCubeScale(self):
        # Simple modifications of the Qt3D QTransform
        self.cubeTransform.setRotationX(self.cubeTransform.rotationX() + 0.5)
        self.cubeTransform.setRotationY(self.cubeTransform.rotationX() + 0.5)

        self.cubeTransform.setScale(self.cubeTransform.scale() + self.cubeScaleDirection * 0.01)

        if self.cubeTransform.scale() < 0.5:
            self.cubeScaleDirection = 1
        elif self.cubeTransform.scale() > 2:
            self.cubeScaleDirection = -1

if __name__ == '__main__':
    app = QGuiApplication(sys.argv)
    view = Window()
    view.show()
    sys.exit(app.exec_())
