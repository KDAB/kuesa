/*
    metallicroughnessshaderdata.cpp

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

#include "metallicroughnessshaderdata_p.h"

QT_BEGIN_NAMESPACE
namespace Kuesa {

MetallicRoughnessShaderData::MetallicRoughnessShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_baseColorUsesTexCoord1(false)
    , m_metallicRoughnessUsesTexCoord1(false)
    , m_normalUsesTexCoord1(false)
    , m_aoUsesTexCoord1(false)
    , m_emissiveUsesTexCoord1(false)
    , m_baseColorFactor(QColor("gray"))
    , m_baseColorMap(nullptr)
    , m_metallicFactor(0.0f)
    , m_roughnessFactor(0.0f)
    , m_metalRoughMap(nullptr)
    , m_normalScale(1.0f)
    , m_normalMap(nullptr)
    , m_ambientOcclusionMap(nullptr)
    , m_emissiveFactor(QColor("black"))
    , m_emissiveMap(nullptr)
{
}

MetallicRoughnessShaderData::~MetallicRoughnessShaderData()
{
}

bool MetallicRoughnessShaderData::isBaseColorUsingTexCoord1() const
{
    return m_baseColorUsesTexCoord1;
}

bool MetallicRoughnessShaderData::isMetallicRoughnessUsingTexCoord1() const
{
    return m_metallicRoughnessUsesTexCoord1;
}

bool MetallicRoughnessShaderData::isNormalUsingTexCoord1() const
{
    return m_normalUsesTexCoord1;
}

bool MetallicRoughnessShaderData::isAOUsingTexCoord1() const
{
    return m_aoUsesTexCoord1;
}

bool MetallicRoughnessShaderData::isEmissiveUsingTexCoord1() const
{
    return m_emissiveUsesTexCoord1;
}

QColor MetallicRoughnessShaderData::baseColorFactor() const
{
    return m_baseColorFactor;
}

Qt3DRender::QAbstractTexture *MetallicRoughnessShaderData::baseColorMap() const
{
    return m_baseColorMap;
}

float MetallicRoughnessShaderData::metallicFactor() const
{
    return m_metallicFactor;
}

float MetallicRoughnessShaderData::roughnessFactor() const
{
    return m_roughnessFactor;
}

Qt3DRender::QAbstractTexture *MetallicRoughnessShaderData::metalRoughMap() const
{
    return m_metalRoughMap;
}

float MetallicRoughnessShaderData::normalScale() const
{
    return m_normalScale;
}

Qt3DRender::QAbstractTexture *MetallicRoughnessShaderData::normalMap() const
{
    return m_normalMap;
}

Qt3DRender::QAbstractTexture *MetallicRoughnessShaderData::ambientOcclusionMap() const
{
    return m_ambientOcclusionMap;
}

QColor MetallicRoughnessShaderData::emissiveFactor() const
{
    return m_emissiveFactor;
}

Qt3DRender::QAbstractTexture *MetallicRoughnessShaderData::emissiveMap() const
{
    return m_emissiveMap;
}

float MetallicRoughnessShaderData::alphaCutoff() const
{
    return m_alphaCutoff;
}

void MetallicRoughnessShaderData::setBaseColorUsesTexCoord1(bool baseColorUsesTexCoord1)
{
    if (m_baseColorUsesTexCoord1 == baseColorUsesTexCoord1)
        return;
    m_baseColorUsesTexCoord1 = baseColorUsesTexCoord1;
    emit baseColorUsesTexCoord1Changed(baseColorUsesTexCoord1);
}

void MetallicRoughnessShaderData::setMetallicRoughnessUsesTexCoord1(bool metallicRoughnessUsesTexCoord1)
{
    if (m_metallicRoughnessUsesTexCoord1 == metallicRoughnessUsesTexCoord1)
        return;
    m_metallicRoughnessUsesTexCoord1 = metallicRoughnessUsesTexCoord1;
    emit metallicRoughnessUsesTexCoord1Changed(metallicRoughnessUsesTexCoord1);
}

void MetallicRoughnessShaderData::setNormalUsesTexCoord1(bool normalUsesTexCoord1)
{
    if (m_normalUsesTexCoord1 == normalUsesTexCoord1)
        return;
    m_normalUsesTexCoord1 = normalUsesTexCoord1;
    emit normalUsesTexCoord1Changed(normalUsesTexCoord1);
}

void MetallicRoughnessShaderData::setAOUsesTexCoord1(bool aoUsesTexCoord1)
{
    if (m_aoUsesTexCoord1 == aoUsesTexCoord1)
        return;
    m_aoUsesTexCoord1 = aoUsesTexCoord1;
    emit aoUsesTexCoord1Changed(aoUsesTexCoord1);
}

void MetallicRoughnessShaderData::setEmissiveUsesTexCoord1(bool emissiveUsesTexCoord1)
{
    if (m_emissiveUsesTexCoord1 == emissiveUsesTexCoord1)
        return;
    m_emissiveUsesTexCoord1 = emissiveUsesTexCoord1;
    emit emissiveUsesTexCoord1Changed(emissiveUsesTexCoord1);
}

void MetallicRoughnessShaderData::setBaseColorFactor(const QColor &baseColorFactor)
{
    if (m_baseColorFactor == baseColorFactor)
        return;
    m_baseColorFactor = baseColorFactor;
    emit baseColorFactorChanged(baseColorFactor);
}

void MetallicRoughnessShaderData::setBaseColorMap(Qt3DRender::QAbstractTexture *baseColorMap)
{
    if (m_baseColorMap == baseColorMap)
        return;
#ifndef QT_OPENGL_ES_2
    if (baseColorMap)
        baseColorMap->setFormat(Qt3DRender::QAbstractTexture::TextureFormat::SRGB8_Alpha8);
#endif
    m_baseColorMap = baseColorMap;
    baseColorMapChanged(baseColorMap);
}

void MetallicRoughnessShaderData::setMetallicFactor(float metallicFactor)
{
    if (m_metallicFactor == metallicFactor)
        return;
    m_metallicFactor = metallicFactor;
    emit metallicFactorChanged(metallicFactor);
}

void MetallicRoughnessShaderData::setRoughnessFactor(float roughnessFactor)
{
    if (m_roughnessFactor == roughnessFactor)
        return;
    m_roughnessFactor = roughnessFactor;
    emit roughnessFactorChanged(roughnessFactor);
}

void MetallicRoughnessShaderData::setMetalRoughMap(Qt3DRender::QAbstractTexture *metalRoughMap)
{
    if (m_metalRoughMap == metalRoughMap)
        return;
    m_metalRoughMap = metalRoughMap;
    emit metalRoughMapChanged(metalRoughMap);
}

void MetallicRoughnessShaderData::setNormalMap(Qt3DRender::QAbstractTexture *normalMap)
{
    if (m_normalMap == normalMap)
        return;
    m_normalMap = normalMap;
    emit normalMapChanged(normalMap);
}

void MetallicRoughnessShaderData::setNormalScale(float normalScale)
{
    if (m_normalScale == normalScale)
        return;
    m_normalScale = normalScale;
    emit normalScaleChanged(normalScale);
}

void MetallicRoughnessShaderData::setAmbientOcclusionMap(Qt3DRender::QAbstractTexture *ambientOcclusionMap)
{
    if (m_ambientOcclusionMap == ambientOcclusionMap)
        return;
    m_ambientOcclusionMap = ambientOcclusionMap;
    emit ambientOcclusionMapChanged(ambientOcclusionMap);
}

void MetallicRoughnessShaderData::setEmissiveFactor(const QColor &emissiveFactor)
{
    if (m_emissiveFactor == emissiveFactor)
        return;
    m_emissiveFactor = emissiveFactor;
    emit emissiveFactorChanged(emissiveFactor);
}

void MetallicRoughnessShaderData::setEmissiveMap(Qt3DRender::QAbstractTexture *emissiveMap)
{
    if (m_emissiveMap == emissiveMap)
        return;

#ifndef QT_OPENGL_ES_2
    if (emissiveMap)
        emissiveMap->setFormat(Qt3DRender::QAbstractTexture::TextureFormat::SRGB8_Alpha8);
#endif
    m_emissiveMap = emissiveMap;
    emit emissiveMapChanged(emissiveMap);
}

void MetallicRoughnessShaderData::setAlphaCutoff(float alphaCutoff)
{
    if (m_alphaCutoff == alphaCutoff)
        return;
    m_alphaCutoff = alphaCutoff;
    emit alphaCutoffChanged(alphaCutoff);
}

} // namespace Kuesa

QT_END_NAMESPACE
