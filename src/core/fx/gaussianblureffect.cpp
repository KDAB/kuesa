/*
    gaussianblureffect.cpp

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

#include "gaussianblureffect.h"
#include "fullscreenquad.h"
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qrendertargetselector.h>

QT_BEGIN_NAMESPACE

using namespace Kuesa;

/*!
 * \class Kuesa::GaussianBlurEffect
 * \inheaderfile Kuesa/GaussianBlurEffect
 * \inmodule Kuesa
 * \since Kuesa 1.0
 * \brief Post-processing effect for blurring the scene
 *
 * GaussianBlurEffect is a post-processing effect that applies a
 * Gaussian blur to the scene. The amount of blurring can be adjusted
 * using the blurPassCount property.
 *
 * \section3 Example
 * \image fx/blur/blur.png
 * \caption Blur effect applied on a Kuesa scene.
 *
 * \note Increasing the number of passes too high may adversely impact
 * rendering performance, especially on lower-end GPUs.
 */

/*!
    \property GaussianBlurEffect::blurPassCount

    \brief the number of blur passes

    This is the number of times to apply the blur filter. More passes result in
    stronger blurring effect but take longer to render.
*/

/*!
    \qmlproperty int GaussianBlurEffect::blurPassCount

    \brief the number of blur passes

    This is the number of times to apply the blur filter. More passes result in
    stronger blurring effect but take longer to render.
*/

GaussianBlurEffect::GaussianBlurEffect(Qt3DCore::QNode *parent)
    : AbstractPostProcessingEffect(parent)
    , m_layer(nullptr)
    , m_blurPassCount(8)
    , m_blurTextureOutput1(new Qt3DRender::QRenderTargetOutput)
    , m_blurTarget1(new Qt3DRender::QRenderTarget)
    , m_blurTarget2(new Qt3DRender::QRenderTarget)
    , m_blurTexture1(nullptr)
    , m_blurTexture2(nullptr)
    , m_blurTextureParam1(new Qt3DRender::QParameter(QStringLiteral("textureSampler"), nullptr))
    , m_blurTextureParam2(new Qt3DRender::QParameter(QStringLiteral("textureSampler"), nullptr))
    , m_widthParameter(new Qt3DRender::QParameter(QStringLiteral("width"), 1))
    , m_heightParameter(new Qt3DRender::QParameter(QStringLiteral("height"), 1))
{
    m_rootFrameGraphNode.reset(new Qt3DRender::QFrameGraphNode);
    m_rootFrameGraphNode->setObjectName(QLatin1String("Gaussian Blur Effect"));

    //make blurTargets children of this, otherwise they'll automatically be parented
    //to the blur pass nodes which get deleted when changing # of passes
    m_blurTarget1->setParent(this);
    m_blurTarget2->setParent(this);

    // set up blur render targets.
    // Target 1 has no texture yet since we'll use the input texture
    m_blurTextureOutput1->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    m_blurTarget1->addOutput(m_blurTextureOutput1);

    auto blurOutput2 = new Qt3DRender::QRenderTargetOutput;
    blurOutput2->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    m_blurTarget2->addOutput(blurOutput2);

    m_blurTexture2 = new Qt3DRender::QTexture2D;
    m_blurTexture2->setFormat(Qt3DRender::QAbstractTexture::RGBA16F);
    m_blurTexture2->setGenerateMipMaps(false);
    m_blurTexture2->setSize(512, 512);
    blurOutput2->setTexture(m_blurTexture2);

    // Set up GaussianBlur Material
    auto blurMaterial = new Qt3DRender::QMaterial(m_rootFrameGraphNode.data());
    auto effect = new Qt3DRender::QEffect;
    blurMaterial->setEffect(effect);

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

    auto blurPass1 = createBlurPass(1);
    auto blurShader = new Qt3DRender::QShaderProgram(blurPass1);
    blurShader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/passthrough.vert"))));

    auto shaderBuilder = new Qt3DRender::QShaderProgramBuilder(blurShader);
    shaderBuilder->setShaderProgram(blurShader);
    shaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/gaussianblur.frag.json")));

    blurPass1->addParameter(m_blurTextureParam1);
    blurPass1->setShaderProgram(blurShader);
    technique->addRenderPass(blurPass1);

    auto blurPass2 = createBlurPass(2);
    blurPass2->addParameter(m_blurTextureParam2);
    m_blurTextureParam2->setValue(QVariant::fromValue(m_blurTexture2));
    blurPass2->setShaderProgram(blurShader);
    technique->addRenderPass(blurPass2);

    blurMaterial->addParameter(m_widthParameter);
    blurMaterial->addParameter(m_heightParameter);

    auto blurQuad = new FullScreenQuad(blurMaterial, m_rootFrameGraphNode.data());
    m_layer = blurQuad->layer();

    auto blurLayerFilter = new Qt3DRender::QLayerFilter(m_rootFrameGraphNode.data());
    blurLayerFilter->addLayer(blurQuad->layer());

    m_blurPassRoot = new Qt3DRender::QFrameGraphNode(blurLayerFilter);

    createBlurPasses();
}

/*!
 * \internal
 *
 * Helper function to create the correct number of blur passes and set their render
 * targets.
 */
void GaussianBlurEffect::createBlurPasses()
{
    for (int i = 0; i < m_blurPassCount - 1; ++i) {
        auto blurTargetSelectorA = new Qt3DRender::QRenderTargetSelector(m_blurPassRoot);
        blurTargetSelectorA->setTarget(m_blurTarget2);

        auto blurPassFilterA = createRenderPassFilter(passName(), 1);
        blurPassFilterA->setParent(blurTargetSelectorA);

        auto blurTargetSelectorB = new Qt3DRender::QRenderTargetSelector(m_blurPassRoot);
        blurTargetSelectorB->setTarget(m_blurTarget1);

        auto blurPassFilterB = createRenderPassFilter(passName(), 2);
        blurPassFilterB->setParent(blurTargetSelectorB);
    }
    auto blurTargetSelectorA = new Qt3DRender::QRenderTargetSelector(m_blurPassRoot);
    blurTargetSelectorA->setTarget(m_blurTarget2);

    auto blurPassFilterA = createRenderPassFilter(passName(), 1);
    blurPassFilterA->setParent(blurTargetSelectorA);

    //render one final blur, but not into any any render target
    auto blurPassFilterB = createRenderPassFilter(passName(), 2);
    blurPassFilterB->setParent(m_blurPassRoot);
}

/*!
 * Returns the frame graph subtree corresponding to the effect's implementation.
 *
 * \sa AbstractPostProcessingEffect::frameGraphSubTree
 */
AbstractPostProcessingEffect::FrameGraphNodePtr GaussianBlurEffect::frameGraphSubTree() const
{
    return m_rootFrameGraphNode;
}

QVector<Qt3DRender::QLayer *> GaussianBlurEffect::layers() const
{
    return QVector<Qt3DRender::QLayer *>{ m_layer };
}

/*!
 * Sets the input texture for the effect.
 *
 * \sa AbstractPostProcessingEffect::setInputTexture
 */
void GaussianBlurEffect::setInputTexture(Qt3DRender::QAbstractTexture *texture)
{
    // reuse the input texture as our blur texture 1
    m_blurTexture1 = texture;
    m_blurTextureParam1->setValue(QVariant::fromValue(texture));
    m_blurTextureOutput1->setTexture(texture);
}

/*!
 * Sets the size of the rendered scene in pixels. This is required to keep
 * a consistent amount of blurring when the scene is resized.
 *
 * \sa AbstractPostProcessingEffect::setSceneSize
 */
void GaussianBlurEffect::setSceneSize(const QSize &size)
{
    m_heightParameter->setValue(size.height());
    m_widthParameter->setValue(size.width());
    // only need to resize texture 2.
    // texture 1 is passed as "input texture" so should be resized elsewhere
    m_blurTexture2->setSize(size.width(), size.height());
}

/*!
 * Returns the number of blur passes.
 *
 * \sa GaussianBlurEffect::setBlurPassCount
 */
int GaussianBlurEffect::blurPassCount() const
{
    return m_blurPassCount;
}

/*!
 * Sets the number of blur passes.
 *
 * \sa GaussianBlurEffect::blurPassCount
 */
void GaussianBlurEffect::setBlurPassCount(int blurPassCount)
{
    if (m_blurPassCount == blurPassCount || blurPassCount == 0)
        return;

    m_blurPassCount = blurPassCount;

    //just do the simple thing for now and delete all children and recreate.
    const auto &childNodes = m_blurPassRoot->childNodes();
    for (auto child : childNodes)
        delete child;
    createBlurPasses();

    emit blurPassCountChanged(m_blurPassCount);
}

Qt3DRender::QRenderPassFilter *GaussianBlurEffect::createRenderPassFilter(const QString &name, const QVariant &value)
{
    auto filter = new Qt3DRender::QRenderPassFilter;
    auto filterKey = new Qt3DRender::QFilterKey;
    filterKey->setName(name);
    filterKey->setValue(value);
    filter->addMatch(filterKey);
    return filter;
}

Qt3DRender::QRenderPass *GaussianBlurEffect::createBlurPass(int pass)
{
    auto blurPass = new Qt3DRender::QRenderPass;
    auto blurPassFilterKey = new Qt3DRender::QFilterKey;
    blurPassFilterKey->setName(passName());
    blurPassFilterKey->setValue(pass);
    blurPass->addFilterKey(blurPassFilterKey);

    blurPass->addParameter(new Qt3DRender::QParameter(QStringLiteral("pass"), pass));
    return blurPass;
}

QString GaussianBlurEffect::passName() const
{
    return QStringLiteral("blurPass");
}

QT_END_NAMESPACE
