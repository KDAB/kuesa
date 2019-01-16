/*
    bufferparser.cpp

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

#include "bufferparser_p.h"

#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QDebug>

#include "gltf2context_p.h"
#include "kuesa_p.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {
const QLatin1String KEY_URI = QLatin1Literal("uri");
const QLatin1String KEY_NAME = QLatin1Literal("name");
const QLatin1String KEY_BYTE_LENGTH = QLatin1Literal("byteLength");
} // namespace

/*!
 * \class BufferParser
 *
 * \brief Parses a GLTF2 JSON Buffer description and stores its content in a \a GLTF2Context.
 *
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
 *
 */
bool BufferParser::parse(const QJsonArray &buffersArray, GLTF2ContextPrivate *context) const
{
    const int bufferDataSize = buffersArray.size();
    for (int bufferId = 0; bufferId < bufferDataSize; ++bufferId) {
        const QJsonObject bufferObject = buffersArray[bufferId].toObject();
        const QString uri = bufferObject.value(KEY_URI).toString();
        const int expectedSize = bufferObject.value(KEY_BYTE_LENGTH).toInt();
        const QString bufferName = bufferObject.value(KEY_NAME).toString();
        bool readSuccess = false;

        if (!uri.isNull()) {
            const QByteArray data = dataFromUri(uri, readSuccess);
            if (readSuccess) {
                const bool hasExpectedSize = data.size() == expectedSize;
                if (hasExpectedSize) {
                    context->addBuffer(data);
                } else {
                    qCWarning(kuesa) << "Unexpected size of" << data.size() << "bytes for buffer" << bufferName << " expected" << expectedSize << "bytes";
                    return false;
                }
            }
        }

        if (!readSuccess) {
            qCWarning(kuesa) << "Failed to read buffer" << bufferName;
            return false;
        }
    }

    return bufferDataSize > 0;
}

QByteArray BufferParser::dataFromUri(const QString &uri, bool &success) const
{
    const QString absPath = m_basePath.absoluteFilePath(uri);
    QFile dataFile(absPath);
    success = dataFile.open(QIODevice::ReadOnly);
    if (success)
        return dataFile.readAll();
    else
        qCWarning(kuesa) << "Failed to open" << uri;

    return QByteArray();
}

QT_END_NAMESPACE
