/*
    DepthOfFieldEffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "gaussianblureffect.h"
#include "depthoffieldeffect.h"
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
#include <QTimer>

QT_BEGIN_NAMESPACE
/**
 * Useful resources:
 * http://on-demand.gputechconf.com/gtc/2014/presentations/S4385-order-independent-transparency-opengl.pdf
 *
 * Evenly spaced points computed with Vogel's method:
 * https://www.arl.army.mil/arlreports/2015/ARL-TR-7333.pdf
 *
 * https://catlikecoding.com/unity/tutorials/advanced-rendering/depth-of-field/
 * https://www.slideshare.net/TiagoAlexSousa/graphics-gems-from-cryengine-3-siggraph-2013
 *
 * http://fileadmin.cs.lth.se/cs/education/edan35/lectures/12dof.pdf
 * https://en.wikibooks.org/wiki/OpenGL_Programming/Depth_of_Field
 * https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch23.html (a bit old ?)
 * https://bartwronski.com/2014/04/07/bokeh-depth-of-field-going-insane-part-1/ (fairly advanced)
 * http://gpupro.blogspot.com/2013/09/gpu-pro-4-skylanders-depth-of-field.html
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

 \sa BloomEffect::focusDistance
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

 \sa BloomEffect::focusRange
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

DepthOfFieldEffect::DepthOfFieldEffect(Qt3DCore::QNode *parent)
    : AbstractPostProcessingEffect(parent)
    , m_layer(nullptr)
    , m_thresholdParameter(new Qt3DRender::QParameter(QStringLiteral("threshold"), 1.0f))
    , m_textureParam(new Qt3DRender::QParameter(QStringLiteral("textureSampler"), nullptr))
    , m_textureSizeParam(new Qt3DRender::QParameter(QStringLiteral("textureSize"), nullptr))
    , m_depthParam(new Qt3DRender::QParameter(QStringLiteral("depthTexture"), nullptr))
    , m_nearPlaneParam(new Qt3DRender::QParameter(QStringLiteral("nearPlane"), nullptr))
    , m_farPlaneParam(new Qt3DRender::QParameter(QStringLiteral("farPlane"), nullptr))
    , m_focusRangeParam(new Qt3DRender::QParameter(QStringLiteral("focusRange"), nullptr))
    , m_focusDistanceParam(new Qt3DRender::QParameter(QStringLiteral("focusDistance"), nullptr))
    , m_radiusParam(new Qt3DRender::QParameter(QStringLiteral("bokehRadius"), nullptr))
{
    setFocusRange(3.f);
    setFocusDistance(8.f);
    setRadius(6.f);
    m_rootFrameGraphNode.reset(new Qt3DRender::QFrameGraphNode);
    m_rootFrameGraphNode->setObjectName(QStringLiteral("Threshold Effect"));

    // Set up Threshold Material
    auto thresholdMaterial = new Qt3DRender::QMaterial(m_rootFrameGraphNode.data());

    auto effect = new Qt3DRender::QEffect;
    thresholdMaterial->setEffect(effect);

    auto setup_technique = [&](Qt3DRender::QGraphicsApiFilter::Api api, int major, int minor,
                               Qt3DRender::QGraphicsApiFilter::OpenGLProfile profile,
                               const QUrl &vertex, const QUrl &fragment) {
        auto technique = new Qt3DRender::QTechnique;
        effect->addTechnique(technique);

        technique->graphicsApiFilter()->setApi(api);
        technique->graphicsApiFilter()->setMajorVersion(major);
        technique->graphicsApiFilter()->setMinorVersion(minor);
        technique->graphicsApiFilter()->setProfile(profile);

        auto techniqueFilterKey = new Qt3DRender::QFilterKey;
        techniqueFilterKey->setName(QStringLiteral("renderingStyle"));
        techniqueFilterKey->setValue(QStringLiteral("forward"));
        technique->addFilterKey(techniqueFilterKey);

        auto renderPass = new Qt3DRender::QRenderPass;
        auto shaderProg = new Qt3DRender::QShaderProgram(renderPass);
        shaderProg->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(vertex));
        shaderProg->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(fragment));

        renderPass->setShaderProgram(shaderProg);

        auto passFilterKey = new Qt3DRender::QFilterKey;
        passFilterKey->setName(QStringLiteral("KuesaDOFPass"));
        renderPass->addFilterKey(passFilterKey);

        renderPass->addParameter(m_textureParam);
        renderPass->addParameter(m_depthParam);
        renderPass->addParameter(m_thresholdParameter);
        renderPass->addParameter(m_nearPlaneParam);
        renderPass->addParameter(m_farPlaneParam);
        renderPass->addParameter(m_textureSizeParam);
        renderPass->addParameter(m_focusRangeParam);
        renderPass->addParameter(m_focusDistanceParam);
        renderPass->addParameter(m_radiusParam);

        technique->addRenderPass(renderPass);
    };

    setup_technique(Qt3DRender::QGraphicsApiFilter::OpenGL, 3, 2,
                    Qt3DRender::QGraphicsApiFilter::CoreProfile,
                    QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/passthrough.vert")),
                    QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/dof.frag")));
    setup_technique(Qt3DRender::QGraphicsApiFilter::OpenGLES, 2, 0,
                    Qt3DRender::QGraphicsApiFilter::NoProfile,
                    QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/passthrough.vert")),
                    QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/dof.frag")));
    setup_technique(Qt3DRender::QGraphicsApiFilter::OpenGLES, 3, 0,
                    Qt3DRender::QGraphicsApiFilter::CoreProfile,
                    QUrl(QStringLiteral("qrc:/kuesa/shaders/es3/passthrough.vert")),
                    QUrl(QStringLiteral("qrc:/kuesa/shaders/es3/dof.frag")));

    auto thresholdQuad = new FullScreenQuad(thresholdMaterial, m_rootFrameGraphNode.data());
    m_layer = thresholdQuad->layer();

    //
    //  FrameGraph Construction
    //
    auto layerFilter = new Qt3DRender::QLayerFilter(m_rootFrameGraphNode.data());
    layerFilter->addLayer(thresholdQuad->layer());
    auto renderPassFilter = new Qt3DRender::QRenderPassFilter(layerFilter);
    auto filterKey = new Qt3DRender::QFilterKey;
    filterKey->setName(QStringLiteral("KuesaDOFPass"));
    renderPassFilter->addMatch(filterKey);
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

void DepthOfFieldEffect::setSceneSize(const QSize &size)
{
    m_textureSizeParam->setValue(QSizeF(size));
}

QT_END_NAMESPACE
