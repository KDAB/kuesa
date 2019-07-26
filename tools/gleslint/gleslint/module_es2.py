# coding: utf8

"""
    module_es2.py

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Timo Buske <timo.buske@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""

from .module import Module
from .dxgiformat import DxgiFormat
from .dds_loader import dds_loader
import sys
import subprocess

class ModuleES2 (Module):

    SUPPORTED_DXGI_FORMATS = [
        "DXGI_FORMAT_R8G8B8A8_TYPELESS",
        "DXGI_FORMAT_R8G8B8A8_UNORM",
        "DXGI_FORMAT_R8G8B8A8_UINT",
        "DXGI_FORMAT_R8_TYPELESS",
        "DXGI_FORMAT_R8_UNORM",
        "DXGI_FORMAT_R8_UINT",
        "DXGI_FORMAT_A8_UNORM",
    ]

    def __init__(self, path, argv):
        Module.__init__(self, path, argv)

    def lint_dds(self, path):
        dds = dds_loader.DDSTexture()
        try:
            dds.load(path)
        except:
            self.add_error(path, "Could not read file.")
            return

        dxgi_format_name = DxgiFormat.get_name_by_index(dds.dxgi_format)
        if dxgi_format_name not in ModuleES2.SUPPORTED_DXGI_FORMATS:
            self.add_error(path, "Unsupported texture format '{}'.".format(dxgi_format_name))

        if dds.type == dds_loader.DDSTexture.Type.TextureCube and dds.mipmap_count > 1:
            self.add_warning(path, "Cubemap with mipmaps can't be used as specular env map. Convert to octahedron map instead!")

        if dds.bpp_or_block_size > 4:
            self.add_warning(path, "Bytes per pixel > 4. Check format!")

        if dds.is_compressed:
            self.add_warning(path, "Compression detected. Check if supported!")

    def lint_png(self, path):
        self.lint_standard_image(path)

    def lint_jpg(self, path):
        self.lint_standard_image(path)

    def lint_standard_image(self, path):
        result = None
        try:
            result = subprocess.check_output(["identify", path]) # Python 2
        except:
            self.add_error(path, "Could not read file.")
            return

        if b"sRGB" in result:
            self.add_warning(path, "sRGB detected! Must not end up in *SRGB* GL texture!")

        if b"8-bit" not in result:
            self.add_error(path, "Image is not 8-bit.")
