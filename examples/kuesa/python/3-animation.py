# 3-animation.py
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
from PySide2.Qt3DAnimation import (Qt3DAnimation)
from Kuesa import (Kuesa)
from KuesaUtils.DefaultEnvMap import (DefaultEnvMap)
from KuesaUtils.Assets import (assetsUrl)

class Window(Qt3DExtras.Qt3DWindow):
    def __init__(self):
        super(Window, self).__init__()

        # Since we are going to use animations, we need to register
        # the Qt3D animation aspect :
        self.animationAspect = Qt3DAnimation.QAnimationAspect(self)
        self.registerAspect(self.animationAspect)

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
        self.gltfImporter.statusChanged.connect(self.on_sceneLoaded)

        self.setRootEntity(self.rootEntity)

    def on_sceneLoaded(self, status):
        if(status != Kuesa.GLTF2Importer.Status.Ready):
            return

        # In this tutorial, we are going to trigger glTF animations.
        # The list of available animations can be checked with the gltfEditor,
        # in the Animation dock.
        self.animationClock = Qt3DAnimation.QClock(self.rootEntity)
        self.animationClock.setPlaybackRate(1)

        self.cubeAnimation = Kuesa.AnimationPlayer(self.rootEntity)
        self.cubeAnimation.setClock(self.animationClock)
        self.cubeAnimation.setSceneEntity(self.rootEntity)
        self.cubeAnimation.setClip("Cube.008Action")
        self.cubeAnimation.setLoopCount(-1)

        self.cubeAnimation.start()
        return

if __name__ == '__main__':
    app = QGuiApplication(sys.argv)
    view = Window()
    view.show()
    sys.exit(app.exec_())
