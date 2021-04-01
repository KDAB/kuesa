/*
    iroglassaddeffect.cpp

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

#include "iroglassaddeffect.h"

#include <Qt3DRender/QCullFace>

#include "iroglassaddtechnique_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
    \class Kuesa::IroGlassAddEffect
    \inheaderfile Kuesa/IroGlassAddEffect
    \inherits Qt3DRender::QEffect
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroGlassAddEffect is the effect for the IroGlassAddMaterial class.
*/

/*!
    \qmltype IroGlassAddEffect
    \instantiates Kuesa::IroGlassAddEffect
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroGlassAddEffect is the effect for the IroGlassAddMaterial class.
*/

IroGlassAddEffect::IroGlassAddEffect(Qt3DCore::QNode *parent)
    : GLTF2MaterialEffect(parent)
{
    m_gl3Technique = new IroGlassAddTechnique(IroGlassAddTechnique::GL3, this);
    m_es3Technique = new IroGlassAddTechnique(IroGlassAddTechnique::ES3, this);
    m_es2Technique = new IroGlassAddTechnique(IroGlassAddTechnique::ES2, this);

    addTechnique(m_gl3Technique);
    addTechnique(m_es3Technique);
    addTechnique(m_es2Technique);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_rhiTechnique = new IroGlassAddTechnique(IroGlassAddTechnique::RHI, this);
    addTechnique(m_rhiTechnique);
#endif
}

IroGlassAddEffect::~IroGlassAddEffect() = default;

void IroGlassAddEffect::updateDoubleSided(bool doubleSided)
{
    const auto cullingMode = doubleSided ? QCullFace::NoCulling : QCullFace::Back;
    m_gl3Technique->setCullingMode(cullingMode);
    m_es3Technique->setCullingMode(cullingMode);
    m_es2Technique->setCullingMode(cullingMode);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_rhiTechnique->setCullingMode(cullingMode);
#endif
}

void IroGlassAddEffect::updateUsingSkinning(bool useSkinning)
{
    // Set Layers on zFill and opaque/Transparent shader builders
    auto layers = m_gl3Technique->enabledLayers();
    if (useSkinning) {
        layers.removeAll(QStringLiteral("no-skinning"));
        layers.append(QStringLiteral("skinning"));
    } else {
        layers.removeAll(QStringLiteral("skinning"));
        layers.append(QStringLiteral("no-skinning"));
    }

    updateLayersOnTechniques(layers);

    m_gl3Technique->setAllowCulling(!useSkinning);
    m_es3Technique->setAllowCulling(!useSkinning);
    m_es2Technique->setAllowCulling(!useSkinning);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_rhiTechnique->setAllowCulling(!useSkinning);
#endif
}

void IroGlassAddEffect::updateOpaque(bool opaque)
{
    m_gl3Technique->setOpaque(opaque);
    m_es3Technique->setOpaque(opaque);
    m_es2Technique->setOpaque(opaque);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_rhiTechnique->setOpaque(opaque);
#endif
}

void IroGlassAddEffect::updateAlphaCutoffEnabled(bool enabled)
{
    auto layers = m_gl3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noHasAlphaCutoff"));
        layers.append(QStringLiteral("hasAlphaCutoff"));
    } else {
        layers.removeAll(QStringLiteral("hasAlphaCutoff"));
        layers.append(QStringLiteral("noHasAlphaCutoff"));
    }
    updateLayersOnTechniques(layers);
}

void IroGlassAddEffect::updateUsingColorAttribute(bool usingColorAttribute)
{
    auto layers = m_gl3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("noHasColorAttr"));
    layers.removeAll(QStringLiteral("hasColorAttr"));
    layers.removeAll(QStringLiteral("hasVertexColor"));
    if (usingColorAttribute) {
        layers.append(QStringLiteral("hasColorAttr"));
        layers.append(QStringLiteral("hasVertexColor"));
    } else {
        layers.append(QStringLiteral("noHasColorAttr"));
    }
    updateLayersOnTechniques(layers);
}

void IroGlassAddEffect::updateUsingNormalAttribute(bool usingNormalAttribute)
{
    auto layers = m_gl3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasVertexNormal"));
    if (usingNormalAttribute)
        layers.append(QStringLiteral("hasVertexNormal"));

    updateLayersOnTechniques(layers);
}

void IroGlassAddEffect::updateUsingTangentAttribute(bool usingTangentAttribute)
{
    auto layers = m_gl3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasVertexTangent"));
    if (usingTangentAttribute)
        layers.append(QStringLiteral("hasVertexTangent"));

    updateLayersOnTechniques(layers);
}

void IroGlassAddEffect::updateUsingTexCoordAttribute(bool usingTexCoordAttribute)
{
    auto layers = m_gl3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasTexCoord"));
    if (usingTexCoordAttribute)
        layers.append(QStringLiteral("hasTexCoord"));

    updateLayersOnTechniques(layers);
}

void IroGlassAddEffect::updateUsingTexCoord1Attribute(bool usingTexCoord1Attribute)
{
    auto layers = m_gl3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasTexCoord1"));
    if (usingTexCoord1Attribute)
        layers.append(QStringLiteral("hasTexCoord1"));

    updateLayersOnTechniques(layers);
}

void IroGlassAddEffect::updateUsingMorphTargets(bool usingMorphTargets)
{
    auto layers = m_gl3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("morphtargets"));
    if (usingMorphTargets)
        layers.append(QStringLiteral("morphtargets"));

    updateLayersOnTechniques(layers);
}

void IroGlassAddEffect::updateInstanced(bool instanced)
{
    auto layers = m_gl3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("instanced"));
    if (instanced)
        layers.append(QStringLiteral("instanced"));

    updateLayersOnTechniques(layers);
}

void IroGlassAddEffect::updateLayersOnTechniques(const QStringList &layers)
{
    m_gl3Technique->setEnabledLayers(layers);
    m_es3Technique->setEnabledLayers(layers);
    m_es2Technique->setEnabledLayers(layers);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    m_rhiTechnique->setEnabledLayers(layers);
#endif
}

} // namespace Kuesa

QT_END_NAMESPACE
