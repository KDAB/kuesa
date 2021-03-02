
/*
    iro2alphasemproperties.cpp

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

#include "iro2alphasemproperties.h"
#include "iro2alphasemshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::Iro2AlphaSemProperties
    \inheaderfile Kuesa/Iro2AlphaSemProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2AlphaSemProperties holds the properties controlling the visual appearance of a Iro2AlphaSemMaterial instance.
*/

/*!
    \qmltype Iro2AlphaSemProperties
    \instantiates Kuesa::Iro2AlphaSemProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2AlphaSemProperties holds the properties controlling the visual appearance of a Iro2AlphaSemMaterial instance.
*/

Iro2AlphaSemProperties::Iro2AlphaSemProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new Iro2AlphaSemShaderData(this))
    , m_reflectionMap(nullptr)
    , m_normalMap(nullptr)
{
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::usesReflectionMapChanged, this, &Iro2AlphaSemProperties::usesReflectionMapChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::reflectionGainChanged, this, &Iro2AlphaSemProperties::reflectionGainChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::reflectionInnerFilterChanged, this, &Iro2AlphaSemProperties::reflectionInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::reflectionOuterFilterChanged, this, &Iro2AlphaSemProperties::reflectionOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::innerAlphaChanged, this, &Iro2AlphaSemProperties::innerAlphaChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::outerAlphaChanged, this, &Iro2AlphaSemProperties::outerAlphaChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::alphaGainChanged, this, &Iro2AlphaSemProperties::alphaGainChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::usesNormalMapChanged, this, &Iro2AlphaSemProperties::usesNormalMapChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::normalMapGainChanged, this, &Iro2AlphaSemProperties::normalMapGainChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::normalScalingChanged, this, &Iro2AlphaSemProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::normalDisturbChanged, this, &Iro2AlphaSemProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::postVertexColorChanged, this, &Iro2AlphaSemProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::postHemiFilterChanged, this, &Iro2AlphaSemProperties::postHemiFilterChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::postGainChanged, this, &Iro2AlphaSemProperties::postGainChanged);
    QObject::connect(m_shaderData, &Iro2AlphaSemShaderData::gltfYUpChanged, this, &Iro2AlphaSemProperties::gltfYUpChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setReflectionMap(new Empty2DTexture());
    setNormalMap(new Empty2DTexture());
#endif
}

Iro2AlphaSemProperties::~Iro2AlphaSemProperties() = default;

Qt3DRender::QShaderData *Iro2AlphaSemProperties::shaderData() const
{
    return m_shaderData;
}

void Iro2AlphaSemProperties::setUsesReflectionMap(bool usesReflectionMap)
{
    m_shaderData->setUsesReflectionMap(usesReflectionMap);
}

void Iro2AlphaSemProperties::setReflectionGain(float reflectionGain)
{
    m_shaderData->setReflectionGain(reflectionGain);
}

void Iro2AlphaSemProperties::setReflectionInnerFilter(const QVector3D &reflectionInnerFilter)
{
    m_shaderData->setReflectionInnerFilter(reflectionInnerFilter);
}

void Iro2AlphaSemProperties::setReflectionOuterFilter(const QVector3D &reflectionOuterFilter)
{
    m_shaderData->setReflectionOuterFilter(reflectionOuterFilter);
}

void Iro2AlphaSemProperties::setInnerAlpha(float innerAlpha)
{
    m_shaderData->setInnerAlpha(innerAlpha);
}

void Iro2AlphaSemProperties::setOuterAlpha(float outerAlpha)
{
    m_shaderData->setOuterAlpha(outerAlpha);
}

void Iro2AlphaSemProperties::setAlphaGain(float alphaGain)
{
    m_shaderData->setAlphaGain(alphaGain);
}

void Iro2AlphaSemProperties::setUsesNormalMap(bool usesNormalMap)
{
    m_shaderData->setUsesNormalMap(usesNormalMap);
}

void Iro2AlphaSemProperties::setNormalMapGain(float normalMapGain)
{
    m_shaderData->setNormalMapGain(normalMapGain);
}

void Iro2AlphaSemProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void Iro2AlphaSemProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void Iro2AlphaSemProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void Iro2AlphaSemProperties::setPostHemiFilter(const QVector3D &postHemiFilter)
{
    m_shaderData->setPostHemiFilter(postHemiFilter);
}

void Iro2AlphaSemProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void Iro2AlphaSemProperties::setGltfYUp(bool gltfYUp)
{
    m_shaderData->setGltfYUp(gltfYUp);
}

void Iro2AlphaSemProperties::setReflectionMap(Qt3DRender::QAbstractTexture *reflectionMap)
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
        d->registerDestructionHelper(m_reflectionMap, &Iro2AlphaSemProperties::setReflectionMap, m_reflectionMap);
    }
    emit reflectionMapChanged(m_reflectionMap);
}

void Iro2AlphaSemProperties::setNormalMap(Qt3DRender::QAbstractTexture *normalMap)
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
        d->registerDestructionHelper(m_normalMap, &Iro2AlphaSemProperties::setNormalMap, m_normalMap);
    }
    emit normalMapChanged(m_normalMap);
}

/*!
    \qmlproperty bool Iro2AlphaSemProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
/*!
    \property Iro2AlphaSemProperties::usesReflectionMap
    Specifies whether we use a reflection map or not.
*/
bool Iro2AlphaSemProperties::usesReflectionMap() const
{
    return m_shaderData->usesReflectionMap();
}

/*!
    \qmlproperty float Iro2AlphaSemProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
/*!
    \property Iro2AlphaSemProperties::reflectionGain
    Specifies the gain factor to be applied to the spherical environment map lookup.
*/
float Iro2AlphaSemProperties::reflectionGain() const
{
    return m_shaderData->reflectionGain();
}

/*!
    \qmlproperty QVector3D Iro2AlphaSemProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2AlphaSemProperties::reflectionInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2AlphaSemProperties::reflectionInnerFilter() const
{
    return m_shaderData->reflectionInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2AlphaSemProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
/*!
    \property Iro2AlphaSemProperties::reflectionOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the spherical environment map lookup.
*/
QVector3D Iro2AlphaSemProperties::reflectionOuterFilter() const
{
    return m_shaderData->reflectionOuterFilter();
}

/*!
    \qmlproperty float Iro2AlphaSemProperties::innerAlpha
    Specifies the inner alpha factor to be applied.
*/
/*!
    \property Iro2AlphaSemProperties::innerAlpha
    Specifies the inner alpha factor to be applied.
*/
float Iro2AlphaSemProperties::innerAlpha() const
{
    return m_shaderData->innerAlpha();
}

/*!
    \qmlproperty float Iro2AlphaSemProperties::outerAlpha
    Specifies the outer alpha factor to be applied.
*/
/*!
    \property Iro2AlphaSemProperties::outerAlpha
    Specifies the outer alpha factor to be applied.
*/
float Iro2AlphaSemProperties::outerAlpha() const
{
    return m_shaderData->outerAlpha();
}

/*!
    \qmlproperty float Iro2AlphaSemProperties::alphaGain
    Specifies the overall gain factor to apply to the alpha channel.
*/
/*!
    \property Iro2AlphaSemProperties::alphaGain
    Specifies the overall gain factor to apply to the alpha channel.
*/
float Iro2AlphaSemProperties::alphaGain() const
{
    return m_shaderData->alphaGain();
}

/*!
    \qmlproperty bool Iro2AlphaSemProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
/*!
    \property Iro2AlphaSemProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
bool Iro2AlphaSemProperties::usesNormalMap() const
{
    return m_shaderData->usesNormalMap();
}

/*!
    \qmlproperty float Iro2AlphaSemProperties::normalMapGain
    Specifies the strength of the normal map.
*/
/*!
    \property Iro2AlphaSemProperties::normalMapGain
    Specifies the strength of the normal map.
*/
float Iro2AlphaSemProperties::normalMapGain() const
{
    return m_shaderData->normalMapGain();
}

/*!
    \qmlproperty QVector3D Iro2AlphaSemProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property Iro2AlphaSemProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D Iro2AlphaSemProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D Iro2AlphaSemProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property Iro2AlphaSemProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D Iro2AlphaSemProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float Iro2AlphaSemProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property Iro2AlphaSemProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float Iro2AlphaSemProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty QVector3D Iro2AlphaSemProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
/*!
    \property Iro2AlphaSemProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
QVector3D Iro2AlphaSemProperties::postHemiFilter() const
{
    return m_shaderData->postHemiFilter();
}

/*!
    \qmlproperty float Iro2AlphaSemProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property Iro2AlphaSemProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float Iro2AlphaSemProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool Iro2AlphaSemProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
/*!
    \property Iro2AlphaSemProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
bool Iro2AlphaSemProperties::gltfYUp() const
{
    return m_shaderData->gltfYUp();
}

Qt3DRender::QAbstractTexture *Iro2AlphaSemProperties::reflectionMap() const
{
    return m_reflectionMap;
}

Qt3DRender::QAbstractTexture *Iro2AlphaSemProperties::normalMap() const
{
    return m_normalMap;
}

} // namespace Kuesa

QT_END_NAMESPACE
