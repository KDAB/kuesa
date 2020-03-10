/*
    gltf2materialeffect.h

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

#include "gltf2materialeffect.h"

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::GLTF2MaterialEffect
    \inheaderfile Kuesa/GLTF2MaterialEffect
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief GLTF2MaterialEffect is a subclass of Qt3DRender::QEffect and is the
    base class of all GLTF2 based material effect subclasses.

    It holds properties to specify which features are enabled such as:
    \list
    \li Vertex Skinning
    \li Opaque
    \li Alpha Cut off
    \li Double Sided
    \endlist
*/

/*!
    \property GLTF2MaterialEffect::useSkinning

    Returns whether vertex skinning is enabled or not.

    \since Kuesa 1.2
*/

/*!
    \property GLTF2MaterialEffect::opaque

    Returns whether the content to be draw is opaque or transparent.

    \since Kuesa 1.2
*/

/*!
    \property GLTF2MaterialEffect::alphaCutOffEnabled

    Returns whether alphaCutOffIsEnabled or not.

    \since Kuesa 1.2
*/

/*!
    \property GLTF2MaterialEffect::doubleSided

    Returns whether the meshes to be rendered with this effect are double
    sided.

    \since Kuesa 1.2
*/

/*!
    \qmltype GLTF2MaterialEffect
    \instantiates Kuesa::GLTF2MaterialEffect
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief GLTF2MaterialEffect is a subclass of Qt3DRender::QEffect and is the
    base class of all GLTF2 based material effect subclasses.

    It holds properties to specify which features are enabled such as:
    \list
    \li Vertex Skinning
    \li Opaque
    \li Alpha Cut off
    \li Double Sided
    \endlist
 */

/*!
    \qmlproperty bool GLTF2MaterialEffect::useSkinning

    Returns whether vertex skinning is enabled or not.

    \since Kuesa 1.2
*/

/*!
    \qmlproperty bool GLTF2MaterialEffect::opaque

    Returns whether the content to be draw is opaque or transparent.

    \since Kuesa 1.2
*/

/*!
    \qmlproperty bool GLTF2MaterialEffect::alphaCutOffEnabled

    Returns whether alphaCutOffIsEnabled or not.

    \since Kuesa 1.2
*/

/*!
    \qmlproperty bool GLTF2MaterialEffect::doubleSided

    Returns whether the meshes to be rendered with this effect are double
    sided.

    \since Kuesa 1.2
*/


GLTF2MaterialEffect::GLTF2MaterialEffect(Qt3DCore::QNode *parent)
    : Qt3DRender::QEffect(parent)
    , m_useSkinning(false)
    , m_opaque(true)
    , m_alphaCutoffEnabled(false)
    , m_doubleSided(true)
{
}

GLTF2MaterialEffect::~GLTF2MaterialEffect()
{
}

bool GLTF2MaterialEffect::isDoubleSided() const
{
    return m_doubleSided;
}

bool GLTF2MaterialEffect::useSkinning() const
{
    return m_useSkinning;
}

bool GLTF2MaterialEffect::isOpaque() const
{
    return m_opaque;
}

bool GLTF2MaterialEffect::isAlphaCutoffEnabled() const
{
    return m_alphaCutoffEnabled;
}

void GLTF2MaterialEffect::setDoubleSided(bool doubleSided)
{
    if (doubleSided == m_doubleSided)
        return;
    m_doubleSided = doubleSided;
    emit doubleSidedChanged(doubleSided);
}

void GLTF2MaterialEffect::setUseSkinning(bool useSkinning)
{
    if (useSkinning == m_useSkinning)
        return;
    m_useSkinning = useSkinning;
    emit useSkinningChanged(m_useSkinning);
}

void GLTF2MaterialEffect::setOpaque(bool opaque)
{
    if (opaque == m_opaque)
        return;
    m_opaque = opaque;
    emit opaqueChanged(m_opaque);

    if (opaque)
        setAlphaCutoffEnabled(false);
}

void GLTF2MaterialEffect::setAlphaCutoffEnabled(bool enabled)
{
    if (enabled == m_alphaCutoffEnabled)
        return;
    m_alphaCutoffEnabled = enabled;
    emit alphaCutoffEnabledChanged(m_alphaCutoffEnabled);
}

} // Kuesa

QT_END_NAMESPACE
