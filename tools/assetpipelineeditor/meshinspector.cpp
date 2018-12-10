/*
    meshinspector.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#include "meshinspector.h"
#include <QSettings>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QAttribute>
#include <Qt3DCore/QEntity>
#include <Kuesa/MetallicRoughnessMaterial>
#include <Kuesa/private/kuesa_utils_p.h>

MeshInspector::MeshInspector(QObject *parent)
    : QObject(parent)
    , m_mesh(nullptr)
    , m_highlightMaterial(nullptr)
    , m_totalSize(0)
    , m_vertexCount(0)
    , m_primitiveCount(0)
    , m_model(new BufferModel(this))
    , m_selectionColor(Qt::blue)
{
    QSettings settings;
    m_selectionColor = settings.value("selectionColor", QVariant::fromValue(m_selectionColor)).value<QColor>();
}

void MeshInspector::setMesh(Qt3DRender::QGeometryRenderer *mesh)
{
    if (mesh == m_mesh)
        return;

    // Un-highlight materials used by previous mesh
    for (const auto &entityMaterialPair : qAsConst(m_meshMaterials)) {
        Qt3DCore::QEntity *entity = entityMaterialPair.first.data();
        Qt3DRender::QMaterial *material = entityMaterialPair.second.data();
        // Remove highlight material from previously highlighted entity
        if (entity && m_highlightMaterial)
            entity->removeComponent(m_highlightMaterial.data());
        // Restore original material on entity
        if (entity && material)
            entity->addComponent(material);
    }
    m_meshMaterials.clear();

    if (mesh) {
        auto entities = mesh->entities();

        // Check if material has been destroyed, recreate if needed
        if (!m_highlightMaterial) {
            m_highlightMaterial = new Kuesa::MetallicRoughnessMaterial();
            m_highlightMaterial->setOpaque(false);
            QColor c = m_selectionColor;
            c.setAlpha(128);
            m_highlightMaterial->setBaseColorFactor(c);
            m_highlightMaterial->setAlphaCutoffEnabled(false);
        }

        for (Qt3DCore::QEntity *entity : entities) {
            Kuesa::MetallicRoughnessMaterial *mat = Kuesa::componentFromEntity<Kuesa::MetallicRoughnessMaterial>(entity);
            if (mat != nullptr) {
                // Replace entity material with highlight material
                entity->removeComponent(mat);
                entity->addComponent(m_highlightMaterial.data());
                // Keep track of old material to restore it later
                m_meshMaterials.push_back({ entity, mat });
            }
        }
    }

    disconnect(m_meshConnection);
    if (mesh)
        m_meshConnection = connect(mesh, &Qt3DCore::QNode::nodeDestroyed, this, [this]() { setMesh(nullptr); });

    m_mesh = mesh;
    m_model->setMesh(m_mesh);

    m_meshName = m_mesh ? m_mesh->objectName() : QString();
    m_totalSize = 0;
    m_primitiveCount = 0;
    m_vertexCount = 0;
    m_primitiveType = m_mesh ? nameForPrimitiveType(m_mesh->primitiveType()) : QString();

    if (m_mesh) {
        for (auto attribute : m_mesh->geometry()->attributes()) {
            if (attribute->attributeType() == Qt3DRender::QAttribute::VertexAttribute) {
                m_vertexCount = static_cast<int>(attribute->count());
            } else if (attribute->attributeType() == Qt3DRender::QAttribute::IndexAttribute) {
                m_primitiveCount += calculatePrimitiveCount(static_cast<int>(attribute->count()), m_mesh->primitiveType());
            }
            //total size includes index and vertex buffers
            m_totalSize += attributeSizeInBytes(attribute);
        }
    }

    emit meshParamsChanged();
}

unsigned int MeshInspector::attributeSizeInBytes(Qt3DRender::QAttribute *attribute)
{
    uint baseTypeSize = 1;
    switch (attribute->vertexBaseType()) {
    case Qt3DRender::QAttribute::Byte:
    case Qt3DRender::QAttribute::UnsignedByte:
        baseTypeSize = 1;
        break;
    case Qt3DRender::QAttribute::Short:
    case Qt3DRender::QAttribute::UnsignedShort:
        baseTypeSize = 2;
        break;
    case Qt3DRender::QAttribute::Int:
    case Qt3DRender::QAttribute::UnsignedInt:
        baseTypeSize = 4;
        break;
    case Qt3DRender::QAttribute::HalfFloat:
        baseTypeSize = 2;
        break;
    case Qt3DRender::QAttribute::Float:
        baseTypeSize = 4;
        break;
    case Qt3DRender::QAttribute::Double:
        baseTypeSize = 8;
        break;
    default:
        Q_UNREACHABLE();
        qWarning() << "Unexpected vertexBaseType " << attribute->vertexBaseType() << " for attribute " << attribute->name();
        break;
    }

    return attribute->count() * attribute->vertexSize() * baseTypeSize;
}

int MeshInspector::calculatePrimitiveCount(int indexCount, int primitiveType)
{
    switch (primitiveType) {
    case Qt3DRender::QGeometryRenderer::Points:
        return indexCount;
    case Qt3DRender::QGeometryRenderer::Lines:
        return indexCount / 2;
    case Qt3DRender::QGeometryRenderer::LineLoop:
        return indexCount;
    case Qt3DRender::QGeometryRenderer::LineStrip:
        return indexCount - 1;
    case Qt3DRender::QGeometryRenderer::Triangles:
        return indexCount / 3;
    case Qt3DRender::QGeometryRenderer::TriangleStrip:
        return indexCount - 2;
    case Qt3DRender::QGeometryRenderer::TriangleFan:
        return indexCount - 2;
    default:
        qWarning() << "Unexpected primitive type " << primitiveType;
        return 0;
    }
}

QString MeshInspector::nameForPrimitiveType(int primitiveType)
{
    switch (primitiveType) {
    case Qt3DRender::QGeometryRenderer::Points:
        return QStringLiteral("Points");
    case Qt3DRender::QGeometryRenderer::Lines:
        return QStringLiteral("Lines");
    case Qt3DRender::QGeometryRenderer::LineLoop:
        return QStringLiteral("LineLoop");
    case Qt3DRender::QGeometryRenderer::LineStrip:
        return QStringLiteral("LineStrip");
    case Qt3DRender::QGeometryRenderer::Triangles:
        return QStringLiteral("Triangles");
    case Qt3DRender::QGeometryRenderer::TriangleStrip:
        return QStringLiteral("TriangleStrip");
    case Qt3DRender::QGeometryRenderer::TriangleFan:
        return QStringLiteral("TriangleFan");
    default:
        return QStringLiteral("Unknown");
    }
}

QString MeshInspector::totalSizeString() const
{
    double size = m_totalSize / 1024.0;
    QString suffix(QStringLiteral(" KB"));
    if (size > 1024.) {
        size = size / 1024.;
        suffix = QStringLiteral(" MB");
    }
    return QString::number(size, 'f', 2) + suffix;
}

void MeshInspector::setSelectionColor(const QColor &c)
{
    if (c == m_selectionColor)
        return;

    m_selectionColor = c;
    QSettings settings;
    settings.setValue("selectionColor", QVariant::fromValue(m_selectionColor));
    if (m_highlightMaterial) {
        QColor c = m_selectionColor;
        c.setAlpha(128);
        m_highlightMaterial->setBaseColorFactor(c);
    }
}

BufferModel *MeshInspector::bufferModel()
{
    return m_model;
}

QColor MeshInspector::selectionColor() const
{
    return m_selectionColor;
}

int MeshInspector::totalSize() const
{
    return m_totalSize;
}

QString MeshInspector::assetName() const
{
    return m_meshName;
}

QString MeshInspector::primitiveType() const
{
    return m_primitiveType;
}

int MeshInspector::primitiveCount() const
{
    return m_primitiveCount;
}

int MeshInspector::vertexCount() const
{
    return m_vertexCount;
}

BufferModel::BufferModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_mesh(nullptr)
{
}

void BufferModel::setMesh(Qt3DRender::QGeometryRenderer *mesh)
{
    if (mesh == m_mesh)
        return;

    m_mesh = mesh;
    beginResetModel();
    if (m_mesh && mesh->geometry())
        m_attributes = m_mesh->geometry()->attributes();
    else
        m_attributes.clear();
    endResetModel();
}

QVariant BufferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case BufferNameColumn:
            return QStringLiteral("bufferName");
        case BufferViewIndexColumn:
            return QStringLiteral("bufferViewIndex");
        case BufferTypeColumn:
            return QStringLiteral("bufferType");
        case VertexTypeColumn:
            return QStringLiteral("vertexType");
        case CountColumn:
            return QStringLiteral("count");
        case VertexSizeColumn:
            return QStringLiteral("vertexSize");
        case OffsetColumn:
            return QStringLiteral("offset");
        case StrideColumn:
            return QStringLiteral("stride");
        }
    }
    return QVariant();
}

int BufferModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_attributes.count();
}

int BufferModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : LastColumn;
}

QVariant BufferModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (!(role == Qt::DisplayRole || role == Qt::EditRole))
        return QVariant();

    const auto attribute = m_attributes.at(index.row());

    switch (index.column()) {
    case BufferNameColumn: {
        auto name = attribute->property("bufferName").toString();
        if (name.isEmpty())
            name = attribute->attributeType() == Qt3DRender::QAttribute::IndexAttribute ? QStringLiteral("Index Buffer") : attribute->name();
        return name;
    }
    case BufferViewIndexColumn:
        return attribute->property("bufferViewIndex");
    case BufferTypeColumn:
        return attribute->attributeType();
    case VertexTypeColumn:
        return vertexBaseTypeName(attribute->vertexBaseType());
    case CountColumn:
        return attribute->count();
    case VertexSizeColumn:
        return attribute->vertexSize();
    case OffsetColumn:
        return attribute->byteOffset();
    case StrideColumn:
        return attribute->byteStride();
    default:
        break;
    }

    return QVariant();
}

QString BufferModel::vertexBaseTypeName(int vertexBaseType)
{
    switch (vertexBaseType) {
    case Qt3DRender::QAttribute::Byte:
        return QStringLiteral("Byte");
    case Qt3DRender::QAttribute::UnsignedByte:
        return QStringLiteral("U-Byte");
    case Qt3DRender::QAttribute::Short:
        return QStringLiteral("Short");
    case Qt3DRender::QAttribute::UnsignedShort:
        return QStringLiteral("U-Short");
    case Qt3DRender::QAttribute::Int:
        return QStringLiteral("Int");
    case Qt3DRender::QAttribute::UnsignedInt:
        return QStringLiteral("U-Int");
    case Qt3DRender::QAttribute::HalfFloat:
        return QStringLiteral("Half Float");
    case Qt3DRender::QAttribute::Float:
        return QStringLiteral("Float");
    case Qt3DRender::QAttribute::Double:
        return QStringLiteral("Double");
    default:
        Q_UNREACHABLE();
        qWarning() << "Unexpected vertexBaseType " << vertexBaseType;
        break;
    }
    return QString();
}
