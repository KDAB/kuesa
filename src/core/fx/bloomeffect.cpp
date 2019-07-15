/*
    bloomeffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "fxutils_p.h"
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
 * \since Kuesa 1.0
 * \brief Post-processing effect for blurring the bright parts of a scene
 *
 * BloomEffect is a post-processing effect that applies a blur to the brightest
 * parts of scene to simulate a fuzzy glow.
 *
 * The effect can be configured to change the threshold defining the bright parts of
 * the scene and the amount of blurring to apply.
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
    \qmlproperty real BloomEffect::threshold

    \brief the brightness level determining which pixels the effect are applied to

    This value determines how bright something has to be for the bloom effect to be
    applied to it. Any pixels below the threshold are ignored.
*/

/*!
    \qmlproperty int BloomEffect::blurPassCount

    \brief the number of blur passes

    This is the number of times to apply the blur to the bright parts of the scene.
    More passes result in stronger blurring effect but take longer to render.
*/

BloomEffect::BloomEffect(Qt3DCore::QNode *parent)
    : AbstractPostProcessingEffect(parent)
    , m_sceneTextureParam(new Qt3DRender::QParameter(QStringLiteral("texture0"), nullptr))
    , m_blurredBrightTextureParam(new Qt3DRender::QParameter(QStringLiteral("texture1"), nullptr))
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

    const QString graphPath = QStringLiteral("qrc:/kuesa/shaders/graphs/bloom.frag.json");
    const QString passFilterValue = QStringLiteral("KuesaBloomPass");
    const QString passFilterName = QStringLiteral("passName");

    auto *gl3Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGL,
                                                3, 2,
                                                Qt3DRender::QGraphicsApiFilter::CoreProfile,
                                                QStringLiteral("qrc:/kuesa/shaders/gl3/passthrough.vert"),
                                                graphPath,
                                                passFilterName, passFilterValue);
    effect->addTechnique(gl3Technique);

    auto *es3Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGLES,
                                                3, 0,
                                                Qt3DRender::QGraphicsApiFilter::NoProfile,
                                                QStringLiteral("qrc:/kuesa/shaders/es3/passthrough.vert"),
                                                graphPath,
                                                passFilterName, passFilterValue);
    effect->addTechnique(es3Technique);

    auto *es2Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGLES,
                                                2, 0,
                                                Qt3DRender::QGraphicsApiFilter::NoProfile,
                                                QStringLiteral("qrc:/kuesa/shaders/es2/passthrough.vert"),
                                                graphPath,
                                                passFilterName, passFilterValue);
    effect->addTechnique(es2Technique);

    m_blurredBrightTextureParam->setValue(QVariant::fromValue(m_blurredBrightTexture));
    effect->addParameter(m_sceneTextureParam);
    effect->addParameter(m_blurredBrightTextureParam);

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

    //create RenderPassFilter parented to layerFilter
    FXUtils::createRenderPassFilter(passFilterName, passFilterValue, bloomLayerFilter);
}

BloomEffect::~BloomEffect()
{
    // need to unparent effect framegraphs otherwise they'll get deleted twice
    m_thresholdEffect->frameGraphSubTree()->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
    m_blurEffect->frameGraphSubTree()->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
}

Qt3DRender::QRenderTarget *BloomEffect::createRenderTarget(Qt3DRender::QAbstractTexture *texture)
{
    auto renderTarget = new Qt3DRender::QRenderTarget(this);
    auto output = new Qt3DRender::QRenderTargetOutput;
    output->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    renderTarget->addOutput(output);
    texture->setFormat(Qt3DRender::QAbstractTexture::RGBA16F);
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

QT_END_NAMESPACE
