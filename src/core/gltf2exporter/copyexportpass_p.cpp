/*
    copyingexportpass_cpp.h

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

#include "copyexportpass_p.h"
#include "gltf2keys_p.h"
#include "gltf2exporter_p.h"
#include "gltf2uri_p.h"
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Kuesa {

/*!
 * \class CopyExportPass
 * \brief glTF export pass that copies referenced assets in another folder.
 * \internal
 */
CopyExportPass::CopyExportPass(const QDir &source, const QDir &destination)
    : m_basePath(source)
    , m_destination(destination)
{
}

void CopyExportPass::addGeneratedFiles(const QStringList &lst)
{
    m_generated << lst;
}

const QStringList &CopyExportPass::errors() const
{
    return m_errors;
}

void CopyExportPass::copyURIs(QJsonObject &root, QLatin1String key)
{
    using namespace GLTF2Import;
    if (m_basePath == m_destination)
        return;

    auto array_it = root.find(key);
    if (array_it == root.end())
        return;

    const auto &array = array_it->toArray();
    for (const auto &val : array) {
        const auto &buffer = val.toObject();
        const auto uri_it = buffer.find(GLTF2Import::KEY_URI);
        if (uri_it == buffer.end())
            continue;

        const auto uri = uri_it->toString();
        if (Uri::kind(uri) != Uri::Kind::Path)
            continue;

        if (m_generated.contains(uri))
            continue;

        QFile srcFile(Uri::localFile(uri_it->toString(), m_basePath));
        const QFileInfo destFile(m_destination, uri_it->toString());

        if (srcFile.exists()) {
            // Copy files from the source to the target directory
            QDir{}.mkpath(destFile.absolutePath());

            const QString absPath = m_destination.absoluteFilePath(uri_it->toString());

            if (QFile::exists(absPath) && !QFile::remove(absPath))
                m_errors << QStringLiteral("Unable to overwrite old file %1").arg(absPath);

            const bool ok = srcFile.copy(absPath);
            if (!ok) {
                m_errors << QStringLiteral("Unable to copy %1").arg(uri_it->toString());
            }
        } else {
            m_errors << QStringLiteral("Tried to copy missing file %1").arg(uri_it->toString());
        }
    }

    *array_it = array;
}

} // namespace Kuesa
QT_END_NAMESPACE
