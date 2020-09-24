/*
    opacitymask.cpp

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

#include "opacitymask.h"
#include "fullscreenquad.h"
#include <private/fxutils_p.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Qt3DRender/qrenderstateset.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qnodepthmask.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \qmltype OpacityMask
    \inherits AbstractPostProcessingEffect
    \inqmlmodule Kuesa

    \brief Masks onscreen content based on the alpha color value of a mask
    texture.

    Given an RGBA mask texture, content of the backbuffer will be rendered as:
    \badcode
    vec4 pixelColor = vec4(inputColor.rgb, inputColor.a * maskColor.a)
    \endcode

    \image fx/opacitymask/noalpha.png

    A premultiplied alpha variant of the algorithm is also available which can
    be of use when combining Qt 3D / Kuesa content with 2D QtQuick content
    (Scene3D). It performs rendering doing:
    \badcode
    vec4 pixelColor = vec4(inputColor.rgb / maskColor.a, inputColor.a * maskColor.a);
    \endcode

    \badcode
    import Kuesa 1.1 as Kuesa

    Kuesa.SceneEnity {
        id: root
        components: [
            RenderSettings {
                Kuesa.ForwardRenderer {
                    postProcessingEffects: [
                        OpacityMask {
                            mask: TextureLoader {
                                source: ":/opacity_mask.png";
                                generateMipMaps: false
                            }
                        }
                    ]
                }
            }
        ]
        ...
    }
    \endcode

    \image fx/opacitymask/alpha.png
 */

/*!
    \class Kuesa::OpacityMask
    \inherits AbstractPostProcessingEffect
    \inmodule Kuesa

    \brief Masks onscreen content based on the alpha color value of a mask
    texture.

    Given an RGBA mask texture, content of the backbuffer will be rendered as:
    \badcode
    vec4 pixelColor = vec4(inputColor.rgb, inputColor.a * maskColor.a)
    \endcode

    \image fx/opacitymask/noalpha.png

    A premultiplied alpha variant of the algorithm is also available which can
    be of use when combining Qt 3D / Kuesa content with 2D QtQuick content
    (Scene3D). It performs rendering doing:
    \badcode
    vec4 pixelColor = vec4(inputColor.rgb / maskColor.a, inputColor.a * maskColor.a);
    \endcode

    \badcode
    #include <Qt3DExtras/Qt3DWindow>
    #include <ForwardRenderer>
    #include <SceneEntity>
    #include <OpacityMask>

    Qt3DExtras::Qt3DWindow win;
    Kuesa::SceneEntity *root = new Kuesa::SceneEntity();
    Kuesa::ForwardRenderer *frameGraph = new Kuesa::ForwardRenderer();
    Kuesa::OpacityMask *opacityMask = new Kuesa::OpacityMask();

    Qt3DRender::QTextureLoader *texture = new Qt3DRender::QTextureLoader();
    texture->setSource(QUrl("file:///opacityMask.png"));
    opacityMask->setMask(texture);

    frameGraph->addPostProcessingEffect(opacityMask);

    win->setRootEntity(root);
    win->setActiveFrameGraph(forwardRenderer);
    ...
    \endcode

    \image fx/opacitymask/alpha.png
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture OpacityMask::mask

    The RGBA texture to use as a mask.
 */

/*!
    \property OpacityMask::mask

    The RGBA texture to use as a mask.
 */

/*!
    \qmlproperty bool OpacityMask::premultipliedAlpha

    Specifies whether the masking should be performed using premultipliedAlpha.
    This can be useful when combining Kuesa and QtQuick with a Scene3D element.
    It is false by default.
 */

/*!
    \property OpacityMask::premultipliedAlpha

    Specifies whether the masking should be performed using premultipliedAlpha.
    This can be useful when combining Kuesa and QtQuick with a Scene3D element.
    It is false by default.
 */

OpacityMask::OpacityMask(Qt3DCore::QNode *parent)
    : AbstractPostProcessingEffect(parent)
    , m_premultipliedAlpha(false)
    , m_layer(nullptr)
    , m_gl3ShaderBuilder(nullptr)
    , m_es3ShaderBuilder(nullptr)
    , m_es2ShaderBuilder(nullptr)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    , m_rhiShaderBuilder(nullptr)
#endif
    , m_maskParameter(new Qt3DRender::QParameter(QStringLiteral("maskTexture"), nullptr))
    , m_inputTextureParameter(new Qt3DRender::QParameter(QStringLiteral("inputTexture"), nullptr))
    , m_fsQuad(nullptr)
{
    m_rootFrameGraphNode.reset(new Qt3DRender::QFrameGraphNode());
    m_rootFrameGraphNode->setObjectName(QStringLiteral("Opacity Mask Effect"));

    // Set up SceneGraph Material and FullScreen Quad
    auto opacityMaskMaterial = new Qt3DRender::QMaterial(m_rootFrameGraphNode.data());

    auto effect = new Qt3DRender::QEffect;
    opacityMaskMaterial->setEffect(effect);

    m_gl3ShaderBuilder = new Qt3DRender::QShaderProgramBuilder();
    m_gl3ShaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/opacitymask.frag.json")));
    m_gl3ShaderBuilder->setEnabledLayers(QStringList() << QStringLiteral("regular"));

    m_es3ShaderBuilder = new Qt3DRender::QShaderProgramBuilder();
    m_es3ShaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/opacitymask.frag.json")));
    m_es3ShaderBuilder->setEnabledLayers(QStringList() << QStringLiteral("regular"));

    m_es2ShaderBuilder = new Qt3DRender::QShaderProgramBuilder();
    m_es2ShaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/opacitymask.frag.json")));
    m_es2ShaderBuilder->setEnabledLayers(QStringList() << QStringLiteral("regular"));

    const QString passFilterName = QStringLiteral("passName");
    const QString passFilterValue = QStringLiteral("opacityMaskPass");

    auto *gl3Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGL,
                                                3, 2,
                                                Qt3DRender::QGraphicsApiFilter::CoreProfile,
                                                QStringLiteral("qrc:/kuesa/shaders/gl3/fullscreen.vert"),
                                                m_gl3ShaderBuilder,
                                                passFilterName, passFilterValue);
    effect->addTechnique(gl3Technique);

    auto *es3Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGLES,
                                                3, 0,
                                                Qt3DRender::QGraphicsApiFilter::NoProfile,
                                                QStringLiteral("qrc:/kuesa/shaders/es3/fullscreen.vert"),
                                                m_es3ShaderBuilder,
                                                passFilterName, passFilterValue);
    effect->addTechnique(es3Technique);

    auto *es2Technique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::OpenGLES,
                                                2, 0,
                                                Qt3DRender::QGraphicsApiFilter::NoProfile,
                                                QStringLiteral("qrc:/kuesa/shaders/es2/fullscreen.vert"),
                                                m_es2ShaderBuilder,
                                                passFilterName, passFilterValue);
    effect->addTechnique(es2Technique);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_rhiShaderBuilder = new Qt3DRender::QShaderProgramBuilder();
    m_rhiShaderBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/opacitymask.frag.json")));
    m_rhiShaderBuilder->setEnabledLayers(QStringList() << QStringLiteral("regular"));

    auto *rhiTechnique = FXUtils::makeTechnique(Qt3DRender::QGraphicsApiFilter::RHI,
                                                1, 0,
                                                Qt3DRender::QGraphicsApiFilter::NoProfile,
                                                QStringLiteral("qrc:/kuesa/shaders/gl45/fullscreen.vert"),
                                                m_rhiShaderBuilder,
                                                passFilterName, passFilterValue);
    effect->addTechnique(rhiTechnique);
#endif

    effect->addParameter(m_maskParameter);
    effect->addParameter(m_inputTextureParameter);

    m_fsQuad = new FullScreenQuad(opacityMaskMaterial, m_rootFrameGraphNode.data());
    m_layer = m_fsQuad->layer();

    // Set up FrameGraph
    auto layerFilter = new Qt3DRender::QLayerFilter(m_rootFrameGraphNode.data());
    layerFilter->addLayer(m_layer);

    m_mainRenderState = new Qt3DRender::QRenderStateSet(layerFilter);
    m_blendRenderState = new Qt3DRender::QRenderStateSet(m_mainRenderState);

    auto blendState = new Qt3DRender::QBlendEquation();
    blendState->setBlendFunction(Qt3DRender::QBlendEquation::Add);
    auto blendArgs = new Qt3DRender::QBlendEquationArguments();
    auto depthTest = new Qt3DRender::QDepthTest();
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Always);
    blendArgs->setSourceRgb(Qt3DRender::QBlendEquationArguments::One);
    blendArgs->setSourceAlpha(Qt3DRender::QBlendEquationArguments::One);
    blendArgs->setDestinationRgb(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    blendArgs->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);

    m_blendRenderState->addRenderState(blendState);
    m_blendRenderState->addRenderState(blendArgs);

    m_mainRenderState->addRenderState(depthTest);
    m_mainRenderState->addRenderState(new Qt3DRender::QNoDepthMask());

    //create RenderPassFilter parented to m_blendRenderState
    FXUtils::createRenderPassFilter(passFilterName, passFilterValue, m_blendRenderState);
}

AbstractPostProcessingEffect::FrameGraphNodePtr OpacityMask::frameGraphSubTree() const
{
    return m_rootFrameGraphNode;
}

QVector<Qt3DRender::QLayer *> OpacityMask::layers() const
{
    return { m_layer };
}

void OpacityMask::setInputTexture(Qt3DRender::QAbstractTexture *texture)
{
    m_inputTextureParameter->setValue(QVariant::fromValue(texture));
}

/*!
 * Sets the normalized viewport rect to \a vp.
 *
 * \sa AbstractPostProcessingEffect::setViewportRect
 */
void OpacityMask::setViewportRect(const QRectF &vp)
{
    m_fsQuad->setViewportRect(vp);
}

void OpacityMask::setMask(Qt3DRender::QAbstractTexture *mask)
{
    if (this->mask() == mask)
        return;
    m_maskParameter->setValue(QVariant::fromValue(mask));
    emit maskChanged(mask);
}

Qt3DRender::QAbstractTexture *OpacityMask::mask() const
{
    return m_maskParameter->value().value<Qt3DRender::QAbstractTexture *>();
}

void OpacityMask::setPremultipliedAlpha(bool premultipliedAlpha)
{
    if (premultipliedAlpha == m_premultipliedAlpha)
        return;
    m_premultipliedAlpha = premultipliedAlpha;
    emit premultipliedAlphaChanged(m_premultipliedAlpha);

    const QString layer = m_premultipliedAlpha ? QStringLiteral("premultiplied_alpha") : QStringLiteral("regular");
    m_gl3ShaderBuilder->setEnabledLayers({ layer });
    m_es3ShaderBuilder->setEnabledLayers({ layer });
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_rhiShaderBuilder->setEnabledLayers({ layer });
#endif
    m_blendRenderState->setEnabled(!m_premultipliedAlpha);
}

bool OpacityMask::premultipliedAlpha() const
{
    return m_premultipliedAlpha;
}

} // namespace Kuesa

QT_END_NAMESPACE
