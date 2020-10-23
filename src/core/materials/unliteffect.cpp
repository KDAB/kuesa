/*
    unliteffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include "unliteffect.h"

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
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qblendequationarguments.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

class UnlitTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2,
        RHI
    };

    explicit UnlitTechnique(Version version, Qt3DCore::QNode *parent = nullptr);

    QStringList enabledLayers() const;
    void setEnabledLayers(const QStringList &layers);
    void setOpaque(bool opaque);
    void setCullingMode(QCullFace::CullingMode mode);
    QCullFace::CullingMode cullingMode() const;
    void setAllowCulling(bool allowCulling);

private:
    Qt3DRender::QCullFace *m_backFaceCulling;
    Qt3DRender::QBlendEquation *m_blendEquation;
    Qt3DRender::QBlendEquationArguments *m_blendArguments;
    Qt3DRender::QShaderProgramBuilder *m_unlitShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_zfillShaderBuilder;
    Qt3DRender::QShaderProgram *m_unlitShader;
    Qt3DRender::QShaderProgram *m_zfillShader;
    Qt3DRender::QRenderPass *m_zfillRenderPass;
    Qt3DRender::QRenderPass *m_opaqueRenderPass;
    Qt3DRender::QRenderPass *m_transparentRenderPass;
    Qt3DRender::QFilterKey *m_techniqueAllowFrustumCullingFilterKey;
};


/*!
    \class Kuesa::UnlitEffect
    \inheaderfile Kuesa/UnlitEffect
    \inmodule Kuesa
    \since Kuesa 1.1

    \brief Kuesa::UnlitEffect is a Qt3DRender::QEffect for
    Kuesa::UnlitMaterial. It provides several properties used to configure the
    material effect. This is done using a custom shader graph and
    activating/deactivating different set of nodes of the graph depending on
    the effect configuration.

    \note Kuesa::UnlitEffect is used to configure the effect for a material,
    but property values must be provided through Kuesa::UnlitMaterial.
    Therefore, this effect must be added to a Kuesa.UnlitMaterial.
 */

/*!
    \qmltype UnlitEffect
    \instantiates Kuesa::UnlitEffect
    \inqmlmodule Kuesa
    \since Kuesa 1.1

    \brief Kuesa.UnlitEffect is a Qt3D.Render.Effect for
    Kuesa.UnlitMaterial.

    It provides several properties used to configure the material effect. This
    is done using a custom shader graph and activating/deactivating different
    set of nodes of the graph depending on the effect configuration.

    \note Kuesa.UnlitEffect is used to configure the effect for a material, but
    property values must be provided through Kuesa.UnlitMaterial. Therefore,
    this effect must be added to a Kuesa.UnlitMaterial.

    \sa Kuesa::UnlitEffect
 */

UnlitTechnique::UnlitTechnique(UnlitTechnique::Version version, Qt3DCore::QNode *parent)
    : QTechnique(parent)
    , m_backFaceCulling(new QCullFace(this))
    , m_blendEquation(new Qt3DRender::QBlendEquation(this))
    , m_blendArguments(new Qt3DRender::QBlendEquationArguments(this))
    , m_unlitShaderBuilder(new QShaderProgramBuilder(this))
    , m_zfillShaderBuilder(new QShaderProgramBuilder(this))
    , m_unlitShader(new QShaderProgram(this))
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
    #if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        { 1, 0, QGraphicsApiFilter::RHI, QGraphicsApiFilter::NoProfile },
    #endif
    };

    graphicsApiFilter()->setApi(apiFilterInfos[version].api);
    graphicsApiFilter()->setProfile(apiFilterInfos[version].profile);
    graphicsApiFilter()->setMajorVersion(apiFilterInfos[version].major);
    graphicsApiFilter()->setMinorVersion(apiFilterInfos[version].minor);

    const auto vertexShaderGraph = QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json"));
    const auto fragmentShaderGraph = QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/unlit.frag.json"));

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

    m_unlitShaderBuilder->setShaderProgram(m_unlitShader);
    m_unlitShaderBuilder->setVertexShaderGraph(vertexShaderGraph);
    m_unlitShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph);

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

    m_opaqueRenderPass->setShaderProgram(m_unlitShader);
    m_opaqueRenderPass->addRenderState(m_backFaceCulling);
    m_opaqueRenderPass->addFilterKey(opaqueFilterKey);
    addRenderPass(m_opaqueRenderPass);

    auto transparentFilterKey = new Qt3DRender::QFilterKey(this);
    transparentFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    transparentFilterKey->setValue(QStringLiteral("Transparent"));

    m_blendEquation->setBlendFunction(Qt3DRender::QBlendEquation::Add);
    m_blendArguments->setSourceRgb(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    m_blendArguments->setSourceAlpha(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    m_blendArguments->setDestinationRgb(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    m_blendArguments->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::One);

    m_transparentRenderPass->setShaderProgram(m_unlitShader);
    m_transparentRenderPass->addRenderState(m_backFaceCulling);
    m_transparentRenderPass->addRenderState(m_blendEquation);
    m_transparentRenderPass->addRenderState(m_blendArguments);
    m_transparentRenderPass->addFilterKey(transparentFilterKey);
    m_transparentRenderPass->setEnabled(false);
    addRenderPass(m_transparentRenderPass);
}

QStringList UnlitTechnique::enabledLayers() const
{
    return m_unlitShaderBuilder->enabledLayers();
}

void UnlitTechnique::setEnabledLayers(const QStringList &layers)
{
    m_unlitShaderBuilder->setEnabledLayers(layers);
    m_zfillShaderBuilder->setEnabledLayers(layers);
}

void UnlitTechnique::setOpaque(bool opaque)
{
    m_zfillRenderPass->setEnabled(opaque);
    m_opaqueRenderPass->setEnabled(opaque);
    m_transparentRenderPass->setEnabled(!opaque);
}

void UnlitTechnique::setCullingMode(QCullFace::CullingMode mode)
{
    m_backFaceCulling->setMode(mode);
}

QCullFace::CullingMode UnlitTechnique::cullingMode() const
{
    return m_backFaceCulling->mode();
}

void UnlitTechnique::setAllowCulling(bool allowCulling)
{
    m_techniqueAllowFrustumCullingFilterKey->setValue(allowCulling);
}


UnlitEffect::UnlitEffect(Qt3DCore::QNode *parent)
    : GLTF2MaterialEffect(parent)
    , m_baseColorMapEnabled(false)
{

    const auto enabledLayers = QStringList{
            // Vertex Shader layers
            QStringLiteral("no-skinning"),
            // Fragment Shader layers
            QStringLiteral("noBaseColorMap"),
            QStringLiteral("noHasColorAttr"),
            QStringLiteral("noHasAlphaCutoff")
    };

    m_unlitGL3Technique = new UnlitTechnique(UnlitTechnique::GL3, this);
    m_unlitGL3Technique->setEnabledLayers(enabledLayers);

    m_unlitES3Technique = new UnlitTechnique(UnlitTechnique::ES3, this);
    m_unlitES3Technique->setEnabledLayers(enabledLayers);

    m_unlitES2Technique = new UnlitTechnique(UnlitTechnique::ES2, this);
    m_unlitES2Technique->setEnabledLayers(enabledLayers);

    addTechnique(m_unlitGL3Technique);
    addTechnique(m_unlitES3Technique);
    addTechnique(m_unlitES2Technique);

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_unlitRHITechnique = new UnlitTechnique(UnlitTechnique::RHI, this);
    m_unlitRHITechnique->setEnabledLayers(enabledLayers);

    addTechnique(m_unlitRHITechnique);
#endif
}

UnlitEffect::~UnlitEffect()
{
}

bool UnlitEffect::isBaseColorMapEnabled() const
{
    return m_baseColorMapEnabled;
}

void UnlitEffect::setBaseColorMapEnabled(bool enabled)
{
    if (m_baseColorMapEnabled == enabled)
        return;

    auto layers = m_unlitGL3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noBaseColorMap"));
        layers.append(QStringLiteral("baseColorMap"));
    } else {
        layers.removeAll(QStringLiteral("baseColorMap"));
        layers.append(QStringLiteral("noBaseColorMap"));
    }
    m_baseColorMapEnabled = enabled;
    m_unlitGL3Technique->setEnabledLayers(layers);
    m_unlitES3Technique->setEnabledLayers(layers);
    m_unlitES2Technique->setEnabledLayers(layers);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_unlitRHITechnique->setEnabledLayers(layers);
#endif
    emit baseColorMapEnabledChanged(enabled);
}

void UnlitEffect::updateDoubleSided(bool doubleSided)
{
    const auto cullingMode = doubleSided ? QCullFace::NoCulling : QCullFace::Back;
    m_unlitGL3Technique->setCullingMode(cullingMode);
    m_unlitES3Technique->setCullingMode(cullingMode);
    m_unlitES2Technique->setCullingMode(cullingMode);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_unlitRHITechnique->setCullingMode(cullingMode);
#endif
}

void UnlitEffect::updateUsingSkinning(bool useSkinning)
{
    // Set Layers on zFill and opaque/Transparent shader builders
    auto layers = m_unlitGL3Technique->enabledLayers();
    if (useSkinning) {
        layers.removeAll(QStringLiteral("no-skinning"));
        layers.append(QStringLiteral("skinning"));
    } else {
        layers.removeAll(QStringLiteral("skinning"));
        layers.append(QStringLiteral("no-skinning"));
    }

    m_unlitGL3Technique->setEnabledLayers(layers);
    m_unlitES3Technique->setEnabledLayers(layers);
    m_unlitES2Technique->setEnabledLayers(layers);
    m_unlitGL3Technique->setAllowCulling(!useSkinning);
    m_unlitES3Technique->setAllowCulling(!useSkinning);
    m_unlitES2Technique->setAllowCulling(!useSkinning);

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_unlitRHITechnique->setEnabledLayers(layers);
    m_unlitRHITechnique->setAllowCulling(!useSkinning);
#endif
}

void UnlitEffect::updateOpaque(bool opaque)
{
    m_unlitGL3Technique->setOpaque(opaque);
    m_unlitES3Technique->setOpaque(opaque);
    m_unlitES2Technique->setOpaque(opaque);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_unlitRHITechnique->setOpaque(opaque);
#endif
}

void UnlitEffect::updateAlphaCutoffEnabled(bool enabled)
{
    auto layers = m_unlitGL3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noHasAlphaCutoff"));
        layers.append(QStringLiteral("hasAlphaCutoff"));
    } else {
        layers.removeAll(QStringLiteral("hasAlphaCutoff"));
        layers.append(QStringLiteral("noHasAlphaCutoff"));
    }
    m_unlitGL3Technique->setEnabledLayers(layers);
    m_unlitES3Technique->setEnabledLayers(layers);
    m_unlitES2Technique->setEnabledLayers(layers);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_unlitRHITechnique->setEnabledLayers(layers);
#endif
}

void UnlitEffect::updateUsingColorAttribute(bool usingColorAttribute)
{
    auto layers = m_unlitGL3Technique->enabledLayers();
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

void UnlitEffect::updateUsingNormalAttribute(bool usingNormalAttribute)
{
    auto layers = m_unlitGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasVertexNormal"));
    if (usingNormalAttribute)
        layers.append(QStringLiteral("hasVertexNormal"));

    updateLayersOnTechniques(layers);
}

void UnlitEffect::updateUsingTangentAttribute(bool usingTangentAttribute)
{
    auto layers = m_unlitGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasVertexTangent"));
    if (usingTangentAttribute)
        layers.append(QStringLiteral("hasVertexTangent"));

    updateLayersOnTechniques(layers);
}

void UnlitEffect::updateUsingTexCoordAttribute(bool usingTexCoordAttribute)
{
    auto layers = m_unlitGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasTexCoord"));
    if (usingTexCoordAttribute)
        layers.append(QStringLiteral("hasTexCoord"));

    updateLayersOnTechniques(layers);
}

void UnlitEffect::updateUsingTexCoord1Attribute(bool usingTexCoord1Attribute)
{
    auto layers = m_unlitGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("hasTexCoord1"));
    if (usingTexCoord1Attribute)
        layers.append(QStringLiteral("hasTexCoord1"));

    updateLayersOnTechniques(layers);
}

void UnlitEffect::updateUsingMorphTargets(bool usingMorphTargets)
{
    auto layers = m_unlitGL3Technique->enabledLayers();
    layers.removeAll(QStringLiteral("morphtargets"));
    if (usingMorphTargets)
        layers.append(QStringLiteral("morphtargets"));

    updateLayersOnTechniques(layers);
}

void UnlitEffect::updateLayersOnTechniques(const QStringList &layers)
{
    m_unlitGL3Technique->setEnabledLayers(layers);
    m_unlitES3Technique->setEnabledLayers(layers);
    m_unlitES2Technique->setEnabledLayers(layers);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_unlitRHITechnique->setEnabledLayers(layers);
#endif
}

} // namespace Kuesa

QT_END_NAMESPACE
