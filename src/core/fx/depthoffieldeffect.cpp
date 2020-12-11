/*
    DepthOfFieldEffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jean-Michaël Celerier <jean-michael.celerier@kdab.com>

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

#include "depthoffieldeffect.h"
#include "fullscreenquad.h"
#include "fx/fxutils_p.h"
#include "framegraphutils_p.h"

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
#include <QTimer>
#include <QVector2D>

QT_BEGIN_NAMESPACE
/**
 * Useful resources:
 * https://on-demand.gputechconf.com/gtc/2014/presentations/S4385-order-independent-transparency-opengl.pdf
 *
 * Evenly spaced points computed with Vogel's method:
 * https://www.arl.army.mil/arlreports/2015/ARL-TR-7333.pdf
 *
 * https://catlikecoding.com/unity/tutorials/advanced-rendering/depth-of-field/
 * https://www.slideshare.net/TiagoAlexSousa/graphics-gems-from-cryengine-3-siggraph-2013
 *
 * https://fileadmin.cs.lth.se/cs/education/edan35/lectures/12dof.pdf
 * https://en.wikibooks.org/wiki/OpenGL_Programming/Depth_of_Field
 * https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch23.html (a bit old ?)
 * https://bartwronski.com/2014/04/07/bokeh-depth-of-field-going-insane-part-1/ (fairly advanced)
 * https://gpupro.blogspot.com/2013/09/gpu-pro-4-skylanders-depth-of-field.html
 * https://www.guerrilla-games.com/read/killzone-shadow-fall-demo-postmortem
 * https://www.slideshare.net/DAMSIGNUP/so4-flexible-shadermanagmentandpostprocessing
 */

using namespace Kuesa;

/*!
 \class Kuesa::DepthOfFieldEffect
 \inheaderfile Kuesa/DepthOfFieldEffect
 \inmodule Kuesa
 \since Kuesa 1.1
 \brief Post-processing effect implementation of a depth of field.

 DepthOfFieldEffect implements an effect which simulates a more realistic
 camera behaviour, where only part of the scene is in-focus, and very far &
 very close objects are out-of-focus and thus blurred.

 \badcode
 #include <Qt3DExtras/Qt3DWindow>
 #include <ForwardRenderer>
 #include <SceneEntity>
 #include <DepthOfFieldEffect>

 Qt3DExtras::Qt3DWindow win;
 Kuesa::SceneEntity *root = new Kuesa::SceneEntity();
 Kuesa::ForwardRenderer *frameGraph = new Kuesa::ForwardRenderer();
 Kuesa::DepthOfFieldEffect *dofEffect = new Kuesa::DepthOfFieldEffect();

 dofEffect->setRadius(10.5f);
 dofEffect->setFocusDistance(100.0f);
 dofEffect->setFocusRange(25.0f);

 frameGraph->addPostProcessingEffect(dofEffect);

 win->setRootEntity(root);
 win->setActiveFrameGraph(forwardRenderer);
 ...
 \endcode

 \section3 Example
 \image fx/dof/dof_distance_5.png
 \caption Depth-of-field effect applied on a Kuesa scene.
*/

/*!
 \property DepthOfFieldEffect::focusRange

 \brief adjusts the breadth of the area which will be in focus.

 This adjusts the size of the part of the scene which will be in focus.
 At 0.0, everything will be blurred. The bigger the value, the more of the
 scene will be in focus, centered on the focusDistance.

 \table
 \header
     \li Range 2.0
     \li Range 8.0
 \row
     \li \inlineimage fx/dof/dof_range_2.png
     \li \inlineimage fx/dof/dof_range_8.png
 \endtable

 \sa DepthOfFieldEffect::focusDistance
*/

/*!
 \property DepthOfFieldEffect::focusDistance

 \brief adjusts the focal distance.

 This adjusts the focal distance, that is, the objects at this distance from
 the camera will be focused; those too close or too far away will be blurred.

 \table
 \header
     \li Distance 5.0
     \li Distance 8.0
 \row
     \li \inlineimage fx/dof/dof_distance_5.png
     \li \inlineimage fx/dof/dof_distance_8.png
 \endtable

 \sa DepthOfFieldEffect::focusRange
*/

/*!
 \property DepthOfFieldEffect::radius

 \brief adjusts the radius of the depth of field effect.

 The bigger the radius is, the more the out-of-focus objects will appear to be blurred.

 \table
 \header
     \li Radius 4.0
     \li Radius 42.0
 \row
     \li \inlineimage fx/dof/dof_radius_4.png
     \li \inlineimage fx/dof/dof_radius_42.png
 \endtable
*/

/*!
 \qmltype DepthOfFieldEffect
 \inherits AbstractPostProcessingEffect
 \inmodule Kuesa
 \since Kuesa 1.1
 \brief Post-processing effect implementation of a depth of field.

 DepthOfFieldEffect implements an effect which simulates a more realistic
 camera behaviour, where only part of the scene is in-focus, and very far &
 very close objects are out-of-focus and thus blurred.

 \badcode
 import Kuesa 1.1 as Kuesa
 import Kuesa.Effects 1.1

 Kuesa.SceneEnity {
     id: root
     components: [
         RenderSettings {
             Kuesa.ForwardRenderer {
                 postProcessingEffects: [
                     DepthOfFieldEffect {
                         radius: 10.0
                         focusDistance: 100.0f
                         focusRange: 25.0f
                     }
                 ]
             }
         }
     ]
     ...
 }
 \endcode

 \section3 Example
 \image fx/dof/dof_distance_5.png
 \caption Depth-of-field effect applied on a Kuesa scene.
*/

/*!
 \qmlproperty float DepthOfFieldEffect::focusRange

 \brief adjusts the breadth of the area which will be in focus.

 This adjusts the size of the part of the scene which will be in focus.
 At 0.0, everything will be blurred. The bigger the value, the more of the
 scene will be in focus, centered on the focusDistance.

 \table
 \header
     \li Range 2.0
     \li Range 8.0
 \row
     \li \inlineimage fx/dof/dof_range_2.png
     \li \inlineimage fx/dof/dof_range_8.png
 \endtable

 \sa DepthOfFieldEffect::focusDistance
*/

/*!
 \qmlproperty float DepthOfFieldEffect::focusDistance

 \brief adjusts the focal distance.

 This adjusts the focal distance, that is, the objects at this distance from
 the camera will be focused; those too close or too far away will be blurred.

 \table
 \header
     \li Distance 5.0
     \li Distance 8.0
 \row
     \li \inlineimage fx/dof/dof_distance_5.png
     \li \inlineimage fx/dof/dof_distance_8.png
 \endtable

 \sa DepthOfFieldEffect::focusRange
*/

/*!
 \qmlproperty float DepthOfFieldEffect::radius

 \brief adjusts the radius of the depth of field effect.

 The bigger the radius is, the more the out-of-focus objects will appear to be blurred.

 \table
 \header
     \li Radius 4.0
     \li Radius 42.0
 \row
     \li \inlineimage fx/dof/dof_radius_4.png
     \li \inlineimage fx/dof/dof_radius_42.png
 \endtable
*/

DepthOfFieldEffect::DepthOfFieldEffect(Qt3DCore::QNode *parent)
    : AbstractPostProcessingEffect(parent)
    , m_layer(nullptr)
    , m_textureParam(new Qt3DRender::QParameter(QStringLiteral("textureSampler"), nullptr))
    , m_textureSizeParam(new Qt3DRender::QParameter(QStringLiteral("texSize"), nullptr))
    , m_depthParam(new Qt3DRender::QParameter(QStringLiteral("depthTexture"), nullptr))
    , m_nearPlaneParam(new Qt3DRender::QParameter(QStringLiteral("nearPlane"), nullptr))
    , m_farPlaneParam(new Qt3DRender::QParameter(QStringLiteral("farPlane"), nullptr))
    , m_focusRangeParam(new Qt3DRender::QParameter(QStringLiteral("focusRange"), nullptr))
    , m_focusDistanceParam(new Qt3DRender::QParameter(QStringLiteral("focusDistance"), nullptr))
    , m_radiusParam(new Qt3DRender::QParameter(QStringLiteral("bokehRadius"), nullptr))
    , m_dofTextureParam(new Qt3DRender::QParameter(QStringLiteral("dofTexture"), nullptr))
    , m_fsQuad(nullptr)
{
    setFocusRange(3.f);
    setFocusDistance(8.f);
    setRadius(6.f);
    m_rootFrameGraphNode.reset(new Qt3DRender::QFrameGraphNode);
    m_rootFrameGraphNode->setObjectName(QStringLiteral("DoF Effect"));

    m_dofTexture = new Qt3DRender::QTexture2D;

    auto blurRenderTarget = new Qt3DRender::QRenderTarget;
    auto dofOutput = new Qt3DRender::QRenderTargetOutput;
    dofOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    dofOutput->setTexture(m_dofTexture);
    m_dofTexture->setFormat(FrameGraphUtils::hasHalfFloatRenderable() ? Qt3DRender::QAbstractTexture::RGBA16F : Qt3DRender::QAbstractTexture::RGBA8_UNorm);
    m_dofTexture->setSize(512, 512);
    m_dofTexture->setGenerateMipMaps(false);
    blurRenderTarget->addOutput(dofOutput);

    m_dofTextureParam->setValue(QVariant::fromValue(m_dofTexture));

    // Set up DoF Material
    auto dofMaterial = new Qt3DRender::QMaterial(m_rootFrameGraphNode.data());

    auto effect = new Qt3DRender::QEffect;
    dofMaterial->setEffect(effect);

    const QString passFilterName = QStringLiteral("passName");
    const QString dofBlurPassName = QStringLiteral("KuesaDOFBlurPass");
    const QString dofCompositionPassName = QStringLiteral("KuesaDOFCompositionPass");

    auto createRenderPass = [&](const QUrl &vertex, const QUrl &fragment, const QString &passName) {
        auto renderPass = FXUtils::createRenderPass(passFilterName, passName);
        auto shaderProg = new Qt3DRender::QShaderProgram(renderPass);
        shaderProg->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(vertex));
        shaderProg->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(fragment));
        renderPass->setShaderProgram(shaderProg);

        renderPass->addParameter(m_textureParam);
        renderPass->addParameter(m_depthParam);
        renderPass->addParameter(m_nearPlaneParam);
        renderPass->addParameter(m_farPlaneParam);
        renderPass->addParameter(m_textureSizeParam);
        renderPass->addParameter(m_focusRangeParam);
        renderPass->addParameter(m_focusDistanceParam);
        renderPass->addParameter(m_radiusParam);

        if (passName == dofCompositionPassName)
            renderPass->addParameter(m_dofTextureParam);

        return renderPass;
    };

    auto gl3Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGL, 3, 2,
                                               Qt3DRender::QGraphicsApiFilter::CoreProfile);
    gl3Technique->addRenderPass(createRenderPass(
            QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/fullscreen.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/dof_blur.frag")),
            dofBlurPassName));

    gl3Technique->addRenderPass(createRenderPass(
            QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/fullscreen.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/dof_composite.frag")),
            dofCompositionPassName));

    effect->addTechnique(gl3Technique);

    auto es2Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGLES, 2, 0,
                                               Qt3DRender::QGraphicsApiFilter::NoProfile);
    es2Technique->addRenderPass(createRenderPass(
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/fullscreen.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/dof_blur.frag")),
            dofBlurPassName));

    es2Technique->addRenderPass(createRenderPass(
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/fullscreen.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/dof_composite.frag")),
            dofCompositionPassName));

    effect->addTechnique(es2Technique);

    auto es3Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGLES, 3, 0,
                                               Qt3DRender::QGraphicsApiFilter::CoreProfile);
    es3Technique->addRenderPass(createRenderPass(
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es3/fullscreen.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es3/dof_blur.frag")),
            dofBlurPassName));

    es3Technique->addRenderPass(createRenderPass(
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es3/fullscreen.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es3/dof_composite.frag")),
            dofCompositionPassName));

    effect->addTechnique(es3Technique);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto rhiTechnique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::RHI, 1, 0,
                                               Qt3DRender::QGraphicsApiFilter::NoProfile);
    rhiTechnique->addRenderPass(createRenderPass(
            QUrl(QStringLiteral("qrc:/kuesa/shaders/gl45/fullscreen.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/gl45/dof_blur.frag")),
            dofBlurPassName));

    rhiTechnique->addRenderPass(createRenderPass(
            QUrl(QStringLiteral("qrc:/kuesa/shaders/gl45/fullscreen.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/gl45/dof_composite.frag")),
            dofCompositionPassName));

    effect->addTechnique(rhiTechnique);
#endif

    m_fsQuad = new FullScreenQuad(dofMaterial, m_rootFrameGraphNode.data());
    m_layer = m_fsQuad->layer();

    //
    //  FrameGraph Construction
    //
    auto layerFilter = new Qt3DRender::QLayerFilter(m_rootFrameGraphNode.data());
    layerFilter->addLayer(m_fsQuad->layer());

    auto blurTargetSelector = new Qt3DRender::QRenderTargetSelector(layerFilter);
    blurTargetSelector->setTarget(blurRenderTarget);

    auto blurPassFilter = FXUtils::createRenderPassFilter(passFilterName, dofBlurPassName, blurTargetSelector);
    auto compositePassFilter = FXUtils::createRenderPassFilter(passFilterName, dofCompositionPassName, layerFilter);
    Q_UNUSED(compositePassFilter);
    Q_UNUSED(blurPassFilter);
}

DepthOfFieldEffect::~DepthOfFieldEffect()
{
}

AbstractPostProcessingEffect::FrameGraphNodePtr DepthOfFieldEffect::frameGraphSubTree() const
{
    return m_rootFrameGraphNode;
}

QVector<Qt3DRender::QLayer *> DepthOfFieldEffect::layers() const
{
    return QVector<Qt3DRender::QLayer *>{ m_layer };
}

void DepthOfFieldEffect::setInputTexture(Qt3DRender::QAbstractTexture *texture)
{
    m_textureParam->setValue(QVariant::fromValue(texture));
}

void DepthOfFieldEffect::setDepthTexture(Qt3DRender::QAbstractTexture *texture)
{
    m_depthParam->setValue(QVariant::fromValue(texture));
}

void DepthOfFieldEffect::setCamera(Qt3DCore::QEntity *camera)
{
    if (camera == nullptr)
        return;
    m_nearPlaneParam->setValue(camera->property("nearPlane").toFloat());
    m_farPlaneParam->setValue(camera->property("farPlane").toFloat());
}

float DepthOfFieldEffect::focusRange() const
{
    return m_focusRange;
}

float DepthOfFieldEffect::radius() const
{
    return m_radius;
}

float DepthOfFieldEffect::focusDistance() const
{
    return m_focusDistance;
}

void DepthOfFieldEffect::updateTextureSizeParam(const QSize &sceneSize,
                                                const QRectF &normalizedVP)
{
    m_textureSizeParam->setValue(QVector2D(sceneSize.width() * normalizedVP.width(),
                                       sceneSize.height() * normalizedVP.height()));
}


/*!
 * Sets the normalized viewport rect to \a vp.
 *
 * \sa AbstractPostProcessingEffect::setViewportRect
 */
void DepthOfFieldEffect::setViewportRect(const QRectF &vp)
{
    m_fsQuad->setViewportRect(vp);
    updateTextureSizeParam({m_dofTexture->width(), m_dofTexture->height()},
                           vp);
}

/*!
 * Sets the focal range to \a focusRange.
 *
 * \sa DepthOfFieldEffect::focusRange
 */
void DepthOfFieldEffect::setFocusRange(float focusRange)
{
    if (qFuzzyCompare(m_focusRange, focusRange))
        return;

    m_focusRange = focusRange;
    m_focusRangeParam->setValue(m_focusRange);
    emit focusRangeChanged(m_focusRange);
}

/*!
 * Sets the blur radius to \a radius.
 *
 * \sa DepthOfFieldEffect::radius
 */
void DepthOfFieldEffect::setRadius(float radius)
{
    if (qFuzzyCompare(m_radius, radius))
        return;

    m_radius = radius;
    m_radiusParam->setValue(m_radius);
    emit radiusChanged(m_radius);
}

/*!
 * Sets the focus distance to \a focusDistance.
 *
 * \sa DepthOfFieldEffect::focusDistance
 */
void DepthOfFieldEffect::setFocusDistance(float focusDistance)
{
    if (qFuzzyCompare(m_focusDistance, focusDistance))
        return;

    m_focusDistance = focusDistance;
    m_focusDistanceParam->setValue(m_focusDistance);
    emit focusDistanceChanged(m_focusDistance);
}

void DepthOfFieldEffect::setWindowSize(const QSize &size)
{
    m_dofTexture->setSize(size.width(), size.height());
    updateTextureSizeParam(size, m_fsQuad->viewportRect());
}

QT_END_NAMESPACE
