/*
    materialinspector.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "materialinspector.h"
#include <Kuesa/materialcollection.h>
#include <Kuesa/metallicroughnessmaterial.h>
#include <Kuesa/metallicroughnesseffect.h>
#include <Kuesa/metallicroughnessproperties.h>
#include <Kuesa/unlitmaterial.h>
#include <Kuesa/unliteffect.h>
#include <Kuesa/unlitproperties.h>
#include <Qt3DRender/qabstracttexture.h>

MaterialInspector::MaterialInspector(QObject *parent)
    : QObject(parent)
    , m_materialProperties(nullptr)
    , m_material(nullptr)
    , m_pbrMaterial(new Kuesa::MetallicRoughnessMaterial)
    , m_unlitMaterial(new Kuesa::UnlitMaterial)
{
    {
        auto *effect = new Kuesa::MetallicRoughnessEffect(m_pbrMaterial);
        effect->setUseSkinning(false);
        effect->setUsingColorAttribute(false);
        m_pbrMaterial->setEffect(effect);
    }
    {
        auto *effect = new Kuesa::UnlitEffect(m_unlitMaterial);
        effect->setUseSkinning(false);
        effect->setUsingColorAttribute(false);
        m_unlitMaterial->setEffect(effect);
    }
}

MaterialInspector::~MaterialInspector()
{
    m_pbrMaterial->deleteLater();
    m_unlitMaterial->deleteLater();
    m_pbrMaterial = nullptr;
    m_unlitMaterial = nullptr;
    m_material = nullptr;
}

void MaterialInspector::setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties)
{
    if (materialProperties == m_materialProperties)
        return;

    disconnect(m_materialConnection);
    if (materialProperties) {
        m_materialConnection = connect(materialProperties, &Qt3DCore::QNode::nodeDestroyed, this, [this]() { setMaterialProperties(nullptr); });

        const auto clientMaterials = materialProperties->clientMaterials();
        if (Kuesa::MetallicRoughnessProperties *pbrProperties = qobject_cast<Kuesa::MetallicRoughnessProperties *>(materialProperties)) {

            if (m_material != m_pbrMaterial) {
                m_material = m_pbrMaterial;
            }

            m_pbrMaterial->setMetallicRoughnessProperties(pbrProperties);
            auto materialEffect = static_cast<Kuesa::MetallicRoughnessEffect *>(m_material->effect());
            auto clientEffect = static_cast<Kuesa::MetallicRoughnessEffect *>(clientMaterials.front()->effect());
            materialEffect->setBrdfLUT(clientEffect->brdfLUT());

            // Configure the material effect
            materialEffect->setOpaque(clientEffect->isOpaque());
            materialEffect->setDoubleSided(clientEffect->isDoubleSided());
            materialEffect->setNormalMapEnabled(clientEffect->isNormalMapEnabled());
            materialEffect->setAlphaCutoffEnabled(clientEffect->isAlphaCutoffEnabled());
            materialEffect->setEmissiveMapEnabled(clientEffect->isEmissiveMapEnabled());
            materialEffect->setBaseColorMapEnabled(clientEffect->isBaseColorMapEnabled());
            materialEffect->setMetalRoughMapEnabled(clientEffect->isMetalRoughMapEnabled());
            materialEffect->setAmbientOcclusionMapEnabled(clientEffect->isAmbientOcclusionMapEnabled());
        } else if (Kuesa::UnlitProperties *unlitProperties = qobject_cast<Kuesa::UnlitProperties *>(materialProperties)) {

            if (m_material != m_unlitMaterial) {
                m_material = m_unlitMaterial;
            }

            m_unlitMaterial->setUnlitProperties(unlitProperties);
            auto materialEffect = static_cast<Kuesa::UnlitEffect *>(m_material->effect());
            auto clientEffect = static_cast<Kuesa::UnlitEffect *>(clientMaterials.front()->effect());
            materialEffect->setOpaque(clientEffect->isOpaque());
            materialEffect->setDoubleSided(clientEffect->isDoubleSided());
            materialEffect->setAlphaCutoffEnabled(clientEffect->isAlphaCutoffEnabled());
            materialEffect->setBaseColorMapEnabled(clientEffect->isBaseColorMapEnabled());
        }
    }

    m_materialProperties = materialProperties;
    emit materialPropertiesChanged();
}

Kuesa::GLTF2MaterialProperties *MaterialInspector::materialProperties() const
{
    return m_materialProperties;
}

Kuesa::GLTF2Material *MaterialInspector::material() const
{
    return m_material;
}

QString MaterialInspector::assetName() const
{
    return m_materialProperties ? m_materialProperties->objectName() : QString();
}

QColor MaterialInspector::baseColor() const
{
    return m_materialProperties ? m_materialProperties->baseColorFactor() : QColor();
}

Qt3DRender::QAbstractTexture *MaterialInspector::baseColorMap() const
{
    return m_materialProperties ? m_materialProperties->baseColorMap() : nullptr;
}

float MaterialInspector::metallicFactor() const
{
    const auto pbrProperties = qobject_cast<Kuesa::MetallicRoughnessProperties *>(m_materialProperties);
    return pbrProperties ? pbrProperties->metallicFactor() : 0.0f;
}

float MaterialInspector::roughnessFactor() const
{
    const auto pbrProperties = qobject_cast<Kuesa::MetallicRoughnessProperties *>(m_materialProperties);
    return pbrProperties ? pbrProperties->roughnessFactor() : 0.0f;
}

Qt3DRender::QAbstractTexture *MaterialInspector::metalRoughMap() const
{
    const auto pbrProperties = qobject_cast<Kuesa::MetallicRoughnessProperties *>(m_materialProperties);
    return pbrProperties ? pbrProperties->metalRoughMap() : nullptr;
}

Qt3DRender::QAbstractTexture *MaterialInspector::normalMap() const
{
    const auto pbrProperties = qobject_cast<Kuesa::MetallicRoughnessProperties *>(m_materialProperties);
    return pbrProperties ? pbrProperties->normalMap() : nullptr;
}

float MaterialInspector::normalScale() const
{
    const auto pbrProperties = qobject_cast<Kuesa::MetallicRoughnessProperties *>(m_materialProperties);
    return pbrProperties ? pbrProperties->normalScale() : 0.0f;
}

Qt3DRender::QAbstractTexture *MaterialInspector::ambientOcclusionMap() const
{
    const auto pbrProperties = qobject_cast<Kuesa::MetallicRoughnessProperties *>(m_materialProperties);
    return pbrProperties ? pbrProperties->ambientOcclusionMap() : nullptr;
}

QColor MaterialInspector::emissiveFactor() const
{
    const auto pbrProperties = qobject_cast<Kuesa::MetallicRoughnessProperties *>(m_materialProperties);
    return pbrProperties ? pbrProperties->emissiveFactor() : QColor();
}

Qt3DRender::QAbstractTexture *MaterialInspector::emissiveMap() const
{
    const auto pbrProperties = qobject_cast<Kuesa::MetallicRoughnessProperties *>(m_materialProperties);
    return pbrProperties ? pbrProperties->emissiveMap() : nullptr;
}

QMatrix3x3 MaterialInspector::textureTransform() const
{
    return QMatrix3x3();
}

bool MaterialInspector::doubleSided() const
{
    if (!m_materialProperties)
        return false;
    auto material = m_materialProperties->clientMaterials();
    if (material.size()) {
        auto effect = material[0]->effect();
        Kuesa::MetallicRoughnessEffect *pbrEffect = qobject_cast<Kuesa::MetallicRoughnessEffect *>(effect);
        if (pbrEffect)
            return pbrEffect->isDoubleSided();
        Kuesa::UnlitEffect *unlitEffect = qobject_cast<Kuesa::UnlitEffect *>(effect);
        if (unlitEffect)
            return unlitEffect->isDoubleSided();
    }
    return false;
}

bool MaterialInspector::opaque() const
{
    if (!m_materialProperties)
        return false;
    auto material = m_materialProperties->clientMaterials();
    if (material.size()) {
        auto effect = material[0]->effect();
        Kuesa::MetallicRoughnessEffect *pbrEffect = qobject_cast<Kuesa::MetallicRoughnessEffect *>(effect);
        if (pbrEffect)
            return pbrEffect->isOpaque();
        Kuesa::UnlitEffect *unlitEffect = qobject_cast<Kuesa::UnlitEffect *>(effect);
        if (unlitEffect)
            return unlitEffect->isOpaque();
    }
    return true;
}

bool MaterialInspector::usesSkinning() const
{
    if (!m_materialProperties)
        return false;
    auto material = m_materialProperties->clientMaterials();
    if (material.size()) {
        auto effect = material[0]->effect();
        Kuesa::MetallicRoughnessEffect *pbrEffect = qobject_cast<Kuesa::MetallicRoughnessEffect *>(effect);
        if (pbrEffect)
            return pbrEffect->useSkinning();
        Kuesa::UnlitEffect *unlitEffect = qobject_cast<Kuesa::UnlitEffect *>(effect);
        if (unlitEffect)
            return unlitEffect->useSkinning();
    }
    return false;
}

bool MaterialInspector::hasTextures() const
{
    const auto pbrProperties = qobject_cast<Kuesa::MetallicRoughnessProperties *>(m_materialProperties);
    if (pbrProperties)
        return (pbrProperties->baseColorMap() //
                || pbrProperties->metalRoughMap() //
                || pbrProperties->normalMap() //
                || pbrProperties->emissiveMap() //
                || pbrProperties->ambientOcclusionMap());

    const auto unlitProperties = qobject_cast<Kuesa::UnlitProperties *>(m_materialProperties);
    if (unlitProperties)
        return (unlitProperties->baseColorMap());
    return false;
}
