/*
    metallicroughnesseffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <Qt3DRender/qcullface.h>
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

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

/*!
 * \class MetallicRoughnessEffect
 * \inheaderfile Kuesa/MetallicRoughnessEffect
 * \inmodule Kuesa
 * \since 1.0
 *
 * \brief Kuesa::MetallicRoughnessEffect is a Qt3DRender::QEffect for
 * Kuesa::MetallicRoughnessMaterial. It provides several properties used to
 * configure the material effect. This is done using a custom shader graph and
 * activating/deactivating different set of nodes of the graph depending on the
 * effect configuration.
 *
 * \note Kuesa::MetallicRoughnessEffect is used to configure the effect for a
 * material, but property values must be provided through
 * Kuesa::MetallicRoughnessMaterial. Therefore, this effect must be added to a
 * Kuesa.MetallicRoughnessMaterial.
 */

/*!
 * \property baseColorMapEnabled True to enable the effect support to specify
 * base color propery with textures
 */

/*!
 *\property metalRoughMapEnabled True to enable the effect support to specify
 * metalness and roughness properties with textures
 */

/*!
 * \property normalMapEnabled True to enable the effect support for normal maps
 */

/*!
 * \property ambientOcclusionMapEnabled True to enbale the effect support for
 * ambient occlusion texture
 */

/*!
 * \property emissiveMapEnabled True to enable the effect support to specify
 * emissive property with txtures
 */

/*!
 * \property usingColorAttribute If true, the base color property is multiplied
 * by the color interpolated attribute of the mesh
 */

/*!
 * \property doubleSided If true, back face culling is disabled and the normals
 * for the back faces are the same as for the front faces mulplied by -1
 */

/*!
 * \property useSkinning If true, a skinning enabled vertex shader is used
 * instead of the default one. This allows to use this effect for rendering
 * skinned meshes
 */

/*!
 * \property opaque If false, alpha blending is enabled for this effect
 */

/*!
 * \property alphaCutoffEnabled If true, alpha cutoff is enabled. Fragments
 * with an alpha value above a threshold are rendered as opaque while fragment
 * an alpha value below the threshold are discarded
 */

/*!
 * \qmltype MetallicRoughnessEffect
 * \instantiates Kuesa::MetallicRoughnessEffect
 * \inmodule Kuesa
 * \since 1.0
 *
 * \brief Kuesa.MetallicRoughnessEffect is a Qt3D.Render.Effect for
 * Kuesa.MetallicRoughnessMaterial. It provides several properties used to
 * configure the material effect. This is done using a custom shader graph and
 * activating/deactivating different set of nodes of the graph depending on the
 * effect configuration.
 *
 * \note Kuesa.MetallicRoughnessEffect is used to configure the effect for a
 * material, but property values must be provided through
 * Kuesa.MetallicRoughnessMaterial. Therefore, this effect must be added to a
 * Kuesa.MetallicRoughnessMaterial.
 */

/*!
 * \qmlproperty bool baseColorMapEnabled True to enable the effect support to
 * specify base color propery with textures
 */

/*!
 *\qmlproperty bool metalRoughMapEnabled True to enable the effect support to
 * specify metalness and roughness properties with textures
 */

/*!
 * \qmlproperty bool normalMapEnabled True to enable the effect support for
 * normal maps
 */

/*!
 * \qmlproperty bool ambientOcclusionMapEnabled True to enbale the effect
 * support for ambient occlusion texture
 */

/*!
 * \qmlproperty bool emissiveMapEnabled True to enable the effect support to
 * specify emissive property with txtures
 */

/*!
 * \qmlproperty bool usingColorAttribute If true, the base color property is
 * multiplied by the color interpolated attribute of the mesh
 */

/*!
 * \qmlproperty bool doubleSided If true, back face culling is disabled and the
 * normals for the back faces are the same as for the front faces mulplied by
 * -1
 */

/*!
 * \qmlproperty bool useSkinning If true, a skinning enabled vertex shader is
 * used instead of the default one. This allows to use this effect for
 * rendering skinned meshes
 */

/*!
 * \qmlproperty bool opaque If false, alpha blending is enabled for this effect
 */

/*!
 * \qmlproperty bool alphaCutoffEnabled If true, alpha cutoff is enabled.
 * Fragments with an alpha value above a threshold are rendered as opaque while
 * fragment an alpha value below the threshold are discarded
 */

MetallicRoughnessEffect::MetallicRoughnessEffect(Qt3DCore::QNode *parent)
    : QEffect(parent)
    , m_baseColorMapEnabled(false)
    , m_metalRoughMapEnabled(false)
    , m_normalMapEnabled(false)
    , m_ambientOcclusionMapEnabled(false)
    , m_emissiveMapEnabled(false)
    , m_usingColorAttribute(false)
    , m_doubleSided(false)
    , m_useSkinning(false)
    , m_invokeInitVertexShaderRequested(false)
    , m_opaque(true)
    , m_alphaCutoffEnabled(false)
    , m_backFaceCulling(new QCullFace(this))
    , m_metalRoughGL3ShaderBuilder(new QShaderProgramBuilder(this))
    , m_metalRoughES3ShaderBuilder(new QShaderProgramBuilder(this))
    , m_metalRoughES2ShaderBuilder(new QShaderProgramBuilder(this))
    , m_metalRoughGL3Shader(new QShaderProgram(this))
    , m_metalRoughES3Shader(new QShaderProgram(this))
    , m_metalRoughES2Shader(new QShaderProgram(this))
{
    const auto enabledLayers = QStringList{ QStringLiteral("noBaseColorMap"),
                                            QStringLiteral("noMetalRoughMap"),
                                            QStringLiteral("noAmbientOcclusionMap"),
                                            QStringLiteral("noEmissiveMap"),
                                            QStringLiteral("noNormalMap"),
                                            QStringLiteral("noHasColorAttr"),
                                            QStringLiteral("noDoubleSided"),
                                            QStringLiteral("noHasAlphaCutoff") };
    const auto fragmentShaderGraph = QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.frag.json"));
    m_metalRoughGL3ShaderBuilder->setShaderProgram(m_metalRoughGL3Shader);
    m_metalRoughGL3ShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph);
    m_metalRoughGL3ShaderBuilder->setEnabledLayers(enabledLayers);

    m_metalRoughES3ShaderBuilder->setShaderProgram(m_metalRoughES3Shader);
    m_metalRoughES3ShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph);
    m_metalRoughES3ShaderBuilder->setEnabledLayers(enabledLayers);

    m_metalRoughES2ShaderBuilder->setShaderProgram(m_metalRoughES2Shader);
    m_metalRoughES2ShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph);
    m_metalRoughES2ShaderBuilder->setEnabledLayers(enabledLayers);

    m_metalRoughGL3Technique = new QTechnique(this);
    m_metalRoughGL3Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
    m_metalRoughGL3Technique->graphicsApiFilter()->setMajorVersion(3);
    m_metalRoughGL3Technique->graphicsApiFilter()->setMinorVersion(1);
    m_metalRoughGL3Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);

    m_metalRoughES3Technique = new QTechnique(this);
    m_metalRoughES3Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGLES);
    m_metalRoughES3Technique->graphicsApiFilter()->setMajorVersion(3);
    m_metalRoughES3Technique->graphicsApiFilter()->setMinorVersion(0);
    m_metalRoughES3Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::NoProfile);

    m_metalRoughES2Technique = new QTechnique(this);
    m_metalRoughES2Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGLES);
    m_metalRoughES2Technique->graphicsApiFilter()->setMajorVersion(2);
    m_metalRoughES2Technique->graphicsApiFilter()->setMinorVersion(0);
    m_metalRoughES2Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::NoProfile);

    auto filterKey = new QFilterKey(this);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));
    m_metalRoughGL3Technique->addFilterKey(filterKey);
    m_metalRoughES3Technique->addFilterKey(filterKey);
    m_metalRoughES2Technique->addFilterKey(filterKey);

    auto zfillGL3Shader = new Qt3DRender::QShaderProgram(this);
    zfillGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/default.vert")))); // from Qt3D
    zfillGL3Shader->setFragmentShaderCode(QByteArray(R"(
                                                     #version 330
                                                     void main() { }
                                                     )"));

    auto zfillES3Shader = new Qt3DRender::QShaderProgram(this);
    zfillES3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es3/default.vert")))); // from Qt3D
    zfillES3Shader->setFragmentShaderCode(QByteArray(R"(
                                                     #version 300 es
                                                     void main() { }
                                                     )"));

    auto zfillES2Shader = new Qt3DRender::QShaderProgram(this);
    zfillES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/default.vert")))); // from Qt3D
    zfillES2Shader->setFragmentShaderCode(QByteArray(R"(
                                                     #version 100
                                                     void main() { }
                                                     )"));

    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("KuesaDrawStage"));
        filterKey->setValue(QStringLiteral("ZFill"));

        m_zfillGL3RenderPass = new QRenderPass(this);
        m_zfillGL3RenderPass->setShaderProgram(zfillGL3Shader);
        m_zfillGL3RenderPass->addRenderState(m_backFaceCulling);
        m_zfillGL3RenderPass->addFilterKey(filterKey);
        m_metalRoughGL3Technique->addRenderPass(m_zfillGL3RenderPass);

        m_zfillES3RenderPass = new QRenderPass(this);
        m_zfillES3RenderPass->setShaderProgram(zfillES3Shader);
        m_zfillES3RenderPass->addRenderState(m_backFaceCulling);
        m_zfillES3RenderPass->addFilterKey(filterKey);
        m_metalRoughES3Technique->addRenderPass(m_zfillES3RenderPass);

        m_zfillES2RenderPass = new QRenderPass(this);
        m_zfillES2RenderPass->setShaderProgram(zfillES2Shader);
        m_zfillES2RenderPass->addRenderState(m_backFaceCulling);
        m_zfillES2RenderPass->addFilterKey(filterKey);
        m_metalRoughES2Technique->addRenderPass(m_zfillES2RenderPass);
    }

    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("KuesaDrawStage"));
        filterKey->setValue(QStringLiteral("Opaque"));

        m_opaqueGL3RenderPass = new QRenderPass(this);
        m_opaqueGL3RenderPass->setShaderProgram(m_metalRoughGL3Shader);
        m_opaqueGL3RenderPass->addRenderState(m_backFaceCulling);
        m_opaqueGL3RenderPass->addFilterKey(filterKey);
        m_metalRoughGL3Technique->addRenderPass(m_opaqueGL3RenderPass);

        m_opaqueES3RenderPass = new QRenderPass(this);
        m_opaqueES3RenderPass->setShaderProgram(m_metalRoughES3Shader);
        m_opaqueES3RenderPass->addRenderState(m_backFaceCulling);
        m_opaqueES3RenderPass->addFilterKey(filterKey);
        m_metalRoughES3Technique->addRenderPass(m_opaqueES3RenderPass);

        m_opaqueES2RenderPass = new QRenderPass(this);
        m_opaqueES2RenderPass->setShaderProgram(m_metalRoughES2Shader);
        m_opaqueES2RenderPass->addRenderState(m_backFaceCulling);
        m_opaqueES2RenderPass->addFilterKey(filterKey);
        m_metalRoughES2Technique->addRenderPass(m_opaqueES2RenderPass);
    }

    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("KuesaDrawStage"));
        filterKey->setValue(QStringLiteral("Transparent"));

        m_transparentGL3RenderPass = new QRenderPass(this);
        m_transparentGL3RenderPass->setShaderProgram(m_metalRoughGL3Shader);
        m_transparentGL3RenderPass->addRenderState(m_backFaceCulling);
        m_transparentGL3RenderPass->addFilterKey(filterKey);
        m_transparentGL3RenderPass->setEnabled(false);
        m_metalRoughGL3Technique->addRenderPass(m_transparentGL3RenderPass);

        m_transparentES3RenderPass = new QRenderPass(this);
        m_transparentES3RenderPass->setShaderProgram(m_metalRoughES3Shader);
        m_transparentES3RenderPass->addRenderState(m_backFaceCulling);
        m_transparentES3RenderPass->addFilterKey(filterKey);
        m_transparentES3RenderPass->setEnabled(false);
        m_metalRoughES3Technique->addRenderPass(m_transparentES3RenderPass);

        m_transparentES2RenderPass = new QRenderPass(this);
        m_transparentES2RenderPass->setShaderProgram(m_metalRoughES2Shader);
        m_transparentES2RenderPass->addRenderState(m_backFaceCulling);
        m_transparentES2RenderPass->addFilterKey(filterKey);
        m_transparentES2RenderPass->setEnabled(false);
        m_metalRoughES2Technique->addRenderPass(m_transparentES2RenderPass);
    }

    addTechnique(m_metalRoughGL3Technique);
    addTechnique(m_metalRoughES3Technique);
    addTechnique(m_metalRoughES2Technique);

    // Note that even though those parameters are not exposed in the API,
    // they need to be kept around for now due to a bug in some drivers/GPUs
    // (at least Intel) which cause issues with unbound textures even if you
    // don't try to sample from them.
    // Can probably go away once we generate the shaders and deal in this
    // case in a better way.
    addParameter(new QParameter(QStringLiteral("envLight.irradiance"), new QTexture2D));
    addParameter(new QParameter(QStringLiteral("envLight.specular"), new QTexture2D));

    QMetaObject::invokeMethod(this, "initVertexShader", Qt::QueuedConnection);
    m_invokeInitVertexShaderRequested = true;
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

bool MetallicRoughnessEffect::isUsingColorAttribute() const
{
    return m_usingColorAttribute;
}

bool MetallicRoughnessEffect::isDoubleSided() const
{
    return m_doubleSided;
}

bool MetallicRoughnessEffect::useSkinning() const
{
    return m_useSkinning;
}

bool MetallicRoughnessEffect::isOpaque() const
{
    return m_opaque;
}

bool MetallicRoughnessEffect::isAlphaCutoffEnabled() const
{
    return m_alphaCutoffEnabled;
}

void MetallicRoughnessEffect::setBaseColorMapEnabled(bool enabled)
{
    if (m_baseColorMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3ShaderBuilder->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noBaseColorMap"));
        layers.append(QStringLiteral("baseColorMap"));
    } else {
        layers.removeAll(QStringLiteral("baseColorMap"));
        layers.append(QStringLiteral("noBaseColorMap"));
    }
    m_baseColorMapEnabled = enabled;
    m_metalRoughGL3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES2ShaderBuilder->setEnabledLayers(layers);
    emit baseColorMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setMetalRoughMapEnabled(bool enabled)
{
    if (m_metalRoughMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3ShaderBuilder->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noMetalRoughMap"));
        layers.append(QStringLiteral("metalRoughMap"));
    } else {
        layers.removeAll(QStringLiteral("metalRoughMap"));
        layers.append(QStringLiteral("noMetalRoughMap"));
    }
    m_metalRoughMapEnabled = enabled;
    m_metalRoughGL3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES2ShaderBuilder->setEnabledLayers(layers);
    emit metalRoughMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setNormalMapEnabled(bool enabled)
{
    if (m_normalMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3ShaderBuilder->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noNormalMap"));
        layers.append(QStringLiteral("normalMap"));
    } else {
        layers.removeAll(QStringLiteral("normalMap"));
        layers.append(QStringLiteral("noNormalMap"));
    }
    m_normalMapEnabled = enabled;
    m_metalRoughGL3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES2ShaderBuilder->setEnabledLayers(layers);
    emit normalMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setAmbientOcclusionMapEnabled(bool enabled)
{
    if (m_ambientOcclusionMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3ShaderBuilder->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noAmbientOcclusionMap"));
        layers.append(QStringLiteral("ambientOcclusionMap"));
    } else {
        layers.removeAll(QStringLiteral("ambientOcclusionMap"));
        layers.append(QStringLiteral("noAmbientOcclusionMap"));
    }
    m_ambientOcclusionMapEnabled = enabled;
    m_metalRoughGL3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES2ShaderBuilder->setEnabledLayers(layers);
    emit ambientOcclusionMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setEmissiveMapEnabled(bool enabled)
{
    if (m_emissiveMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3ShaderBuilder->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noEmissiveMap"));
        layers.append(QStringLiteral("emissiveMap"));
    } else {
        layers.removeAll(QStringLiteral("emissiveMap"));
        layers.append(QStringLiteral("noEmissiveMap"));
    }
    m_emissiveMapEnabled = enabled;
    m_metalRoughGL3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES2ShaderBuilder->setEnabledLayers(layers);
    emit emissiveMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setUsingColorAttribute(bool usingColorAttribute)
{
    if (m_usingColorAttribute == usingColorAttribute)
        return;

    auto layers = m_metalRoughGL3ShaderBuilder->enabledLayers();
    if (usingColorAttribute) {
        layers.removeAll(QStringLiteral("noHasColorAttr"));
        layers.append(QStringLiteral("hasColorAttr"));
    } else {
        layers.removeAll(QStringLiteral("hasColorAttr"));
        layers.append(QStringLiteral("noHasColorAttr"));
    }
    m_usingColorAttribute = usingColorAttribute;
    m_metalRoughGL3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES2ShaderBuilder->setEnabledLayers(layers);
    emit usingColorAttributeChanged(usingColorAttribute);
}

void MetallicRoughnessEffect::setDoubleSided(bool doubleSided)
{
    if (m_doubleSided == doubleSided)
        return;

    auto layers = m_metalRoughGL3ShaderBuilder->enabledLayers();
    if (doubleSided) {
        layers.removeAll(QStringLiteral("noDoubleSided"));
        layers.append(QStringLiteral("doubleSided"));
        m_backFaceCulling->setMode(QCullFace::NoCulling);
    } else {
        layers.removeAll(QStringLiteral("doubleSided"));
        layers.append(QStringLiteral("noDoubleSided"));
        m_backFaceCulling->setMode(QCullFace::Back);
    }
    m_doubleSided = doubleSided;
    m_metalRoughGL3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES2ShaderBuilder->setEnabledLayers(layers);
    emit doubleSidedChanged(doubleSided);
}

void MetallicRoughnessEffect::setUseSkinning(bool useSkinning)
{
    if (useSkinning == m_useSkinning)
        return;
    m_useSkinning = useSkinning;
    emit useSkinningChanged(m_useSkinning);
    if (!m_invokeInitVertexShaderRequested)
        initVertexShader();
}

void MetallicRoughnessEffect::setOpaque(bool opaque)
{
    if (opaque == m_opaque)
        return;
    m_opaque = opaque;
    m_zfillGL3RenderPass->setEnabled(opaque);
    m_zfillES3RenderPass->setEnabled(opaque);
    m_zfillES2RenderPass->setEnabled(opaque);
    m_opaqueGL3RenderPass->setEnabled(opaque);
    m_opaqueES3RenderPass->setEnabled(opaque);
    m_opaqueES2RenderPass->setEnabled(opaque);
    m_transparentGL3RenderPass->setEnabled(!opaque);
    m_transparentES3RenderPass->setEnabled(!opaque);
    m_transparentES2RenderPass->setEnabled(!opaque);
    if (opaque)
        setAlphaCutoffEnabled(false);
}

void MetallicRoughnessEffect::setAlphaCutoffEnabled(bool enabled)
{
    if (m_alphaCutoffEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3ShaderBuilder->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noHasAlphaCutoff"));
        layers.append(QStringLiteral("hasAlphaCutoff"));
    } else {
        layers.removeAll(QStringLiteral("hasAlphaCutoff"));
        layers.append(QStringLiteral("noHasAlphaCutoff"));
    }
    m_alphaCutoffEnabled = enabled;
    m_metalRoughGL3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES3ShaderBuilder->setEnabledLayers(layers);
    m_metalRoughES2ShaderBuilder->setEnabledLayers(layers);
    emit alphaCutoffEnabledChanged(enabled);
}

void MetallicRoughnessEffect::initVertexShader()
{
    if (m_useSkinning) {
        m_metalRoughGL3Shader->setVertexShaderCode(
                QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/skinned.vert"))));
        m_metalRoughES3Shader->setVertexShaderCode(
                QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/es3/skinned.vert"))));
        m_metalRoughES2Shader->setVertexShaderCode(
                QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/skinned.vert"))));
    } else {
        m_metalRoughGL3Shader->setVertexShaderCode(
                QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/simple.vert"))));
        m_metalRoughES3Shader->setVertexShaderCode(
                QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/es3/simple.vert"))));
        m_metalRoughES2Shader->setVertexShaderCode(
                QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/simple.vert"))));
    }
    m_invokeInitVertexShaderRequested = false;
}

} // namespace Kuesa

QT_END_NAMESPACE
