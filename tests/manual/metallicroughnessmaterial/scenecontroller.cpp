/*
    scenecontroller.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Kevin Ottens <kevin.ottens@kdab.com>

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

#include "scenecontroller.h"

SceneController::SceneController(QObject *parent)
    : QObject(parent)
    , m_baseColorFactor("white")
    , m_useBaseColorMap(false)
    , m_metallicFactor(1.0f)
    , m_roughnessFactor(1.0f)
    , m_useMetalRoughMap(false)
    , m_normalScale(1.0f)
    , m_useNormalMap(false)
    , m_useAmbientOcclusionMap(false)
    , m_emissiveFactor("black")
    , m_useEmissiveMap(false)
    , m_useColorAttribute(false)
    , m_doubleSided(false)
    , m_toneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::Reinhard)
{
}

QColor SceneController::baseColorFactor() const
{
    return m_baseColorFactor;
}

bool SceneController::useBaseColorMap() const
{
    return m_useBaseColorMap;
}

float SceneController::metallicFactor() const
{
    return m_metallicFactor;
}

float SceneController::roughnessFactor() const
{
    return m_roughnessFactor;
}

bool SceneController::useMetalRoughMap() const
{
    return m_useMetalRoughMap;
}

float SceneController::normalScale() const
{
    return m_normalScale;
}

bool SceneController::useNormalMap() const
{
    return m_useNormalMap;
}

bool SceneController::useAmbientOcclusionMap() const
{
    return m_useAmbientOcclusionMap;
}

QColor SceneController::emissiveFactor() const
{
    return m_emissiveFactor;
}

bool SceneController::useEmissiveMap() const
{
    return m_useEmissiveMap;
}

QMatrix3x3 SceneController::textureTransform() const
{
    return m_textureTransform;
}

bool SceneController::useColorAttribute() const
{
    return m_useColorAttribute;
}

bool SceneController::doubleSided() const
{
    return m_doubleSided;
}

Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping SceneController::toneMappingAlgorithm() const
{
    return m_toneMappingAlgorithm;
}

void SceneController::setBaseColorFactor(const QColor &baseColorFactor)
{
    if (m_baseColorFactor == baseColorFactor)
        return;

    m_baseColorFactor = baseColorFactor;
    emit baseColorFactorChanged(baseColorFactor);
}

void SceneController::setUseBaseColorMap(bool useBaseColorMap)
{
    if (m_useBaseColorMap == useBaseColorMap)
        return;

    m_useBaseColorMap = useBaseColorMap;
    emit useBaseColorMapChanged(useBaseColorMap);
}

void SceneController::setMetallicFactor(float metallicFactor)
{
    if (qFuzzyCompare(m_metallicFactor, metallicFactor))
        return;

    m_metallicFactor = metallicFactor;
    emit metallicFactorChanged(metallicFactor);
}

void SceneController::setRoughnessFactor(float roughnessFactor)
{
    if (qFuzzyCompare(m_roughnessFactor, roughnessFactor))
        return;

    m_roughnessFactor = roughnessFactor;
    emit roughnessFactorChanged(roughnessFactor);
}

void SceneController::setUseMetalRoughMap(bool useMetalRoughMap)
{
    if (m_useMetalRoughMap == useMetalRoughMap)
        return;

    m_useMetalRoughMap = useMetalRoughMap;
    emit useMetalRoughMapChanged(useMetalRoughMap);
}

void SceneController::setNormalScale(float normalScale)
{
    if (qFuzzyCompare(m_normalScale, normalScale))
        return;

    m_normalScale = normalScale;
    emit normalScaleChanged(normalScale);
}

void SceneController::setUseNormalMap(bool useNormalMap)
{
    if (m_useNormalMap == useNormalMap)
        return;

    m_useNormalMap = useNormalMap;
    emit useNormalMapChanged(useNormalMap);
}

void SceneController::setUseAmbientOcclusionMap(bool useAmbientOcclusionMap)
{
    if (m_useAmbientOcclusionMap == useAmbientOcclusionMap)
        return;

    m_useAmbientOcclusionMap = useAmbientOcclusionMap;
    emit useAmbientOcclusionMapChanged(useAmbientOcclusionMap);
}

void SceneController::setEmissiveFactor(const QColor &emissiveFactor)
{
    if (m_emissiveFactor == emissiveFactor)
        return;

    m_emissiveFactor = emissiveFactor;
    emit emissiveFactorChanged(emissiveFactor);
}

void SceneController::setUseEmissiveMap(bool useEmissiveMap)
{
    if (m_useEmissiveMap == useEmissiveMap)
        return;

    m_useEmissiveMap = useEmissiveMap;
    emit useEmissiveMapChanged(useEmissiveMap);
}

void SceneController::setTextureTransform(const QMatrix3x3 &textureTransform)
{
    if (m_textureTransform == textureTransform)
        return;

    m_textureTransform = textureTransform;
    emit textureTransformChanged(textureTransform);
}

void SceneController::setUseColorAttribute(bool useColorAttribute)
{
    if (m_useColorAttribute == useColorAttribute)
        return;

    m_useColorAttribute = useColorAttribute;
    emit useColorAttributeChanged(useColorAttribute);
}

void SceneController::setDoubleSided(bool doubleSided)
{
    if (m_doubleSided == doubleSided)
        return;

    m_doubleSided = doubleSided;
    emit doubleSidedChanged(doubleSided);
}

void SceneController::setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping algorithm)
{
    if (m_toneMappingAlgorithm == algorithm)
        return;
    m_toneMappingAlgorithm = algorithm;
    emit toneMappingAlgorithmChanged(algorithm);
}
