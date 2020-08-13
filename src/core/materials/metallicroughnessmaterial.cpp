/*
    metallicroughnessmaterial.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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

#include "metallicroughnessmaterial.h"
#include "kuesa_p.h"

#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qparameter.h>

#include <type_traits>

#include "metallicroughnessproperties.h"
#include "metallicroughnessshaderdata_p.h"

#include "sceneentity.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::MetallicRoughnessMaterial
    \inheaderfile Kuesa/MetallicRoughnessMaterial
    \inmodule Kuesa
    \since Kuesa 1.0

    \brief Kuesa::MetallicRoughnessMaterial is a ready to use physically based
    rendering (PBR) material based on the glTF 2.0 material description.

    It is configured through the set of properties held on the
    MetallicRoughnessProperties instance it can receive.

    \note Kuesa::MetallicRoughnessMaterial is implemented using a custom shader
    graph. Some changes in the properties will activate or deactivate some
    nodes of the shader graph and will trigger a recompilation of the shader.
    It is recommended to create the variations of the material that you need
    once up front at application initialisation time.

    \note The effect behind this material needs a brdfLut texture. If the brdfLut
    texture in the effect is not set and the material is added to a subtree of a
    Kuesa::SceneEntity, it will look for the Kuesa::SceneEntity and will use the
    brdfLut texture in the texture collection.

    \sa Kuesa::MetallicRoughnessProperties
*/

/*!
    \property MetallicRoughnessMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

/*!
    \qmltype MetallicRoughnessMaterial
    \instantiates Kuesa::MetallicRoughnessMaterial
    \inqmlmodule Kuesa
    \since Kuesa 1.0

    \brief Kuesa.MetallicRoughnessMaterial is a ready to use physically based
    rendering (PBR) material based on the glTF 2.0 material description.

    It is configured through the set of properties held on the
    MetallicRoughnessProperties instance it can receive.

    \note Kuesa.MetallicRoughnessMaterial is implemented using a custom shader
    graph. Some changes in the properties will activate or deactivate some
    nodes of the shader graph and will trigger a recompilation of the shader.
    It is recommended to create the variations of the material that you need
    once up front at application initialisation time.

    \note The effect behind this material needs a brdfLut texture. If the brdfLut
    texture in the effect is not set and the material is added to a subtree of a
    Kuesa.SceneEntity, it will look for the Kuesa.SceneEntity and will use the
    brdfLut texture in the texture collection.

    \sa Kuesa::MetallicRoughnessProperties
 */

/*!
    \qmlproperty MetallicRoughnessProperties MetallicRoughnessMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */


MetallicRoughnessMaterial::MetallicRoughnessMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent)
    , m_materialProperties(nullptr)
    , m_metallicRoughnessShaderDataParameter(new Qt3DRender::QParameter(QStringLiteral("metallicRoughness"), {}))
    , m_baseColorMapParameter(new Qt3DRender::QParameter(QStringLiteral("baseColorMap"), {}))
    , m_normalMapParameter(new Qt3DRender::QParameter(QStringLiteral("normalMap"), {}))
    , m_emissiveMapParameter(new Qt3DRender::QParameter(QStringLiteral("emissiveMap"), {}))
    , m_ambientOcclusionMapParameter(new Qt3DRender::QParameter(QStringLiteral("ambientOcclusionMap"), {}))
    , m_metalRoughMapParameter(new Qt3DRender::QParameter(QStringLiteral("metalRoughMap"), {}))

{
    addParameter(m_metallicRoughnessShaderDataParameter);
    addParameter(m_baseColorMapParameter);
    addParameter(m_normalMapParameter);
    addParameter(m_emissiveMapParameter);
    addParameter(m_ambientOcclusionMapParameter);
    addParameter(m_metalRoughMapParameter);
}

MetallicRoughnessMaterial::~MetallicRoughnessMaterial()
{
}

MetallicRoughnessProperties *MetallicRoughnessMaterial::materialProperties() const
{
    return m_materialProperties;
}

void MetallicRoughnessMaterial::setMaterialProperties(MetallicRoughnessProperties *metallicRoughnessProperties)
{
    if (m_materialProperties != metallicRoughnessProperties) {
        if (m_materialProperties)
            m_materialProperties->disconnect(this);

        m_materialProperties = metallicRoughnessProperties;
        emit materialPropertiesChanged(metallicRoughnessProperties);

        if (m_materialProperties) {
            QObject::connect(m_materialProperties, &MetallicRoughnessProperties::baseColorMapChanged,
                             this, [this](Qt3DRender::QAbstractTexture *t) { m_baseColorMapParameter->setValue(QVariant::fromValue(t)); });
            QObject::connect(m_materialProperties, &MetallicRoughnessProperties::emissiveMapChanged,
                             this, [this](Qt3DRender::QAbstractTexture *t) { m_emissiveMapParameter->setValue(QVariant::fromValue(t)); });
            QObject::connect(m_materialProperties, &MetallicRoughnessProperties::normalMapChanged,
                             this, [this](Qt3DRender::QAbstractTexture *t) { m_normalMapParameter->setValue(QVariant::fromValue(t)); });
            QObject::connect(m_materialProperties, &MetallicRoughnessProperties::metalRoughMapChanged,
                             this, [this](Qt3DRender::QAbstractTexture *t) { m_metalRoughMapParameter->setValue(QVariant::fromValue(t)); });
            QObject::connect(m_materialProperties, &MetallicRoughnessProperties::ambientOcclusionMapChanged,
                             this, [this](Qt3DRender::QAbstractTexture *t) { m_ambientOcclusionMapParameter->setValue(QVariant::fromValue(t)); });

            // Enforce sRGB for baseColorMap and emissiveMap
            QObject::connect(m_materialProperties, &MetallicRoughnessProperties::baseColorMapChanged, this, &MetallicRoughnessMaterial::enforceSRGBOnTexture);
            QObject::connect(m_materialProperties, &MetallicRoughnessProperties::emissiveMapChanged, this, &MetallicRoughnessMaterial::enforceSRGBOnTexture);

            m_baseColorMapParameter->setValue(QVariant::fromValue(m_materialProperties->baseColorMap()));
            m_emissiveMapParameter->setValue(QVariant::fromValue(m_materialProperties->emissiveMap()));
            m_normalMapParameter->setValue(QVariant::fromValue(m_materialProperties->normalMap()));
            m_metalRoughMapParameter->setValue(QVariant::fromValue(m_materialProperties->metalRoughMap()));
            m_ambientOcclusionMapParameter->setValue(QVariant::fromValue(m_materialProperties->ambientOcclusionMap()));

            enforceSRGBOnTexture(m_materialProperties->baseColorMap());
            enforceSRGBOnTexture(m_materialProperties->emissiveMap());

            m_metallicRoughnessShaderDataParameter->setValue(QVariant::fromValue(metallicRoughnessProperties->shaderData()));
            metallicRoughnessProperties->addClientMaterial(this);
        }
    }
}

void MetallicRoughnessMaterial::enforceSRGBOnTexture(QAbstractTexture *t) const
{
    if (!t)
        return;
#ifndef QT_OPENGL_ES_2
    const bool isSRGB = true;
#else
    const bool isSRGB = false;
#endif
    if (isSRGB)
        t->setFormat(Qt3DRender::QAbstractTexture::TextureFormat::SRGB8_Alpha8);
}

} // namespace Kuesa

QT_END_NAMESPACE
