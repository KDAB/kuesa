/*
    bloomeffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#include "bloomeffect.h"
#include "gaussianblureffect.h"
#include "thresholdeffect.h"
#include "fullscreenquad.h"
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qrendertargetselector.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qrendertarget.h>

QT_BEGIN_NAMESPACE

using namespace Kuesa;

/*!
 * \class Kuesa::BloomEffect
 * \inheaderfile Kuesa/BloomEffect
 * \inmodule Kuesa
 * \since 1.0
 * \brief Post-processing effect for blurring the bright parts of a scene
 *
 * BloomEffect is a post-processing effect that applies a blur to the brightest
 * parts of scene to simulate a fuzzy glow.
 *
 * The effect can be configured to change the threshold defining the bright parts of
 * the scene and the amount of blurring to apply.
 */

/*!
    \property BloomEffect::exposure

    \brief adjusts the overall brightness

    This adjusts the overall brightness of the result after applying the bloom.
    0.0 corresponds to no adjustment and each increment of 1 doubles the brightness.
*/

/*!
    \property BloomEffect::threshold

    \brief the brightness level determining which pixels the effect are applied to

    This value determines how bright something has to be for the bloom effect to be
    applied to it. Any pixels below the threshold are ignored.

    \sa ThresholdEffect::threshold
*/

/*!
    \property BloomEffect::blurPassCount

    \brief the number of blur passes

    This is the number of times to apply the blur to the bright parts of the scene.
    More passes result in stronger blurring effect but take longer to render.

    \sa GaussianBlurEffect::blurPassCount
*/

/*!
    \qmlproperty BloomEffect::exposure

    \brief adjusts the overall brightness

    This adjusts the overall brightness of the result after applying the bloom.
    0.0 corresponds to no adjustment and each increment of 1 doubles the brightness.
*/

/*!
    \qmlproperty BloomEffect::threshold

    \brief the brightness level determining which pixels the effect are applied to

    This value determines how bright something has to be for the bloom effect to be
    applied to it. Any pixels below the threshold are ignored.
*/

/*!
    \qmlproperty BloomEffect::blurPassCount

    \brief the number of blur passes

    This is the number of times to apply the blur to the bright parts of the scene.
    More passes result in stronger blurring effect but take longer to render.
*/

BloomEffect::BloomEffect(Qt3DCore::QNode *parent)
    : AbstractPostProcessingEffect(parent)
    , m_sceneTextureParam(new Qt3DRender::QParameter(QStringLiteral("texture0"), nullptr))
    , m_blurredBrightTextureParam(new Qt3DRender::QParameter(QStringLiteral("texture1"), nullptr))
    , m_exposureParam(new Qt3DRender::QParameter(QStringLiteral("exposure"), 0.0))
{
    m_rootFrameGraphNode.reset(new Qt3DRender::QFrameGraphNode);
    m_rootFrameGraphNode->setObjectName(QStringLiteral("Bloom Effect"));

    m_brightTexture = new Qt3DRender::QTexture2D;
    auto thresholdRenderTarget = createRenderTarget(m_brightTexture);

    m_blurredBrightTexture = new Qt3DRender::QTexture2D;
    auto blurRenderTarget = createRenderTarget(m_blurredBrightTexture);

    // Set up Threshold Material
    m_thresholdEffect = new ThresholdEffect(this);
    m_layers += m_thresholdEffect->layers();

    // Set up Gaussian Blur
    m_blurEffect = new GaussianBlurEffect(this);
    m_blurEffect->setInputTexture(m_brightTexture);
    m_layers += m_blurEffect->layers();

    // Set up Bloom Material
    auto bloomMaterial = new Qt3DRender::QMaterial(m_rootFrameGraphNode.data());

    auto effect = new Qt3DRender::QEffect;
    bloomMaterial->setEffect(effect);

    auto technique = new Qt3DRender::QTechnique;
    effect->addTechnique(technique);
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(3);
    technique->graphicsApiFilter()->setMinorVersion(2);
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    auto techniqueFilterKey = new Qt3DRender::QFilterKey;
    techniqueFilterKey->setName(QStringLiteral("renderingStyle"));
    techniqueFilterKey->setValue(QStringLiteral("forward"));
    technique->addFilterKey(techniqueFilterKey);

    auto bloomRenderPass = new Qt3DRender::QRenderPass;

    auto shaderProg = new Qt3DRender::QShaderProgram(bloomRenderPass);
    shaderProg->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/passthrough.vert"))));

    auto shaderBuilder = new Qt3DRender::QShaderProgramBuilder(shaderProg);
    shaderBuilder->setShaderProgram(shaderProg);
    shaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/bloom.frag.json")));

    bloomRenderPass->setShaderProgram(shaderProg);

    auto passFilterKey = new Qt3DRender::QFilterKey;
    passFilterKey->setName(passName());
    bloomRenderPass->addFilterKey(passFilterKey);

    bloomRenderPass->addParameter(m_sceneTextureParam);
    bloomRenderPass->addParameter(m_blurredBrightTextureParam);
    bloomRenderPass->addParameter(m_exposureParam);
    m_blurredBrightTextureParam->setValue(QVariant::fromValue(m_blurredBrightTexture));

    technique->addRenderPass(bloomRenderPass);

    auto bloomQuad = new FullScreenQuad(bloomMaterial, m_rootFrameGraphNode.data());
    m_layers.push_back(bloomQuad->layer());

    connect(m_thresholdEffect, &ThresholdEffect::thresholdChanged, this, &BloomEffect::thresholdChanged);
    connect(m_blurEffect, &GaussianBlurEffect::blurPassCountChanged, this, &BloomEffect::blurPassCountChanged);

    //
    //  FrameGraph Construction
    //

    // Threshold Pass
    auto thresholdTargetSelector = new Qt3DRender::QRenderTargetSelector(m_rootFrameGraphNode.data());
    thresholdTargetSelector->setTarget(thresholdRenderTarget);
    auto thresholdFrameGraph = m_thresholdEffect->frameGraphSubTree();
    thresholdFrameGraph->setParent(thresholdTargetSelector);

    // Blur Pass
    auto blurTargetSelector = new Qt3DRender::QRenderTargetSelector(m_rootFrameGraphNode.data());
    blurTargetSelector->setTarget(blurRenderTarget);
    auto blurFrameGraph = m_blurEffect->frameGraphSubTree();
    blurFrameGraph->setParent(blurTargetSelector);

    // Bloom Pass
    auto bloomLayerFilter = new Qt3DRender::QLayerFilter(m_rootFrameGraphNode.data());
    bloomLayerFilter->addLayer(bloomQuad->layer());
    auto bloomRenderPassFilter = createRenderPassFilter(passName());
    bloomRenderPassFilter->setParent(bloomLayerFilter);
}

BloomEffect::~BloomEffect()
{
    // need to unparent effect framegraphs otherwise they'll get deleted twice
    m_thresholdEffect->frameGraphSubTree()->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
    m_blurEffect->frameGraphSubTree()->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
}

Qt3DRender::QRenderPassFilter *BloomEffect::createRenderPassFilter(const QString &name, const QVariant &value)
{
    auto filter = new Qt3DRender::QRenderPassFilter;
    auto filterKey = new Qt3DRender::QFilterKey;
    filterKey->setName(name);
    filterKey->setValue(value);
    filter->addMatch(filterKey);
    return filter;
}

Qt3DRender::QRenderTarget *BloomEffect::createRenderTarget(Qt3DRender::QAbstractTexture *texture)
{
    auto renderTarget = new Qt3DRender::QRenderTarget(this);
    auto output = new Qt3DRender::QRenderTargetOutput;
    output->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    renderTarget->addOutput(output);
    texture->setFormat(Qt3DRender::QAbstractTexture::RGBA8_UNorm);
    texture->setSize(512, 512);
    texture->setGenerateMipMaps(false);
    output->setTexture(texture);
    return renderTarget;
}

/*!
 * Returns the frame graph subtree corresponding to the effect's implementation.
 *
 * \sa AbstractPostProcessingEffect::frameGraphSubTree
 */
AbstractPostProcessingEffect::FrameGraphNodePtr BloomEffect::frameGraphSubTree() const
{
    return m_rootFrameGraphNode;
}

/*!
 * Sets the size of the rendered scene in pixels.
 *
 * \sa AbstractPostProcessingEffect::setSceneSize
 */
void BloomEffect::setSceneSize(const QSize &size)
{
    m_blurEffect->setSceneSize(size);
    m_thresholdEffect->setSceneSize(size);
    m_brightTexture->setSize(size.width(), size.height());
    m_blurredBrightTexture->setSize(size.width(), size.height());
}

/*!
 * Sets the input texture for the effect.
 *
 * \sa AbstractPostProcessingEffect::setInputTexture
 */
void BloomEffect::setInputTexture(Qt3DRender::QAbstractTexture *texture)
{
    m_thresholdEffect->setInputTexture(texture);
    m_sceneTextureParam->setValue(QVariant::fromValue(texture));
}

QVector<Qt3DRender::QLayer *> BloomEffect::layers() const
{
    return m_layers;
}

/*!
 * Returns the exposure value.
 *
 * \sa BloomEffect::setExposure
 */
float BloomEffect::exposure() const
{
    return m_exposureParam->value().toFloat();
}

/*!
 * Returns the threshold value.
 *
 * \sa BloomEffect::SetThreshold
 */
float BloomEffect::threshold() const
{
    return m_thresholdEffect->threshold();
}

/*!
 * Returns the number of blur passes to apply.
 *
 * \sa BloomEffect::setBlurPassCount
 */
int BloomEffect::blurPassCount() const
{
    return m_blurEffect->blurPassCount();
}

/*!
 * Sets the exposure value.
 *
 * \sa BloomEffect::exposure
 */
void BloomEffect::setExposure(float exposure)
{
    if (qFuzzyCompare(m_exposureParam->value().toFloat(), exposure))
        return;

    m_exposureParam->setValue(exposure);
    emit exposureChanged(exposure);
}

/*!
 * Sets the threshold value.
 *
 * \sa BloomEffect::threshold
 */
void BloomEffect::setThreshold(float threshold)
{
    m_thresholdEffect->setThreshold(threshold);
}

/*!
 * Sets the number of blur passes to apply.
 *
 * \sa BloomEffect::blurPassCount
 */
void BloomEffect::setBlurPassCount(int blurPassCount)
{
    m_blurEffect->setBlurPassCount(blurPassCount);
}

QString BloomEffect::passName() const
{
    return QStringLiteral("bloomPass");
}

QT_END_NAMESPACE
