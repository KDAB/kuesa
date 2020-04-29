/*
    bufferparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "bufferparser_p.h"

#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QDebug>

#include "gltf2context_p.h"
#include "gltf2uri_p.h"
#include "kuesa_p.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {
const QLatin1String KEY_URI = QLatin1String("uri");
const QLatin1String KEY_NAME = QLatin1String("name");
const QLatin1String KEY_BYTE_LENGTH = QLatin1String("byteLength");
} // namespace

/*!
 * \class Kuesa::GLTF2Import::BufferParser
 *
 * \brief Parses a GLTF2 JSON Buffer description and stores its content in a \a
 * GLTF2Context.
 * \internal
 */

BufferParser::BufferParser(const QDir &basePath)
    : m_basePath(basePath)
{
}

/*!
 * Returns true if the buffers are successfully parsed. Returns false otherwise.
 *
 * The buffers are stored in the \a context
 * The return value may be false under the following case:
 * \list
 * \li The buffers description doesn't contain any buffer entry
 * \li One or more of the referenced buffer description is invalid
 * \li One or more of the referenced buffer resources failed to be read
 * \li One or more of the referenced buffer resources have a different size
 * than what the description specified
 * \endlist
 * \internal
 */
bool BufferParser::parse(const QJsonArray &buffersArray, GLTF2Context *context) const
{
    const qint32 bufferDataSize = buffersArray.size();
    for (qint32 bufferId = 0; bufferId < bufferDataSize; ++bufferId) {
        const QJsonObject bufferObject = buffersArray[bufferId].toObject();
        const QString uri = bufferObject.value(KEY_URI).toString();
        const qint32 expectedSize = bufferObject.value(KEY_BYTE_LENGTH).toInt();
        const QString bufferName = bufferObject.value(KEY_NAME).toString();
        bool readSuccess = false;

        if (!uri.isNull()) {
            const QByteArray data = Uri::fetchData(uri, m_basePath, readSuccess);
            if (readSuccess) {
                if (Uri::kind(uri) == Uri::Kind::Path)
                    context->addLocalFile(uri);

                const bool hasExpectedSize = data.size() == expectedSize;
                if (hasExpectedSize) {
                    context->addBuffer(data);
                } else {
                    qCWarning(kuesa) << "Unexpected size of" << data.size() << "bytes for buffer" << bufferName << " expected" << expectedSize << "bytes";
                    return false;
                }
            }
        } else if (!context->bufferChunk().isEmpty() && bufferId == 0) {
            readSuccess = true;
            context->addBuffer(context->bufferChunk());
        }

        if (!readSuccess) {
            qCWarning(kuesa) << "Failed to read buffer" << bufferName << " (" << bufferId << ")";
            return false;
        }
    }

    return bufferDataSize > 0;
}
QT_END_NAMESPACE
