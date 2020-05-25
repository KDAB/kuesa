/*
    gaussianblureffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "forwardrenderer.h"
#include "fullscreenquad.h"
#include "fxutils_p.h"
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
 * \brief Post-processing effect for blurring the scene.
 *
 * GaussianBlurEffect is a post-processing effect that applies a
 * Gaussian blur to the scene. The amount of blurring can be adjusted
 * using the blurPassCount property.
 *
 * \badcode
 * #include <Qt3DExtras/Qt3DWindow>
 * #include <ForwardRenderer>
 * #include <SceneEntity>
 * #include <GaussianBlurEffect>
 *
 * Qt3DExtras::Qt3DWindow win;
 * Kuesa::SceneEntity *root = new Kuesa::SceneEntity();
 * Kuesa::ForwardRenderer *frameGraph = new Kuesa::ForwardRenderer();
 * Kuesa::GaussianBlurEffect *blurEffect = new Kuesa::GaussianBlurEffect();
 *
 * blurEffect->setBlurPassCount(2);
 *
 * frameGraph->addPostProcessingEffect(blurEffect);
 *
 * win->setRootEntity(root);
 * win->setActiveFrameGraph(forwardRenderer);
 *
 * ...
 * \endcode
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
 * \qmltype GaussianBlurEffect
 *  \inherits AbstractPostProcessingEffect
 * \inqmlmodule Kuesa
 * \since Kuesa 1.0
 * \brief Post-processing effect for blurring the scene.
 *
 * GaussianBlurEffect is a post-processing effect that applies a
 * Gaussian blur to the scene. The amount of blurring can be adjusted
 * using the blurPassCount property.
 *
 * \badcode
 * import Kuesa 1.1 as Kuesa
 *
 * Kuesa.SceneEnity {
 *     id: root
 *     components: [
 *         RenderSettings {
 *              Kuesa.ForwardRenderer {
 *                  postProcessingEffects: [
 *                      GaussianBlurEffect {
 *                          blurPassCount: 2
 *                      }
 *                  ]
 *              }
 *         }
 *     ]
 *    ...
 * }
 * \endcode
 *
 * \section3 Example
 * \image fx/blur/blur.png
 * \caption Blur effect applied on a Kuesa scene.
 *
 * \note Increasing the number of passes too high may adversely impact
 * rendering performance, especially on lower-end GPUs.
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

    //make blurTargets children of m_rootFrameGraphNode, otherwise they'll automatically
    //be parented to the blur pass nodes which get deleted when changing # of passes
    m_blurTarget1->setParent(m_rootFrameGraphNode.get());
    m_blurTarget2->setParent(m_rootFrameGraphNode.get());

    // set up blur render targets.
    // Target 1 has no texture yet since we'll use the input texture
    m_blurTextureOutput1->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    m_blurTarget1->addOutput(m_blurTextureOutput1);

    auto blurOutput2 = new Qt3DRender::QRenderTargetOutput;
    blurOutput2->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    m_blurTarget2->addOutput(blurOutput2);

    m_blurTexture2 = new Qt3DRender::QTexture2D;
    m_blurTexture2->setFormat(ForwardRenderer::hasHalfFloatRenderable() ? Qt3DRender::QAbstractTexture::RGBA16F :  Qt3DRender::QAbstractTexture::RGBA8U);
    m_blurTexture2->setGenerateMipMaps(false);
    m_blurTexture2->setSize(512, 512);
    blurOutput2->setTexture(m_blurTexture2);

    m_blurTextureParam2->setValue(QVariant::fromValue(m_blurTexture2));

    // Set up GaussianBlur Material
    auto blurMaterial = new Qt3DRender::QMaterial(m_rootFrameGraphNode.data());
    auto effect = new Qt3DRender::QEffect;
    blurMaterial->setEffect(effect);

    auto makeTechnique = [this](Qt3DRender::QGraphicsApiFilter::Api api,
                                int majorVersion, int minorVersion,
                                Qt3DRender::QGraphicsApiFilter::OpenGLProfile profile,
                                const QString &vertexShader) -> Qt3DRender::QTechnique * {
        auto *technique = FXUtils::makeTechnique(api, majorVersion, minorVersion, profile);

        // create shader
        auto blurShader = new Qt3DRender::QShaderProgram(technique);
        blurShader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(vertexShader)));
        auto shaderBuilder = new Qt3DRender::QShaderProgramBuilder(blurShader);
        shaderBuilder->setShaderProgram(blurShader);
        shaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/gaussianblur.frag.json")));

        auto createAndAddBlurPass = [technique, blurShader](const QString &passName, int pass, Qt3DRender::QParameter *textureParam) -> Qt3DRender::QRenderPass * {
            auto blurPass = FXUtils::createRenderPass(passName, pass);
            blurPass->addParameter(new Qt3DRender::QParameter(QStringLiteral("pass"), pass));
            blurPass->addParameter(textureParam);
            blurPass->setShaderProgram(blurShader);
            technique->addRenderPass(blurPass);
            return blurPass;
        };

        createAndAddBlurPass(passName(), 1, m_blurTextureParam1);
        createAndAddBlurPass(passName(), 2, m_blurTextureParam2);

        return technique;
    };

    auto *gl3Technique = makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGL,
                                       3, 2,
                                       Qt3DRender::QGraphicsApiFilter::CoreProfile,
                                       QStringLiteral("qrc:/kuesa/shaders/gl3/passthrough.vert"));

    effect->addTechnique(gl3Technique);

    auto *es3Technique = makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGLES,
                                       3, 0,
                                       Qt3DRender::QGraphicsApiFilter::NoProfile,
                                       QStringLiteral("qrc:/kuesa/shaders/es3/passthrough.vert"));

    effect->addTechnique(es3Technique);

    auto *es2Technique = makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGLES,
                                       2, 0,
                                       Qt3DRender::QGraphicsApiFilter::NoProfile,
                                       QStringLiteral("qrc:/kuesa/shaders/es2/passthrough.vert"));

    effect->addTechnique(es2Technique);

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

        auto blurPassFilterA = FXUtils::createRenderPassFilter(passName(), 1);
        blurPassFilterA->setParent(blurTargetSelectorA);

        auto blurTargetSelectorB = new Qt3DRender::QRenderTargetSelector(m_blurPassRoot);
        blurTargetSelectorB->setTarget(m_blurTarget1);

        auto blurPassFilterB = FXUtils::createRenderPassFilter(passName(), 2);
        blurPassFilterB->setParent(blurTargetSelectorB);
    }
    auto blurTargetSelectorA = new Qt3DRender::QRenderTargetSelector(m_blurPassRoot);
    blurTargetSelectorA->setTarget(m_blurTarget2);

    auto blurPassFilterA = FXUtils::createRenderPassFilter(passName(), 1);
    blurPassFilterA->setParent(blurTargetSelectorA);

    //render one final blur, but not into any any render target
    auto blurPassFilterB = FXUtils::createRenderPassFilter(passName(), 2);
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
 * Sets the input texture for the effect to \a texture.
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
 * Sets the size of the rendered scene in pixels to \a size. This is required
 * to keep a consistent amount of blurring when the scene is resized.
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
 * Sets the number of blur passes to \a blurPassCount.
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

QString GaussianBlurEffect::passName() const
{
    return QStringLiteral("blurPass");
}

QT_END_NAMESPACE
