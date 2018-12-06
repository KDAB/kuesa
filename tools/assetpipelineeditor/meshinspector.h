/*
    meshinspector.h

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

#ifndef MESHINSPECTOR_H
#define MESHINSPECTOR_H

#include <QAbstractTableModel>
#include <QVector>
#include <QColor>
#include <QPointer>

namespace Qt3DCore {
class QEntity;
}

namespace Qt3DRender {
class QGeometryRenderer;
class QAttribute;
class QMaterial;
} // namespace Qt3DRender

namespace Kuesa {
class AbstractAssetCollection;
class MetallicRoughnessMaterial;
} // namespace Kuesa

class BufferModel;

class MeshInspector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString assetName READ assetName NOTIFY meshParamsChanged)
    Q_PROPERTY(int totalSize READ totalSize NOTIFY meshParamsChanged)
    Q_PROPERTY(QString totalSizeString READ totalSizeString NOTIFY meshParamsChanged)
    Q_PROPERTY(int vertexCount READ vertexCount NOTIFY meshParamsChanged)
    Q_PROPERTY(QString primitiveType READ primitiveType NOTIFY meshParamsChanged)
    Q_PROPERTY(int primitiveCount READ primitiveCount NOTIFY meshParamsChanged)
    Q_PROPERTY(QObject *bufferModel READ bufferModel CONSTANT)
    Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor NOTIFY selectionColorChanged)
public:
    explicit MeshInspector(QObject *parent = nullptr);
    virtual ~MeshInspector() {}

    void setMesh(Qt3DRender::QGeometryRenderer *mesh);

    QString assetName() const;
    int totalSize() const;
    int vertexCount() const;
    int primitiveCount() const;
    QString primitiveType() const;
    BufferModel *bufferModel();
    QColor selectionColor() const;

    static unsigned int attributeSizeInBytes(Qt3DRender::QAttribute *attribute);
    static unsigned int calculatePrimitiveCount(int indexCount, int primitiveType);
    static QString nameForPrimitiveType(int primitiveType);
    QString totalSizeString() const;

    void setSelectionColor(const QColor &c);

Q_SIGNALS:
    void meshParamsChanged();
    void selectionColorChanged(QColor selectionColor);

private:
    Qt3DRender::QGeometryRenderer *m_mesh;
    QPointer<Kuesa::MetallicRoughnessMaterial> m_highlightMaterial;
    QVector<QPair<QPointer<Qt3DCore::QEntity>, QPointer<Qt3DRender::QMaterial>>> m_meshMaterials;

    QString m_meshName;
    int m_totalSize;
    int m_vertexCount;
    int m_primitiveCount;
    QString m_primitiveType;

    BufferModel *m_model;
    QObject m_bufferModel;
    QMetaObject::Connection m_meshConnection;

    QColor m_selectionColor;
};

class BufferModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BufferModel(QObject *parent = nullptr);

    enum AttributeColumns {
        BufferNameColumn = 0,
        BufferViewIndexColumn,
        BufferTypeColumn,
        VertexTypeColumn,
        CountColumn,
        VertexSizeColumn,
        OffsetColumn,
        StrideColumn,
        LastColumn
    };

    void setMesh(Qt3DRender::QGeometryRenderer *mesh);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    static QString vertexBaseTypeName(int vertexBaseType);

private:
    Qt3DRender::QGeometryRenderer *m_mesh;
    QVector<Qt3DRender::QAttribute *> m_attributes;
};

#endif // MESHINSPECTOR_H
