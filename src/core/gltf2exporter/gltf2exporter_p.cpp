/*
    gltf2exporter_p.cpp

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

#include <qtkuesa-config.h>
#if defined(KUESA_DRACO_COMPRESSION)
#include <Kuesa/private/draco_prefix_p.h>
#include <draco/compression/encode.h>
#include "dracocompressor_p.h"
#include "dracoexportpass_p.h"
#endif
#include "embedexportpass_p.h"
#include "separateexportpass_p.h"
#include "copyexportpass_p.h"

#include "gltf2importer.h"
#include "gltf2exporter_p.h"
#include "gltf2context_p.h"
#include "gltf2uri_p.h"
#include "gltf2keys_p.h"
#include "kuesa_p.h"

#include <QFile>
QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QGeometryRenderer;
class QGeometry;
} // namespace Qt3DRender

namespace Kuesa {

QString generateUniqueFilename(const QDir &dir, QString filename)
{
    const QFileInfo fi(filename);
    const auto basename = fi.baseName();
    const auto ext = fi.completeSuffix();
    int num = 1;
    while (dir.exists(filename)) {
        filename = QStringLiteral("%1-%2.%3").arg(basename).arg(num).arg(ext);
        ++num;
    }
    return filename;
}

void GLTF2ExportConfiguration::setMeshEncodingSpeed(int speed)
{
    if (speed >= 0 && speed <= 10)
        m_encodingSpeed = speed;
}

void GLTF2ExportConfiguration::setMeshDecodingSpeed(int speed)
{
    if (speed >= 0 && speed <= 10)
        m_decodingSpeed = speed;
}

int GLTF2ExportConfiguration::meshEncodingSpeed() const
{
    return m_encodingSpeed;
}

int GLTF2ExportConfiguration::meshDecodingSpeed() const
{
    return m_decodingSpeed;
}

void GLTF2ExportConfiguration::setAttributeQuantizationLevel(
        GLTF2ExportConfiguration::MeshAttribute attribute,
        int level)
{
    if (level >= 0 && level <= 10)
        m_quantization[attribute] = level;
}

int GLTF2ExportConfiguration::attributeQuantizationLevel(
        GLTF2ExportConfiguration::MeshAttribute attribute) const
{
    auto it = m_quantization.find(attribute);
    if (it != m_quantization.end()) {
        return it.value();
    }
    return 0;
}

void GLTF2ExportConfiguration::setMeshCompressionEnabled(bool enabled)
{
    m_meshCompression = enabled;
}

bool GLTF2ExportConfiguration::meshCompressionEnabled() const
{
    return m_meshCompression;
}

auto GLTF2ExportConfiguration::embedding() const -> Embed
{
    return m_embedding;
}

void GLTF2ExportConfiguration::setEmbedding(Embed e)
{
    m_embedding = e;
}

GLTF2Exporter::GLTF2Exporter(QObject *parent)
    : QObject(parent)
{
}

SceneEntity *GLTF2Exporter::scene() const
{
    return m_scene;
}

QStringList GLTF2Exporter::errors() const
{
    return m_errors;
}

GLTF2ExportConfiguration GLTF2Exporter::configuration() const
{
    return m_conf;
}

void GLTF2Exporter::setConfiguration(const GLTF2ExportConfiguration &conf)
{
    m_conf = std::move(conf);
}

void GLTF2Exporter::setContext(GLTF2Import::GLTF2Context *context)
{
    m_context = context;
}

void GLTF2Exporter::setContextFromImporter(GLTF2Importer *importer)
{
    m_context = importer->m_context;
}

QStringList GLTF2Exporter::overwritableFiles(const QDir &target) const
{
    QStringList files;
    for (const auto &file : m_context->localFiles()) {
        auto filePath = GLTF2Import::Uri::localFile(file, target);
        if (!filePath.startsWith(QStringLiteral(":/"))) {
            if (QFileInfo::exists(filePath))
                files.append(filePath);
        }
    }
    return files;
}

GLTF2Exporter::Export GLTF2Exporter::saveInFolder(const QDir &source, const QDir &target)
{
    m_errors.clear();
    if (!m_context) {
        m_errors << QStringLiteral("Tried to save GLTF without a context");
        return {};
    }

    if (!QFileInfo(target.absolutePath()).isWritable()) {
        m_errors << QStringLiteral("Cannot write to output directory %1").arg(target.absolutePath());
        return {};
    }

    QJsonObject rootObject = m_context->json().object();
    QString compressedBufferFilename;
    if (rootObject.isEmpty()) {
        m_errors << QStringLiteral("Nothing to save");
        return {};
    }

    CopyExportPass copy_pass(source, target);

#if defined(KUESA_DRACO_COMPRESSION)
    if (m_conf.meshCompressionEnabled()) {
        DracoExportPass pass(m_context->filename(), source, target, rootObject, m_conf, *m_context);
        rootObject = pass.compress();
        compressedBufferFilename = pass.compressedBufferFilename();
        if (!compressedBufferFilename.isEmpty()) {
            copy_pass.addGeneratedFiles(pass.generatedFiles());
        }
        if (rootObject.empty()) {
            m_errors << QStringLiteral("Draco compression failed");
            return {};
        }
    }
#endif

    switch (m_conf.embedding()) {
    case GLTF2ExportConfiguration::Embed::Keep: {
        copy_pass.copyURIs(rootObject, GLTF2Import::KEY_BUFFERS);
        copy_pass.copyURIs(rootObject, GLTF2Import::KEY_IMAGES);
        if (!copy_pass.errors().empty())
            m_errors << copy_pass.errors();
        break;
    }
    case GLTF2ExportConfiguration::Embed::None: {
        // In this case we do both copy and extraction
        {
            SeparateExportPass pass(m_context->filename(), target);
            pass.separateURIs(rootObject);
            if (!pass.errors().empty())
                m_errors << pass.errors();

            copy_pass.addGeneratedFiles(pass.generatedFiles());
        }

        {
            copy_pass.copyURIs(rootObject, GLTF2Import::KEY_BUFFERS);
            copy_pass.copyURIs(rootObject, GLTF2Import::KEY_IMAGES);
            if (!copy_pass.errors().empty())
                m_errors << copy_pass.errors();
        }
        break;
    }
    case GLTF2ExportConfiguration::Embed::All: {
        EmbedExportPass pass(source);
        pass.embedURIsInArray(rootObject, GLTF2Import::KEY_BUFFERS);
        pass.embedURIsInArray(rootObject, GLTF2Import::KEY_IMAGES);
        if (!pass.errors().empty())
            m_errors << pass.errors();
        break;
    }
    }

    Export e;
    e.m_json = std::move(rootObject);
    e.m_compressedBufferFilename = std::move(compressedBufferFilename);
    return e;
}

void GLTF2Exporter::setScene(SceneEntity *scene)
{
    if (m_scene == scene)
        return;

    m_scene = scene;
    emit sceneChanged(m_scene);
}

bool GLTF2Exporter::Export::success() const
{
    return !m_json.empty();
}

const QJsonObject &GLTF2Exporter::Export::json() const
{
    return m_json;
}

const QString &GLTF2Exporter::Export::compressedBufferFilename() const
{
    return m_compressedBufferFilename;
}

} // namespace Kuesa

QT_END_NAMESPACE
