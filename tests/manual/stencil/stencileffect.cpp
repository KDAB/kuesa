#include "stencileffect.h"

#include <Qt3DRender>
#include <Kuesa/FullScreenQuad>

#include <Qt3DRender/QStencilTest>
#include <Qt3DRender/QStencilTestArguments>
#include <Qt3DRender/QStencilOperation>
#include <Qt3DRender/QStencilOperationArguments>

StencilEffect::StencilEffect()
    : Kuesa::AbstractPostProcessingEffect()
    , m_color(new Qt3DRender::QParameter("color", QColor{}, this))
    , m_inputTexture(new Qt3DRender::QParameter("inputColorTexture", nullptr, this))
    , m_depthTexture(new Qt3DRender::QParameter("inputDepthTexture", nullptr, this))
{
    QObject::connect(m_color, &Qt3DRender::QParameter::valueChanged,
                     [this]() {
                         emit colorChanged(color());
                     });

    m_rootFramegraphNode.reset(new Qt3DRender::QFrameGraphNode());
    m_rootFramegraphNode->setObjectName(QStringLiteral("Opacity Mask Effect"));

    // Set up SceneGraph Material and FullScreen Quad
    auto opacityMaskMaterial = new Qt3DRender::QMaterial(m_rootFramegraphNode.data());

    auto effect = new Qt3DRender::QEffect;
    opacityMaskMaterial->setEffect(effect);

    auto *gl3Technique = new Qt3DRender::QTechnique(opacityMaskMaterial);
    effect->addTechnique(gl3Technique);
    gl3Technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    gl3Technique->graphicsApiFilter()->setMajorVersion(3);
    gl3Technique->graphicsApiFilter()->setMinorVersion(3);
    gl3Technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    auto *gl3RenderPass = new Qt3DRender::QRenderPass(gl3Technique);
    gl3Technique->addRenderPass(gl3RenderPass);
    auto *shaderProgram = new Qt3DRender::QShaderProgram(gl3RenderPass);
    gl3RenderPass->setShaderProgram(shaderProgram);

    const auto vertexShader = QStringLiteral("qrc:/kuesa/shaders/gl3/passthrough.vert");
    const auto fragmentShader = QStringLiteral("qrc:/stencileffect.frag");
    shaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(vertexShader)));
    shaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(fragmentShader)));
    gl3RenderPass->setShaderProgram(shaderProgram);

    opacityMaskMaterial->addParameter(m_color);
    opacityMaskMaterial->addParameter(m_inputTexture);
    opacityMaskMaterial->addParameter(m_depthTexture);

    auto fullScreenQuad = new Kuesa::FullScreenQuad(opacityMaskMaterial, m_rootFramegraphNode.data());
    auto *layer = new Qt3DRender::QLayer;
    fullScreenQuad->addComponent(layer);
    m_layers.append(layer);

    Qt3DRender::QLayerFilter *layerFilter = new Qt3DRender::QLayerFilter(m_rootFramegraphNode.data());
    layerFilter->addLayer(layer);

    Qt3DRender::QRenderStateSet *renderStateSet = new Qt3DRender::QRenderStateSet(layerFilter);

    Qt3DRender::QStencilTest *stencilTest = new Qt3DRender::QStencilTest(renderStateSet);
    stencilTest->front()->setStencilFunction(Qt3DRender::QStencilTestArguments::Equal);
    stencilTest->back()->setStencilFunction(Qt3DRender::QStencilTestArguments::Equal);
    stencilTest->front()->setComparisonMask(0xffffffff);
    stencilTest->front()->setReferenceValue(0);
    stencilTest->back()->setComparisonMask(0xffffffff);
    stencilTest->back()->setReferenceValue(0);

    renderStateSet->addRenderState(stencilTest);
}

QColor StencilEffect::color() const
{
    return m_color->value().value<QColor>();
}

Kuesa::AbstractPostProcessingEffect::FrameGraphNodePtr StencilEffect::frameGraphSubTree() const
{
    return m_rootFramegraphNode;
}

void StencilEffect::setInputTexture(Qt3DRender::QAbstractTexture *texture)
{
    m_inputTexture->setValue(QVariant::fromValue(texture));
}

void StencilEffect::setDepthTexture(Qt3DRender::QAbstractTexture *texture)
{
    m_depthTexture->setValue(QVariant::fromValue(texture));
}

QVector<Qt3DRender::QLayer *> StencilEffect::layers() const
{
    return m_layers;
}

void StencilEffect::setColor(QColor color)
{
    m_color->setValue(color);
}
