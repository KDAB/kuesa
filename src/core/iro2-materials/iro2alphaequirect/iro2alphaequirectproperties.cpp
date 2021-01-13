
/*
    iro2alphaequirectproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Nicolas Guichard <nicolas.guichard@kdab.com>

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

#include "iro2alphaequirectproperties.h"
#include "iro2alphaequirectshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {


/*!
    \class Kuesa::Iro2AlphaEquiRectProperties
    \inheaderfile Kuesa/Iro2AlphaEquiRectProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2AlphaEquiRectProperties holds the properties controlling the visual appearance of a Iro2AlphaEquiRectMaterial instance.
*/

/*!
    \qmltype Iro2AlphaEquiRectProperties
    \instantiates Kuesa::Iro2AlphaEquiRectProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2AlphaEquiRectProperties holds the properties controlling the visual appearance of a Iro2AlphaEquiRectMaterial instance.
*/

Iro2AlphaEquiRectProperties::Iro2AlphaEquiRectProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new Iro2AlphaEquiRectShaderData(this))
    , m_reflectionMap(nullptr)
    , m_normalMap(nullptr)
{
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::usesReflectionMapChanged, this, &Iro2AlphaEquiRectProperties::usesReflectionMapChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::reflectionGainChanged, this, &Iro2AlphaEquiRectProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::reflectionInnerFilterChanged, this, &Iro2AlphaEquiRectProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::reflectionOuterFilterChanged, this, &Iro2AlphaEquiRectProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::innerAlphaChanged, this, &Iro2AlphaEquiRectProperties::innerAlphaChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::outerAlphaChanged, this, &Iro2AlphaEquiRectProperties::outerAlphaChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::alphaGainChanged, this, &Iro2AlphaEquiRectProperties::alphaGainChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::usesNormalMapChanged, this, &Iro2AlphaEquiRectProperties::usesNormalMapChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::normalMapGainChanged, this, &Iro2AlphaEquiRectProperties::normalMapGainChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::normalScalingChanged, this, &Iro2AlphaEquiRectProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::normalDisturbChanged, this, &Iro2AlphaEquiRectProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::postVertexColorChanged, this, &Iro2AlphaEquiRectProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::postHemiFilterChanged, this, &Iro2AlphaEquiRectProperties::postHemiFilterChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::postGainChanged, this, &Iro2AlphaEquiRectProperties::postGainChanged);
    QObject::connect(m_shaderData, &Iro2AlphaEquiRectShaderData::gltfYUpChanged, this, &Iro2AlphaEquiRectProperties::gltfYUpChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setReflectionMap(new Empty2DTexture());
    setNormalMap(new Empty2DTexture());
#endif
}

Iro2AlphaEquiRectProperties::~Iro2AlphaEquiRectProperties() = default;

Qt3DRender::QShaderData *Iro2AlphaEquiRectProperties::shaderData() const
{
    return m_shaderData;
}

void Iro2AlphaEquiRectProperties::setUsesReflectionMap(bool usesReflectionMap)
{
    m_shaderData->setUsesReflectionMap(usesReflectionMap);
}

void Iro2AlphaEquiRectProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void Iro2AlphaEquiRectProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void Iro2AlphaEquiRectProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void Iro2AlphaEquiRectProperties::setInnerAlpha(float innerAlpha)
{
    m_shaderData->setInnerAlpha(innerAlpha);
}

void Iro2AlphaEquiRectProperties::setOuterAlpha(float outerAlpha)
{
    m_shaderData->setOuterAlpha(outerAlpha);
}

void Iro2AlphaEquiRectProperties::setAlphaGain(float alphaGain)
{
    m_shaderData->setAlphaGain(alphaGain);
}

void Iro2AlphaEquiRectProperties::setUsesNormalMap(bool usesNormalMap)
{
    m_shaderData->setUsesNormalMap(usesNormalMap);
}

void Iro2AlphaEquiRectProperties::setNormalMapGain(float normalMapGain)
{
    m_shaderData->setNormalMapGain(normalMapGain);
}

void Iro2AlphaEquiRectProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void Iro2AlphaEquiRectProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void Iro2AlphaEquiRectProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void Iro2AlphaEquiRectProperties::setPostHemiFilter(const QVector3D &postHemiFilter)
{
    m_shaderData->setPostHemiFilter(postHemiFilter);
}

void Iro2AlphaEquiRectProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void Iro2AlphaEquiRectProperties::setGltfYUp(bool gltfYUp)
{
    m_shaderData->setGltfYUp(gltfYUp);
}

void Iro2AlphaEquiRectProperties::setReflectionMap(Qt3DRender::QAbstractTexture * reflectionMap)
{
    if (m_reflectionMap == reflectionMap)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_reflectionMap != nullptr)
        d->unregisterDestructionHelper(m_reflectionMap);
    m_reflectionMap = reflectionMap;
    if (m_reflectionMap != nullptr) {
        if (m_reflectionMap->parent() == nullptr)
            m_reflectionMap->setParent(this);
        d->registerDestructionHelper(m_reflectionMap, &Iro2AlphaEquiRectProperties::setReflectionMap, m_reflectionMap);
    }
    emit reflectionMapChanged(m_reflectionMap);
}

void Iro2AlphaEquiRectProperties::setNormalMap(Qt3DRender::QAbstractTexture * normalMap)
{
    if (m_normalMap == normalMap)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_normalMap != nullptr)
        d->unregisterDestructionHelper(m_normalMap);
    m_normalMap = normalMap;
    if (m_normalMap != nullptr) {
        if (m_normalMap->parent() == nullptr)
            m_normalMap->setParent(this);
        d->registerDestructionHelper(m_normalMap, &Iro2AlphaEquiRectProperties::setNormalMap, m_normalMap);
    }
    emit normalMapChanged(m_normalMap);
}


/*!
    \qmlproperty bool Iro2AlphaEquiRectProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
/*!
    \property Iro2AlphaEquiRectProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
bool Iro2AlphaEquiRectProperties::usesReflectionMap() const
{
    return m_shaderData->usesReflectionMap();
}

/*!
    \qmlproperty float Iro2AlphaEquiRectProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property Iro2AlphaEquiRectProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float Iro2AlphaEquiRectProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty QVector3D Iro2AlphaEquiRectProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2AlphaEquiRectProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2AlphaEquiRectProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2AlphaEquiRectProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2AlphaEquiRectProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2AlphaEquiRectProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty float Iro2AlphaEquiRectProperties::innerAlpha
    Specifies the inner alpha factor to be applied.
*/
/*!
    \property Iro2AlphaEquiRectProperties::innerAlpha
    Specifies the inner alpha factor to be applied.
*/
float Iro2AlphaEquiRectProperties::innerAlpha() const
{
    return m_shaderData->innerAlpha();
}

/*!
    \qmlproperty float Iro2AlphaEquiRectProperties::outerAlpha
    Specifies the outer alpha factor to be applied.
*/
/*!
    \property Iro2AlphaEquiRectProperties::outerAlpha
    Specifies the outer alpha factor to be applied.
*/
float Iro2AlphaEquiRectProperties::outerAlpha() const
{
    return m_shaderData->outerAlpha();
}

/*!
    \qmlproperty float Iro2AlphaEquiRectProperties::alphaGain
    Specifies the overall gain factor to apply to the alpha channel.
*/
/*!
    \property Iro2AlphaEquiRectProperties::alphaGain
    Specifies the overall gain factor to apply to the alpha channel.
*/
float Iro2AlphaEquiRectProperties::alphaGain() const
{
    return m_shaderData->alphaGain();
}

/*!
    \qmlproperty bool Iro2AlphaEquiRectProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
/*!
    \property Iro2AlphaEquiRectProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
bool Iro2AlphaEquiRectProperties::usesNormalMap() const
{
    return m_shaderData->usesNormalMap();
}

/*!
    \qmlproperty float Iro2AlphaEquiRectProperties::normalMapGain
    Specifies the strength of the normal map.
*/
/*!
    \property Iro2AlphaEquiRectProperties::normalMapGain
    Specifies the strength of the normal map.
*/
float Iro2AlphaEquiRectProperties::normalMapGain() const
{
    return m_shaderData->normalMapGain();
}

/*!
    \qmlproperty QVector3D Iro2AlphaEquiRectProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property Iro2AlphaEquiRectProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D Iro2AlphaEquiRectProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D Iro2AlphaEquiRectProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property Iro2AlphaEquiRectProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D Iro2AlphaEquiRectProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float Iro2AlphaEquiRectProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property Iro2AlphaEquiRectProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float Iro2AlphaEquiRectProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty QVector3D Iro2AlphaEquiRectProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
/*!
    \property Iro2AlphaEquiRectProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
QVector3D Iro2AlphaEquiRectProperties::postHemiFilter() const
{
    return m_shaderData->postHemiFilter();
}

/*!
    \qmlproperty float Iro2AlphaEquiRectProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property Iro2AlphaEquiRectProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float Iro2AlphaEquiRectProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool Iro2AlphaEquiRectProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
/*!
    \property Iro2AlphaEquiRectProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
bool Iro2AlphaEquiRectProperties::gltfYUp() const
{
    return m_shaderData->gltfYUp();
}

Qt3DRender::QAbstractTexture * Iro2AlphaEquiRectProperties::reflectionMap() const
{
    return m_reflectionMap;
}

Qt3DRender::QAbstractTexture * Iro2AlphaEquiRectProperties::normalMap() const
{
    return m_normalMap;
}


} // namespace Kuesa

QT_END_NAMESPACE
