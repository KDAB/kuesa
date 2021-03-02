/*
    textureinfoparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include <QJsonObject>
#include <QJsonArray>
#include "textureinfoparser_p.h"
#include "gltf2keys_p.h"

namespace {
const QLatin1String KEY_INDEX = QLatin1String("index");
const QLatin1String KEY_TEXCOORD = QLatin1String("texCoord");
const QLatin1String KEY_KHR_TEXTURE_TRANSFORM = QLatin1String("KHR_texture_transform");
const QLatin1String KEY_KHR_TEXTURE_TRANSFORM_OFFSET = QLatin1String("offset");
const QLatin1String KEY_KHR_TEXTURE_TRANSFORM_SCALE = QLatin1String("scale");
const QLatin1String KEY_KHR_TEXTURE_TRANSFORM_ROTATION = QLatin1String("rotation");
} // namespace

QT_BEGIN_NAMESPACE

Kuesa::GLTF2Import::TextureInfo Kuesa::GLTF2Import::TextureInfo::parse(const QJsonObject &textureInfoObj)
{
    TextureInfo info;
    info.index = textureInfoObj.value(KEY_INDEX).toInt(-1);
    info.texCoord = std::max(textureInfoObj.value(KEY_TEXCOORD).toInt(0), 0);
    const auto &extensionsObject = textureInfoObj[KEY_EXTENSIONS];
    if (!extensionsObject.isUndefined()) {
        const auto &khr_texture_transformObject = extensionsObject[KEY_KHR_TEXTURE_TRANSFORM];
        if (!khr_texture_transformObject.isUndefined()) {
            const auto &offsetArray = khr_texture_transformObject[KEY_KHR_TEXTURE_TRANSFORM_OFFSET].toArray({ 0.0, 0.0 });
            const auto &rotationObject = khr_texture_transformObject[KEY_KHR_TEXTURE_TRANSFORM_ROTATION];
            const auto &scaleArray = khr_texture_transformObject[KEY_KHR_TEXTURE_TRANSFORM_SCALE].toArray({ 1.0, 1.0 });
            const auto &indexObject = khr_texture_transformObject[KEY_TEXCOORD];

            if (!indexObject.isUndefined())
                info.texCoord = indexObject.toInt();

            // TODO add checks
            info.khr_texture_transform = TextureInfo::KHR_texture_transform{ QVector2D(static_cast<float>(offsetArray[0].toDouble()),
                                                                                       static_cast<float>(offsetArray[1].toDouble())),
                                                                             static_cast<float>(rotationObject.toDouble(0.0)),
                                                                             QVector2D(static_cast<float>(scaleArray[0].toDouble()),
                                                                                       static_cast<float>(scaleArray[1].toDouble())) };
        }
    }
    return info;
}

QT_END_NAMESPACE
