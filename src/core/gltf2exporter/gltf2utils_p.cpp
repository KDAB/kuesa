/*
    gltf2utils_p.cpp

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

#include "gltf2utils_p.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QLatin1String>
#include <algorithm>
#include <private/kuesa_p.h>

QT_BEGIN_NAMESPACE
namespace Kuesa {
/*!
 * Add an extension to a glTF object if it is not already registered
 * \internal
 */
void addExtension(QJsonObject &object, const QString &where, const QString &extension)
{
    auto extensions = object[where].toArray();
    auto ext_it = std::find_if(extensions.begin(), extensions.end(), [&](const QJsonValue &v) {
        return v.toString() == extension;
    });

    if (ext_it == extensions.end()) {
        extensions.push_back(extension);
        object[where] = std::move(extensions);
    }
}

/*!
 * Replace a Json array in an object, or remove it from the object if the array is empty.
 * \internal
 */
void replaceJsonArray(QJsonObject &object, QLatin1String k, QJsonArray &arr)
{
    auto it = object.find(k);
    if (it != object.end() && arr.empty())
        object.erase(it);
    else if (!arr.empty())
        *it = std::move(arr);
}

/*!
 * Adds the given JSON object to the array with the given name, which is a
 * child of the given root object.
 * Returns the index of the inserted element within the array
 * \internal
 */
int addToJsonChildArray(QJsonObject &object, const QString &name, const QJsonObject &toInsert)
{
    const auto it = object.find(name);
    if (it == object.constEnd())
        return -1;

    QJsonArray array = it.value().toArray();
    const int index = array.size();
    array.append(toInsert);
    object[name] = std::move(array);
    return index;
}

quint8 accessorDataTypeToBytes(Qt3DRender::QAttribute::VertexBaseType type)
{
    switch (type) {
    case Qt3DRender::QAttribute::Byte:
    case Qt3DRender::QAttribute::UnsignedByte:
        return 1;
    case Qt3DRender::QAttribute::Short:
    case Qt3DRender::QAttribute::UnsignedShort:
        return 2;
    case Qt3DRender::QAttribute::Float:
    case Qt3DRender::QAttribute::UnsignedInt:
    case Qt3DRender::QAttribute::Int:
        return 4;
    case Qt3DRender::QAttribute::Double:
        return 8;
    default:
        qCWarning(kuesa, "Invalid data type");
        return 0;
    }
}

} // namespace Kuesa
QT_END_NAMESPACE
