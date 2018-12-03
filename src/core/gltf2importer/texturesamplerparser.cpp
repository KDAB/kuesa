/*
    texturesamplerparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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

#include "texturesamplerparser_p.h"

#include "gltf2context_p.h"

#include <QJsonValue>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
using namespace Kuesa::GLTF2Import;

namespace {
const QLatin1String KEY_MIN_FILTER = QLatin1Literal("minFilter");
const QLatin1String KEY_MAG_FILTER = QLatin1Literal("magFilter");
const QLatin1String KEY_WRAP_S = QLatin1Literal("wrapS");
const QLatin1String KEY_WRAP_T = QLatin1Literal("wrapT");

bool isValidMinification(const int v)
{
    switch (v) {
    case Qt3DRender::QAbstractTexture::Filter::Nearest:
    case Qt3DRender::QAbstractTexture::Filter::Linear:
    case Qt3DRender::QAbstractTexture::Filter::NearestMipMapLinear:
    case Qt3DRender::QAbstractTexture::Filter::NearestMipMapNearest:
    case Qt3DRender::QAbstractTexture::Filter::LinearMipMapLinear:
    case Qt3DRender::QAbstractTexture::Filter::LinearMipMapNearest:
        return true;
    default:
        return false;
    }
}

bool isValidMagnification(const int v)
{
    switch (v) {
    case Qt3DRender::QAbstractTexture::Filter::Nearest:
    case Qt3DRender::QAbstractTexture::Filter::Linear:
        return true;
    default:
        return false;
    }
}

bool isValidWrapMode(const int v)
{
    switch (v) {
    case Qt3DRender::QTextureWrapMode::Repeat:
    case Qt3DRender::QTextureWrapMode::MirroredRepeat:
    case Qt3DRender::QTextureWrapMode::ClampToBorder:
    case Qt3DRender::QTextureWrapMode::ClampToEdge:
        return true;
    default:
        return false;
    }
}

} // anonymous

bool Kuesa::GLTF2Import::TextureSamplerParser::parse(const QJsonArray &imageSamplers, Kuesa::GLTF2Import::GLTF2Context *context)
{
    const int nbImageSamplers = imageSamplers.size();
    for (const auto &imageSamplerValue : imageSamplers) {
        const QJsonObject &imageSamplerObject = imageSamplerValue.toObject();
        TextureSampler textureSampler;
        textureSampler.textureWrapMode = std::make_shared<Qt3DRender::QTextureWrapMode>();
        textureSampler.textureWrapMode->setX(Qt3DRender::QTextureWrapMode::Repeat);
        textureSampler.textureWrapMode->setY(Qt3DRender::QTextureWrapMode::Repeat);
        textureSampler.textureWrapMode->setZ(Qt3DRender::QTextureWrapMode::Repeat);

        const QJsonValue minificationFilter = imageSamplerObject.value(KEY_MIN_FILTER);
        if (!minificationFilter.isUndefined()) {
            const int v = minificationFilter.toInt();
            if (!isValidMinification(v))
                return false;
            textureSampler.minificationFilter = static_cast<Qt3DRender::QAbstractTexture::Filter>(v);
        }

        const QJsonValue magnificationFilter = imageSamplerObject.value(KEY_MAG_FILTER);
        if (!magnificationFilter.isUndefined()) {
            const int v = magnificationFilter.toInt();
            if (!isValidMagnification(v))
                return false;
            textureSampler.magnificationFilter = static_cast<Qt3DRender::QAbstractTexture::Filter>(v);
        }

        const QJsonValue wrapS = imageSamplerObject.value(KEY_WRAP_S);
        if (!wrapS.isUndefined()) {
            const int v = wrapS.toInt();
            if (!isValidWrapMode(v))
                return false;
            textureSampler.textureWrapMode->setX(static_cast<Qt3DRender::QTextureWrapMode::WrapMode>(v));
        }

        const QJsonValue wrapT = imageSamplerObject.value(KEY_WRAP_T);
        if (!wrapT.isUndefined()) {
            const int v = wrapT.toInt();
            if (!isValidWrapMode(v))
                return false;
            textureSampler.textureWrapMode->setY(static_cast<Qt3DRender::QTextureWrapMode::WrapMode>(v));
        }

        context->addTextureSampler(textureSampler);
    }

    return nbImageSamplers > 0;
}

QT_END_NAMESPACE
