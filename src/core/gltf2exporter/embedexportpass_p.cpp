/*
    embeddingexportpass_cpp.h

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

#include "embedexportpass_p.h"
#include "gltf2keys_p.h"
#include "gltf2exporter_p.h"
#include "gltf2uri_p.h"
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Kuesa {

/*!
 * \class EmbedExportPass
 * \brief glTF export pass that embeds referenced assets as base64 buffers in the glTF file.
 * \internal
 */
EmbedExportPass::EmbedExportPass(const QDir &source)
    : m_basePath(source)
{
}

const QStringList &EmbedExportPass::errors() const
{
    return m_errors;
}

void EmbedExportPass::embedURIsInArray(QJsonObject &root, QLatin1String key)
{
    auto array_it = root.find(key);
    if (array_it == root.end())
        return;

    auto array = array_it->toArray();
    for (QJsonValueRef val_v : array) {
        auto obj = val_v.toObject();
        if (embedUri(obj)) {
            val_v = obj;
        }
    }
    *array_it = array;
}

bool EmbedExportPass::embedUri(QJsonObject &object)
{
    using namespace GLTF2Import;
    const auto uri_it = object.find(GLTF2Import::KEY_URI);
    if (uri_it == object.end())
        return false;

    const auto &uri = uri_it->toString();
    if (Uri::kind(uri) != Uri::Kind::Path)
        return false;

    QFile resource(Uri::localFile(uri, m_basePath));
    if (!resource.open(QIODevice::ReadOnly)) {
        m_errors << QStringLiteral("Cannot open: %1").arg(uri);
        return false;
    }

    *uri_it = QString::fromLatin1(GLTF2Import::Uri::toBase64Uri(resource.readAll()));
    return true;
}

} // namespace Kuesa
QT_END_NAMESPACE
