/*
    gltf2uri.cpp

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

#include "gltf2uri_p.h"
#include "gltf2context_p.h"
#include "kuesa_p.h"

QT_BEGIN_NAMESPACE

namespace Kuesa {
namespace GLTF2Import {
namespace Uri {
//! TODO: URL support ?

Kind kind(const QString &uri)
{
    return uri.startsWith(QLatin1String("data:"), Qt::CaseInsensitive)
            ? Kind::Data
            : Kind::Path;
}

/*!
 * \internal
 * Create a QUrl containing the absolute path to a file referenced in a glTF uri.
 */
QUrl absoluteUrl(const QString &uri, const QDir &basePath)
{
    QUrl url;
    if (uri.startsWith(QLatin1String("qrc:"), Qt::CaseInsensitive)) {
        url = QUrl(uri);
    } else if (uri.startsWith(QLatin1String("file:"), Qt::CaseInsensitive)) {
        url = QUrl::fromLocalFile(basePath.absoluteFilePath(QUrl(uri).toLocalFile()));
    } else {
        const QString absolutePath = basePath.absoluteFilePath(uri);

        // Handling the case of Qt resources
        // QUrl(":/path") actually gives QUrl("")
        // However QUrl("qrc:/path) is valid
        if (absolutePath.startsWith(QLatin1String(":/")))
            url = QUrl(QStringLiteral("qrc") + absolutePath);
        else
            url = QUrl::fromLocalFile(absolutePath);
    }
    return url;
}

/*!
 * \internal
 * Create a QString containing the absolute path to a file referenced in a glTF uri.
 */
QString localFile(const QString &uri, const QDir &basePath)
{
    QString path;
    if (uri.startsWith(QLatin1String("qrc:///"), Qt::CaseInsensitive)) {
        path = QLatin1String(":/") + uri.mid(7);
    } else if (uri.startsWith(QLatin1String("qrc:/"), Qt::CaseInsensitive)) {
        path = QLatin1String(":/") + uri.mid(5);
    } else if (uri.startsWith(QLatin1String(":/"), Qt::CaseInsensitive)) {
        path = uri;
    } else if (uri.startsWith(QLatin1String("file:"), Qt::CaseInsensitive)) {
        path = basePath.absoluteFilePath(QUrl(uri).toLocalFile());
    } else {
        path = basePath.absoluteFilePath(uri);
    }
    return path;
}

/*!
 * \internal
 * Converts a glTF data uri into a binary data buffer.
 */
QByteArray parseEmbeddedData(const QString &uri)
{
    QByteArray data = uri.toLatin1();

    // checking format for https://tools.ietf.org/html/rfc2397
    if (!data.startsWith(QByteArray("data:")))
        return {};

    const int separatorPos = data.indexOf(',');
    if (separatorPos == -1)
        return {};

    const QByteArray header = data.left(separatorPos);
    if (!header.endsWith(QByteArray(";base64"))) {
        qCWarning(Kuesa::kuesa, "URI Embedded data currently only supports Base64 encoding");
        return {};
    }

    return QByteArray::fromBase64(data.remove(0, separatorPos + 1));
}

/*!
 * \internal
 * Converts binary data into a data uri embeddable in a glTF object.
 */
QByteArray toBase64Uri(const QByteArray &arr)
{
    return "data:application/octet-stream;base64," + arr.toBase64();
}

/*!
 * \internal
 * Get the binary data buffer referenced by a glTF uri.
 */
QByteArray fetchData(const QString &uri, const QDir &basePath, bool &success)
{
    switch (kind(uri)) {
    case Kind::Data: {
        auto data = parseEmbeddedData(uri);
        success = !data.isEmpty();
        return data;
    }
    case Kind::Path: {
        QFile dataFile(localFile(uri, basePath));
        success = dataFile.open(QIODevice::ReadOnly);
        if (success)
            return dataFile.readAll();
        else
            qCWarning(Kuesa::kuesa) << "Failed to open" << uri;
    }
    }

    return {};
}
} // namespace Uri
} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE
