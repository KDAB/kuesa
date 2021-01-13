/*
    gltf2uri_p.h

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

#ifndef KUESA_GLTF2IMPORT_GLTF2URI_P_H
#define KUESA_GLTF2IMPORT_GLTF2URI_P_H

#include <QString>
#include <QDir>
#include <Kuesa/private/kuesa_global_p.h>

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

QT_BEGIN_NAMESPACE

class QString;
class QDir;

namespace Kuesa {
namespace GLTF2Import {
namespace Uri {
enum class Kind {
    Path,
    Data
};

KUESA_PRIVATE_EXPORT
Uri::Kind kind(const QString &uri);

KUESA_PRIVATE_EXPORT
QUrl absoluteUrl(const QString &uri, const QDir &basePath);

KUESA_PRIVATE_EXPORT
QString localFile(const QString &uri, const QDir &basePath);

KUESA_PRIVATE_EXPORT
QByteArray parseEmbeddedData(const QString &uri);

KUESA_PRIVATE_EXPORT
QByteArray toBase64Uri(const QByteArray &arr);

KUESA_PRIVATE_EXPORT
QByteArray fetchData(const QString &uri, const QDir &basePath, bool &success);
} // namespace Uri
} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_GLTF2URI_P_H
