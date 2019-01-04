/*
    meshparser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "bufferaccessorparser_p.h"

QT_BEGIN_NAMESPACE

#if defined(KUESA_DRACO_COMPRESSION)
namespace draco {
class PointCloud;
};
#endif

class QJsonArray;
namespace Qt3DRender {
class QGeometryRenderer;
class QGeometry;
} // namespace Qt3DRender

namespace Kuesa {
namespace GLTF2Import {

class GLTF2ContextPrivate;
struct BufferView;

struct Primitive {
    Qt3DRender::QGeometryRenderer *primitiveRenderer = nullptr;
    qint32 materialIdx = -1;
    bool hasColorAttr = false;
};

struct Mesh {
    void clear();

    QVector<Primitive> meshPrimitives;
    QString name;
};

class Q_AUTOTEST_EXPORT MeshParser
{
public:
    MeshParser();

    bool parse(const QJsonArray &meshArray, GLTF2ContextPrivate *context);

private:
    bool geometryFromJSON(Qt3DRender::QGeometry *geometry, const QJsonObject &json, bool &hasColorAttr);
    bool geometryAttributesFromJSON(Qt3DRender::QGeometry *geometry, const QJsonObject &json, QStringList existingAttributes, bool &hasColorAttr);
#if defined(KUESA_DRACO_COMPRESSION)
    bool geometryDracoFromJSON(Qt3DRender::QGeometry *geometry, const QJsonObject &json, bool &hasColorAttr);
    bool geometryAttributesDracoFromJSON(Qt3DRender::QGeometry *geometry, const QJsonObject &json, const draco::PointCloud *pointCloud, QStringList &existingAttributes, bool &hasColorAttr);
#endif

    GLTF2ContextPrivate *m_context;
    QHash<int, Qt3DRender::QBuffer *> m_qbuffers;
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_MESHPARSER_P_H
