/*
    metallicroughnessshaderdata.cpp

    This file is part of Kuesa.

    Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Qt3DRender/private/qshaderdata_p.h>

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
    , m_metallicFactor(0.0f)
    , m_roughnessFactor(0.0f)
    , m_normalScale(1.0f)
    , m_emissiveFactor(QColor("black"))
    , m_alphaCutoff(0.0f)
    , m_receivesShadows(true)
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

float MetallicRoughnessShaderData::metallicFactor() const
{
    return m_metallicFactor;
}

float MetallicRoughnessShaderData::roughnessFactor() const
{
    return m_roughnessFactor;
}

QMatrix3x3 MetallicRoughnessShaderData::metalRoughMapTextureTransform() const
{
    return m_metalRoughMapTextureTransform;
}

float MetallicRoughnessShaderData::normalScale() const
{
    return m_normalScale;
}

QMatrix3x3 MetallicRoughnessShaderData::normalMapTextureTransform() const
{
    return m_normalMapTextureTransform;
}

QMatrix3x3 MetallicRoughnessShaderData::ambientOcclusionMapTextureTransform() const
{
    return m_ambientOcclusionMapTextureTransform;
}

QColor MetallicRoughnessShaderData::emissiveFactor() const
{
    return m_emissiveFactor;
}

QMatrix3x3 MetallicRoughnessShaderData::emissiveMapTextureTransform() const
{
    return m_emissiveMapTextureTransform;
}

float MetallicRoughnessShaderData::alphaCutoff() const
{
    return m_alphaCutoff;
}

bool MetallicRoughnessShaderData::receivesShadows() const
{
    return m_receivesShadows;
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

void MetallicRoughnessShaderData::setBaseColorMapTextureTransform(const QMatrix3x3 &m)
{
    if (m_baseColorMapTextureTransform != m)
    {
        m_baseColorMapTextureTransform = m;
        emit baseColorMapTextureTransform();
    }
}

void MetallicRoughnessShaderData::setEmissiveMapTextureTransform(const QMatrix3x3 &m)
{
    if (m_emissiveMapTextureTransform != m)
    {
        m_emissiveMapTextureTransform = m;
        emit emissiveMapTextureTransform();
    }
}

void MetallicRoughnessShaderData::setMetalRoughMapTextureTransform(const QMatrix3x3 &m)
{
    if (m_metalRoughMapTextureTransform != m)
    {
        m_metalRoughMapTextureTransform = m;
        emit metalRoughMapTextureTransform();
    }
}

void MetallicRoughnessShaderData::setNormalMapTextureTransform(const QMatrix3x3 &m)
{
    if (m_normalMapTextureTransform != m)
    {
        m_normalMapTextureTransform = m;
        emit normalMapTextureTransform();
    }
}

void MetallicRoughnessShaderData::setAmbientOcclusionMapTextureTransform(const QMatrix3x3 &m)
{
    if (m_ambientOcclusionMapTextureTransform != m)
    {
        m_ambientOcclusionMapTextureTransform = m;
        emit ambientOcclusionMapTextureTransform();
    }
}

void MetallicRoughnessShaderData::setNormalScale(float normalScale)
{
    if (m_normalScale == normalScale)
        return;
    m_normalScale = normalScale;
    emit normalScaleChanged(normalScale);
}

void MetallicRoughnessShaderData::setEmissiveFactor(const QColor &emissiveFactor)
{
    if (m_emissiveFactor == emissiveFactor)
        return;
    m_emissiveFactor = emissiveFactor;
    emit emissiveFactorChanged(emissiveFactor);
}

void MetallicRoughnessShaderData::setAlphaCutoff(float alphaCutoff)
{
    if (m_alphaCutoff == alphaCutoff)
        return;
    m_alphaCutoff = alphaCutoff;
    emit alphaCutoffChanged(alphaCutoff);
}

void MetallicRoughnessShaderData::setReceivesShadows(bool receivesShadows)
{
    if (m_receivesShadows == receivesShadows)
        return;
    m_receivesShadows = receivesShadows;
    emit receivesShadowsChanged(receivesShadows);
}

QMatrix3x3 Kuesa::MetallicRoughnessShaderData::baseColorMapTextureTransform() const
{
    return m_baseColorMapTextureTransform;
}

} // namespace Kuesa

QT_END_NAMESPACE
