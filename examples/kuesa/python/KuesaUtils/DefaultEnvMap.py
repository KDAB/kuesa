#
# This file is part of Kuesa.
#
# Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
