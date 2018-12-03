/*
    bufferviewsparser_p.h

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

#ifndef KUESA_GLTF2IMPORT_BUFFERVIEWSPARSER_P_H
#define KUESA_GLTF2IMPORT_BUFFERVIEWSPARSER_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QByteArray>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>


QT_BEGIN_NAMESPACE

namespace Kuesa {
namespace GLTF2Import {

class GLTF2Context;

/*!
 * \brief It contains the information parsed by the BufferViewParser.
 *
 * An invalid BufferView can be constructed if the data referenced by the buffer view does not exist.
 * In this case, the \a bufferData property will be a default constructed QByteArray and the rest of properties will be -1.
 */
struct BufferView
{
    BufferView();

    QByteArray bufferData;
    int bufferIdx;
    int byteOffset;
    int byteLength;
    int byteStride;
};

class Q_AUTOTEST_EXPORT BufferViewsParser
{
public:
    BufferViewsParser();

    bool parse(const QJsonArray &bufferViewsArray, GLTF2Context *context);
    QVector<BufferView> parse(const QJsonArray &bufferViewsArray, const QVector<QByteArray> &buffers);

private:
    Qt3DRender::QAttribute::AttributeType viewTargetFromJSON(int target);
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_BUFFERVIEWSPARSER_P_H
