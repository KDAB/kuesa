/*
    metallicroughnesseffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

class MetallicRoughnessTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2
    };

    explicit MetallicRoughnessTechnique(Version version, Qt3DCore::QNode *parent = nullptr);

    QStringList enabledLayers() const;
    void setEnabledLayers(const QStringList &layers);
    void setOpaque(bool opaque);
    void setCullingMode(QCullFace::CullingMode mode);
    void setAllowCulling(bool allowCulling);

private:
    Qt3DRender::QCullFace *m_backFaceCulling;
    Qt3DRender::QShaderProgramBuilder *m_metalRoughShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_zfillShaderBuilder;
    Qt3DRender::QShaderProgram *m_metalRoughShader;
    Qt3DRender::QShaderProgram *m_zfillShader;
    Qt3DRender::QRenderPass *m_zfillRenderPass;
    Qt3DRender::QRenderPass *m_opaqueRenderPass;
    Qt3DRender::QRenderPass *m_transparentRenderPass;
    Qt3DRender::QFilterKey *m_techniqueAllowFrustumCullingFilterKey;
};

MetallicRoughnessTechnique::MetallicRoughnessTechnique(Version version, Qt3DCore::QNode *parent)
    : QTechnique(parent)
    , m_backFaceCulling(new QCullFace(this))
    , m_metalRoughShaderBuilder(new QShaderProgramBuilder(this))
    , m_zfillShaderBuilder(new QShaderProgramBuilder(this))
    , m_metalRoughShader(new QShaderProgram(this))
    , m_zfillShader(new QShaderProgram(this))
    , m_zfillRenderPass(new QRenderPass(this))
    , m_opaqueRenderPass(new QRenderPass(this))
    , m_transparentRenderPass(new QRenderPass(this))
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

    m_transparentRenderPass->setShaderProgram(m_metalRoughShader);
    m_transparentRenderPass->addRenderState(m_backFaceCulling);
    m_transparentRenderPass->addFilterKey(transparentFilterKey);
    m_transparentRenderPass->setEnabled(false);
    addRenderPass(m_transparentRenderPass);
}

QStringList MetallicRoughnessTechnique::enabledLayers() const
{
    return m_metalRoughShaderBuilder->enabledLayers();
}

void MetallicRoughnessTechnique::setEnabledLayers(const QStringList &layers)
{
    m_metalRoughShaderBuilder->setEnabledLayers(layers);
    m_zfillShaderBuilder->setEnabledLayers(layers);
}

void MetallicRoughnessTechnique::setOpaque(bool opaque)
{
    m_zfillRenderPass->setEnabled(opaque);
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
    \property MetallicRoughnessEffect::usingColorAttribute

    If true, the base color property is multiplied by the color interpolated
    attribute of the mesh
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
    \qmlproperty bool MetallicRoughnessEffect::usingColorAttribute

    If true, the base color property is multiplied by the color interpolated
    attribute of the mesh
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
    : QEffect(parent)
    , m_baseColorMapEnabled(false)
    , m_metalRoughMapEnabled(false)
    , m_normalMapEnabled(false)
    , m_ambientOcclusionMapEnabled(false)
    , m_emissiveMapEnabled(false)
    , m_usingColorAttribute(false)
    , m_doubleSided(false)
    , m_useSkinning(false)
    , m_opaque(true)
    , m_alphaCutoffEnabled(false)
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

    // Note that even though those parameters are not exposed in the API,
    // they need to be kept around for now due to a bug in some drivers/GPUs
    // (at least Intel) which cause issues with unbound textures even if you
    // don't try to sample from them.
    // Can probably go away once we generate the shaders and deal in this
    // case in a better way.
#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
    addParameter(new QParameter(QStringLiteral("envLight.irradiance"), new QTexture2D));
    addParameter(new QParameter(QStringLiteral("envLight.specular"), new QTexture2D));
#endif

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

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noBaseColorMap"));
        layers.append(QStringLiteral("baseColorMap"));
    } else {
        layers.removeAll(QStringLiteral("baseColorMap"));
        layers.append(QStringLiteral("noBaseColorMap"));
    }
    m_baseColorMapEnabled = enabled;
    m_metalRoughGL3Technique->setEnabledLayers(layers);
    m_metalRoughES3Technique->setEnabledLayers(layers);
    m_metalRoughES2Technique->setEnabledLayers(layers);
    emit baseColorMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setMetalRoughMapEnabled(bool enabled)
{
    if (m_metalRoughMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noMetalRoughMap"));
        layers.append(QStringLiteral("metalRoughMap"));
    } else {
        layers.removeAll(QStringLiteral("metalRoughMap"));
        layers.append(QStringLiteral("noMetalRoughMap"));
    }
    m_metalRoughMapEnabled = enabled;
    m_metalRoughGL3Technique->setEnabledLayers(layers);
    m_metalRoughES3Technique->setEnabledLayers(layers);
    m_metalRoughES2Technique->setEnabledLayers(layers);
    emit metalRoughMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setNormalMapEnabled(bool enabled)
{
    if (m_normalMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noNormalMap"));
        layers.append(QStringLiteral("normalMap"));
    } else {
        layers.removeAll(QStringLiteral("normalMap"));
        layers.append(QStringLiteral("noNormalMap"));
    }
    m_normalMapEnabled = enabled;
    m_metalRoughGL3Technique->setEnabledLayers(layers);
    m_metalRoughES3Technique->setEnabledLayers(layers);
    m_metalRoughES2Technique->setEnabledLayers(layers);
    emit normalMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setAmbientOcclusionMapEnabled(bool enabled)
{
    if (m_ambientOcclusionMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noAmbientOcclusionMap"));
        layers.append(QStringLiteral("ambientOcclusionMap"));
    } else {
        layers.removeAll(QStringLiteral("ambientOcclusionMap"));
        layers.append(QStringLiteral("noAmbientOcclusionMap"));
    }
    m_ambientOcclusionMapEnabled = enabled;
    m_metalRoughGL3Technique->setEnabledLayers(layers);
    m_metalRoughES3Technique->setEnabledLayers(layers);
    m_metalRoughES2Technique->setEnabledLayers(layers);
    emit ambientOcclusionMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setEmissiveMapEnabled(bool enabled)
{
    if (m_emissiveMapEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noEmissiveMap"));
        layers.append(QStringLiteral("emissiveMap"));
    } else {
        layers.removeAll(QStringLiteral("emissiveMap"));
        layers.append(QStringLiteral("noEmissiveMap"));
    }
    m_emissiveMapEnabled = enabled;
    m_metalRoughGL3Technique->setEnabledLayers(layers);
    m_metalRoughES3Technique->setEnabledLayers(layers);
    m_metalRoughES2Technique->setEnabledLayers(layers);
    emit emissiveMapEnabledChanged(enabled);
}

void MetallicRoughnessEffect::setUsingColorAttribute(bool usingColorAttribute)
{
    if (m_usingColorAttribute == usingColorAttribute)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    if (usingColorAttribute) {
        layers.removeAll(QStringLiteral("noHasColorAttr"));
        layers.append(QStringLiteral("hasColorAttr"));
    } else {
        layers.removeAll(QStringLiteral("hasColorAttr"));
        layers.append(QStringLiteral("noHasColorAttr"));
    }
    m_usingColorAttribute = usingColorAttribute;
    m_metalRoughGL3Technique->setEnabledLayers(layers);
    m_metalRoughES3Technique->setEnabledLayers(layers);
    m_metalRoughES2Technique->setEnabledLayers(layers);
    emit usingColorAttributeChanged(usingColorAttribute);
}

void MetallicRoughnessEffect::setDoubleSided(bool doubleSided)
{
    if (m_doubleSided == doubleSided)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    if (doubleSided) {
        layers.removeAll(QStringLiteral("noDoubleSided"));
        layers.append(QStringLiteral("doubleSided"));
    } else {
        layers.removeAll(QStringLiteral("doubleSided"));
        layers.append(QStringLiteral("noDoubleSided"));
    }
    m_doubleSided = doubleSided;
    m_metalRoughGL3Technique->setEnabledLayers(layers);
    m_metalRoughES3Technique->setEnabledLayers(layers);
    m_metalRoughES2Technique->setEnabledLayers(layers);
    const auto cullingMode = doubleSided ? QCullFace::NoCulling : QCullFace::Back;
    m_metalRoughGL3Technique->setCullingMode(cullingMode);
    m_metalRoughES3Technique->setCullingMode(cullingMode);
    m_metalRoughES2Technique->setCullingMode(cullingMode);
    emit doubleSidedChanged(doubleSided);
}

void MetallicRoughnessEffect::setUseSkinning(bool useSkinning)
{
    if (useSkinning == m_useSkinning)
        return;
    m_useSkinning = useSkinning;
    emit useSkinningChanged(m_useSkinning);

    // Set Layers on zFill and opaque/Transparent shader builders
    auto layers = m_metalRoughGL3Technique->enabledLayers();
    if (m_useSkinning) {
        layers.removeAll(QStringLiteral("no-skinning"));
        layers.append(QStringLiteral("skinning"));
    } else {
        layers.removeAll(QStringLiteral("no-skinning"));
        layers.append(QStringLiteral("skinning"));
    }

    m_metalRoughGL3Technique->setEnabledLayers(layers);
    m_metalRoughES3Technique->setEnabledLayers(layers);
    m_metalRoughES2Technique->setEnabledLayers(layers);
    m_metalRoughGL3Technique->setAllowCulling(!m_useSkinning);
    m_metalRoughES3Technique->setAllowCulling(!m_useSkinning);
    m_metalRoughES2Technique->setAllowCulling(!m_useSkinning);
}

void MetallicRoughnessEffect::setOpaque(bool opaque)
{
    if (opaque == m_opaque)
        return;
    m_opaque = opaque;
    m_metalRoughGL3Technique->setOpaque(opaque);
    m_metalRoughES3Technique->setOpaque(opaque);
    m_metalRoughES2Technique->setOpaque(opaque);
    if (opaque)
        setAlphaCutoffEnabled(false);

    emit opaqueChanged(opaque);
}

void MetallicRoughnessEffect::setAlphaCutoffEnabled(bool enabled)
{
    if (m_alphaCutoffEnabled == enabled)
        return;

    auto layers = m_metalRoughGL3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noHasAlphaCutoff"));
        layers.append(QStringLiteral("hasAlphaCutoff"));
    } else {
        layers.removeAll(QStringLiteral("hasAlphaCutoff"));
        layers.append(QStringLiteral("noHasAlphaCutoff"));
    }
    m_alphaCutoffEnabled = enabled;
    m_metalRoughGL3Technique->setEnabledLayers(layers);
    m_metalRoughES3Technique->setEnabledLayers(layers);
    m_metalRoughES2Technique->setEnabledLayers(layers);
    emit alphaCutoffEnabledChanged(enabled);
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

} // namespace Kuesa

QT_END_NAMESPACE
