# 1-cube.py
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

        # Scene creation
        # Kuesa content must be child of a Kuesa.SceneEntity.
        # These entites are all themselves standard Qt3D::QEntity
        self.rootEntity = Kuesa.SceneEntity()

        # GLTF2Importer will load the glTF 2.0 content and add
        # it as a child in the SceneEntity.
        self.gltfImporter = Kuesa.GLTF2Importer(self.rootEntity)
        self.gltfImporter.setSceneEntity(self.rootEntity)
        self.gltfImporter.setSource(assetsUrl() + "/models/Box.glb")

        # Since Kuesa is based on a PBR pipeline, we need an environment map.
        self.rootEntity.addComponent(DefaultEnvMap(self.rootEntity))

        # Camera.
        self.camera().setPosition(QVector3D(10, 1.5, 10))
        self.camera().setViewCenter(QVector3D(0, .5, 0))
        self.camera().setUpVector(QVector3D(0, 1, 0))
        self.camera().setAspectRatio(16. / 9.)

        # Camera controls.
        self.camController = Qt3DExtras.QOrbitCameraController(self.rootEntity)
        self.camController.setCamera(self.camera())

        # Frame graph.
        self.fg = Kuesa.ForwardRenderer()
        self.fg.setCamera(self.camera())
        self.fg.setClearColor("white")
        self.setActiveFrameGraph(self.fg)

        self.setRootEntity(self.rootEntity)

if __name__ == '__main__':
    app = QGuiApplication(sys.argv)
    view = Window()
    view.show()
    sys.exit(app.exec_())
