/*
    gltf2utils_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_GLTF2EXPORTER_GLTF2UTILS_P_H
#define KUESA_GLTF2EXPORTER_GLTF2UTILS_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QAttribute>
#else
#include <Qt3DRender/QAttribute>
#endif

QT_BEGIN_NAMESPACE

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
namespace Qt3DGeometry = Qt3DCore;
#else
namespace Qt3DGeometry = Qt3DRender;
#endif

class QJsonObject;
class QJsonArray;
class QLatin1String;
class QString;

namespace Kuesa {

void addExtension(QJsonObject &rootObject, const QString &where, const QString &extension);

void replaceJsonArray(QJsonObject &m_root, QLatin1String k, QJsonArray &arr);

int addToJsonChildArray(QJsonObject &object, const QString &name, const QJsonObject &toInsert);

quint8 accessorDataTypeToBytes(Qt3DGeometry::QAttribute::VertexBaseType type);

} // namespace Kuesa
QT_END_NAMESPACE
#endif // KUESA_GLTF2EXPORTER_GLTF2UTILS_P_H
