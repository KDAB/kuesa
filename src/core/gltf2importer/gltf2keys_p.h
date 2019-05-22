/*
    gltf2keys_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jean-Michaël Celerier <jean-michael.celerier@kdab.com>

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
*/

#ifndef KUESA_GLTF2IMPORT_GLTF2KEYS_P_H
#define KUESA_GLTF2IMPORT_GLTF2KEYS_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QtCore/qglobal.h>
#include <QtCore/QString>
#include <qtkuesa-config.h>

QT_BEGIN_NAMESPACE
namespace Kuesa {
namespace GLTF2Import {

const QLatin1String KEY_BUFFERS = QLatin1Literal("buffers");
const QLatin1String KEY_BUFFERVIEWS = QLatin1Literal("bufferViews");
const QLatin1String KEY_ACCESSORS = QLatin1Literal("accessors");
const QLatin1String KEY_NODES = QLatin1Literal("nodes");
const QLatin1String KEY_MESHES = QLatin1Literal("meshes");
const QLatin1String KEY_SCENE = QLatin1Literal("scene");
const QLatin1String KEY_SCENES = QLatin1Literal("scenes");
const QLatin1String KEY_KDAB_KUESA_LAYER_EXTENSION = QLatin1String("KDAB_Kuesa_Layers");
const QLatin1String KEY_MSFT_DDS_EXTENSION = QLatin1String("MSFT_texture_dds");
const QLatin1String KEY_KUESA_LAYERS = QLatin1Literal("layers");
const QLatin1String KEY_CAMERAS = QLatin1Literal("cameras");
const QLatin1String KEY_IMAGES = QLatin1Literal("images");
const QLatin1String KEY_TEXTURE_SAMPLERS = QLatin1Literal("samplers");
const QLatin1String KEY_TEXTURES = QLatin1String("textures");
const QLatin1String KEY_EXTENSIONS = QLatin1String("extensions");
const QLatin1String KEY_EXTENSIONS_USED = QLatin1String("extensionsUsed");
const QLatin1String KEY_EXTENSIONS_REQUIRED = QLatin1String("extensionsRequired");
const QLatin1String KEY_ANIMATIONS = QLatin1String("animations");
const QLatin1String KEY_MATERIALS = QLatin1String("materials");
const QLatin1String KEY_SKINS = QLatin1String("skins");
const QLatin1String KEY_BYTELENGTH = QLatin1String("byteLength");
const QLatin1String KEY_URI = QLatin1String("uri");
const QLatin1String KEY_PRIMITIVES = QLatin1String("primitives");
const QLatin1String KEY_NAME = QLatin1String("name");
const QLatin1String KEY_BUFFER = QLatin1String("buffer");
const QLatin1String KEY_BUFFERVIEW = QLatin1String("bufferView");
const QLatin1String KEY_ATTRIBUTES = QLatin1String("attributes");
const QLatin1String KEY_BYTEOFFSET = QLatin1String("byteOffset");
const QLatin1String KEY_BYTESTRIDE = QLatin1String("byteStride");
const QLatin1String KEY_INDICES = QLatin1String("indices");
const QLatin1String KEY_VALUES = QLatin1String("values");
const QLatin1String KEY_SPARSE = QLatin1String("sparse");
const QLatin1String KEY_MIMETYPE = QLatin1String("mimeType");
#if defined(KUESA_DRACO_COMPRESSION)
const QLatin1String KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION = QLatin1String("KHR_draco_mesh_compression");
#endif
const QLatin1String KEY_KHR_MATERIALS_UNLIT = QLatin1Literal("KHR_materials_unlit");
const QLatin1String KEY_KHR_LIGHTS_PUNCTUAL_EXTENSION = QLatin1Literal("KHR_lights_punctual");
const QLatin1String KEY_KHR_PUNCTUAL_LIGHTS = QLatin1Literal("lights");

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_GLTF2KEYS_P_H
