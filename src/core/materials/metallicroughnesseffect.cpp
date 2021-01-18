/*
    metallicroughnesseffect.cpp

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

#include "metallicroughnesseffect.h"

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qnodepthmask.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <private/framegraphutils_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

MetallicRoughnessTechnique::MetallicRoughnessTechnique(Version version, Qt3DCore::QNode *parent)
    : QTechnique(parent)
    , m_backFaceCulling(new QCullFace(this))
    , m_blendEquation(new Qt3DRender::QBlendEquation(this))
    , m_blendArguments(new Qt3DRender::QBlendEquationArguments(this))
    , m_metalRoughShaderBuilder(new QShaderProgramBuilder(this))
    , m_zfillShaderBuilder(new QShaderProgramBuilder(this))
    , m_cubeMapShadowShaderBuilder(new QShaderProgramBuilder(this))
    , m_metalRoughShader(new QShaderProgram(this))
    , m_zfillShader(new QShaderProgram(this))
    , m_zfillRenderPass(new QRenderPass(this))
    , m_opaqueRenderPass(new QRenderPass(this))
    , m_transparentRenderPass(new QRenderPass(this))
    , m_cubeMapShadowRenderPass(new QRenderPass(this))
    , m_techniqueAllowFrustumCullingFilterKey(new QFilterKey(this))
{
    struct ApiFilterInfo {
        int major;
        int minor;
        QGraphicsApiFilter::Api api;
        QGraphicsApiFilter::OpenGLProfile profile;
    };

    const ApiFilterInfo apiFilterInfos[] = {
        { 3, 1, QGraphicsApiFilter::OpenGL, QGraphicsApiFilter::CoreProfile },
        { 3, 0, QGraphicsApiFilter::OpenGLES, QGraphicsApiFilter::NoProfile },
        { 2, 0, QGraphicsApiFilter::OpenGLES, QGraphicsApiFilter::NoProfile },
    #if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        { 1, 0, QGraphicsApiFilter::RHI, QGraphicsApiFilter::NoProfile },
    #endif
    };

    graphicsApiFilter()->setApi(apiFilterInfos[version].api);
    graphicsApiFilter()->setProfile(apiFilterInfos[version].profile);
    graphicsApiFilter()->setMajorVersion(apiFilterInfos[version].major);
    graphicsApiFilter()->setMinorVersion(apiFilterInfos[version].minor);

    const auto vertexShaderGraph = QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json"));
    const auto fragmentShaderGraph = QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.frag.json"));

    const QByteArray zFillFragmentShaderCode[] = {
        QByteArray(R"(
                   #version 330
                   void main() { }
                   )"),
        QByteArray(R"(
                   #version 300 es
                   void main() { }
                   )"),
        QByteArray(R"(
                   #version 100
                   void main() { }
                   )"),
        QByteArray(R"(
                   #version 450
                   void main() { }
                   )")
    };

    m_metalRoughShaderBuilder->setShaderProgram(m_metalRoughShader);
    m_metalRoughShaderBuilder->setVertexShaderGraph(vertexShaderGraph);
    m_metalRoughShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph);

    m_zfillShaderBuilder->setShaderProgram(m_zfillShader);
    m_zfillShaderBuilder->setVertexShaderGraph(vertexShaderGraph);
    m_zfillShader->setFragmentShaderCode(zFillFragmentShaderCode[version]);

    auto filterKey = new QFilterKey(this);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));
    addFilterKey(filterKey);

    m_techniqueAllowFrustumCullingFilterKey->setName(QStringLiteral("allowCulling"));
    m_techniqueAllowFrustumCullingFilterKey->setValue(true);
    addFilterKey(m_techniqueAllowFrustumCullingFilterKey);

    auto zfillFilterKey = new Qt3DRender::QFilterKey(this);
    zfillFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    zfillFilterKey->setValue(QStringLiteral("ZFill"));

    m_zfillRenderPass->setShaderProgram(m_zfillShader);
    m_zfillRenderPass->addRenderState(m_backFaceCulling);
    m_zfillRenderPass->addFilterKey(zfillFilterKey);
    addRenderPass(m_zfillRenderPass);

    auto opaqueFilterKey = new Qt3DRender::QFilterKey(this);
    opaqueFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    opaqueFilterKey->setValue(QStringLiteral("Opaque"));

    m_opaqueRenderPass->setShaderProgram(m_metalRoughShader);
    m_opaqueRenderPass->addRenderState(m_backFaceCulling);
    m_opaqueRenderPass->addFilterKey(opaqueFilterKey);
    addRenderPass(m_opaqueRenderPass);

    auto transparentFilterKey = new Qt3DRender::QFilterKey(this);
    transparentFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    transparentFilterKey->setValue(QStringLiteral("Transparent"));

    auto transparentPassFilterKey = new Qt3DRender::QFilterKey(this);
    transparentPassFilterKey->setName(QStringLiteral("Pass"));
    transparentPassFilterKey->setValue(QStringLiteral("pass0"));

    m_blendEquation->setBlendFunction(Qt3DRender::QBlendEquation::Add);
    m_blendArguments->setSourceRgb(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    m_blendArguments->setSourceAlpha(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    m_blendArguments->setDestinationRgb(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    m_blendArguments->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::One);

    m_transparentRenderPass->setShaderProgram(m_metalRoughShader);
    m_transparentRenderPass->addRenderState(m_backFaceCulling);
    m_transparentRenderPass->addRenderState(m_blendEquation);
    m_transparentRenderPass->addRenderState(m_blendArguments);
    m_transparentRenderPass->addFilterKey(transparentFilterKey);
    m_transparentRenderPass->addFilterKey(transparentPassFilterKey);
    m_transparentRenderPass->setEnabled(false);
    addRenderPass(m_transparentRenderPass);

    if (FrameGraphUtils::hasGeometryShaderSupport()) {
        auto cubeShadowShaderProg = new Qt3DRender::QShaderProgram(this);
        m_cubeMapShadowShaderBuilder->setShaderProgram(cubeShadowShaderProg);
        m_cubeMapShadowShaderBuilder->setVertexShaderGraph(vertexShaderGraph);
        cubeShadowShaderProg->setGeometryShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl("qrc:/kuesa/shaders/gl3/shadow_cube.geom")));
        cubeShadowShaderProg->setFragmentShaderCode(zFillFragmentShaderCode[version]);

        m_cubeMapShadowRenderPass->setShaderProgram(cubeShadowShaderProg);

        auto cubeShadowMapFilterKey = new Qt3DRender::QFilterKey(this);
        cubeShadowMapFilterKey->setName(QStringLiteral("KuesaDrawStage"));
        cubeShadowMapFilterKey->setValue(QStringLiteral("CubeShadowMap"));
        m_cubeMapShadowRenderPass->addFilterKey(cubeShadowMapFilterKey);
        m_cubeMapShadowRenderPass->addRenderState(m_backFaceCulling);

        addRenderPass(m_cubeMapShadowRenderPass);
    }
}

QStringList MetallicRoughnessTechnique::enabledLayers() const
{
    return m_metalRoughShaderBuilder->enabledLayers();
}

void MetallicRoughnessTechnique::setEnabledLayers(const QStringList &layers)
{
    m_metalRoughShaderBuilder->setEnabledLayers(layers);
    m_zfillShaderBuilder->setEnabledLayers(layers);
    m_cubeMapShadowShaderBuilder->setEnabledLayers(layers);
}

void MetallicRoughnessTechnique::setOpaque(bool opaque)
{
    m_zfillRenderPass->setEnabled(opaque);
    m_cubeMapShadowRenderPass->setEnabled(opaque);
    m_opaqueRenderPass->setEnabled(opaque);
    m_transparentRenderPass->setEnabled(!opaque);
}

void MetallicRoughnessTechnique::setCullingMode(QCullFace::CullingMode mode)
{
    m_backFaceCulling->setMode(mode);
}

void MetallicRoughnessTechnique::setAllowCulling(bool allowCulling)
{
    m_techniqueAllowFrustumCullingFilterKey->setValue(allowCulling);
}

QShaderProgramBuilder *MetallicRoughnessTechnique::metalRoughShaderBuilder() const
{
    return m_metalRoughShaderBuilder;
}

/*!
    \class Kuesa::MetallicRoughnessEffect
    \inheaderfile Kuesa/MetallicRoughnessEffect
    \inmodule Kuesa
    \since Kuesa 1.0

    \brief Kuesa::MetallicRoughnessEffect is a Qt3DRender::QEffect for
    Kuesa::MetallicRoughnessMaterial. It provides several properties used to
    configure the material effect. This is done using a custom shader graph and
    activating/deactivating different set of nodes of the graph depending on
    the effect configuration.

    \note Kuesa::MetallicRoughnessEffect is used to configure the effect for a
    material, but property values must be provided through
    Kuesa::MetallicRoughnessMaterial. Therefore, this effect must be added to a
    Kuesa.MetallicRoughnessMaterial.
 */

/*!
    \property MetallicRoughnessEffect::baseColorMapEnabled

    True to enable the effect support to specify base color propery with
    textures
 */

/*!
    \property MetallicRoughnessEffect::metalRoughMapEnabled

    True to enable the effect support to specify metalness and roughness
    properties with textures
 */

/*!
    \property MetallicRoughnessEffect::normalMapEnabled

    True to enable the effect support for normal maps
 */

/*!
    \property MetallicRoughnessEffect::ambientOcclusionMapEnabled

    True to enbale the effect support for ambient occlusion texture
 */

/*!
    \property MetallicRoughnessEffect::emissiveMapEnabled

    True to enable the effect support to specify emissive property with txtures
 */

/*!
 * \property MetallicRoughnessEffect::doubleSided

    If true, back face culling is disabled and the normals for the back faces
    are the same as for the front faces mulplied by -1
 */

/*!
    \property MetallicRoughnessEffect::useSkinning

    If true, a skinning enabled vertex shader is used instead of the default
    one. This allows to use this effect for rendering skinned meshes
 */

/*!
    \property MetallicRoughnessEffect::opaque

    If false, alpha blending is enabled for this effect
 */

/*!
    \property MetallicRoughnessEffect::alphaCutoffEnabled

    If true, alpha cutoff is enabled. Fragments with an alpha value above a
    threshold are rendered as opaque while fragment an alpha value below the
    threshold are discarded
 */

/*!
    \property MetallicRoughnessEffect::brdfLUT

    brdfLUT references a texture containing lookup tables for the split sum approximation
    in the PBR rendering. This is used internally by the material.

    When creating an instance of Kuesa::MetallicRoughnessMaterial, users should assign
    a texture to this property of the effect. A shared instance can be retrieved from
    the Kuesa::TextureCollection using the name "_kuesa_brdfLUT"

    \since Kuesa 1.1
 */

/*!
    \qmltype MetallicRoughnessEffect
    \instantiates Kuesa::MetallicRoughnessEffect
    \inqmlmodule Kuesa
    \since Kuesa 1.0

    \brief Kuesa.MetallicRoughnessEffect is a Qt3D.Render.Effect for
    Kuesa.MetallicRoughnessMaterial.

    It provides several properties used to configure the material effect. This
    is done using a custom shader graph and activating/deactivating different
    set of nodes of the graph depending on the effect configuration.

    \note Kuesa.MetallicRoughnessEffect is used to configure the effect for a
    material, but property values must be provided through
    Kuesa.MetallicRoughnessMaterial. Therefore, this effect must be added to a
    Kuesa.MetallicRoughnessMaterial.

    \sa Kuesa::MetallicRoughnessEffect
 */

/*!
    \qmlproperty bool MetallicRoughnessEffect::baseColorMapEnabled

    True to enable the effect support to specify base color propery with textures
 */

/*!
    \qmlproperty bool MetallicRoughnessEffect::metalRoughMapEnabled

    True to enable the effect support to specify metalness and roughness
    properties with textures
 */

/*!
    \qmlproperty bool MetallicRoughnessEffect::normalMapEnabled

    True to enable the effect support for normal maps
 */

/*!
    \qmlproperty bool MetallicRoughnessEffect::ambientOcclusionMapEnabled

    True to enbale the effect support for ambient occlusion texture
 */

/*!
    \qmlproperty bool MetallicRoughnessEffect::emissiveMapEnabled

    True to enable the effect support to specify emissive property with txtures
 */

/*!
    \qmlproperty bool MetallicRoughnessEffect::doubleSided

    If true, back face culling is disabled and the normals for the back faces
    are the same as for the front faces mulplied by -1
 */

/*!
    \qmlproperty bool MetallicRoughnessEffect::useSkinning

    If true, a skinning enabled vertex shader is used instead of the default
    one. This allows to use this effect for rendering skinned meshes
 */

/*!
    \qmlproperty bool MetallicRoughnessEffect::opaque

     If false, alpha blending is enabled for this effect
 */

/*!
    \qmlproperty bool MetallicRoughnessEffect::alphaCutoffEnabled

    If true, alpha cutoff is enabled. Fragments with an alpha value above a
    threshold are rendered as opaque while fragment an alpha value below the
    threshold are discarded
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture MetallicRoughnessEffect::brdfLUT

    brdfLUT references a texture containing lookup tables for the split sum approximation
    in the PBR rendering. This is used internally by the material.

    When creating an instance of Kuesa::MetallicRoughnessMaterial, users should assign
    a texture to this property of the effect. A shared instance can be retrieved from
    the Kuesa::TextureCollection using the name "_kuesa_brdfLUT"

    \since Kuesa 1.1
 */


MetallicRoughnessEffect::MetallicRoughnessEffect(Qt3DCore::QNode *parent)
    : GLTF2MaterialEffect(parent)
    , m_baseColorMapEnabled(false)
    , m_metalRoughMapEnabled(false)
    , m_normalMapEnabled(false)
    , m_ambientOcclusionMapEnabled(false)
    , m_emissiveMapEnabled(false)
    , m_brdfLUTParameter(new QParameter(this))
{
    const auto enabledLayers = QStringList{
        // Vertex Shader layers
        QStringLiteral("no-skinning"),
        // Fragment Shader layers
        QStringLiteral("noBaseColorMap"),
        QStringLiteral("noMetalRoughMap"),
        QStringLiteral("noAmbientOcclusionMap"),
        QStringLiteral("noEmissiveMap"),
        QStringLiteral("noNormalMap"),
        QStringLiteral("noHasColorAttr"),
        QStringLiteral("noDoubleSided"),
        QStringLiteral("noHasAlphaCutoff"),
        #if QT_VERSION >= QT_VERSION_CHECK(5, 15, 3) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            QStringLiteral("shadows")
        #endif
    };

    m_metalRoughGL3Technique = new MetallicRoughnessTechnique(MetallicRoughnessTechnique::GL3, this);
    m_metalRoughGL3Technique->setEnabledLayers(enabledLayers);

    m_metalRoughES3Technique = new MetallicRoughnessTechnique(MetallicRoughnessTechnique::ES3, this);
    m_metalRoughES3Technique->setEnabledLayers(enabledLayers);

    m_metalRoughES2Technique = new MetallicRoughnessTechnique(MetallicRoughnessTechnique::ES2, this);
    m_metalRoughES2Technique->setEnabledLayers(enabledLayers);

    addTechnique(m_metalRoughGL3Technique);
    addTechnique(m_metalRoughES3Technique);
    addTechnique(m_metalRoughES2Technique);

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_metalRoughRHITechnique = new MetallicRoughnessTechnique(MetallicRoughnessTechnique::RHI, this);
    m_metalRoughRHITechnique->setEnabledLayers(enabledLayers);

    addTechnique(m_metalRoughRHITechnique);
#endif

    // Note that even though those parameters are not exposed in the API,
    // they need to be kept around for now due to a bug in some drivers/GPUs
    // (at least Intel) which cause issues with unbound textures even if you
    // don't try to sample from them.
    // Can probably go away once we generate the shaders and deal in this
    // case in a better way.
    m_brdfLUTParameter->setName(QLatin1String("brdfLUT"));
    addParameter(m_brdfLUTParameter);
}

MetallicRoughnessEffect::~MetallicRoughnessEffect()
{
}

bool MetallicRoughnessEffect::isBaseColorMapEnabled() const
{
    return m_baseColorMapEnabled;
}

bool MetallicRoughnessEffect::isMetalRoughMapEnabled() const
{
    return m_metalRoughMapEnabled;
}

bool MetallicRoughnessEffect::isNormalMapEnabled() const
{
    return m_normalMapEnabled;
}

bool MetallicRoughnessEffect::isAmbientOcclusionMapEnabled() const
{
    return m_ambientOcclusionMapEnabled;
}

bool MetallicRoughnessEffect::isEmissiveMapEnabled() const
{
    return m_emissiveMapEnabled;
}

void MetallicRoughnessEffect::setBaseColorMapEnabled(bool enabled)
{
    if (m_baseColorMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("noBaseColorMap"));
    layers.removeAll(QStringLiteral("baseColorMap"));
    if (enabled) {
        layers.append(QStringLiteral("baseColorMap"));
    } else {
        layers.append(QStringLiteral("noBaseColorMap"));
    }
    updateLayersOnTechniques(layers);
    m_baseColorMapEnabled = enabled;
    emit baseColorMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setMetalRoughMapEnabled(bool enabled)
{
    if (m_metalRoughMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("noMetalRoughMap"));
    layers.removeAll(QStringLiteral("metalRoughMap"));
    if (enabled) {
        layers.append(QStringLiteral("metalRoughMap"));
    } else {
        layers.append(QStringLiteral("noMetalRoughMap"));
    }
    updateLayersOnTechniques(layers);
    m_metalRoughMapEnabled = enabled;
    emit metalRoughMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setNormalMapEnabled(bool enabled)
{
    if (m_normalMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("noNormalMap"));
    layers.removeAll(QStringLiteral("normalMap"));
    if (enabled) {
        layers.append(QStringLiteral("normalMap"));
    } else {
        layers.append(QStringLiteral("noNormalMap"));
    }
    updateLayersOnTechniques(layers);
    m_normalMapEnabled = enabled;
    emit normalMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setAmbientOcclusionMapEnabled(bool enabled)
{
    if (m_ambientOcclusionMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("noAmbientOcclusionMap"));
    layers.removeAll(QStringLiteral("ambientOcclusionMap"));
    if (enabled) {
        layers.append(QStringLiteral("ambientOcclusionMap"));
    } else {
        layers.append(QStringLiteral("noAmbientOcclusionMap"));
    }
    updateLayersOnTechniques(layers);
    m_ambientOcclusionMapEnabled = enabled;
    emit ambientOcclusionMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setEmissiveMapEnabled(bool enabled)
{
    if (m_emissiveMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("noEmissiveMap"));
    layers.removeAll(QStringLiteral("emissiveMap"));
    if (enabled) {
        layers.append(QStringLiteral("emissiveMap"));
    } else {
        layers.append(QStringLiteral("noEmissiveMap"));
    }
    updateLayersOnTechniques(layers);
    m_emissiveMapEnabled = enabled;
    emit emissiveMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::updateUsingColorAttribute(bool usingColorAttribute)
{
    auto layers = m_metalRoughGL3Technique->enabledLayers();
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

void MetallicRoughnessEffect::updateUsingNormalAttribute(bool usingNormalAttribute)
{
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasVertexNormal"));
    if (usingNormalAttribute)
        layers.append(QStringLiteral("hasVertexNormal"));

    updateLayersOnTechniques(layers);
}

void MetallicRoughnessEffect::updateUsingTangentAttribute(bool usingTangentAttribute)
{
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasVertexTangent"));
    if (usingTangentAttribute)
        layers.append(QStringLiteral("hasVertexTangent"));

    updateLayersOnTechniques(layers);
}

void MetallicRoughnessEffect::updateUsingTexCoordAttribute(bool usingTexCoordAttribute)
{
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasTexCoord"));
    if (usingTexCoordAttribute)
        layers.append(QStringLiteral("hasTexCoord"));

    updateLayersOnTechniques(layers);
}

void MetallicRoughnessEffect::updateUsingTexCoord1Attribute(bool usingTexCoord1Attribute)
{
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasTexCoord1"));
    if (usingTexCoord1Attribute)
        layers.append(QStringLiteral("hasTexCoord1"));

    updateLayersOnTechniques(layers);
}

void MetallicRoughnessEffect::updateDoubleSided(bool doubleSided)
{
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("noDoubleSided"));
    layers.removeAll(QStringLiteral("doubleSided"));
    if (doubleSided) {
        layers.append(QStringLiteral("doubleSided"));
    } else {
        layers.append(QStringLiteral("noDoubleSided"));
    }

    updateLayersOnTechniques(layers);

    const auto cullingMode = doubleSided ? QCullFace::NoCulling : QCullFace::Back;
    m_metalRoughGL3Technique->setCullingMode(cullingMode);
    m_metalRoughES3Technique->setCullingMode(cullingMode);
    m_metalRoughES2Technique->setCullingMode(cullingMode);

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_metalRoughRHITechnique->setCullingMode(cullingMode);
#endif
}

void MetallicRoughnessEffect::updateUsingSkinning(bool useSkinning)
{
    // Set Layers on zFill and opaque/Transparent shader builders
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("no-skinning"));
    layers.removeAll(QStringLiteral("skinning"));
    if (useSkinning) {
        layers.append(QStringLiteral("skinning"));
    } else {
        layers.append(QStringLiteral("no-skinning"));
    }

    updateLayersOnTechniques(layers);

    m_metalRoughGL3Technique->setAllowCulling(!useSkinning);
    m_metalRoughES3Technique->setAllowCulling(!useSkinning);
    m_metalRoughES2Technique->setAllowCulling(!useSkinning);

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_metalRoughRHITechnique->setAllowCulling(!useSkinning);
#endif
}

void MetallicRoughnessEffect::updateOpaque(bool opaque)
{
    m_metalRoughGL3Technique->setOpaque(opaque);
    m_metalRoughES3Technique->setOpaque(opaque);
    m_metalRoughES2Technique->setOpaque(opaque);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_metalRoughRHITechnique->setOpaque(opaque);
#endif
}

void MetallicRoughnessEffect::updateAlphaCutoffEnabled(bool enabled)
{
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("noHasAlphaCutoff"));
    layers.removeAll(QStringLiteral("hasAlphaCutoff"));
    if (enabled) {
        layers.append(QStringLiteral("hasAlphaCutoff"));
    } else {
        layers.append(QStringLiteral("noHasAlphaCutoff"));
    }
    updateLayersOnTechniques(layers);
}

void MetallicRoughnessEffect::updateUsingMorphTargets(bool usingMorphTargets)
{
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("morphtargets"));
    if (usingMorphTargets)
        layers.append(QStringLiteral("morphtargets"));

    updateLayersOnTechniques(layers);
}

void MetallicRoughnessEffect::updateUsingCubeMapArrays(bool usingCubeMapArrays)
{
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    //    layers.removeAll(QStringLiteral("noCubeMapArrays"));
    layers.removeAll(QStringLiteral("useCubeMapArrays"));
    if (usingCubeMapArrays)
        layers.append(QStringLiteral("useCubeMapArrays"));

    updateLayersOnTechniques(layers);
}

void MetallicRoughnessEffect::updateInstanced(bool instanced)
{
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("instanced"));
    if (instanced)
        layers.append(QStringLiteral("instanced"));

    updateLayersOnTechniques(layers);
}

Qt3DRender::QAbstractTexture *MetallicRoughnessEffect::brdfLUT() const
{
    return m_brdfLUTParameter->value().value<Qt3DRender::QAbstractTexture *>();
}

void MetallicRoughnessEffect::setBrdfLUT(Qt3DRender::QAbstractTexture *brdfLUT)
{
    auto d = Qt3DCore::QNodePrivate::get(this);
    auto current = this->brdfLUT();
    if (brdfLUT == current)
        return;
    if (current)
        d->unregisterDestructionHelper(current);
    if (brdfLUT) {
        d->registerDestructionHelper(brdfLUT, &MetallicRoughnessEffect::setBrdfLUT, brdfLUT);
        if (!brdfLUT->parentNode())
            brdfLUT->setParent(this);
    }
    m_brdfLUTParameter->setValue(QVariant::fromValue(brdfLUT));

    emit brdfLUTChanged(brdfLUT);
}

void MetallicRoughnessEffect::updateLayersOnTechniques(const QStringList &layers)
{
    m_metalRoughGL3Technique->setEnabledLayers(layers);
    m_metalRoughES3Technique->setEnabledLayers(layers);
    m_metalRoughES2Technique->setEnabledLayers(layers);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_metalRoughRHITechnique->setEnabledLayers(layers);
#endif
}

} // namespace Kuesa

QT_END_NAMESPACE
