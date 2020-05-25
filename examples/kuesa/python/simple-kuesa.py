# simple-kuesa.py
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

import sys
import os
import platform
from PySide2.QtCore import(Property, QObject, QPropertyAnimation, Signal, Slot)
from PySide2.QtGui import (QGuiApplication, QMatrix4x4, QQuaternion, QVector3D, QWindow)
from PySide2.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QCheckBox, QPushButton, QApplication)
from PySide2.Qt3DCore import (Qt3DCore)
from PySide2.Qt3DRender import (Qt3DRender)
from PySide2.Qt3DExtras import (Qt3DExtras)
from PySide2.Qt3DAnimation import (Qt3DAnimation)
from Kuesa import (Kuesa)

class OrbitTransformController(QObject):
    def __init__(self, parent):
        super(OrbitTransformController, self).__init__(parent)
        self._target = None
        self._matrix = QMatrix4x4()
        self._radius = 1
        self._angle = 0

    def setTarget(self, t):
        self._target = t

    def getTarget(self):
        return self._target

    def setRadius(self, radius):
        if self._radius != radius:
            self._radius = radius
            self.updateMatrix()
            self.radiusChanged.emit()

    def getRadius(self):
        return self._radius

    def setAngle(self, angle):
        if self._angle != angle:
            self._angle = angle
            self.updateMatrix()
            self.angleChanged.emit()

    def getAngle(self):
        return self._angle

    def updateMatrix(self):
        self._matrix.setToIdentity()
        self._matrix.rotate(self._angle, QVector3D(0, 1, 0))
        self._matrix.translate(self._radius, 0, 0)
        if self._target is not None:
            self._target.setMatrix(self._matrix)

    angleChanged = Signal()
    radiusChanged = Signal()
    angle = Property(float, getAngle, setAngle, notify=angleChanged)
    radius = Property(float, getRadius, setRadius, notify=radiusChanged)

class Window(Qt3DExtras.Qt3DWindow):
    def __init__(self):
        super(Window, self).__init__()

        self.animationAspect = Qt3DAnimation.QAnimationAspect(self)
        self.registerAspect(self.animationAspect)

        # Camera
        self.camera().setPosition(QVector3D(5.5, 1.5, 5.5))
        self.camera().setViewCenter(QVector3D(0, .5, 0))
        self.camera().setUpVector(QVector3D(0, 1, 0))
        self.camera().setAspectRatio(4. / 3.)

        # For camera controls
        self.createScene()
        self.camController = Qt3DExtras.QOrbitCameraController(self.rootEntity)
        self.camController.setLinearSpeed(50)
        self.camController.setLookSpeed(180)
        self.camController.setCamera(self.camera())

        self.fg = Kuesa.ForwardRenderer()
        self.fg.setCamera(self.camera())
        self.fg.setClearColor("white")
        self.setActiveFrameGraph(self.fg)

        self.setRootEntity(self.rootEntity)

    def createScene(self):
        wd = os.path.dirname(os.path.abspath(__file__))

        # Root entity
        self.rootEntity = Kuesa.SceneEntity()
        self.rootEntity.loadingDone.connect(self.onLoadingDone)

        self.gltfImporter = Kuesa.GLTF2Importer(self.rootEntity)
        self.gltfImporter.setSceneEntity(self.rootEntity)
        self.gltfImporter.setSource("file://" + wd + "/../assets/models/car/DodgeViper-draco.gltf")

        self.el = Qt3DRender.QEnvironmentLight(self.rootEntity)
        self.tli = Qt3DRender.QTextureLoader(self.rootEntity)
        self.tli.setSource("file://" + wd + "/../assets/envmaps/pink_sunrise/pink_sunrise" + ("_16f" if platform.system() == "Darwin" else "") + "_irradiance.dds")
        self.tli.setMinificationFilter(Qt3DRender.QAbstractTexture.LinearMipMapLinear)
        self.tli.setMagnificationFilter(Qt3DRender.QAbstractTexture.Linear)
        self.tli.wrapMode().setX(Qt3DRender.QTextureWrapMode.ClampToEdge)
        self.tli.wrapMode().setY(Qt3DRender.QTextureWrapMode.ClampToEdge)
        self.tli.setGenerateMipMaps(0)
        self.el.setIrradiance(self.tli)
        self.tls = Qt3DRender.QTextureLoader(self.rootEntity)
        self.tls.setSource("file://" + wd + "/../assets/envmaps/pink_sunrise/pink_sunrise" + ("_16f" if platform.system() == "Darwin" else "") + "_specular.dds")
        self.tls.setMinificationFilter(Qt3DRender.QAbstractTexture.LinearMipMapLinear)
        self.tls.setMagnificationFilter(Qt3DRender.QAbstractTexture.Linear)
        self.tls.wrapMode().setX(Qt3DRender.QTextureWrapMode.ClampToEdge)
        self.tls.wrapMode().setY(Qt3DRender.QTextureWrapMode.ClampToEdge)
        self.tls.setGenerateMipMaps(0)
        self.el.setSpecular(self.tls)
        self.rootEntity.addComponent(self.el)

    def onLoadingDone(self):
        self.hoodClock = Qt3DAnimation.QClock(self.rootEntity)
        self.hoodAnimation = Kuesa.AnimationPlayer(self.rootEntity)
        self.hoodAnimation.setClock(self.hoodClock)
        self.hoodAnimation.setSceneEntity(self.rootEntity)
        self.hoodAnimation.setClip("HoodAction")

        self.leftDoorClock = Qt3DAnimation.QClock(self.rootEntity)
        self.leftDoorAnimation = Kuesa.AnimationPlayer(self.rootEntity)
        self.leftDoorAnimation.setClock(self.leftDoorClock)
        self.leftDoorAnimation.setSceneEntity(self.rootEntity)
        self.leftDoorAnimation.setClip("DoorLAction")

        self.rightDoorClock = Qt3DAnimation.QClock(self.rootEntity)
        self.rightDoorAnimation = Kuesa.AnimationPlayer(self.rootEntity)
        self.rightDoorAnimation.setClock(self.rightDoorClock)
        self.rightDoorAnimation.setSceneEntity(self.rootEntity)
        self.rightDoorAnimation.setClip("DoorRAction")

        self.sweepCam = self.rootEntity.camera("SweepCam")
        if self.sweepCam:
            self.sweepCam.setAspectRatio(self.width() / self.height())
            self.sweepCamCenterAnimation = Kuesa.AnimationPlayer(self.rootEntity)
            self.sweepCamCenterAnimation.setSceneEntity(self.rootEntity)
            self.sweepCamCenterAnimation.setClip("SweepCamCenterAction")
            self.sweepCamCenterAnimation.setLoopCount(Kuesa.AnimationPlayer.Infinite)
            self.sweepCamPitchAnimation = Kuesa.AnimationPlayer(self.rootEntity)
            self.sweepCamPitchAnimation.setSceneEntity(self.rootEntity)
            self.sweepCamPitchAnimation.setClip("SweepCamPitchAction")
            self.sweepCamPitchAnimation.setLoopCount(Kuesa.AnimationPlayer.Infinite)

    def toggleCameraAnimation(self, running):
        self.camController.enabled = not running
        if running:
            self.fg.setCamera(self.sweepCam)
            self.sweepCamCenterAnimation.start()
            self.sweepCamPitchAnimation.start()
        else:
            self.fg.setCamera(self.camera())
            self.sweepCamCenterAnimation.stop()
            self.sweepCamPitchAnimation.stop()

    def toggleHood(self, open):
        self.flipAnimation(self.hoodAnimation, self.hoodClock, open)

    def toggleLeftDoor(self, open):
        self.flipAnimation(self.leftDoorAnimation, self.leftDoorClock, open)

    def toggleRightDoor(self, open):
        self.flipAnimation(self.rightDoorAnimation, self.rightDoorClock, open)

    def flipAnimation(self, animation, clock, open):
        if open:
            clock.setPlaybackRate(1)
            if animation.normalizedTime() > 0.:
                animation.setNormalizedTime(1 - animation.normalizedTime())
            animation.start()
        else:
            clock.setPlaybackRate(-1)
            if animation.normalizedTime() > 0 and animation.normalizedTime() < 1:
                animation.setNormalizedTime(1 - animation.normalizedTime())
            animation.start()


class MainWindow(QWidget):
    def __init__(self):
        super(MainWindow, self).__init__()

        self.window3d = Window()

        mainLayout = QVBoxLayout()
        mainLayout.addWidget(QWidget.createWindowContainer(self.window3d, self))
        mainLayout.setContentsMargins(0, 0, 0, 0)

        bottomLayout = QHBoxLayout()
        bottomLayout.setContentsMargins(20, 0, 20, 20)
        bottomLayout.addStretch()
        openHoodCB = QCheckBox("Open Hood", self)
        openHoodCB.toggled.connect(self.window3d.toggleHood)
        bottomLayout.addWidget(openHoodCB)
        bottomLayout.addStretch()
        openLeftDoorCB = QCheckBox("Open Left Door", self)
        openLeftDoorCB.toggled.connect(self.window3d.toggleLeftDoor)
        bottomLayout.addWidget(openLeftDoorCB)
        bottomLayout.addStretch()
        openRightDoorCB = QCheckBox("Open Right Door", self)
        openRightDoorCB.toggled.connect(self.window3d.toggleRightDoor)
        bottomLayout.addWidget(openRightDoorCB)
        bottomLayout.addStretch()
        self.toggleCameraAnimationPB = QPushButton("Play", self)
        self.toggleCameraAnimationPB.setCheckable(True)
        self.toggleCameraAnimationPB.toggled.connect(self.toggleCameraAnimation)
        bottomLayout.addWidget(self.toggleCameraAnimationPB)
        bottomLayout.addStretch()
        mainLayout.addLayout(bottomLayout)

        self.setLayout(mainLayout)
        self.resize(1024, 768)

    def toggleCameraAnimation(self, checked):
        if checked:
            self.toggleCameraAnimationPB.setText("Pause")
        else:
            self.toggleCameraAnimationPB.setText("Play")
        self.window3d.toggleCameraAnimation(checked)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    view = MainWindow()
    view.show()
    sys.exit(app.exec_())
