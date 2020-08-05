/*
    tonemappingandgammacorrectioneffect.cpp

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

#include "tonemappingandgammacorrectioneffect.h"
#include "fullscreenquad.h"
#include <private/fxutils_p.h>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgramBuilder>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QAbstractTexture>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::ToneMappingAndGammaCorrectionEffect
    \since Kuesa 1.1
    \inmodule Kuesa
    \brief Perform conversion from linear color space to sRGB space.
    \inherits Kuesa::AbstractPostProcessingEffect

    This effects performs exposure correction, tone mapping and gamma
    correction in this order.

    It uses a gamma of 2.2 to perform the correction, but this can be
    overridden if required.

    \note If using the ForwardRenderer FrameGraph, this effect is automatically
    added as the last step of the pipeline, after any user defined post
    processing effect. Therefore, you shouldn't need to instantiate this effect
    yourself if using that FrameGraph.

    If you care about Tone Mapping implementation details, the algorithms in
    use are detailly explained \l
    {http://filmicworlds.com/blog/filmic-tonemapping-operators/} {here}.
 */

/*!
    \qmltype ToneMappingAndGammaCorrectionEffect
    \since Kuesa 1.1
    \inqmlmodule Kuesa
    \brief Perform conversion from linear color space to sRGB space.
    \inherits AbstractPostProcessingEffect

    This effects performs exposure correction, tone mapping and gamma
    correction in this order.

    It uses a gamma of 2.2 to perform the correction, but this can be
    overridden if required.

    \note If using the ForwardRenderer FrameGraph, this effect is automatically
    added as the last step of the pipeline, after any user defined post
    processing effect. Therefore, you shouldn't need to instantiate this effect
    yourself if using that FrameGraph.

    If you care about Tone Mapping implementation details, the algorithms in
    use are detailly explained \l
    {http://filmicworlds.com/blog/filmic-tonemapping-operators/} {here}.
 */

/*!
    \enum ToneMappingAndGammaCorrectionEffect::ToneMapping

    This enum lists the various Tone Mapping Algorithms

    \value None No Tone Mapping (default)
    \value Reinhard Reinhard Tone Mapping
    \value Filmic Filmic Tone Mapping
    \value Uncharted Hable John's Uncharted Tone Mapping
 */

/*!
    \property ToneMappingAndGammaCorrectionEffect::toneMappingAlgorithm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \default ToneMappingAndGammaCorrectionEffect::None
 */

/*!
    \qmlproperty ToneMappingAndGammaCorrectionEffect.ToneMapping ToneMappingAndGammaCorrectionEffect::toneMappingAlgoritm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \default ToneMappingAndGammaCorrectionEffect.None
 */

/*!
    \property Kuesa::ToneMappingAndGammaCorrectionEffect::gamma

    Holds the gamma value to use for gamma correction conversion
    that brings linear colors to sRGB colors.
    \default 2.2
 */

/*!
    \qmlproperty real Kuesa::ToneMappingAndGammaCorrectionEffect::gamma

    Holds the gamma value to use for gamma correction conversion
    that brings linear colors to sRGB colors.
    \default 2.2
 */

/*!
    \property Kuesa::ToneMappingAndGammaCorrectionEffect::exposure
    Exposure correction factor used before the linear to sRGB conversion.
    \default 0
 */

/*!
    \qmlproperty real Kuesa::ToneMappingAndGammaCorrectionEffect::exposure
    Exposure correction factor used before the linear to sRGB conversion.
    \default 0
 */

namespace {

QString shaderGraphLayerForToneMappingAlgorithm(ToneMappingAndGammaCorrectionEffect::ToneMapping algorithm)
{
    switch (algorithm) {
    case ToneMappingAndGammaCorrectionEffect::Reinhard:
        return QStringLiteral("useReinhardToneMap");
    case ToneMappingAndGammaCorrectionEffect::Filmic:
        return QStringLiteral("useFilmicToneMap");
    case ToneMappingAndGammaCorrectionEffect::Uncharted:
        return QStringLiteral("useUnchartedToneMap");
    case ToneMappingAndGammaCorrectionEffect::None:
        break;
    }
    return QStringLiteral("noToneMap");
}

} // namespace

ToneMappingAndGammaCorrectionEffect::ToneMappingAndGammaCorrectionEffect(Qt3DCore::QNode *parent)
    : AbstractPostProcessingEffect(parent)
    , m_layer(nullptr)
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    , m_rhiShaderBuilder(nullptr)
#endif
    , m_gl3ShaderBuilder(nullptr)
    , m_es3ShaderBuilder(nullptr)
    , m_es2ShaderBuilder(nullptr)
    // Purposely use finalExposure (because Qt3D also defines a per camera exposure uniform)
    , m_exposureParameter(new Qt3DRender::QParameter(QStringLiteral("finalExposure"), 1.0f))
    , m_gammaParameter(new Qt3DRender::QParameter(QStringLiteral("gamma"), 2.2f))
    , m_inputTextureParameter(new Qt3DRender::QParameter(QStringLiteral("inputTexture"), nullptr))
    , m_toneMappingAlgorithm(ToneMapping::None)
{
    QObject::connect(m_gammaParameter, &Qt3DRender::QParameter::valueChanged,
                     this, [this](QVariant value) { emit gammaChanged(value.toFloat()); });
    QObject::connect(m_exposureParameter, &Qt3DRender::QParameter::valueChanged,
                     this, [this](QVariant value) { emit exposureChanged(value.toFloat()); });

    m_rootFrameGraphNode.reset(new Qt3DRender::QRenderStateSet());
    m_rootFrameGraphNode->setObjectName(QStringLiteral("ToneMappingAndGammaCorrectionEffect"));

    // Set up Material
    Qt3DRender::QMaterial *gammaCorrectionMaterial = new Qt3DRender::QMaterial(m_rootFrameGraphNode.data());
    const QString passName = QStringLiteral("passName");
    const QString passFilterValue = QStringLiteral("gammaCorrectionPass");
    {
        Qt3DRender::QEffect *effect = new Qt3DRender::QEffect();
        gammaCorrectionMaterial->setEffect(effect);
        const QStringList enabledShaderLayers = { shaderGraphLayerForToneMappingAlgorithm(m_toneMappingAlgorithm) };

        m_gl3ShaderBuilder = new Qt3DRender::QShaderProgramBuilder();
        m_gl3ShaderBuilder->setEnabledLayers(enabledShaderLayers);
        m_gl3ShaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/gammacorrection.frag.json")));

        m_es3ShaderBuilder = new Qt3DRender::QShaderProgramBuilder();
        m_es3ShaderBuilder->setEnabledLayers(enabledShaderLayers);
        m_es3ShaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/gammacorrection.frag.json")));

        m_es2ShaderBuilder = new Qt3DRender::QShaderProgramBuilder();
        m_es2ShaderBuilder->setEnabledLayers(enabledShaderLayers);
        m_es2ShaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/gammacorrection.frag.json")));

        auto *gl3Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGL,
                                                    3, 2,
                                                    Qt3DRender::QGraphicsApiFilter::CoreProfile,
                                                    QStringLiteral("qrc:/kuesa/shaders/gl3/passthrough.vert"),
                                                    m_gl3ShaderBuilder,
                                                    passName, passFilterValue);
        effect->addTechnique(gl3Technique);

        auto *es3Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGLES,
                                                    3, 0,
                                                    Qt3DRender::QGraphicsApiFilter::NoProfile,
                                                    QStringLiteral("qrc:/kuesa/shaders/es3/passthrough.vert"),
                                                    m_es3ShaderBuilder,
                                                    passName, passFilterValue);
        effect->addTechnique(es3Technique);

        auto *es2Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGLES,
                                                    2, 0,
                                                    Qt3DRender::QGraphicsApiFilter::NoProfile,
                                                    QStringLiteral("qrc:/kuesa/shaders/es2/passthrough.vert"),
                                                    m_es2ShaderBuilder,
                                                    passName, passFilterValue);
        effect->addTechnique(es2Technique);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        m_rhiShaderBuilder = new Qt3DRender::QShaderProgramBuilder();
        m_rhiShaderBuilder->setEnabledLayers(enabledShaderLayers);
        m_rhiShaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/gammacorrection.frag.json")));

        auto *rhiTechnique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::RHI,
                                                    1, 0,
                                                    Qt3DRender::QGraphicsApiFilter::NoProfile,
                                                    QStringLiteral("qrc:/kuesa/shaders/gl45/passthrough.vert"),
                                                    m_rhiShaderBuilder,
                                                    passName, passFilterValue);
        effect->addTechnique(rhiTechnique);
#endif


        effect->addParameter(m_gammaParameter);
        effect->addParameter(m_inputTextureParameter);
        effect->addParameter(m_exposureParameter);
    }

    // Set up FrameGraph
    {
        auto fullScreenQuad = new FullScreenQuad(gammaCorrectionMaterial, m_rootFrameGraphNode.data());
        m_layer = fullScreenQuad->layer();

        // Set up FrameGraph
        auto layerFilter = new Qt3DRender::QLayerFilter(m_rootFrameGraphNode.data());
        layerFilter->addLayer(m_layer);

        //create RenderPassFilter parented to layerFilter
        FXUtils::createRenderPassFilter(passName, passFilterValue, layerFilter);
    }
}

ToneMappingAndGammaCorrectionEffect::~ToneMappingAndGammaCorrectionEffect()
{
}

float ToneMappingAndGammaCorrectionEffect::exposure() const
{
    return m_exposureParameter->value().toFloat();
}

float ToneMappingAndGammaCorrectionEffect::gamma() const
{
    return m_gammaParameter->value().toFloat();
}

void ToneMappingAndGammaCorrectionEffect::setGamma(float gamma)
{
    m_gammaParameter->setValue(gamma);
}

AbstractPostProcessingEffect::FrameGraphNodePtr ToneMappingAndGammaCorrectionEffect::frameGraphSubTree() const
{
    return m_rootFrameGraphNode;
}

void ToneMappingAndGammaCorrectionEffect::setInputTexture(Qt3DRender::QAbstractTexture *texture)
{
    m_inputTextureParameter->setValue(QVariant::fromValue(texture));
}

QVector<Qt3DRender::QLayer *> ToneMappingAndGammaCorrectionEffect::layers() const
{
    return { m_layer };
}

void ToneMappingAndGammaCorrectionEffect::setExposure(float exposure)
{
    m_exposureParameter->setValue(exposure);
}

/*!
    Returns the tone mapping algorithm used by the shader.
    \default ToneMappingAndGammaCorrectionEffect::None
    \since Kuesa 1.1
 */
ToneMappingAndGammaCorrectionEffect::ToneMapping ToneMappingAndGammaCorrectionEffect::toneMappingAlgorithm() const
{
    return m_toneMappingAlgorithm;
}

/*!
    Sets the tone mapping algorithm to \a algorithm,
    \since Kuesa 1.1
*/
void ToneMappingAndGammaCorrectionEffect::setToneMappingAlgorithm(ToneMappingAndGammaCorrectionEffect::ToneMapping algorithm)
{
    if (m_toneMappingAlgorithm == algorithm)
        return;

    const QString oldLayerName = shaderGraphLayerForToneMappingAlgorithm(m_toneMappingAlgorithm);
    const QString newLayerName = shaderGraphLayerForToneMappingAlgorithm(algorithm);

    auto layers = m_gl3ShaderBuilder->enabledLayers();
    layers.removeAll(oldLayerName);
    layers.append(newLayerName);
    m_es2ShaderBuilder->setEnabledLayers(layers);
    m_es3ShaderBuilder->setEnabledLayers(layers);
    m_gl3ShaderBuilder->setEnabledLayers(layers);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    m_rhiShaderBuilder->setEnabledLayers(layers);
#endif

    m_toneMappingAlgorithm = algorithm;
    emit toneMappingAlgorithmChanged(algorithm);
}

} // namespace Kuesa

QT_END_NAMESPACE
