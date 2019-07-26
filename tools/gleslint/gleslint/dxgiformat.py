# coding: utf8

"""
    dxgiformat.py

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

class DxgiFormat (object):

    FORMATS = [
        "DXGI_FORMAT_UNKNOWN",
        "DXGI_FORMAT_R32G32B32A32_TYPELESS",
        "DXGI_FORMAT_R32G32B32A32_FLOAT",
        "DXGI_FORMAT_R32G32B32A32_UINT",
        "DXGI_FORMAT_R32G32B32A32_SINT",
        "DXGI_FORMAT_R32G32B32_TYPELESS",
        "DXGI_FORMAT_R32G32B32_FLOAT",
        "DXGI_FORMAT_R32G32B32_UINT",
        "DXGI_FORMAT_R32G32B32_SINT",
        "DXGI_FORMAT_R16G16B16A16_TYPELESS",
        "DXGI_FORMAT_R16G16B16A16_FLOAT",
        "DXGI_FORMAT_R16G16B16A16_UNORM",
        "DXGI_FORMAT_R16G16B16A16_UINT",
        "DXGI_FORMAT_R16G16B16A16_SNORM",
        "DXGI_FORMAT_R16G16B16A16_SINT",
        "DXGI_FORMAT_R32G32_TYPELESS",
        "DXGI_FORMAT_R32G32_FLOAT",
        "DXGI_FORMAT_R32G32_UINT",
        "DXGI_FORMAT_R32G32_SINT",
        "DXGI_FORMAT_R32G8X24_TYPELESS",
        "DXGI_FORMAT_D32_FLOAT_S8X24_UINT",
        "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS",
        "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT",
        "DXGI_FORMAT_R10G10B10A2_TYPELESS",
        "DXGI_FORMAT_R10G10B10A2_UNORM",
        "DXGI_FORMAT_R10G10B10A2_UINT",
        "DXGI_FORMAT_R11G11B10_FLOAT",
        "DXGI_FORMAT_R8G8B8A8_TYPELESS",
        "DXGI_FORMAT_R8G8B8A8_UNORM",
        "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB",
        "DXGI_FORMAT_R8G8B8A8_UINT",
        "DXGI_FORMAT_R8G8B8A8_SNORM",
        "DXGI_FORMAT_R8G8B8A8_SINT",
        "DXGI_FORMAT_R16G16_TYPELESS",
        "DXGI_FORMAT_R16G16_FLOAT",
        "DXGI_FORMAT_R16G16_UNORM",
        "DXGI_FORMAT_R16G16_UINT",
        "DXGI_FORMAT_R16G16_SNORM",
        "DXGI_FORMAT_R16G16_SINT",
        "DXGI_FORMAT_R32_TYPELESS",
        "DXGI_FORMAT_D32_FLOAT",
        "DXGI_FORMAT_R32_FLOAT",
        "DXGI_FORMAT_R32_UINT",
        "DXGI_FORMAT_R32_SINT",
        "DXGI_FORMAT_R24G8_TYPELESS",
        "DXGI_FORMAT_D24_UNORM_S8_UINT",
        "DXGI_FORMAT_R24_UNORM_X8_TYPELESS",
        "DXGI_FORMAT_X24_TYPELESS_G8_UINT",
        "DXGI_FORMAT_R8G8_TYPELESS",
        "DXGI_FORMAT_R8G8_UNORM",
        "DXGI_FORMAT_R8G8_UINT",
        "DXGI_FORMAT_R8G8_SNORM",
        "DXGI_FORMAT_R8G8_SINT",
        "DXGI_FORMAT_R16_TYPELESS",
        "DXGI_FORMAT_R16_FLOAT",
        "DXGI_FORMAT_D16_UNORM",
        "DXGI_FORMAT_R16_UNORM",
        "DXGI_FORMAT_R16_UINT",
        "DXGI_FORMAT_R16_SNORM",
        "DXGI_FORMAT_R16_SINT",
        "DXGI_FORMAT_R8_TYPELESS",
        "DXGI_FORMAT_R8_UNORM",
        "DXGI_FORMAT_R8_UINT",
        "DXGI_FORMAT_R8_SNORM",
        "DXGI_FORMAT_R8_SINT",
        "DXGI_FORMAT_A8_UNORM",
        "DXGI_FORMAT_R1_UNORM",
        "DXGI_FORMAT_R9G9B9E5_SHAREDEXP",
        "DXGI_FORMAT_R8G8_B8G8_UNORM",
        "DXGI_FORMAT_G8R8_G8B8_UNORM",
        "DXGI_FORMAT_BC1_TYPELESS",
        "DXGI_FORMAT_BC1_UNORM",
        "DXGI_FORMAT_BC1_UNORM_SRGB",
        "DXGI_FORMAT_BC2_TYPELESS",
        "DXGI_FORMAT_BC2_UNORM",
        "DXGI_FORMAT_BC2_UNORM_SRGB",
        "DXGI_FORMAT_BC3_TYPELESS",
        "DXGI_FORMAT_BC3_UNORM",
        "DXGI_FORMAT_BC3_UNORM_SRGB",
        "DXGI_FORMAT_BC4_TYPELESS",
        "DXGI_FORMAT_BC4_UNORM",
        "DXGI_FORMAT_BC4_SNORM",
        "DXGI_FORMAT_BC5_TYPELESS",
        "DXGI_FORMAT_BC5_UNORM",
        "DXGI_FORMAT_BC5_SNORM",
        "DXGI_FORMAT_B5G6R5_UNORM",
        "DXGI_FORMAT_B5G5R5A1_UNORM",
        "DXGI_FORMAT_B8G8R8A8_UNORM",
        "DXGI_FORMAT_B8G8R8X8_UNORM",
        "DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNOR",
        "DXGI_FORMAT_B8G8R8A8_TYPELESS",
        "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB",
        "DXGI_FORMAT_B8G8R8X8_TYPELESS",
        "DXGI_FORMAT_B8G8R8X8_UNORM_SRGB",
        "DXGI_FORMAT_BC6H_TYPELESS",
        "DXGI_FORMAT_BC6H_UF16",
        "DXGI_FORMAT_BC6H_SF16",
        "DXGI_FORMAT_BC7_TYPELESS",
        "DXGI_FORMAT_BC7_UNORM",
        "DXGI_FORMAT_BC7_UNORM_SRGB",
        "DXGI_FORMAT_AYUV",
        "DXGI_FORMAT_Y410",
        "DXGI_FORMAT_Y416",
        "DXGI_FORMAT_NV12",
        "DXGI_FORMAT_P010",
        "DXGI_FORMAT_P016",
        "DXGI_FORMAT_420_OPAQUE",
        "DXGI_FORMAT_YUY2",
        "DXGI_FORMAT_Y210",
        "DXGI_FORMAT_Y216",
        "DXGI_FORMAT_NV11",
        "DXGI_FORMAT_AI44",
        "DXGI_FORMAT_IA44",
        "DXGI_FORMAT_P8",
        "DXGI_FORMAT_A8P8",
        "DXGI_FORMAT_B4G4R4A4_UNORM",
        "DXGI_FORMAT_P208",
        "DXGI_FORMAT_V208",
        "DXGI_FORMAT_V408",
        "DXGI_FORMAT_FORCE_UINT"
    ]

    cached = False
    reverse = {}

    @staticmethod
    def cache_reverse():
        if not DxgiFormat.cached:
            DxgiFormat.cached = True
            for index, name in enumerate(DxgiFormats.FORMATS):
                DxgiFormat.reverse[name] = index

    @staticmethod
    def get_index_by_name(name):
        DxgiFormat.cache_reverse()
        return DxgiFormat.reverse[name]

    @staticmethod
    def get_name_by_index(index):
        return DxgiFormat.FORMATS[index]
