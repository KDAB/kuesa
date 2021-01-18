/*
    fullscreenquad.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "fullscreenquad.h"
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DExtras/qplanemesh.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Kuesa/private/kuesa_global_p.h> // For Qt3DGeometry namespace alias
#include <QVector3D>
#include <QVector2D>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#else
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#endif

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
 * \class Kuesa::FullScreenQuad
 * \inheaderfile Kuesa/FullScreenQuad
 * \inmodule Kuesa
 * \since Kuesa 1.0
 * \brief FullScreenQuad is a convenience Qt3DCore::QEntity subclass
 * use to create a full screen quad.
 *
 * Its useful for creating background which must be rendered independently from the scene
 * or to create screen space effects on an image.
 *
 * A Qt3DRender::QLayer is added to the FullScreenQuad entity which can be retrieved with layer().
 */


namespace {

struct V {
    QVector3D pos;
    QVector2D tCoord;
};
static_assert(sizeof(V) == 5 * sizeof(float), "Unexpected size for struct V");

} // anonymous

/*!
 * \param material A material to be aggregated to the quad entity.
 * \param parent The parent node for the quad entity.
 *
 * This material \a material can be used to implement a gradient color
 * background or a screen space effect
 */
FullScreenQuad::FullScreenQuad(Qt3DRender::QMaterial *material, Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_buffer(new Qt3DGeometry::QBuffer())
{
    m_layer = new Qt3DRender::QLayer(this);

    auto *geometryRenderer = new Qt3DRender::QGeometryRenderer();
    auto *geometry = new Qt3DGeometry::QGeometry();
    auto *positionAttribute = new Qt3DGeometry::QAttribute();
    auto *texCoordAttribute = new Qt3DGeometry::QAttribute();

    positionAttribute->setName(Qt3DGeometry::QAttribute::defaultPositionAttributeName());
    positionAttribute->setAttributeType(Qt3DGeometry::QAttribute::VertexAttribute);
    positionAttribute->setVertexBaseType(Qt3DGeometry::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setByteStride(sizeof(V));
    positionAttribute->setByteOffset(0);
    positionAttribute->setBuffer(m_buffer);
    positionAttribute->setCount(6);

    texCoordAttribute->setName(Qt3DGeometry::QAttribute::defaultTextureCoordinateAttributeName());
    texCoordAttribute->setAttributeType(Qt3DGeometry::QAttribute::VertexAttribute);
    texCoordAttribute->setVertexBaseType(Qt3DGeometry::QAttribute::Float);
    texCoordAttribute->setVertexSize(2);
    texCoordAttribute->setByteStride(sizeof(V));
    texCoordAttribute->setByteOffset(sizeof(QVector3D));
    texCoordAttribute->setBuffer(m_buffer);
    texCoordAttribute->setCount(6);

    geometry->addAttribute(positionAttribute);
    geometry->addAttribute(texCoordAttribute);

    geometryRenderer->setGeometry(geometry);

    updateBufferData();

    addComponent(m_layer);
    addComponent(geometryRenderer);
    addComponent(material);
}

FullScreenQuad::~FullScreenQuad()
{
}

/*!
 * \brief FullScreenQuad::layer
 * \return The Qt3DRender::QLayer added to this quad entity.
 *
 * This layer can be used to filter render views and separate the quad entity
 * render from the scene render.
 */
Qt3DRender::QLayer *FullScreenQuad::layer() const
{
    return m_layer;
}

Qt3DGeometry::QBuffer *FullScreenQuad::buffer() const
{
    return m_buffer;
}

/*!
 *
 * Specifies the normalized coordinate rectangle \a vp subset of the input
 * texture on which to apply the material. This usually should match the
 * viewport rect used to render the scene.
 */
void FullScreenQuad::setViewportRect(const QRectF &vp)
{
    if (vp != m_viewportRect) {
        m_viewportRect = vp;
        updateBufferData();
    }
}

QRectF FullScreenQuad::viewportRect() const
{
    return m_viewportRect;
}

void FullScreenQuad::updateBufferData()
{
    QByteArray rawData;
    rawData.resize(6 * sizeof(V));

    V *vertices = reinterpret_cast<V *>(rawData.data());

    const float xMin = -1.0f + 2.0f * m_viewportRect.x();
    const float xMax = xMin + 2.0f * m_viewportRect.width();
    const float yMin = -1.0f + 2.0f * (1.0 - m_viewportRect.y() - m_viewportRect.height());
    const float yMax = yMin + 2.0f * m_viewportRect.height();

    vertices[0] = { { xMin, yMax, 0.0 }, { 0.0, 1.0 } };
    vertices[1] = { { xMin, yMin, 0.0 }, { 0.0, 0.0 } };
    vertices[2] = { { xMax, yMax, 0.0 }, { 1.0, 1.0 } };
    vertices[3] = { { xMax, yMax, 0.0 }, { 1.0, 1.0 } };
    vertices[4] = { { xMin, yMin, 0.0 }, { 0.0, 0.0 } };
    vertices[5] = { { xMax, yMin, 0.0 }, { 1.0, 0.0 } };

    m_buffer->setData(rawData);
}

} // namespace Kuesa

QT_END_NAMESPACE
