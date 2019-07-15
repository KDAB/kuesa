/*
    thresholdeffect.cpp

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

#include "thresholdeffect.h"
#include "fullscreenquad.h"
#include "fxutils_p.h"
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qlayerfilter.h>

QT_BEGIN_NAMESPACE

using namespace Kuesa;

/*!
 * \class Kuesa::ThresholdEffect
 * \inheaderfile Kuesa/ThresholdEffect
 * \inmodule Kuesa
 * \since Kuesa 1.0
 * \brief Post-processing effect showing only pixels brighter than a specified value
 *
 * ThresholdEffect is a post-processing effect that passes through any pixel
 * above a certain brightness value and sets all others to black.
 */

/*!
    \property ThresholdEffect::threshold

    \brief the brightness value used as a cut-off

    This value is used to determine how bright a pixel needs to be to pass
    the threshold.

 \table
 \header
     \li Threshold 0.12
     \li Threshold 0.35
 \row
     \li \inlineimage fx/threshold/t12.png
     \li \inlineimage fx/threshold/t35.png
 \endtable

*/

/*!
    \qmlproperty float ThresholdEffect::threshold

    \brief the brightness value used as a cut-off

    This value is used to determine how bright a pixel needs to be to pass
    the threshold.

 \table
 \header
     \li Threshold 0.12
     \li Threshold 0.35
 \row
     \li \inlineimage fx/threshold/t12.png
     \li \inlineimage fx/threshold/t35.png
 \endtable

*/

ThresholdEffect::ThresholdEffect(Qt3DCore::QNode *parent)
    : AbstractPostProcessingEffect(parent)
    , m_layer(nullptr)
    , m_thresholdParameter(new Qt3DRender::QParameter(QStringLiteral("threshold"), 1.0f))
    , m_textureParam(new Qt3DRender::QParameter(QStringLiteral("textureSampler"), nullptr))
{
    m_rootFrameGraphNode.reset(new Qt3DRender::QFrameGraphNode);
    m_rootFrameGraphNode->setObjectName(QStringLiteral("Threshold Effect"));

    // Set up Threshold Material
    auto thresholdMaterial = new Qt3DRender::QMaterial(m_rootFrameGraphNode.data());

    auto effect = new Qt3DRender::QEffect;
    thresholdMaterial->setEffect(effect);

    const QString graphPath = QStringLiteral("qrc:/kuesa/shaders/graphs/threshold.frag.json");
    const QString passFilterValue = QStringLiteral("KuesaThresholdPass");
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

    effect->addParameter(m_textureParam);
    effect->addParameter(m_thresholdParameter);

    auto thresholdQuad = new FullScreenQuad(thresholdMaterial, m_rootFrameGraphNode.data());
    m_layer = thresholdQuad->layer();

    //
    //  FrameGraph Construction
    //
    auto thresholdLayerFilter = new Qt3DRender::QLayerFilter(m_rootFrameGraphNode.data());
    thresholdLayerFilter->addLayer(thresholdQuad->layer());

    //create RenderPassFilter parented to layerFilter
    FXUtils::createRenderPassFilter(passFilterName, passFilterValue, thresholdLayerFilter);
}

/*!
 * Returns the frame graph subtree corresponding to the effect's implementation.
 *
 * \sa AbstractPostProcessingEffect::frameGraphSubTree
 */
AbstractPostProcessingEffect::FrameGraphNodePtr ThresholdEffect::frameGraphSubTree() const
{
    return m_rootFrameGraphNode;
}

/*!
 * Returns the current threshold.
 *
 * \sa ThresholdEffect::setThreshold
 */
float ThresholdEffect::threshold() const
{
    return m_thresholdParameter->value().toFloat();
}

/*!
 * Sets the threshold value.
 *
 * \sa ThresholdEffect::threshold
 */
void ThresholdEffect::setThreshold(float threshold)
{
    if (qFuzzyCompare(this->threshold(), threshold))
        return;

    m_thresholdParameter->setValue(threshold);
    emit thresholdChanged(threshold);
}

QVector<Qt3DRender::QLayer *> ThresholdEffect::layers() const
{
    return QVector<Qt3DRender::QLayer *>{ m_layer };
}

/*!
 * Sets the input texture for the effect.
 *
 * \sa AbstractPostProcessingEffect::setInputTexture
 */
void ThresholdEffect::setInputTexture(Qt3DRender::QAbstractTexture *texture)
{
    m_textureParam->setValue(QVariant::fromValue(texture));
}

QT_END_NAMESPACE
