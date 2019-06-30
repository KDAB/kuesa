/*
    metallicroughnessproperties.cpp

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

#include "metallicroughnessproperties.h"
#include "metallicroughnessshaderdata_p.h"

namespace Kuesa {

MetallicRoughnessProperties::MetallicRoughnessProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_metallicRoughnessShaderData(new MetallicRoughnessShaderData(this))

{
    QObject::connect(this, &GLTF2MaterialProperties::baseColorMapChanged,
                     m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::setBaseColorMap);
    QObject::connect(this, &GLTF2MaterialProperties::baseColorFactorChanged,
                     m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::setBaseColorFactor);
    QObject::connect(this, &GLTF2MaterialProperties::baseColorUsesTexCoord1Changed,
                     m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::setBaseColorUsesTexCoord1);
    QObject::connect(this, &GLTF2MaterialProperties::alphaCutoffChanged,
                     m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::setAlphaCutoff);


    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::metallicRoughnessUsesTexCoord1Changed,
                     this, &MetallicRoughnessProperties::metallicRoughnessUsesTexCoord1Changed);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::normalUsesTexCoord1Changed,
                     this, &MetallicRoughnessProperties::normalUsesTexCoord1Changed);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::aoUsesTexCoord1Changed,
                     this, &MetallicRoughnessProperties::aoUsesTexCoord1Changed);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::emissiveUsesTexCoord1Changed,
                     this, &MetallicRoughnessProperties::emissiveUsesTexCoord1Changed);

    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::metallicFactorChanged,
                     this, &MetallicRoughnessProperties::metallicFactorChanged);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::roughnessFactorChanged,
                     this, &MetallicRoughnessProperties::roughnessFactorChanged);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::metalRoughMapChanged,
                     this, &MetallicRoughnessProperties::metalRoughMapChanged);

    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::normalScaleChanged,
                     this, &MetallicRoughnessProperties::normalScaleChanged);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::normalMapChanged,
                     this, &MetallicRoughnessProperties::normalMapChanged);

    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::ambientOcclusionMapChanged,
                     this, &MetallicRoughnessProperties::ambientOcclusionMapChanged);

    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::emissiveFactorChanged,
                     this, &MetallicRoughnessProperties::emissiveFactorChanged);
    QObject::connect(m_metallicRoughnessShaderData, &MetallicRoughnessShaderData::emissiveMapChanged,
                     this, &MetallicRoughnessProperties::emissiveMapChanged);
}

Qt3DRender::QShaderData *MetallicRoughnessProperties::shaderData() const
{
    return m_metallicRoughnessShaderData;
}

bool MetallicRoughnessProperties::isMetallicRoughnessUsingTexCoord1() const
{
    return m_metallicRoughnessShaderData->isMetallicRoughnessUsingTexCoord1();
}

bool MetallicRoughnessProperties::isNormalUsingTexCoord1() const
{
    return m_metallicRoughnessShaderData->isNormalUsingTexCoord1();
}

bool MetallicRoughnessProperties::isAOUsingTexCoord1() const
{
    return m_metallicRoughnessShaderData->isAOUsingTexCoord1();
}

bool MetallicRoughnessProperties::isEmissiveUsingTexCoord1() const
{
    return m_metallicRoughnessShaderData->isEmissiveUsingTexCoord1();
}

float MetallicRoughnessProperties::metallicFactor() const
{
    return m_metallicRoughnessShaderData->metallicFactor();
}

float MetallicRoughnessProperties::roughnessFactor() const
{
    return m_metallicRoughnessShaderData->roughnessFactor();
}

Qt3DRender::QAbstractTexture *MetallicRoughnessProperties::metalRoughMap() const
{
    return m_metallicRoughnessShaderData->metalRoughMap();
}

float MetallicRoughnessProperties::normalScale() const
{
    return m_metallicRoughnessShaderData->normalScale();
}

Qt3DRender::QAbstractTexture *MetallicRoughnessProperties::normalMap() const
{
    return m_metallicRoughnessShaderData->normalMap();
}

Qt3DRender::QAbstractTexture *MetallicRoughnessProperties::ambientOcclusionMap() const
{
    return m_metallicRoughnessShaderData->ambientOcclusionMap();
}

QColor MetallicRoughnessProperties::emissiveFactor() const
{
    return m_metallicRoughnessShaderData->emissiveFactor();
}

Qt3DRender::QAbstractTexture *MetallicRoughnessProperties::emissiveMap() const
{
    return m_metallicRoughnessShaderData->emissiveMap();
}
void MetallicRoughnessProperties::setMetallicRoughnessUsesTexCoord1(bool metallicRoughnessUsesTexCoord1)
{
    m_metallicRoughnessShaderData->setMetallicRoughnessUsesTexCoord1(metallicRoughnessUsesTexCoord1);
}

void MetallicRoughnessProperties::setNormalUsesTexCoord1(bool normalUsesTexCoord1)
{
    m_metallicRoughnessShaderData->setNormalUsesTexCoord1(normalUsesTexCoord1);
}

void MetallicRoughnessProperties::setAOUsesTexCoord1(bool aoUsesTexCoord1)
{
    m_metallicRoughnessShaderData->setAOUsesTexCoord1(aoUsesTexCoord1);
}

void MetallicRoughnessProperties::setEmissiveUsesTexCoord1(bool emissiveUsesTexCoord1)
{
    m_metallicRoughnessShaderData->setEmissiveUsesTexCoord1(emissiveUsesTexCoord1);
}

void MetallicRoughnessProperties::setMetallicFactor(float metallicFactor)
{
    m_metallicRoughnessShaderData->setMetallicFactor(metallicFactor);
}

void MetallicRoughnessProperties::setRoughnessFactor(float roughnessFactor)
{
    m_metallicRoughnessShaderData->setRoughnessFactor(roughnessFactor);
}

void MetallicRoughnessProperties::setMetalRoughMap(Qt3DRender::QAbstractTexture *metalRoughMap)
{
    m_metallicRoughnessShaderData->setMetalRoughMap(metalRoughMap);
}

void MetallicRoughnessProperties::setNormalMap(Qt3DRender::QAbstractTexture *normalMap)
{
    m_metallicRoughnessShaderData->setNormalMap(normalMap);
}

void MetallicRoughnessProperties::setNormalScale(float normalScale)
{
    m_metallicRoughnessShaderData->setNormalScale(normalScale);
}

void MetallicRoughnessProperties::setAmbientOcclusionMap(Qt3DRender::QAbstractTexture *ambientOcclusionMap)
{
    m_metallicRoughnessShaderData->setAmbientOcclusionMap(ambientOcclusionMap);
}

void MetallicRoughnessProperties::setEmissiveFactor(const QColor &emissiveFactor)
{
    m_metallicRoughnessShaderData->setEmissiveFactor(emissiveFactor);
}

void MetallicRoughnessProperties::setEmissiveMap(Qt3DRender::QAbstractTexture *emissiveMap)
{
    m_metallicRoughnessShaderData->setEmissiveMap(emissiveMap);
}

} // Kuesa
