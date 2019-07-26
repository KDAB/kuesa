/*
    separateexportpass_cpp.h

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

#include "separateexportpass_p.h"
#include "gltf2keys_p.h"
#include "gltf2exporter_p.h"
#include "gltf2uri_p.h"
#include <QJsonArray>
#include <QDebug>
#include <QMimeDatabase>
QT_BEGIN_NAMESPACE
namespace Kuesa {

namespace {

QString bufferNamer(const QString &basename, const QJsonObject &obj, const QByteArray & /*data*/, int count)
{
    QString name;

    const auto name_it = obj.constFind(GLTF2Import::KEY_NAME);
    if (name_it != obj.constEnd()) {
        name = name_it->toString();
    } else {
        name = basename;
    }

    if (name.isEmpty())
        name = QStringLiteral("buffer");

    return QStringLiteral("%1-%2.bin").arg(name).arg(count);
}

QString imageNamer(const QString &basename, const QJsonObject &obj, const QByteArray &data, int count)
{
    QString name;
    const auto name_it = obj.constFind(GLTF2Import::KEY_NAME);
    if (name_it != obj.constEnd()) {
        name = name_it->toString();
    } else {
        name = basename;
    }

    if (name.isEmpty())
        name = QStringLiteral("image");

    QString suffix;
    const auto mime_it = obj.constFind(GLTF2Import::KEY_MIMETYPE);
    if (mime_it != obj.constEnd()) {
        const auto mime = mime_it->toString();
        if (mime == QLatin1String("image/jpeg")) {
            suffix = QStringLiteral("jpeg");
        } else if (mime == QLatin1String("image/png")) {
            suffix = QStringLiteral("png");
        }
    }

    if (suffix.isEmpty()) {
        static const QMimeDatabase db;
        const auto mimeType = db.mimeTypeForData(data);
        suffix = mimeType.preferredSuffix();
    }

    return QStringLiteral("%1-%2.%3").arg(name).arg(count).arg(suffix);
}

} // namespace

/*!
 * \class SeparateExportPass
 * \brief glTF export pass that takes all the base64-encoded embedded buffers and saves them in files.
 * \internal
 */
SeparateExportPass::SeparateExportPass(const QString &filename, const QDir &destination)
    : m_basename(QFileInfo(filename).baseName())
    , m_destination(destination)
{
}

const QStringList &SeparateExportPass::errors() const
{
    return m_errors;
}

const QStringList &SeparateExportPass::generatedFiles() const
{
    return m_generated;
}

void SeparateExportPass::separateURIs(QJsonObject &root)
{
    separateURIsInArray(root, GLTF2Import::KEY_BUFFERS, bufferNamer);
    m_extractedCount = 0;
    separateURIsInArray(root, GLTF2Import::KEY_IMAGES, imageNamer);
}

void SeparateExportPass::separateURIsInArray(QJsonObject &root, QLatin1String key, NamingFunction namer)
{
    auto array_it = root.find(key);
    if (array_it == root.end())
        return;

    auto array = array_it->toArray();
    for (QJsonValueRef val_v : array) {
        auto obj = val_v.toObject();
        if (separateEncodedUri(obj, namer)) {
            val_v = obj;
        }
    }
    *array_it = array;
}

bool SeparateExportPass::separateEncodedUri(QJsonObject &object, NamingFunction namer)
{
    using namespace GLTF2Import;

    auto uri_it = object.find(GLTF2Import::KEY_URI);
    if (uri_it == object.end())
        return false;

    const auto &uri = uri_it->toString();
    if (Uri::kind(uri) != Uri::Kind::Data)
        return false;

    // Decode the data
    const auto data = Uri::parseEmbeddedData(uri);

    // Get a proper filename
    const auto basename = namer(m_basename, object, data, m_extractedCount);
    const auto file = generateUniqueFilename(m_destination, basename);
    const auto filepath = m_destination.filePath(file);

    // Write the resource file
    QFile resource(filepath);
    if (!resource.open(QIODevice::WriteOnly)) {
        m_errors << QStringLiteral("Cannot create: %1").arg(uri);
        return false;
    }
    resource.write(data);

    *uri_it = file;

    m_generated.push_back(file);
    ++m_extractedCount;
    return true;
}

} // namespace Kuesa
QT_END_NAMESPACE
