/*
    meshparser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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

#ifndef KUESA_GLTF2IMPORT_MESHPARSER_P_H
#define KUESA_GLTF2IMPORT_MESHPARSER_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QtCore/qglobal.h>
#include <QtCore/QVector>
#include <Kuesa/private/kuesa_global_p.h>
#include <qtkuesa-config.h>
#include "bufferaccessorparser_p.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QGeometry>
#else
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#endif
#include <Qt3DRender/QGeometryRenderer>

QT_BEGIN_NAMESPACE


#if defined(KUESA_DRACO_COMPRESSION)
namespace draco {
class PointCloud;
}
#endif

class QJsonArray;

namespace Kuesa {
namespace GLTF2Import {

class GLTF2Context;
struct BufferView;

struct MorphTargetAttribute {
    QString name;
    qint32 accessorIdx = -1;
};

struct AttributeInfo {
    bool isDracoAttribute = false;
    bool isIndexAttribute = false;
    QString name; // GLTF Semantic Name
    QString attributeName; // Qt3D Attribute Name
    qint32 accessorIdx = -1;
    qint32 dracoAttributeId = -1;
};

struct MorphTarget {
    std::vector<MorphTargetAttribute> attributes;
};

struct Primitive {
    Qt3DRender::QGeometryRenderer *primitiveRenderer = nullptr;
    qint32 materialIdx = -1;
    bool hasColorAttr = false;
    bool hasNormalAttr = false;
    bool hasTangentAttr = false;
    bool hasTexCoordAttr = false;
    bool hasTexCoord1Attr = false;
    bool isDracoCompressed = false;
    bool hasMorphTargets = false;
    qint32 dracoBufferViewIdx = -1;
    Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType = Qt3DRender::QGeometryRenderer::Triangles;

    std::vector<MorphTarget> morphTargets;
    std::vector<AttributeInfo> attributeInfo;
};

struct Mesh {
    QVector<Primitive> meshPrimitives;
    QString name;
    quint8 morphTargetCount = 0;
    QVector<float> morphTargetWeights;
    qint32 meshIdx = -1;
};

class Q_AUTOTEST_EXPORT PrimitiveBuilder
{
public:
    explicit PrimitiveBuilder(GLTF2Context *context);

    bool generateGeometryRendererForPrimitive(Primitive &primitive);

private:
    bool generateAttributes(Qt3DGeometry::QGeometry *geometry,
                            const Primitive &primitive);
    bool generateDracoAttributes(Qt3DGeometry::QGeometry *geometry,
                                 const Primitive &primitive);
    bool generateMorphTargetAttributes(Qt3DGeometry::QGeometry *geometry,
                                       const Primitive &primitive);
    Qt3DGeometry::QAttribute *createAttribute(qint32 accessorIndex,
                                              const QString &attributeName,
                                              const QString &semanticName);
private:
    GLTF2Context *m_context;
    QHash<qint32, Qt3DGeometry::QBuffer *> m_qViewBuffers;
    QHash<qint32, Qt3DGeometry::QBuffer *> m_qAccessorBuffers;
};

class Q_AUTOTEST_EXPORT MeshParser
{
public:
    MeshParser();

    bool parse(const QJsonArray &meshArray, GLTF2Context *context);

private:

    bool geometryFromJSON(const QJsonObject &json,
                          Primitive &primitive);
    bool geometryAttributesFromJSON(const QJsonObject &json,
                                    QStringList existingAttributes,
                                    Primitive &primitive);
    bool geometryMorphTargetsFromJSON(const QJsonObject &json,
                                      Primitive &primitive);
#if defined(KUESA_DRACO_COMPRESSION)
    bool geometryDracoFromJSON(const QJsonObject &json,
                               Primitive &primitive);
    bool geometryAttributesDracoFromJSON(const QJsonObject &json,
                                         QStringList &existingAttributes,
                                         Primitive &primitive);
#endif
    GLTF2Context *m_context;
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_MESHPARSER_P_H
