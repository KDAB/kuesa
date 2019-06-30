/*
    gltf2materialproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include "gltf2materialproperties.h"

#include <Qt3DRender/QMaterial>

QT_BEGIN_NAMESPACE

namespace Kuesa {

GLTF2MaterialProperties::GLTF2MaterialProperties(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
    , m_alphaCutOff(0.0f)
    , m_usesTexCoord1(false)
    , m_baseColorTexture(nullptr)
    , m_baseColorFactor(QColor("gray"))
{
}

GLTF2MaterialProperties::~GLTF2MaterialProperties()
{
    for (auto &connection : m_connections)
        QObject::disconnect(connection);
}

void GLTF2MaterialProperties::addClientMaterial(Qt3DRender::QMaterial *material)
{
    m_clientMaterials.push_back(material);
    m_connections.push_back(QObject::connect(material, &Qt3DRender::QMaterial::nodeDestroyed,
                                             [this, material]() {
                                                 m_clientMaterials.removeAll(material);
                                             }));
}

QVector<Qt3DRender::QMaterial *> GLTF2MaterialProperties::clientMaterials() const
{
    return m_clientMaterials;
}

bool GLTF2MaterialProperties::isBaseColorUsingTexCoord1() const
{
    return m_usesTexCoord1;
}

QColor GLTF2MaterialProperties::baseColorFactor() const
{
    return m_baseColorFactor;
}

Qt3DRender::QAbstractTexture *GLTF2MaterialProperties::baseColorMap() const
{
    return m_baseColorTexture;
}

float GLTF2MaterialProperties::alphaCutoff() const
{
    return m_alphaCutOff;
}

QMatrix3x3 GLTF2MaterialProperties::textureTransform() const
{
    return m_textureTransform;
}

void GLTF2MaterialProperties::setBaseColorUsesTexCoord1(bool baseColorUsesTexCoord1)
{
    if (m_usesTexCoord1 == baseColorUsesTexCoord1)
        return;
    m_usesTexCoord1 = baseColorUsesTexCoord1;
    emit baseColorUsesTexCoord1Changed(m_usesTexCoord1);
}

void GLTF2MaterialProperties::setBaseColorFactor(const QColor &baseColorFactor)
{
    if (m_baseColorFactor == baseColorFactor)
        return;
    m_baseColorFactor = baseColorFactor;
    emit baseColorFactorChanged(m_baseColorFactor);
}

void GLTF2MaterialProperties::setBaseColorMap(Qt3DRender::QAbstractTexture *baseColorMap)
{
    if (m_baseColorTexture == baseColorMap)
        return;
    m_baseColorTexture = baseColorMap;
    emit baseColorMapChanged(m_baseColorTexture);
}

void GLTF2MaterialProperties::setAlphaCutoff(float alphaCutoff)
{
    if (m_alphaCutOff == alphaCutoff)
        return;
    m_alphaCutOff = alphaCutoff;
    emit alphaCutoffChanged(m_alphaCutOff);
}

void GLTF2MaterialProperties::setTextureTransform(const QMatrix3x3 &textureTransform)
{
    if (m_textureTransform == textureTransform)
        return;
    m_textureTransform = textureTransform;
    emit textureTransformChanged(m_textureTransform);
}

} // Kuesa

QT_END_NAMESPACE
