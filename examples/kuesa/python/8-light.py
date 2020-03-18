# 7-qwidgets.py
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
from PySide2.QtCore import(Qt, Property, QObject, QPropertyAnimation, Signal, Slot)
from PySide2.QtGui import (QGuiApplication, QMatrix4x4, QQuaternion, QVector3D, QWindow)
from PySide2.QtWidgets import (QApplication, QMainWindow, QSlider, QHBoxLayout, QFormLayout, QWidget, QCheckBox, QSizePolicy)
from PySide2.Qt3DCore import (Qt3DCore)
from PySide2.Qt3DRender import (Qt3DRender)
from PySide2.Qt3DExtras import (Qt3DExtras)
from Kuesa import (Kuesa)
from KuesaUtils.DefaultEnvMap import (DefaultEnvMap)
from KuesaUtils.Assets import (assetsUrl)

# Create our scene as usual
class Window(Qt3DExtras.Qt3DWindow):
    def __init__(self):
        super(Window, self).__init__()

        # Default set-up - by now you know the drill :-)
        self.rootEntity = Kuesa.SceneEntity()
        self.rootEntity.addComponent(DefaultEnvMap(self.rootEntity))

        self.gltfImporter = Kuesa.GLTF2Importer(self.rootEntity)
        self.gltfImporter.setSceneEntity(self.rootEntity)
        self.gltfImporter.setSource(assetsUrl() + "/models/duck/Duck.glb")

        self.camera().setPosition(QVector3D(10, 1.5, 10))
        self.camera().setViewCenter(QVector3D(0, .5, 0))
        self.camera().setUpVector(QVector3D(0, 1, 0))
        self.camera().setAspectRatio(16. / 9.)

        self.camController = Qt3DExtras.QOrbitCameraController(self.rootEntity)
        self.camController.setCamera(self.camera())

        self.fg = Kuesa.ForwardRenderer()
        self.fg.setCamera(self.camera())
        self.fg.setClearColor("white")
        self.setActiveFrameGraph(self.fg)

        envmap_root = assetsUrl() + "/envmaps/pink_sunrise"
        envmap_name = "pink_sunrise" + ("_16f" if platform.system() == "Darwin" else "") + "_radiance"
        self.skybox = Kuesa.Skybox(self.rootEntity)
        self.skybox.setBaseName(envmap_root  + "/" + envmap_name)
        self.skybox.setExtension(".dds")

        # Now we create some lights.
        # All lights have an intensity and a color.
        # Specific light types have relevant additional properties.
        # Point light
        self.pointLightEntity = Qt3DCore.QEntity(self.rootEntity)
        self.pointLightTransform = Qt3DCore.QTransform(self.pointLightEntity)
        self.pointLightTransform.setTranslation(QVector3D(20, -10, -10))

        self.pointLight = Kuesa.PointLight(self.pointLightEntity)
        self.pointLight.setIntensity(1000.0)
        self.pointLight.setColor("red")
        
        self.pointLightEntity.addComponent(self.pointLightTransform)
        self.pointLightEntity.addComponent(self.pointLight)
        
        # Spot light
        self.spotLightEntity = Qt3DCore.QEntity(self.rootEntity)
        self.spotLightTransform = Qt3DCore.QTransform(self.spotLightEntity)
        self.spotLightTransform.setTranslation(QVector3D(-10, 10, 10))
        
        self.spotLight = Kuesa.SpotLight(self.spotLightEntity)
        self.spotLight.setIntensity(1000.0)
        self.spotLight.setColor("green")
        self.spotLight.setInnerConeAngle(50)
        self.spotLight.setOuterConeAngle(100)
        self.spotLight.setRange(1000)

        self.spotLightEntity.addComponent(self.spotLightTransform)
        self.spotLightEntity.addComponent(self.spotLight)
        
        # Directional light
        self.directionalLightEntity = Qt3DCore.QEntity(self.rootEntity)
        self.directionalLightTransform = Qt3DCore.QTransform(self.directionalLightEntity)
        
        self.directionalLight = Kuesa.DirectionalLight(self.directionalLightEntity)
        self.directionalLight.setIntensity(100.0)
        self.directionalLight.setColor("blue")
        self.directionalLight.setDirection(QVector3D(10, 10, -10))
        
        self.directionalLightEntity.addComponent(self.directionalLightTransform)
        self.directionalLightEntity.addComponent(self.directionalLight)

        self.setRootEntity(self.rootEntity)

    def enablePointLight(self, val):
        self.pointLight.setIntensity(val) 

    def enableSpotLight(self, val):
        self.spotLight.setIntensity(val)

    def enableDirectionalLight(self, val):
        self.directionalLight.setIntensity(val)

if __name__ == '__main__':
    app = QApplication(sys.argv)

    # Create some widgets to control light intensity
    view = QMainWindow()

    mainWidg = QWidget()
    mainLay = QHBoxLayout(mainWidg)
    
    controlWidg = QWidget()
    controlWidg.setMaximumWidth(400)
    controlLay = QFormLayout(controlWidg)
    c1 = QSlider(Qt.Horizontal)
    c1.setRange(0, 1000)
    c1.setValue(1000)
    c2 = QSlider(Qt.Horizontal)
    c2.setRange(0, 1000)
    c2.setValue(1000)
    c3 = QSlider(Qt.Horizontal)
    c3.setRange(0, 100)
    c3.setValue(100)

    controlLay.addRow("Point light", c1)
    controlLay.addRow("Spot light", c2)
    controlLay.addRow("Directional light", c3)

    # We use QWidget.createWindowContainer to host the 3D content without any indirection.
    view3d = Window()
    container = QWidget.createWindowContainer(view3d, view)
    container.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
    container.setMinimumWidth(500)
    container.setMinimumHeight(500)


    mainLay.addWidget(controlWidg)
    mainLay.addWidget(container)
    
    view.setCentralWidget(mainWidg)
   
    view.show()

    # Communication between the QWidget UI and the 3D scene
    c1.valueChanged.connect(lambda val: view3d.enablePointLight(val))
    c2.valueChanged.connect(lambda val: view3d.enableSpotLight(val))
    c3.valueChanged.connect(lambda val: view3d.enableDirectionalLight(val))

    sys.exit(app.exec_())
