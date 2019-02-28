/*
    gltf2utils_p.cpp

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

#include "gltf2utils_p.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QLatin1String>
#include <algorithm>

QT_BEGIN_NAMESPACE
namespace Kuesa {
/*!
 * Add an extension to a glTF object if it is not already registered
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
 */
void replaceJsonArray(QJsonObject &object, QLatin1String k, QJsonArray &arr)
{
    auto it = object.find(k);
    if (it != object.end() && arr.empty())
        object.erase(it);
    else if (!arr.empty())
        *it = std::move(arr);
}

} // namespace Kuesa
QT_END_NAMESPACE
