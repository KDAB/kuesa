/*
    materialinspector.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Qt3DRender/qabstracttexture.h>

MaterialInspector::MaterialInspector(QObject *parent)
    : QObject(parent)
    , m_material(nullptr)
{
}

MaterialInspector::~MaterialInspector()
{
}

void MaterialInspector::setMaterial(Kuesa::MetallicRoughnessMaterial *material)
{
    if (material == m_material)
        return;

    disconnect(m_materialConnection);
    if (material)
        m_materialConnection = connect(material, &Qt3DCore::QNode::nodeDestroyed, this, [this]() { setMaterial(nullptr); });

    m_material = material;

    emit materialParamsChanged();
}

Kuesa::MetallicRoughnessMaterial *MaterialInspector::material() const
{
    return m_material;
}

QString MaterialInspector::assetName() const
{
    return m_material ? m_material->objectName() : QString();
}

QColor MaterialInspector::baseColor() const
{
    return m_material ? m_material->baseColorFactor() : QColor();
}

Qt3DRender::QAbstractTexture *MaterialInspector::baseColorMap() const
{
    return m_material ? m_material->baseColorMap() : nullptr;
}

float MaterialInspector::metallicFactor() const
{
    return m_material ? m_material->metallicFactor() : 0.0f;
}

float MaterialInspector::roughnessFactor() const
{
    return m_material ? m_material->roughnessFactor() : 0.0f;
}

Qt3DRender::QAbstractTexture *MaterialInspector::metalRoughMap() const
{
    return m_material ? m_material->metalRoughMap() : nullptr;
}

Qt3DRender::QAbstractTexture *MaterialInspector::normalMap() const
{
    return m_material ? m_material->normalMap() : nullptr;
}

float MaterialInspector::normalScale() const
{
    return m_material ? m_material->normalScale() : 0.0f;
}

Qt3DRender::QAbstractTexture *MaterialInspector::ambientOcclusionMap() const
{
    return m_material ? m_material->ambientOcclusionMap() : nullptr;
}

QColor MaterialInspector::emissiveFactor() const
{
    return m_material ? m_material->emissiveFactor() : QColor();
}

Qt3DRender::QAbstractTexture *MaterialInspector::emissiveMap() const
{
    return m_material ? m_material->emissiveMap() : nullptr;
}

QMatrix3x3 MaterialInspector::textureTransform() const
{
    return m_material ? m_material->textureTransform() : QMatrix3x3();
}

bool MaterialInspector::usingColorAttributes() const
{
    return m_material ? m_material->isUsingColorAttribute() : false;
}

bool MaterialInspector::doubleSided() const
{
    return m_material ? m_material->isDoubleSided() : false;
}

bool MaterialInspector::useSkinning() const
{
    return m_material ? m_material->useSkinning() : false;
}

bool MaterialInspector::opaque() const
{
    return m_material ? m_material->isOpaque() : false;
}

bool MaterialInspector::hasTextures() const
{
    return m_material && (m_material->baseColorMap() || m_material->metalRoughMap() || m_material->normalMap() || m_material->emissiveMap() || m_material->ambientOcclusionMap());
}
