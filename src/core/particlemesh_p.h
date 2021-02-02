/*
    particlemesh_p.h

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mauro Persano <mauro.persano@kdab.com>

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

#ifndef KUESA_PARTICLEMESH_P_H
#define KUESA_PARTICLEMESH_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/QGeometryRenderer>

#include <Kuesa/private/kuesa_global_p.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QGeometry>
#else
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#endif

QT_BEGIN_NAMESPACE

namespace Kuesa {

class ParticleMesh : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT
    Q_PROPERTY(int particleCount READ particleCount WRITE setParticleCount NOTIFY particleCountChanged)
    Q_PROPERTY(Qt3DGeometry::QBuffer *vertexBuffer READ vertexBuffer WRITE setVertexBuffer NOTIFY vertexBufferChanged)

public:
    explicit ParticleMesh(Qt3DCore::QNode *parent = nullptr);

    int particleCount() const;
    Qt3DGeometry::QBuffer *vertexBuffer() const;

public Q_SLOTS:
    void setParticleCount(int particleCount);
    void setVertexBuffer(Qt3DGeometry::QBuffer *vertexBuffer);

Q_SIGNALS:
    void particleCountChanged(int particleCount);
    void vertexBufferChanged(Qt3DGeometry::QBuffer *vertexBuffer);
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_PARTICLEMESH_P_H
