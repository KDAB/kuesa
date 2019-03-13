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

        if dds.bpp_or_block_size > 3:
            self.add_warning(path, "Bytes per pixel > 3. Check format!")

        if dds.is_compressed:
            self.add_warning(path, "Compression detected. Check if supported!")

    def lint_png(self, path):
        self.lint_standard_image(path)

    def lint_jpg(self, path):
        self.lint_standard_image(path)

    def lint_standard_image(self, path):
        result = None
        try:
            result = subprocess.run(["identify", path], capture_output=True)
        except:
            self.add_error(path, "Could not read file.")
            return
        stdout = result.stdout

        if b"sRGB" in stdout:
            self.add_warning(path, "sRGB detected! Must not end up in *SRGB* GL texture!")

        if b"8-bit" not in stdout:
            self.add_error(path, "Image is not 8-bit.")
