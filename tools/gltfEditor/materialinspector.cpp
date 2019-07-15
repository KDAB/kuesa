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
#include <Qt3DRender/qabstracttexture.h>

MaterialInspector::MaterialInspector(QObject *parent)
    : QObject(parent)
    , m_materialProperties(nullptr)
    , m_material(new Kuesa::MetallicRoughnessMaterial)
{
    auto *effect = new Kuesa::MetallicRoughnessEffect(m_material);
    effect->setUseSkinning(false);
    effect->setUsingColorAttribute(false);
    m_material->setEffect(effect);
}

MaterialInspector::~MaterialInspector()
{
}

void MaterialInspector::setMaterialProperties(Kuesa::MetallicRoughnessProperties *materialProperties)
{
    if (materialProperties == m_materialProperties)
        return;

    disconnect(m_materialConnection);
    if (materialProperties) {
        m_materialConnection = connect(materialProperties, &Qt3DCore::QNode::nodeDestroyed, this, [this]() { setMaterialProperties(nullptr); });
        m_material->setMetallicRoughnessProperties(materialProperties);
        auto clientEffect = static_cast<Kuesa::MetallicRoughnessEffect *>(materialProperties->clientMaterials()[0]->effect());
        auto materialEffect = static_cast<Kuesa::MetallicRoughnessEffect *>(m_material->effect());
        static_cast<Kuesa::MetallicRoughnessEffect *>(m_material->effect())->setBrdfLUT(clientEffect->brdfLUT());

        // Configure the material effect
        materialEffect->setOpaque(clientEffect->isOpaque());
        materialEffect->setDoubleSided(clientEffect->isDoubleSided());
        materialEffect->setNormalMapEnabled(clientEffect->isNormalMapEnabled());
        materialEffect->setAlphaCutoffEnabled(clientEffect->isAlphaCutoffEnabled());
        materialEffect->setEmissiveMapEnabled(clientEffect->isEmissiveMapEnabled());
        materialEffect->setBaseColorMapEnabled(clientEffect->isBaseColorMapEnabled());
        materialEffect->setMetalRoughMapEnabled(clientEffect->isMetalRoughMapEnabled());
        materialEffect->setToneMappingAlgorithm(clientEffect->toneMappingAlgorithm());
        materialEffect->setAmbientOcclusionMapEnabled(clientEffect->isAmbientOcclusionMapEnabled());
    }

    m_materialProperties = materialProperties;

    emit materialPropertiesChanged();
}

Kuesa::MetallicRoughnessProperties *MaterialInspector::materialProperties() const
{
    return m_materialProperties;
}

Kuesa::MetallicRoughnessMaterial *MaterialInspector::material() const
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
    return m_materialProperties ? m_materialProperties->metallicFactor() : 0.0f;
}

float MaterialInspector::roughnessFactor() const
{
    return m_materialProperties ? m_materialProperties->roughnessFactor() : 0.0f;
}

Qt3DRender::QAbstractTexture *MaterialInspector::metalRoughMap() const
{
    return m_materialProperties ? m_materialProperties->metalRoughMap() : nullptr;
}

Qt3DRender::QAbstractTexture *MaterialInspector::normalMap() const
{
    return m_materialProperties ? m_materialProperties->normalMap() : nullptr;
}

float MaterialInspector::normalScale() const
{
    return m_materialProperties ? m_materialProperties->normalScale() : 0.0f;
}

Qt3DRender::QAbstractTexture *MaterialInspector::ambientOcclusionMap() const
{
    return m_materialProperties ? m_materialProperties->ambientOcclusionMap() : nullptr;
}

QColor MaterialInspector::emissiveFactor() const
{
    return m_materialProperties ? m_materialProperties->emissiveFactor() : QColor();
}

Qt3DRender::QAbstractTexture *MaterialInspector::emissiveMap() const
{
    return m_materialProperties ? m_materialProperties->emissiveMap() : nullptr;
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
        return static_cast<Kuesa::MetallicRoughnessEffect *>(effect)->isDoubleSided();
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
        return static_cast<Kuesa::MetallicRoughnessEffect *>(effect)->isOpaque();
    }
    return true;
}

bool MaterialInspector::hasTextures() const
{
    return m_materialProperties && (m_materialProperties->baseColorMap() //
                                    || m_materialProperties->metalRoughMap() //
                                    || m_materialProperties->normalMap() //
                                    || m_materialProperties->emissiveMap() //
                                    || m_materialProperties->ambientOcclusionMap());
}
