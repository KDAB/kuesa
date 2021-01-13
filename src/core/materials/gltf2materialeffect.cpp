/*
    gltf2materialeffect.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Kuesa/private/framegraphutils_p.h>
#include <QTimer>

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
    \li Vertex Attributes provides by the mesh
    \endlist
*/

/*!
    \property GLTF2MaterialEffect::usingSkinning

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
    \property GLTF2MaterialEffect::usingColorAttribute

    If true, the base color property is multiplied by the color interpolated
    attribute of the mesh

    \since Kuesa 1.3
 */

/*!
    \property GLTF2MaterialEffect::usingNormalAttribute

    If true, the normal attribute provided by the mesh is used.

    \since Kuesa 1.3
 */

/*!
    \property GLTF2MaterialEffect::usingTangentAttribute

    If true, the tangent attribute provided by the mesh is used.

    \since Kuesa 1.3
 */

/*!
    \property GLTF2MaterialEffect::usingTexCoordAttribute

    If true, the texCoord attribute provided by the mesh is used.

    \since Kuesa 1.3
 */

/*!
    \property GLTF2MaterialEffect::usingTexCoord1Attribute

    If true, the texCoord1 attribute provided by the mesh is used.

    \since Kuesa 1.3
 */

/*!
    \property GLTF2MaterialEffect::usingMorphTargets

    If true, the morphtarget support is enabled.

    \since Kuesa 1.3
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
    \li Vertex Attributes provides by the mesh
    \endlist
 */

/*!
    \qmlproperty bool GLTF2MaterialEffect::usingSkinning

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

/*!
    \qmlproperty bool GLTF2MaterialEffect::usingColorAttribute

    If true, the base color property is multiplied by the color interpolated
    attribute of the mesh

    \since Kuesa 1.3
 */

/*!
    \qmlproperty bool GLTF2MaterialEffect::usingNormalAttribute

    If true, the normal attribute provided by the mesh is used.

    \since Kuesa 1.3
 */

/*!
    \qmlproperty bool GLTF2MaterialEffect::usingTangentAttribute

    If true, the tangent attribute provided by the mesh is used.

    \since Kuesa 1.3
 */

/*!
    \qmlproperty bool GLTF2MaterialEffect::usingTexCoordAttribute

    If true, the texCoord attribute provided by the mesh is used.

    \since Kuesa 1.3
 */

/*!
    \qmlproperty bool GLTF2MaterialEffect::usingTexCoord1Attribute

    If true, the texCoord1 attribute provided by the mesh is used.

    \since Kuesa 1.3
 */

/*!
    \qmlproperty bool GLTF2MaterialEffect::usingMorphTargets

    If true, the morphtarget support is enabled.

    \since Kuesa 1.3
 */

GLTF2MaterialEffect::GLTF2MaterialEffect(Qt3DCore::QNode *parent)
    : Qt3DRender::QEffect(parent)
    , m_usingSkinning(false)
    , m_opaque(true)
    , m_alphaCutoffEnabled(false)
    , m_doubleSided(false)
    , m_usingColorAttribute(false)
    , m_usingNormalAttribute(false)
    , m_usingTangentAttribute(false)
    , m_usingTexCoordAttribute(false)
    , m_usingTexCoord1Attribute(false)
    , m_usingMorphTargets(false)
    , m_instanced(false)
{
    // Call the update methods once the subclass instances has been created
    QTimer::singleShot(0, this, &GLTF2MaterialEffect::initialize);
}

GLTF2MaterialEffect::~GLTF2MaterialEffect()
{
}

bool GLTF2MaterialEffect::isDoubleSided() const
{
    return m_doubleSided;
}

bool GLTF2MaterialEffect::isUsingSkinning() const
{
    return m_usingSkinning;
}

bool GLTF2MaterialEffect::isOpaque() const
{
    return m_opaque;
}

bool GLTF2MaterialEffect::isAlphaCutoffEnabled() const
{
    return m_alphaCutoffEnabled;
}

bool GLTF2MaterialEffect::isUsingColorAttribute() const
{
    return m_usingColorAttribute;
}

bool GLTF2MaterialEffect::isUsingNormalAttribute() const
{
    return m_usingNormalAttribute;
}

bool GLTF2MaterialEffect::isUsingTangentAttribute() const
{
    return m_usingTangentAttribute;
}

bool GLTF2MaterialEffect::isUsingTexCoordAttribute() const
{
    return m_usingTexCoordAttribute;
}

bool GLTF2MaterialEffect::isUsingTexCoord1Attribute() const
{
    return m_usingTexCoord1Attribute;
}

bool GLTF2MaterialEffect::isUsingMorphTargets() const
{
    return m_usingMorphTargets;
}

bool GLTF2MaterialEffect::isInstanced() const
{
    return m_instanced;
}

void GLTF2MaterialEffect::setDoubleSided(bool doubleSided)
{
    if (doubleSided == m_doubleSided)
        return;
    m_doubleSided = doubleSided;
    emit doubleSidedChanged(doubleSided);

    updateDoubleSided(m_doubleSided);
}

void GLTF2MaterialEffect::setUsingSkinning(bool useSkinning)
{
    if (useSkinning == m_usingSkinning)
        return;
    m_usingSkinning = useSkinning;
    emit usingSkinningChanged(m_usingSkinning);

    updateUsingSkinning(m_usingSkinning);
}

void GLTF2MaterialEffect::setOpaque(bool opaque)
{
    if (opaque == m_opaque)
        return;
    m_opaque = opaque;
    emit opaqueChanged(m_opaque);

    if (opaque)
        setAlphaCutoffEnabled(false);

    updateOpaque(m_opaque);
}

void GLTF2MaterialEffect::setAlphaCutoffEnabled(bool enabled)
{
    if (enabled == m_alphaCutoffEnabled)
        return;
    m_alphaCutoffEnabled = enabled;
    emit alphaCutoffEnabledChanged(m_alphaCutoffEnabled);

    updateAlphaCutoffEnabled(m_alphaCutoffEnabled);
}

void GLTF2MaterialEffect::setUsingColorAttribute(bool usingColorAttribute)
{
    if (m_usingColorAttribute == usingColorAttribute)
        return;

    m_usingColorAttribute = usingColorAttribute;
    emit usingColorAttributeChanged(usingColorAttribute);

    updateUsingColorAttribute(m_usingColorAttribute);
}

void GLTF2MaterialEffect::setUsingNormalAttribute(bool usingNormalAttribute)
{
    if (m_usingNormalAttribute == usingNormalAttribute)
        return;
    m_usingNormalAttribute = usingNormalAttribute;
    emit usingNormalAttributeChanged(usingNormalAttribute);

    updateUsingNormalAttribute(m_usingNormalAttribute);
}

void GLTF2MaterialEffect::setUsingTangentAttribute(bool usingTangentAttribute)
{
    if (m_usingTangentAttribute == usingTangentAttribute)
        return;
    m_usingTangentAttribute = usingTangentAttribute;
    emit usingTangentAttributeChanged(usingTangentAttribute);

    updateUsingTangentAttribute(usingTangentAttribute);
}

void GLTF2MaterialEffect::setUsingTexCoordAttribute(bool usingTexCoordAttribute)
{
    if (m_usingTexCoordAttribute == usingTexCoordAttribute)
        return;
    m_usingTexCoordAttribute = usingTexCoordAttribute;
    emit usingTexCoordAttributeChanged(usingTexCoordAttribute);

    updateUsingTexCoordAttribute(m_usingTexCoordAttribute);
}

void GLTF2MaterialEffect::setUsingTexCoord1Attribute(bool usingTexCoord1Attribute)
{
    if (m_usingTexCoord1Attribute == usingTexCoord1Attribute)
        return;
    m_usingTexCoord1Attribute = usingTexCoord1Attribute;
    emit usingTexCoord1AttributeChanged(usingTexCoord1Attribute);

    updateUsingTexCoord1Attribute(m_usingTexCoord1Attribute);
}

void GLTF2MaterialEffect::setUsingMorphTargets(bool usingMorphTargets)
{
    if (m_usingMorphTargets == usingMorphTargets)
        return;
    m_usingMorphTargets = usingMorphTargets;
    emit usingMorphTargetsChanged(usingMorphTargets);

    updateUsingMorphTargets(m_usingMorphTargets);
}

void GLTF2MaterialEffect::updateUsingCubeMapArrays(bool usingCubeMapArrays)
{
    Q_UNUSED(usingCubeMapArrays);
}

void GLTF2MaterialEffect::setInstanced(bool instanced)
{
    if (m_instanced == instanced)
        return;
    m_instanced = instanced;
    emit instancedChanged(instanced);

    updateInstanced(instanced);
}

void GLTF2MaterialEffect::initialize()
{
    updateOpaque(GLTF2MaterialEffect::isOpaque());
    updateUsingSkinning(GLTF2MaterialEffect::isUsingSkinning());
    updateDoubleSided(GLTF2MaterialEffect::isDoubleSided());
    updateAlphaCutoffEnabled(GLTF2MaterialEffect::isAlphaCutoffEnabled());
    updateUsingColorAttribute(GLTF2MaterialEffect::isUsingColorAttribute());
    updateUsingNormalAttribute(GLTF2MaterialEffect::isUsingNormalAttribute());
    updateUsingTangentAttribute(GLTF2MaterialEffect::isUsingTangentAttribute());
    updateUsingTexCoordAttribute(GLTF2MaterialEffect::isUsingTexCoordAttribute());
    updateUsingTexCoord1Attribute(GLTF2MaterialEffect::isUsingTexCoord1Attribute());
    updateUsingMorphTargets(GLTF2MaterialEffect::isUsingMorphTargets());
    // whether to use cubeMapArrays is only set once at startup and does change
    updateUsingCubeMapArrays(FrameGraphUtils::hasCubeMapArrayTextures());
    updateInstanced(GLTF2MaterialEffect::isInstanced());
}

} // Kuesa

QT_END_NAMESPACE
