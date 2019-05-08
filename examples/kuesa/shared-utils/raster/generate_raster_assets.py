#!/usr/bin/env python3

from PySide2.QtCore import SIGNAL, SLOT, QObject
from PySide2.QtGui import QGuiApplication
from PySide2.QtQuick import QQuickView
from PySide2.QtQml import QQmlEngine
from PySide2.QtCore import QUrl

url = QUrl("generate_raster_assets.qml")
app = QGuiApplication([])
view = QQuickView()
engine = view.engine()

QObject.connect(engine, SIGNAL("quit()"), app, SLOT("quit()"))

view.setSource(url)
view.show()
app.exec_()
