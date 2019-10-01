import os
import platform
from PySide2.Qt3DCore import (Qt3DCore)
from PySide2.Qt3DRender import (Qt3DRender)
from .Assets import (assetsUrl)

class DefaultEnvMap(Qt3DRender.QEnvironmentLight):
    def __init__(self, parent):
        super(DefaultEnvMap, self).__init__(parent)

        envmap_root = assetsUrl() + "/envmaps/pink_sunrise"
        envmap_name = "pink_sunrise" + ("_16f" if platform.system() == "Darwin" else "")

        self.tli = Qt3DRender.QTextureLoader(self)
        self.tli.setSource(envmap_root + "/" + envmap_name + "_irradiance.dds")
        self.tli.setMinificationFilter(Qt3DRender.QAbstractTexture.LinearMipMapLinear)
        self.tli.setMagnificationFilter(Qt3DRender.QAbstractTexture.Linear)
        self.tli.wrapMode().setX(Qt3DRender.QTextureWrapMode.ClampToEdge)
        self.tli.wrapMode().setY(Qt3DRender.QTextureWrapMode.ClampToEdge)
        self.tli.setGenerateMipMaps(0)
        self.setIrradiance(self.tli)

        self.tls = Qt3DRender.QTextureLoader(self)
        self.tls.setSource(envmap_root + "/" + envmap_name + "_specular.dds")
        self.tls.setMinificationFilter(Qt3DRender.QAbstractTexture.LinearMipMapLinear)
        self.tls.setMagnificationFilter(Qt3DRender.QAbstractTexture.Linear)
        self.tls.wrapMode().setX(Qt3DRender.QTextureWrapMode.ClampToEdge)
        self.tls.wrapMode().setY(Qt3DRender.QTextureWrapMode.ClampToEdge)
        self.tls.setGenerateMipMaps(0)
        self.setSpecular(self.tls)
