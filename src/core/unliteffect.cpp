/*
    unliteffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include "unliteffect.h"

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qnodepthmask.h>
#include <Qt3DRender/qdepthtest.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

class UnlitTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2
    };

    explicit UnlitTechnique(Version version, Qt3DCore::QNode *parent = nullptr);

    QStringList enabledLayers() const;
    void setEnabledLayers(const QStringList &layers);
    void setOpaque(bool opaque);
    void setCullingMode(QCullFace::CullingMode mode);
    QCullFace::CullingMode cullingMode() const;

private:
    Qt3DRender::QCullFace *m_backFaceCulling;
    Qt3DRender::QShaderProgramBuilder *m_unlitShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_zfillShaderBuilder;
    Qt3DRender::QShaderProgram *m_unlitShader;
    Qt3DRender::QShaderProgram *m_zfillShader;
    Qt3DRender::QRenderPass *m_zfillRenderPass;
    Qt3DRender::QRenderPass *m_opaqueRenderPass;
    Qt3DRender::QRenderPass *m_transparentRenderPass;
};


/*!
    \class Kuesa::UnlitEffect
    \inheaderfile Kuesa/UnlitEffect
    \inmodule Kuesa
    \since 1.1

    \brief Kuesa::UnlitEffect is a Qt3DRender::QEffect for
    Kuesa::UnlitMaterial. It provides several properties used to configure the
    material effect. This is done using a custom shader graph and
    activating/deactivating different set of nodes of the graph depending on
    the effect configuration.

    \note Kuesa::UnlitEffect is used to configure the effect for a material,
    but property values must be provided through Kuesa::UnlitMaterial.
    Therefore, this effect must be added to a Kuesa.UnlitMaterial.
 */

/*!
    \qmltype UnlitEffect
    \instantiates Kuesa::UnlitEffect
    \inqmlmodule Kuesa
    \since 1.1

    \brief Kuesa.UnlitEffect is a Qt3D.Render.Effect for
    Kuesa.UnlitMaterial.

    It provides several properties used to configure the material effect. This
    is done using a custom shader graph and activating/deactivating different
    set of nodes of the graph depending on the effect configuration.

    \note Kuesa.UnlitEffect is used to configure the effect for a material, but
    property values must be provided through Kuesa.UnlitMaterial. Therefore,
    this effect must be added to a Kuesa.UnlitMaterial.

    \sa Kuesa.UnlitEffect
 */

UnlitTechnique::UnlitTechnique(UnlitTechnique::Version version, Qt3DCore::QNode *parent)
    : QTechnique(parent)
    , m_backFaceCulling(new QCullFace(this))
    , m_unlitShaderBuilder(new QShaderProgramBuilder(this))
    , m_zfillShaderBuilder(new QShaderProgramBuilder(this))
    , m_unlitShader(new QShaderProgram(this))
    , m_zfillShader(new QShaderProgram(this))
    , m_zfillRenderPass(new QRenderPass(this))
    , m_opaqueRenderPass(new QRenderPass(this))
    , m_transparentRenderPass(new QRenderPass(this))
{
    struct ApiFilterInfo {
        int major;
        int minor;
        QGraphicsApiFilter::Api api;
        QGraphicsApiFilter::OpenGLProfile profile;
    };

    const ApiFilterInfo apiFilterInfos[] = {
        { 3, 1, QGraphicsApiFilter::OpenGL, QGraphicsApiFilter::CoreProfile },
        { 3, 0, QGraphicsApiFilter::OpenGLES, QGraphicsApiFilter::NoProfile },
        { 2, 0, QGraphicsApiFilter::OpenGLES, QGraphicsApiFilter::NoProfile },
    };

    graphicsApiFilter()->setApi(apiFilterInfos[version].api);
    graphicsApiFilter()->setProfile(apiFilterInfos[version].profile);
    graphicsApiFilter()->setMajorVersion(apiFilterInfos[version].major);
    graphicsApiFilter()->setMinorVersion(apiFilterInfos[version].minor);

    const auto vertexShaderGraph = QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json"));
    const auto fragmentShaderGraph = QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/unlit.frag.json"));

    const QByteArray zFillFragmentShaderCode[] = {
        QByteArray(R"(
                   #version 330
                   void main() { }
                   )"),
        QByteArray(R"(
                   #version 300 es
                   void main() { }
                   )"),
        QByteArray(R"(
                   #version 100
                   void main() { }
                   )")
    };

    m_unlitShaderBuilder->setShaderProgram(m_unlitShader);
    m_unlitShaderBuilder->setVertexShaderGraph(vertexShaderGraph);
    m_unlitShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph);

    m_zfillShaderBuilder->setShaderProgram(m_zfillShader);
    m_zfillShaderBuilder->setVertexShaderGraph(vertexShaderGraph);
    m_zfillShader->setFragmentShaderCode(zFillFragmentShaderCode[version]);

    auto filterKey = new QFilterKey(this);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));
    addFilterKey(filterKey);

    auto zfillFilterKey = new Qt3DRender::QFilterKey(this);
    zfillFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    zfillFilterKey->setValue(QStringLiteral("ZFill"));

    m_zfillRenderPass->setShaderProgram(m_zfillShader);
    m_zfillRenderPass->addRenderState(m_backFaceCulling);
    m_zfillRenderPass->addFilterKey(zfillFilterKey);
    addRenderPass(m_zfillRenderPass);

    auto opaqueFilterKey = new Qt3DRender::QFilterKey(this);
    opaqueFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    opaqueFilterKey->setValue(QStringLiteral("Opaque"));

    m_opaqueRenderPass->setShaderProgram(m_unlitShader);
    m_opaqueRenderPass->addRenderState(m_backFaceCulling);
    m_opaqueRenderPass->addFilterKey(opaqueFilterKey);
    addRenderPass(m_opaqueRenderPass);

    auto transparentFilterKey = new Qt3DRender::QFilterKey(this);
    transparentFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    transparentFilterKey->setValue(QStringLiteral("Transparent"));

    m_transparentRenderPass->setShaderProgram(m_unlitShader);
    m_transparentRenderPass->addRenderState(m_backFaceCulling);
    m_transparentRenderPass->addFilterKey(transparentFilterKey);
    m_transparentRenderPass->setEnabled(false);
    addRenderPass(m_transparentRenderPass);
}

QStringList UnlitTechnique::enabledLayers() const
{
    return m_unlitShaderBuilder->enabledLayers();
}

void UnlitTechnique::setEnabledLayers(const QStringList &layers)
{
    m_unlitShaderBuilder->setEnabledLayers(layers);
    m_zfillShaderBuilder->setEnabledLayers(layers);
}

void UnlitTechnique::setOpaque(bool opaque)
{
    m_zfillRenderPass->setEnabled(opaque);
    m_opaqueRenderPass->setEnabled(opaque);
    m_transparentRenderPass->setEnabled(!opaque);
}

void UnlitTechnique::setCullingMode(QCullFace::CullingMode mode)
{
    m_backFaceCulling->setMode(mode);
}

QCullFace::CullingMode UnlitTechnique::cullingMode() const
{
    return m_backFaceCulling->mode();
}


UnlitEffect::UnlitEffect(Qt3DCore::QNode *parent)
    : QEffect(parent)
    , m_baseColorMapEnabled(false)
    , m_usingColorAttribute(false)
    , m_useSkinning(false)
    , m_invokeInitVertexShaderRequested(false)
    , m_opaque(true)
    , m_alphaCutoffEnabled(false)
{

    const auto enabledLayers = QStringList{
            // Vertex Shader layers
            QStringLiteral("no-skinning"),
            // Fragment Shader layers
            QStringLiteral("noBaseColorMap"),
            QStringLiteral("noHasColorAttr"),
            QStringLiteral("noHasAlphaCutoff")
    };

    m_unlitGL3Technique = new UnlitTechnique(UnlitTechnique::GL3, this);
    m_unlitGL3Technique->setEnabledLayers(enabledLayers);

    m_unlitES3Technique = new UnlitTechnique(UnlitTechnique::ES3, this);
    m_unlitES3Technique->setEnabledLayers(enabledLayers);

    m_unlitES2Technique = new UnlitTechnique(UnlitTechnique::ES2, this);
    m_unlitES2Technique->setEnabledLayers(enabledLayers);

    addTechnique(m_unlitGL3Technique);
    addTechnique(m_unlitES3Technique);
    addTechnique(m_unlitES2Technique);
}

UnlitEffect::~UnlitEffect()
{
}

bool UnlitEffect::isBaseColorMapEnabled() const
{
    return m_baseColorMapEnabled;
}

bool UnlitEffect::isUsingColorAttribute() const
{
    return m_usingColorAttribute;
}

bool UnlitEffect::isDoubleSided() const
{
    return m_unlitGL3Technique->cullingMode() == QCullFace::NoCulling;
}

bool UnlitEffect::useSkinning() const
{
    return m_useSkinning;
}

bool UnlitEffect::isOpaque() const
{
    return m_opaque;
}

bool UnlitEffect::isAlphaCutoffEnabled() const
{
    return m_alphaCutoffEnabled;
}

void UnlitEffect::setBaseColorMapEnabled(bool enabled)
{
    if (m_baseColorMapEnabled == enabled)
        return;

    auto layers = m_unlitGL3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noBaseColorMap"));
        layers.append(QStringLiteral("baseColorMap"));
    } else {
        layers.removeAll(QStringLiteral("baseColorMap"));
        layers.append(QStringLiteral("noBaseColorMap"));
    }
    m_baseColorMapEnabled = enabled;
    m_unlitGL3Technique->setEnabledLayers(layers);
    m_unlitES3Technique->setEnabledLayers(layers);
    m_unlitES2Technique->setEnabledLayers(layers);
    emit baseColorMapEnabledChanged(enabled);
}

void UnlitEffect::setUsingColorAttribute(bool usingColorAttribute)
{
    if (m_usingColorAttribute == usingColorAttribute)
        return;

    auto layers = m_unlitGL3Technique->enabledLayers();
    if (usingColorAttribute) {
        layers.removeAll(QStringLiteral("noHasColorAttr"));
        layers.append(QStringLiteral("hasColorAttr"));
    } else {
        layers.removeAll(QStringLiteral("hasColorAttr"));
        layers.append(QStringLiteral("noHasColorAttr"));
    }
    m_usingColorAttribute = usingColorAttribute;
    m_unlitGL3Technique->setEnabledLayers(layers);
    m_unlitES3Technique->setEnabledLayers(layers);
    m_unlitES2Technique->setEnabledLayers(layers);
    emit usingColorAttributeChanged(usingColorAttribute);
}

void UnlitEffect::setDoubleSided(bool doubleSided)
{
    const auto cullingMode = doubleSided ? QCullFace::NoCulling : QCullFace::Back;
    m_unlitGL3Technique->setCullingMode(cullingMode);
    m_unlitES3Technique->setCullingMode(cullingMode);
    m_unlitES2Technique->setCullingMode(cullingMode);
}

void UnlitEffect::setUseSkinning(bool useSkinning)
{
    if (useSkinning == m_useSkinning)
        return;
    m_useSkinning = useSkinning;
    emit useSkinningChanged(m_useSkinning);

    // Set Layers on zFill and opaque/Transparent shader builders
    auto layers = m_unlitGL3Technique->enabledLayers();
    if (m_useSkinning) {
        layers.removeAll(QStringLiteral("no-skinning"));
        layers.append(QStringLiteral("skinning"));
    } else {
        layers.removeAll(QStringLiteral("no-skinning"));
        layers.append(QStringLiteral("skinning"));
    }

    m_unlitGL3Technique->setEnabledLayers(layers);
    m_unlitES3Technique->setEnabledLayers(layers);
    m_unlitES2Technique->setEnabledLayers(layers);
}

void UnlitEffect::setOpaque(bool opaque)
{
    if (opaque == m_opaque)
        return;
    m_opaque = opaque;
    m_unlitGL3Technique->setOpaque(opaque);
    m_unlitES3Technique->setOpaque(opaque);
    m_unlitES2Technique->setOpaque(opaque);

    if (opaque)
        setAlphaCutoffEnabled(false);

    emit opaqueChanged(opaque);
}

void UnlitEffect::setAlphaCutoffEnabled(bool enabled)
{
    if (m_alphaCutoffEnabled == enabled)
        return;

    auto layers = m_unlitES3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noHasAlphaCutoff"));
        layers.append(QStringLiteral("hasAlphaCutoff"));
    } else {
        layers.removeAll(QStringLiteral("hasAlphaCutoff"));
        layers.append(QStringLiteral("noHasAlphaCutoff"));
    }
    m_alphaCutoffEnabled = enabled;
    m_unlitES3Technique->setEnabledLayers(layers);
    m_unlitES3Technique->setEnabledLayers(layers);
    m_unlitES3Technique->setEnabledLayers(layers);
    emit alphaCutoffEnabledChanged(enabled);
}

} // namespace Kuesa

QT_END_NAMESPACE
