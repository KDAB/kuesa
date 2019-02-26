/*
    imageparser.cpp

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

#include "imageparser_p.h"
#include "gltf2uri_p.h"

#include <kuesa_p.h>
#include <gltf2context_p.h>

#include <QJsonObject>

QT_BEGIN_NAMESPACE
using namespace Kuesa::GLTF2Import;

const QLatin1String KEY_URI = QLatin1Literal("uri");
const QLatin1String KEY_BUFFER_VIEW = QLatin1Literal("bufferView");
const QLatin1String KEY_NAME = QLatin1Literal("name");
const QLatin1String KEY_MIMETYPE = QLatin1Literal("mimeType");

ImageParser::ImageParser(const QDir &basePath)
    : m_basePath(basePath)
{
}

bool ImageParser::parse(const QJsonArray &imageArray, GLTF2Context *context) const
{
    const int nbImages = imageArray.size();
    for (const auto &imageValue : imageArray) {
        const auto imageObject = imageValue.toObject();
        const auto &uriValue = imageObject.value(KEY_URI);
        const auto &bufferViewValue = imageObject.value(KEY_BUFFER_VIEW);

        if (uriValue.isUndefined() && bufferViewValue.isUndefined()) {
            qCWarning(kuesa, "An image needs an uri or a bufferView");
            return false;
        }

        if (!uriValue.isUndefined() && !bufferViewValue.isUndefined()) {
            qCWarning(kuesa, "An image needs only a bufferView or an uri");
            return false;
        }

        auto image = Image();
        if (bufferViewValue.isUndefined()) {
            const QString uriString = uriValue.toString();
            switch (Uri::kind(uriString)) {
            case Uri::Kind::Data: {
                image.data = Uri::parseEmbeddedData(uriString);
                if (image.data.isEmpty())
                    return false;
                image.url = uriString;
                break;
            }
            case Uri::Kind::Path: {
                context->addLocalFile(uriString);
                image.url = Uri::absoluteUrl(uriString, m_basePath);
                break;
            }
            }
        } else {
            const BufferView bufferData = context->bufferView(bufferViewValue.toInt());
            image.data = bufferData.bufferData;

            const auto &mimeTypeValue = imageObject.value(KEY_MIMETYPE);
            if (mimeTypeValue.isUndefined()) {
                qCWarning(kuesa, "Missing mime type for image buffer");
                return false;
            }
            image.mimeType = mimeTypeValue.toString();
        }

        image.name = imageObject[KEY_NAME].toString();
        context->addImage(image);
    }

    return nbImages > 0;
}

QT_END_NAMESPACE
