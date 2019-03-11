/*
    gltf2exporter_p.cpp

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#endif

#include <QFile>
#include <SceneEntity>
#include "gltf2exporter_p.h"
#include "gltf2context_p.h"
#include "gltf2keys_p.h"
#include "gltf2uri_p.h"
#include "gltf2importer.h"
#include "kuesa_p.h"
#include <set>
#include <Kuesa/private/gltf2context_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QGeometryRenderer;
class QGeometry;
} // namespace Qt3DRender

namespace Kuesa {
namespace {

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
} // namespace

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

namespace {
#if defined(KUESA_DRACO_COMPRESSION)
class GLTF2DracoCompressor
{
public:
    GLTF2DracoCompressor(
            QDir source,
            QDir destination,
            const QJsonObject &rootObject,
            const GLTF2ExportConfiguration &conf,
            GLTF2Import::GLTF2Context &context)
        : m_root(rootObject)
        , m_buffers(rootObject[GLTF2Import::KEY_BUFFERS].toArray())
        , m_bufferViews(rootObject[GLTF2Import::KEY_BUFFERVIEWS].toArray())
        , m_accessors(rootObject[GLTF2Import::KEY_ACCESSORS].toArray())
        , m_meshes(rootObject[GLTF2Import::KEY_MESHES].toArray())
        , m_images(rootObject[GLTF2Import::KEY_IMAGES].toArray())
        , m_compressedBufferIndex(m_buffers.size()) // Index of the added buffer
        , m_newBufferViewIndex(m_bufferViews.size()) // Index of where the bufferView are added
        , m_basePath(source)
        , m_destination(destination)
        , m_compressedBufferFilename(generateUniqueFilename(m_basePath, QStringLiteral("compressedBuffer.bin")))
        , m_conf(conf)
        , m_context(context)
    {
    }

    const QString &compressedBufferFilename() const noexcept
    {
        return m_compressedBufferFilename;
    }

    QJsonObject compress()
    {
        // https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_draco_mesh_compression/README.md

        // 1. Compress all the meshes
        for (int i = 0, n = m_context.meshesCount(); i < n; i++) {
            const auto mesh_json = m_meshes[i].toObject();
            m_meshes[i] = compressMesh(mesh_json, i);
        }

        if (m_compressedBuffer.isEmpty()) {
            return m_root; // Nothing changed
        }

        // 2. Save all the compressed data in a monolithic buffer and add it to the buffer list
        {
            // TODO maybe warn the user of unused assets ?
            QFile compressedBufferFile(m_destination.filePath({ m_compressedBufferFilename }));
            if (compressedBufferFile.open(QIODevice::WriteOnly)) {
                compressedBufferFile.write(m_compressedBuffer);
                compressedBufferFile.close();
            } else {
                m_errors << QStringLiteral("Could not open %1 for writing.").arg(compressedBufferFile.fileName());
                return {};
            }

            QJsonObject compressedBufferObject;
            compressedBufferObject[GLTF2Import::KEY_BYTELENGTH] = m_compressedBuffer.size();
            compressedBufferObject[GLTF2Import::KEY_URI] = m_compressedBufferFilename;
            m_buffers.push_back(compressedBufferObject);
        }

        // 3. Adjust the accessors: they don't need to point to a bufferView anymore since it's the Draco extension which is aware of them
        // TODO what happens if an accessor was referred by a mesh but also something else ?
        std::set<int> bufferViews_to_clean = cleanAccessors(m_accessors, m_accessorsToClean);

        // 4. Remove the data from the buffers and adjust the remaining bufferViews
        // We have to update *all* the indices everywhere - not only for meshes.
        removeBufferViews(bufferViews_to_clean);

        // 5. Finalize the JSON
        auto replace_json = [&](QJsonObject &m_root, const QLatin1String &k, QJsonArray &arr) {
            auto it = m_root.find(k);
            if (it != m_root.end() && arr.empty())
                m_root.erase(it);
            else if (!arr.empty())
                *it = std::move(arr);
        };
        replace_json(m_root, GLTF2Import::KEY_BUFFERS, m_buffers);
        replace_json(m_root, GLTF2Import::KEY_BUFFERVIEWS, m_bufferViews);
        replace_json(m_root, GLTF2Import::KEY_ACCESSORS, m_accessors);
        replace_json(m_root, GLTF2Import::KEY_MESHES, m_meshes);
        replace_json(m_root, GLTF2Import::KEY_IMAGES, m_images);

        addExtension(m_root, GLTF2Import::KEY_EXTENSIONS_REQUIRED, GLTF2Import::KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION);
        addExtension(m_root, GLTF2Import::KEY_EXTENSIONS_USED, GLTF2Import::KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION);

        return m_root;
    }

private:
    struct CompressedGLTFPrimitive {
        QJsonObject primitiveJson;
        QByteArray compressedData;
        QJsonObject newBufferView;
        std::set<int> accessorsToClean;
    };

    QStringList m_errors;

    QJsonObject m_root;
    QJsonArray m_buffers;
    QJsonArray m_bufferViews;
    QJsonArray m_accessors;
    QJsonArray m_meshes;
    QJsonArray m_images;

    QByteArray m_compressedBuffer;
    const int m_compressedBufferIndex;
    int m_newBufferViewIndex;
    std::set<int> m_accessorsToClean;
    QDir m_basePath, m_destination;
    QString m_compressedBufferFilename;

    const GLTF2ExportConfiguration &m_conf;
    GLTF2Import::GLTF2Context &m_context;

    // Compress a single mesh
    QJsonObject compressMesh(QJsonObject mesh_json, int mesh_idx)
    {
        const auto &primitives = m_context.mesh(mesh_idx).meshPrimitives;
        auto primitives_json = mesh_json[GLTF2Import::KEY_PRIMITIVES].toArray();

        Q_ASSERT_X(primitives_json.size() == primitives.size(), "GLTF2DracoCompressor::compressMesh", "Bad primitive count");

        for (int p = 0; p < primitives_json.size(); ++p) {
            auto primitive_json = primitives_json[p].toObject();

            // First check if the primitive is already draco-compressed
            auto ext_it = primitive_json.find(GLTF2Import::KEY_EXTENSIONS);
            if (ext_it != primitive_json.end()) {
                auto ext = ext_it->toObject();
                if (ext.contains(GLTF2Import::KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION)) {
                    continue;
                }
            }

            auto compressed_mesh = compressPrimitive(
                    primitive_json,
                    primitives[p]);

            if (!compressed_mesh.primitiveJson.empty()) {
                //* all the side-effects of this method are in this block: */
                primitives_json[p] = compressed_mesh.primitiveJson;
                m_compressedBuffer.push_back(compressed_mesh.compressedData);
                m_bufferViews.push_back(compressed_mesh.newBufferView);
                m_accessorsToClean.insert(compressed_mesh.accessorsToClean.begin(), compressed_mesh.accessorsToClean.end());

                m_newBufferViewIndex++;
            } else {
                const auto meshName = mesh_json[GLTF2Import::KEY_NAME].toString();
                if (!meshName.isEmpty())
                    m_errors << QStringLiteral("A mesh could not be compressed: %1 -> %2").arg(meshName).arg(p);
                else
                    m_errors << QStringLiteral("A mesh could not be compressed: %1 -> %2").arg(mesh_idx).arg(p);
            }
        }

        mesh_json[GLTF2Import::KEY_PRIMITIVES] = std::move(primitives_json);
        return mesh_json;
    }

    // Compress a single primitive of a mesh
    CompressedGLTFPrimitive compressPrimitive(
            QJsonObject primitive_json, const Kuesa::GLTF2Import::Primitive &primitive) const
    {
        CompressedGLTFPrimitive compressed_primitive;

        // Do the compression
        const auto compressed = Kuesa::DracoCompression::compressMesh(*primitive.primitiveRenderer->geometry(), m_conf);
        if (!compressed.buffer)
            return {};

        const int offset = m_compressedBuffer.size();
        auto &eb = *compressed.buffer.get();
        const int eb_size = static_cast<int>(eb.size());

        compressed_primitive.compressedData = QByteArray{ eb.data(), eb_size };

        // For now we allocate new bufferViews per compressed chunk; then we should do a pass to remove / replace unused bv ?

        // Allocate a new buffer view
        {
            compressed_primitive.newBufferView[GLTF2Import::KEY_BUFFER] = m_compressedBufferIndex;
            compressed_primitive.newBufferView[GLTF2Import::KEY_BYTEOFFSET] = offset;
            compressed_primitive.newBufferView[GLTF2Import::KEY_BYTELENGTH] = eb_size;
        }

        // Create or modify the extension object
        {
            auto ext_obj = primitive_json[GLTF2Import::KEY_EXTENSIONS].toObject();

            {
                QJsonObject draco_ext;
                draco_ext[GLTF2Import::KEY_BUFFERVIEW] = m_newBufferViewIndex;
                {
                    QJsonObject draco_ext_attr;
                    for (const auto &attribute : compressed.attributes) {
                        draco_ext_attr[attribute.first] = attribute.second;
                    }
                    draco_ext[GLTF2Import::KEY_ATTRIBUTES] = draco_ext_attr;
                }

                ext_obj[GLTF2Import::KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION] = draco_ext;
            }

            primitive_json[GLTF2Import::KEY_EXTENSIONS] = ext_obj;
        }

        // Mark the primitive's accessors
        {
            auto it = primitive_json.find(GLTF2Import::KEY_INDICES);
            if (it != primitive_json.end() && it->isDouble()) {
                compressed_primitive.accessorsToClean.insert(it->toInt());
            }

            it = primitive_json.find(GLTF2Import::KEY_ATTRIBUTES);
            if (it != primitive_json.end() && it->isObject()) {
                const auto attributes = it->toObject();
                for (const auto &attr : attributes) {
                    if (attr.isDouble())
                        compressed_primitive.accessorsToClean.insert(attr.toInt());
                }
            }
        }
        compressed_primitive.primitiveJson = primitive_json;

        return compressed_primitive;
    }

    void removeBufferViews(const std::set<int> &indicesToRemove)
    {
        // First compute the new indices of the buffer views
        std::map<int, int> bufferViewIndex;
        int currentOffset = 0;
        for (int i = 0, n = m_bufferViews.size(); i < n; i++) {
            if (indicesToRemove.count(i) != 0) {
                currentOffset++;
            }
            bufferViewIndex[i] = i - currentOffset;
        }

        auto updateExistingIndex = [&](QJsonObject &obj, const QLatin1String &key) {
            auto it = obj.find(key);
            if (it != obj.end()) {
                it.value() = bufferViewIndex.at(it.value().toInt());
            }
        };

        // Fix them in the accessors
        for (int i = 0; i < m_accessors.size(); i++) {
            auto acc = m_accessors[i].toObject();

            updateExistingIndex(acc, GLTF2Import::KEY_BUFFERVIEW);

            {
                auto it = acc.find(GLTF2Import::KEY_SPARSE);
                if (it != acc.end()) {
                    auto sparse = it->toObject();
                    {
                        auto indices = sparse.value(GLTF2Import::KEY_INDICES).toObject();
                        updateExistingIndex(indices, GLTF2Import::KEY_BUFFERVIEW);
                        sparse[GLTF2Import::KEY_INDICES] = std::move(indices);
                    }
                    {
                        auto values = sparse.value(GLTF2Import::KEY_VALUES).toObject();
                        updateExistingIndex(values, GLTF2Import::KEY_BUFFERVIEW);
                        sparse[GLTF2Import::KEY_VALUES] = std::move(values);
                    }
                    *it = std::move(sparse);
                }
            }

            m_accessors[i] = std::move(acc);
        }

        // Fix them in the meshes
        for (int i = 0; i < m_meshes.size(); i++) {
            auto mesh = m_meshes[i].toObject();
            auto primitives = mesh[GLTF2Import::KEY_PRIMITIVES].toArray();
            for (int p = 0; p < primitives.size(); p++) {
                auto primitive = primitives[p].toObject();

                auto ext_it = primitive.find(GLTF2Import::KEY_EXTENSIONS);
                if (ext_it != primitive.end()) {
                    auto ext = ext_it->toObject();

                    auto draco_it = ext.find(GLTF2Import::KEY_KHR_DRACO_MESH_COMPRESSION_EXTENSION);
                    if (draco_it != ext.end()) {
                        auto draco = draco_it->toObject();
                        updateExistingIndex(draco, GLTF2Import::KEY_BUFFERVIEW);
                        *draco_it = std::move(draco);
                    }

                    *ext_it = std::move(ext);
                }

                primitives[p] = std::move(primitive);
            }
            mesh[GLTF2Import::KEY_PRIMITIVES] = std::move(primitives);
            m_meshes[i] = std::move(mesh);
        }

        // Fix them in the images
        for (int i = 0; i < m_images.size(); i++) {
            auto img = m_images[i].toObject();
            updateExistingIndex(img, GLTF2Import::KEY_BUFFERVIEW);
            m_images[i] = std::move(img);
        }

        // Remove the buffer views
        QJsonArray removedBufferViews;
        // (note: this loop could be merged with the earlier one
        // but this would be a bit less readable imho)
        for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
            auto bv_it = m_bufferViews.begin() + (*it);

            removedBufferViews.push_back(std::move(*bv_it));
            m_bufferViews.erase(bv_it);
        }

        cleanupBuffers(removedBufferViews);
    }

    struct BufferParts {
        int offset{};
        int length{};
        int stride{};
        friend bool operator==(const BufferParts &lhs, const BufferParts &rhs)
        {
            return lhs.offset == rhs.offset;
        }
        friend bool operator<(const BufferParts &lhs, const BufferParts &rhs)
        {
            return lhs.offset < rhs.offset;
        }
    };

    // Maps the new index of an offset in a buffer after removal of prior parts
    static int mapBufferOffset(const std::set<BufferParts> &removed_parts, int offset)
    {
        const auto compare = [](const BufferParts &lhs, int rhs) { return lhs.offset < rhs; };
        const auto end = std::lower_bound(
                removed_parts.begin(), removed_parts.end(), offset, compare);

        for (auto it = removed_parts.begin(); it != end; ++it) {
            offset -= it->length;
        }

        return offset;
    }

    // Removes the unused data in buffers
    void cleanupBuffers(const QJsonArray &removedBufferViews)
    {
        using namespace GLTF2Import;
        std::map<int, std::set<BufferParts>> bufferParts;
        // List and order all the parts of the buffers to remove
        for (const QJsonValue &bv_value : removedBufferViews) {
            auto bv = bv_value.toObject();
            auto it = bv.find(GLTF2Import::KEY_BUFFER);
            if (it != bv.end()) {
                const int buffer_idx = it.value().toInt();
                const auto buf = m_buffers[buffer_idx].toObject();
                const auto uri = buf[GLTF2Import::KEY_URI].toString();
                const bool is_embedded = Uri::kind(uri) == Uri::Kind::Data;

                if (is_embedded || QFileInfo::exists(Uri::localFile(uri, m_basePath))) {
                    BufferParts p;
                    p.offset = bv[GLTF2Import::KEY_BYTEOFFSET].toInt();
                    p.length = bv[GLTF2Import::KEY_BYTELENGTH].toInt();
                    p.stride = bv[GLTF2Import::KEY_BYTESTRIDE].toInt();

                    bufferParts[buffer_idx].insert(p);
                } else {
                    m_errors << QStringLiteral("File does not exist: %1").arg(uri);
                }
            }
        }

        // Remove the parts in the buffers
        std::vector<int> buffersToRemove;

        // TODO we should check that all buffer objects map to a unique file
        // else this won't work at all
        for (const auto &buffer : bufferParts) {
            QJsonObject buf = m_buffers[buffer.first].toObject();

            const auto uri = buf[GLTF2Import::KEY_URI].toString();
            const bool is_embedded = Uri::kind(uri) == Uri::Kind::Data;

            bool success = false;
            auto data = Uri::fetchData(uri, m_basePath, success);
            if (!success) {
                if (is_embedded)
                    m_errors << QStringLiteral("Could not read embedded buffer");
                else
                    m_errors << QStringLiteral("Could not read %1").arg(m_basePath.absoluteFilePath(uri));
                continue;
            }

            int removed_length = 0;

            // TODO handle overlapping bufferViews
            // TODO handle strided buffers
            for (auto it = buffer.second.rbegin(); it != buffer.second.rend(); ++it) {
                data.remove(it->offset, it->length);
                removed_length += it->length;
            }

            if (data.isEmpty()) {
                // Actually we could precompute this case...
                buffersToRemove.push_back(buffer.first);
            } else {
                if (!is_embedded) {
                    QDir{}.mkpath(QFileInfo(m_destination, uri).absolutePath());
                    QFile target_f(m_destination.filePath(uri));
                    if (!target_f.open(QIODevice::WriteOnly)) {
                        m_errors << QStringLiteral("Could not open %1 for writing").arg(target_f.fileName());
                        continue;
                    }
                    const auto written = target_f.write(data);
                    if (written != data.size()) {
                        m_errors << QStringLiteral("Could not write buffer %1 entirely").arg(target_f.fileName());
                        continue;
                    }
                } else {
                    QByteArray output{ "data:application/octet-stream;base64," };
                    output += data.toBase64();
                    buf[GLTF2Import::KEY_URI] = QString::fromLatin1(output);
                }

                const auto len_it = buf.find(GLTF2Import::KEY_BYTELENGTH);
                if (len_it != buf.end()) {
                    (*len_it) = len_it->toInt() - removed_length;
                }

                m_buffers[buffer.first] = std::move(buf);
            }
        }

        // Change the offsets in the bufferViews
        for (auto bv_it = m_bufferViews.begin(); bv_it != m_bufferViews.end(); ++bv_it) {
            auto bv = bv_it->toObject();
            auto it = bv.find(GLTF2Import::KEY_BUFFER);
            if (it != bv.end()) {
                const int buffer_idx = it.value().toInt();

                // Change the offset of the data in the buffer
                auto offset_it = bv.find(GLTF2Import::KEY_BYTEOFFSET);
                (*offset_it) = mapBufferOffset(bufferParts[buffer_idx], offset_it->toInt());

                // Change the buffer index if there are removed buffers
                int new_buffer_idx = buffer_idx;
                for (int removed_buf : buffersToRemove) {
                    if (removed_buf < buffer_idx)
                        new_buffer_idx--;
                    else
                        break;
                }
                (*it) = new_buffer_idx;
                (*bv_it) = std::move(bv);
            }
        }

        // Remove unused buffers
        for (int buffer : buffersToRemove) {
            m_buffers.erase(m_buffers.begin() + buffer);
        }

        // TODO Do a final pass to check if there are any unreferenced buffers
        // Or it could maybe be an additional "lint" pass not part of the draco one ?
    }

    // Add an extension if it is not already registered - this could be moved at a more generic place
    // if further extensions are to be added
    static void addExtension(QJsonObject &rootObject, const QString &where, const QString &extension)
    {
        auto extensions = rootObject[where].toArray();
        auto ext_it = std::find_if(extensions.begin(), extensions.end(), [&](const QJsonValue &v) {
            return v.toString() == extension;
        });

        if (ext_it == extensions.end()) {
            extensions.push_back(extension);
            rootObject[where] = std::move(extensions);
        }
    }

    // Given an accessor array, remove their "bufferView": key, and return the corresponding set of
    // bufferViews
    static std::set<int> cleanAccessors(QJsonArray &accessors, const std::set<int> &indices)
    {
        std::set<int> bufferViews;
        for (int accessor : indices) {
            auto acc = accessors[accessor].toObject();
            auto it = acc.find(GLTF2Import::KEY_BUFFERVIEW);
            if (it != acc.end()) {
                bufferViews.insert(it.value().toInt());
                acc.erase(it);

                acc.remove(GLTF2Import::KEY_BYTEOFFSET);
            }
            accessors[accessor] = std::move(acc);
        }
        return bufferViews;
    }
};
#endif

} // namespace

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
    for (const auto& file : m_context->localFiles()) {
        auto filePath = GLTF2Import::Uri::localFile(file, target);
        if (!filePath.startsWith(QStringLiteral(":/"))) {
            if (QFileInfo::exists(filePath))
                files.append(filePath);
        }
    }
    return files;
}

auto GLTF2Exporter::saveInFolder(
        const QDir &source,
        const QDir &target) -> Export
{
    m_errors.clear();
    if (!m_context) {
        m_errors << QLatin1String("Tried to save GLTF without a context");
        return {};
    }

    if (!QFileInfo(target.absolutePath()).isWritable()) {
        m_errors << QStringLiteral("Cannot write to output directory %1").arg(target.absolutePath());
        return {};
    }

    QJsonObject rootObject = m_context->json().object();
    QString compressedBuffer;
    if (rootObject.isEmpty()) {
        m_errors << QStringLiteral("Nothing to save");
        return {};
    }

#if defined(KUESA_DRACO_COMPRESSION)
    if (m_conf.meshCompressionEnabled()) {
        GLTF2DracoCompressor compressor(source, target, rootObject, m_conf, *m_context);
        rootObject = compressor.compress();
        compressedBuffer = compressor.compressedBufferFilename();
        if (rootObject.empty()) {
            m_errors << QStringLiteral("Draco compression failed");
            return {};
        }
    }
#endif

    if (source != target) {
        // Copy buffers
        auto buffers_it = rootObject.find(GLTF2Import::KEY_BUFFERS);
        if (buffers_it != rootObject.end()) {
            copyURIs(source, target, buffers_it->toArray());
        };

        // Copy images
        auto images_it = rootObject.find(GLTF2Import::KEY_IMAGES);
        if (images_it != rootObject.end()) {
            copyURIs(source, target, images_it->toArray());
        }
    }

    Export e;
    e.m_json = std::move(rootObject);
    e.m_compressedBufferFilename = std::move(compressedBuffer);
    return e;
}

// Copies all the files referenced in an array of GLTF objects
void GLTF2Exporter::copyURIs(const QDir &source, const QDir &target, const QJsonArray &array)
{
    using namespace GLTF2Import;
    for (const auto &val : array) {
        const auto &buffer = val.toObject();
        auto uri_it = buffer.find(GLTF2Import::KEY_URI);
        if (uri_it == buffer.end())
            return;

        const auto uri = uri_it->toString();
        if (Uri::kind(uri) != Uri::Kind::Path)
            return;

        QFile srcFile(Uri::localFile(uri_it->toString(), source));
        const QFileInfo destFile(target, uri_it->toString());

        if (!destFile.exists()) {
            if (srcFile.exists()) {
                // Copy files from the source to the target directory
                QDir{}.mkpath(destFile.absolutePath());

                bool ok = srcFile.copy(target.absoluteFilePath(uri_it->toString()));
                if (!ok) {
                    m_errors << QStringLiteral("Unable to copy %1").arg(uri_it->toString());
                }
            } else {
                m_errors << QStringLiteral("Tried to copy missing file %1").arg(uri_it->toString());
            }
        }
    }
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
