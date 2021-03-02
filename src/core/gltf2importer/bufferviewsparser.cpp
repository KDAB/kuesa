/*
    bufferviewsparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "bufferviewsparser_p.h"
#include "assetkeyparser_p.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>

#include "gltf2context_p.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;
using namespace Qt3DGeometry;

namespace {
const QLatin1String KEY_BUFFER = QLatin1String("buffer");
const QLatin1String KEY_BYTEOFFSET = QLatin1String("byteOffset");
const QLatin1String KEY_BYTELENGTH = QLatin1String("byteLength");
const QLatin1String KEY_BYTESTRIDE = QLatin1String("byteStride");
} // namespace

BufferView::BufferView()
    : byteStride(-1)
    , bufferIdx(-1)
    , byteOffset(-1)
    , byteLength(-1)
{
}

/*!
 * \class Kuesa::GLTF2Import::BufferViewsParser
 *
 * \brief Parses a GLTF2 JSON BufferView description and return a QVector of
 * BufferView.
 * \internal
 */

BufferViewsParser::BufferViewsParser()
{
}

/*!
 * Returns true if the \a bufferViewsArray is parsed correctly. False otherwise
 *
 * If the buffer view references a buffer that does not exists in the context,
 * will return false.
 * \internal
 */
bool BufferViewsParser::parse(const QJsonArray &bufferViewsArray, GLTF2Context *context)
{
    const qint32 bufferViewSize = bufferViewsArray.size();

    for (qint32 bufferViewId = 0; bufferViewId < bufferViewSize; ++bufferViewId) {
        const QJsonObject viewObject = bufferViewsArray[bufferViewId].toObject();

        const qint16 bufferIdx = qint16(viewObject.value(KEY_BUFFER).toInt(-1));
        const qint32 byteOffset = viewObject.value(KEY_BYTEOFFSET).toInt();
        const qint32 byteLength = viewObject.value(KEY_BYTELENGTH).toInt();
        const qint16 byteStride = qint16(viewObject.value(KEY_BYTESTRIDE).toInt());

        const QByteArray &data = context->buffer(bufferIdx);
        if (!data.isNull()) {
            BufferView view;
            view.bufferData = data.mid(byteOffset, byteLength);
            view.bufferIdx = bufferIdx;
            view.byteOffset = byteOffset;
            view.byteLength = byteLength;
            view.byteStride = byteStride;

            // Parse Share Key Extension
            AssetKeyParser::parse(view, viewObject);

            context->addBufferView(view);
        } else {
            return false;
        }
    }

    return true;
}

QT_END_NAMESPACE
