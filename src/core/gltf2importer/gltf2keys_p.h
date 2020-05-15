/*
    gltf2keys_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

const QLatin1String KEY_BUFFERS = QLatin1String("buffers");
const QLatin1String KEY_BUFFERVIEWS = QLatin1String("bufferViews");
const QLatin1String KEY_ACCESSORS = QLatin1String("accessors");
const QLatin1String KEY_NODES = QLatin1String("nodes");
const QLatin1String KEY_MESHES = QLatin1String("meshes");
const QLatin1String KEY_SCENE = QLatin1String("scene");
const QLatin1String KEY_SCENES = QLatin1String("scenes");
const QLatin1String KEY_KDAB_KUESA_LAYER_EXTENSION = QLatin1String("KDAB_kuesa_layers");
const QLatin1String KEY_KDAB_KUESA_SHADOWS_EXTENSION = QLatin1String("KDAB_kuesa_shadows");
const QLatin1String KEY_MSFT_DDS_EXTENSION = QLatin1String("MSFT_texture_dds");
const QLatin1String KEY_KUESA_LAYERS = QLatin1String("layers");
const QLatin1String KEY_KUESA_SHADOWS = QLatin1String("shadows");
const QLatin1String KEY_CAMERAS = QLatin1String("cameras");
const QLatin1String KEY_IMAGES = QLatin1String("images");
const QLatin1String KEY_TEXTURE_SAMPLERS = QLatin1String("samplers");
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
const QLatin1String KEY_KHR_MATERIALS_UNLIT = QLatin1String("KHR_materials_unlit");
const QLatin1String KEY_KDAB_CUSTOM_MATERIAL = QLatin1String("KDAB_custom_material");
const QLatin1String KEY_KHR_LIGHTS_PUNCTUAL_EXTENSION = QLatin1String("KHR_lights_punctual");
const QLatin1String KEY_EXT_PROPERTY_ANIMATION_EXTENSION = QLatin1String("EXT_property_animation");
const QLatin1String KEY_KHR_TEXTURE_TRANSFORM = QLatin1String("KHR_texture_transform");
const QLatin1String KEY_KHR_PUNCTUAL_LIGHTS = QLatin1String("lights");
const QLatin1String KEY_KHR_PUNCTUAL_LIGHT = QLatin1String("light");
const QLatin1String KEY_KDAB_REFLECTION_PLANES_EXTENSION = QLatin1String("KDAB_kuesa_reflection_planes");
const QLatin1String KEY_PLANE = QLatin1String("plane");
const QLatin1String KEY_BUFFERVIEWTARGET = QLatin1String("target");
const QLatin1String KEY_COMPONENTTYPE = QLatin1String("componentType");
const QLatin1String KEY_COUNT = QLatin1String("count");
const QLatin1String KEY_TYPE = QLatin1String("type");
const QLatin1String KEY_MAX = QLatin1String("max");
const QLatin1String KEY_MIN = QLatin1String("min");
const QLatin1String KEY_MODE = QLatin1String("mode");
const QLatin1String KEY_TARGETS = QLatin1String("targets");
const QLatin1String KEY_WEIGHTS = QLatin1String("weights");
const QLatin1String KEY_NORMALIZED = QLatin1String("normalized");
const QLatin1String KEY_SPARSE_INDICES = QLatin1String("indices");
const QLatin1String KEY_SPARSE_VALUES = QLatin1String("values");
const QLatin1String KEY_MATERIAL = QLatin1String("material");
const QLatin1String KEY_CHANNELS = QLatin1String("channels");
const QLatin1String KEY_SAMPLERS = QLatin1String("samplers");
const QLatin1String KEY_SAMPLER = QLatin1String("sampler");
const QLatin1String KEY_TARGET = QLatin1String("target");
const QLatin1String KEY_INPUT = QLatin1String("input");
const QLatin1String KEY_OUTPUT = QLatin1String("output");
const QLatin1String KEY_INTERPOLATION = QLatin1String("interpolation");
const QLatin1String KEY_PATH = QLatin1String("path");
const QLatin1String KEY_NODE = QLatin1String("node");
const QLatin1String KEY_MATRIX = QLatin1String("matrix");
const QLatin1String KEY_TRANSLATION = QLatin1String("translation");
const QLatin1String KEY_ROTATION = QLatin1String("rotation");
const QLatin1String KEY_SCALE = QLatin1String("scale");
const QLatin1String KEY_MESH = QLatin1String("mesh");
const QLatin1String KEY_CAMERA = QLatin1String("camera");
const QLatin1String KEY_SKIN = QLatin1String("skin");
const QLatin1String KEY_CHILDREN = QLatin1String("children");
const QLatin1String KEY_NODE_KUESA_LAYERS = QLatin1String("layers");

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_GLTF2KEYS_P_H
