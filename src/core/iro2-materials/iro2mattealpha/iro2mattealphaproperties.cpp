/*
    iro2mattealphaproperties.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    This file was auto-generated

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

#include "iro2mattealphaproperties.h"
#include "iro2mattealphashaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/empty2dtexture_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::Iro2MatteAlphaProperties
    \inheaderfile Kuesa/Iro2MatteAlphaProperties
    \inherits Kuesa::GLTF2MaterialProperties
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2MatteAlphaProperties holds the properties controlling the visual appearance of a Iro2MatteAlphaMaterial instance.
*/

/*!
    \qmltype Iro2MatteAlphaProperties
    \instantiates Kuesa::Iro2MatteAlphaProperties
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::Iro2MatteAlphaProperties holds the properties controlling the visual appearance of a Iro2MatteAlphaMaterial instance.
*/

Iro2MatteAlphaProperties::Iro2MatteAlphaProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new Iro2MatteAlphaShaderData(this))
    , m_matteMap(nullptr)
    , m_normalMap(nullptr)
{
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::usesMatteMapChanged, this, &Iro2MatteAlphaProperties::usesMatteMapChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::matteGainChanged, this, &Iro2MatteAlphaProperties::matteGainChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::matteInnerFilterChanged, this, &Iro2MatteAlphaProperties::matteInnerFilterChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::matteOuterFilterChanged, this, &Iro2MatteAlphaProperties::matteOuterFilterChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::uvOffsetChanged, this, &Iro2MatteAlphaProperties::uvOffsetChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::innerAlphaChanged, this, &Iro2MatteAlphaProperties::innerAlphaChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::outerAlphaChanged, this, &Iro2MatteAlphaProperties::outerAlphaChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::alphaGainChanged, this, &Iro2MatteAlphaProperties::alphaGainChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::usesNormalMapChanged, this, &Iro2MatteAlphaProperties::usesNormalMapChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::normalMapGainChanged, this, &Iro2MatteAlphaProperties::normalMapGainChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::normalScalingChanged, this, &Iro2MatteAlphaProperties::normalScalingChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::normalDisturbChanged, this, &Iro2MatteAlphaProperties::normalDisturbChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::postVertexColorChanged, this, &Iro2MatteAlphaProperties::postVertexColorChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::postHemiFilterChanged, this, &Iro2MatteAlphaProperties::postHemiFilterChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::postGainChanged, this, &Iro2MatteAlphaProperties::postGainChanged);
    QObject::connect(m_shaderData, &Iro2MatteAlphaShaderData::gltfYUpChanged, this, &Iro2MatteAlphaProperties::gltfYUpChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setMatteMap(new Empty2DTexture());
    setNormalMap(new Empty2DTexture());
#endif
}

Iro2MatteAlphaProperties::~Iro2MatteAlphaProperties() = default;

Qt3DRender::QShaderData *Iro2MatteAlphaProperties::shaderData() const
{
    return m_shaderData;
}

void Iro2MatteAlphaProperties::setUsesMatteMap(bool usesMatteMap)
{
    m_shaderData->setUsesMatteMap(usesMatteMap);
}

void Iro2MatteAlphaProperties::setMatteGain(float matteGain)
{
    m_shaderData->setMatteGain(matteGain);
}

void Iro2MatteAlphaProperties::setMatteInnerFilter(const QVector3D &matteInnerFilter)
{
    m_shaderData->setMatteInnerFilter(matteInnerFilter);
}

void Iro2MatteAlphaProperties::setMatteOuterFilter(const QVector3D &matteOuterFilter)
{
    m_shaderData->setMatteOuterFilter(matteOuterFilter);
}

void Iro2MatteAlphaProperties::setUvOffset(const QVector2D &uvOffset)
{
    m_shaderData->setUvOffset(uvOffset);
}

void Iro2MatteAlphaProperties::setInnerAlpha(float innerAlpha)
{
    m_shaderData->setInnerAlpha(innerAlpha);
}

void Iro2MatteAlphaProperties::setOuterAlpha(float outerAlpha)
{
    m_shaderData->setOuterAlpha(outerAlpha);
}

void Iro2MatteAlphaProperties::setAlphaGain(float alphaGain)
{
    m_shaderData->setAlphaGain(alphaGain);
}

void Iro2MatteAlphaProperties::setUsesNormalMap(bool usesNormalMap)
{
    m_shaderData->setUsesNormalMap(usesNormalMap);
}

void Iro2MatteAlphaProperties::setNormalMapGain(float normalMapGain)
{
    m_shaderData->setNormalMapGain(normalMapGain);
}

void Iro2MatteAlphaProperties::setNormalScaling(const QVector3D &normalScaling)
{
    m_shaderData->setNormalScaling(normalScaling);
}

void Iro2MatteAlphaProperties::setNormalDisturb(const QVector2D &normalDisturb)
{
    m_shaderData->setNormalDisturb(normalDisturb);
}

void Iro2MatteAlphaProperties::setPostVertexColor(float postVertexColor)
{
    m_shaderData->setPostVertexColor(postVertexColor);
}

void Iro2MatteAlphaProperties::setPostHemiFilter(const QVector3D &postHemiFilter)
{
    m_shaderData->setPostHemiFilter(postHemiFilter);
}

void Iro2MatteAlphaProperties::setPostGain(float postGain)
{
    m_shaderData->setPostGain(postGain);
}

void Iro2MatteAlphaProperties::setGltfYUp(bool gltfYUp)
{
    m_shaderData->setGltfYUp(gltfYUp);
}

void Iro2MatteAlphaProperties::setMatteMap(Qt3DRender::QAbstractTexture *matteMap)
{
    if (m_matteMap == matteMap)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_matteMap != nullptr)
        d->unregisterDestructionHelper(m_matteMap);
    m_matteMap = matteMap;
    if (m_matteMap != nullptr) {
        if (m_matteMap->parent() == nullptr)
            m_matteMap->setParent(this);
        d->registerDestructionHelper(m_matteMap, &Iro2MatteAlphaProperties::setMatteMap, m_matteMap);
    }
    emit matteMapChanged(m_matteMap);
}

void Iro2MatteAlphaProperties::setNormalMap(Qt3DRender::QAbstractTexture *normalMap)
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
        d->registerDestructionHelper(m_normalMap, &Iro2MatteAlphaProperties::setNormalMap, m_normalMap);
    }
    emit normalMapChanged(m_normalMap);
}

/*!
    \qmlproperty bool Iro2MatteAlphaProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
/*!
    \property Iro2MatteAlphaProperties::usesMatteMap
    Specifies whether we use a matte map or not.
*/
bool Iro2MatteAlphaProperties::usesMatteMap() const
{
    return m_shaderData->usesMatteMap();
}

/*!
    \qmlproperty float Iro2MatteAlphaProperties::matteGain
    Specifies the gain to apply to the matte color.
*/
/*!
    \property Iro2MatteAlphaProperties::matteGain
    Specifies the gain to apply to the matte color.
*/
float Iro2MatteAlphaProperties::matteGain() const
{
    return m_shaderData->matteGain();
}

/*!
    \qmlproperty QVector3D Iro2MatteAlphaProperties::matteInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the matte map lookup.
*/
/*!
    \property Iro2MatteAlphaProperties::matteInnerFilter
    Specifies the inner color filter to be applied based on fresnel on the matte map lookup.
*/
QVector3D Iro2MatteAlphaProperties::matteInnerFilter() const
{
    return m_shaderData->matteInnerFilter();
}

/*!
    \qmlproperty QVector3D Iro2MatteAlphaProperties::matteOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the matte map lookup.
*/
/*!
    \property Iro2MatteAlphaProperties::matteOuterFilter
    Specifies the outer color filter to be applied based on fresnel on the matte map lookup.
*/
QVector3D Iro2MatteAlphaProperties::matteOuterFilter() const
{
    return m_shaderData->matteOuterFilter();
}

/*!
    \qmlproperty QVector2D Iro2MatteAlphaProperties::uvOffset
    Applies an offset to texture lookup.
*/
/*!
    \property Iro2MatteAlphaProperties::uvOffset
    Applies an offset to texture lookup.
*/
QVector2D Iro2MatteAlphaProperties::uvOffset() const
{
    return m_shaderData->uvOffset();
}

/*!
    \qmlproperty float Iro2MatteAlphaProperties::innerAlpha
    Specifies the inner alpha factor to be applied.
*/
/*!
    \property Iro2MatteAlphaProperties::innerAlpha
    Specifies the inner alpha factor to be applied.
*/
float Iro2MatteAlphaProperties::innerAlpha() const
{
    return m_shaderData->innerAlpha();
}

/*!
    \qmlproperty float Iro2MatteAlphaProperties::outerAlpha
    Specifies the outer alpha factor to be applied.
*/
/*!
    \property Iro2MatteAlphaProperties::outerAlpha
    Specifies the outer alpha factor to be applied.
*/
float Iro2MatteAlphaProperties::outerAlpha() const
{
    return m_shaderData->outerAlpha();
}

/*!
    \qmlproperty float Iro2MatteAlphaProperties::alphaGain
    Specifies the overall gain factor to apply to the alpha channel.
*/
/*!
    \property Iro2MatteAlphaProperties::alphaGain
    Specifies the overall gain factor to apply to the alpha channel.
*/
float Iro2MatteAlphaProperties::alphaGain() const
{
    return m_shaderData->alphaGain();
}

/*!
    \qmlproperty bool Iro2MatteAlphaProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
/*!
    \property Iro2MatteAlphaProperties::usesNormalMap
    Specifies whether we use a normal map or not.
*/
bool Iro2MatteAlphaProperties::usesNormalMap() const
{
    return m_shaderData->usesNormalMap();
}

/*!
    \qmlproperty float Iro2MatteAlphaProperties::normalMapGain
    Specifies the strength of the normal map.
*/
/*!
    \property Iro2MatteAlphaProperties::normalMapGain
    Specifies the strength of the normal map.
*/
float Iro2MatteAlphaProperties::normalMapGain() const
{
    return m_shaderData->normalMapGain();
}

/*!
    \qmlproperty QVector3D Iro2MatteAlphaProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
/*!
    \property Iro2MatteAlphaProperties::normalScaling
    Specifies a factor to be applied to the normal.
*/
QVector3D Iro2MatteAlphaProperties::normalScaling() const
{
    return m_shaderData->normalScaling();
}

/*!
    \qmlproperty QVector2D Iro2MatteAlphaProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
/*!
    \property Iro2MatteAlphaProperties::normalDisturb
    Specifies a disturbance factor that will be added to the normal
*/
QVector2D Iro2MatteAlphaProperties::normalDisturb() const
{
    return m_shaderData->normalDisturb();
}

/*!
    \qmlproperty float Iro2MatteAlphaProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
/*!
    \property Iro2MatteAlphaProperties::postVertexColor
    Specifies the factor which controls how much of the per vertex color we want to transmit.
*/
float Iro2MatteAlphaProperties::postVertexColor() const
{
    return m_shaderData->postVertexColor();
}

/*!
    \qmlproperty QVector3D Iro2MatteAlphaProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
/*!
    \property Iro2MatteAlphaProperties::postHemiFilter
    Specifies the color to be applied to the lower hemisphere. It is multiplied like a negative diffuse light coming from below.
*/
QVector3D Iro2MatteAlphaProperties::postHemiFilter() const
{
    return m_shaderData->postHemiFilter();
}

/*!
    \qmlproperty float Iro2MatteAlphaProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
/*!
    \property Iro2MatteAlphaProperties::postGain
    Specifies the gain factor to be applied to the final result.
*/
float Iro2MatteAlphaProperties::postGain() const
{
    return m_shaderData->postGain();
}

/*!
    \qmlproperty bool Iro2MatteAlphaProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
/*!
    \property Iro2MatteAlphaProperties::gltfYUp
    Specifies whether this material should consider the Y and Z axis as being inverted.
*/
bool Iro2MatteAlphaProperties::gltfYUp() const
{
    return m_shaderData->gltfYUp();
}

Qt3DRender::QAbstractTexture *Iro2MatteAlphaProperties::matteMap() const
{
    return m_matteMap;
}

Qt3DRender::QAbstractTexture *Iro2MatteAlphaProperties::normalMap() const
{
    return m_normalMap;
}

} // namespace Kuesa

QT_END_NAMESPACE
