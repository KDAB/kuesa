
/*
    iromattebackgroundeffect.cpp

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

#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QShaderProgramBuilder>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QDepthTest>
#include "iromattebackgroundeffect.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

class IroMatteBackgroundTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2
    };

    explicit IroMatteBackgroundTechnique(Version version, Qt3DCore::QNode *parent = nullptr)
        : QTechnique(parent)
        , m_backFaceCulling(new QCullFace(this))
        , m_noDepthMask(new QNoDepthMask(this))
        , m_depthTest(new QDepthTest(this))
        , m_renderShaderBuilder(new QShaderProgramBuilder(this))
        , m_renderShader(new QShaderProgram(this))
        , m_backgroundRenderPass(new QRenderPass(this))
        , m_techniqueAllowFrustumCullingFilterKey(new QFilterKey(this))
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

        const QUrl vertexShaderGraph[] = {
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromattebackground.vert.json")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromattebackground.vert.json")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromattebackground.vert.json"))
        };

        const QUrl fragmentShaderGraph[] = {
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteopaque.frag.json")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteopaque.frag.json")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteopaque.frag.json"))
        };

        const QByteArray renderableVertexShaderCode[] = {
            QByteArray(R"()"),
            QByteArray(R"()"),
            QByteArray(R"()")
        };

        const QByteArray renderableFragmentShaderCode[] = {
            QByteArray(R"()"),
            QByteArray(R"()"),
            QByteArray(R"()")
        };

         const QByteArray renderableGeometryShaderCode[] = {
            QByteArray(R"()"),
            QByteArray(R"()"),
            QByteArray(R"()")
        };

        // Use default vertex shader graph if no vertex shader code was specified
        if (renderableVertexShaderCode[version].isEmpty()) {
            m_renderShaderBuilder->setShaderProgram(m_renderShader);
            m_renderShaderBuilder->setVertexShaderGraph(vertexShaderGraph[version]);
        } else {
            m_renderShader->setVertexShaderCode(renderableVertexShaderCode[version]);
        }

        if (renderableFragmentShaderCode[version].isEmpty()) {
            m_renderShaderBuilder->setShaderProgram(m_renderShader);
            m_renderShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph[version]);
        } else {
            m_renderShader->setFragmentShaderCode(renderableFragmentShaderCode[version]);
        }

         // Set geometry shader code if one was specified
        m_renderShader->setGeometryShaderCode(renderableGeometryShaderCode[version]);

        auto filterKey = new QFilterKey(this);
        filterKey->setName(QStringLiteral("renderingStyle"));
        filterKey->setValue(QStringLiteral("forward"));
        addFilterKey(filterKey);

        m_techniqueAllowFrustumCullingFilterKey->setName(QStringLiteral("allowCulling"));
        m_techniqueAllowFrustumCullingFilterKey->setValue(false);
        addFilterKey(m_techniqueAllowFrustumCullingFilterKey);

        auto opaqueFilterKey = new Qt3DRender::QFilterKey(this);
        opaqueFilterKey->setName(QStringLiteral("KuesaDrawStage"));
        opaqueFilterKey->setValue(QStringLiteral("Opaque"));

        m_depthTest->setDepthFunction(QDepthTest::LessOrEqual);

        m_backgroundRenderPass->setShaderProgram(m_renderShader);
        m_backgroundRenderPass->addRenderState(m_backFaceCulling);
        m_backgroundRenderPass->addFilterKey(opaqueFilterKey);
        m_backgroundRenderPass->addRenderState(m_noDepthMask);
        m_backgroundRenderPass->addRenderState(m_depthTest);
        addRenderPass(m_backgroundRenderPass);
    }

    QStringList enabledLayers() const
    {
        return m_renderShaderBuilder->enabledLayers();
    }

    void setEnabledLayers(const QStringList &layers)
    {
        m_renderShaderBuilder->setEnabledLayers(layers);
    }

    void setOpaque(bool)
    {
    }

    void setCullingMode(QCullFace::CullingMode mode)
    {
        m_backFaceCulling->setMode(mode);
    }

    QCullFace::CullingMode cullingMode() const
    {
        return m_backFaceCulling->mode();
    }

    void setAllowCulling(bool)
    {
    }

private:
    Qt3DRender::QCullFace *m_backFaceCulling;
    Qt3DRender::QNoDepthMask *m_noDepthMask;
    Qt3DRender::QDepthTest *m_depthTest;
    Qt3DRender::QShaderProgramBuilder *m_renderShaderBuilder;
    Qt3DRender::QShaderProgram *m_renderShader;
    Qt3DRender::QRenderPass *m_backgroundRenderPass;
    Qt3DRender::QFilterKey *m_techniqueAllowFrustumCullingFilterKey;
};


/*!
    \class Kuesa::IroMatteBackgroundEffect
    \inheaderfile Kuesa/IroMatteBackgroundEffect
    \inherits Qt3DRender::QEffect
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteBackgroundEffect is the effect for the IroMatteBackgroundMaterial class
*/

/*!
    \qmltype IroMatteBackgroundEffect
    \instantiates Kuesa::IroMatteBackgroundEffect
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::IroMatteBackgroundEffect is the effect for the IroMatteBackgroundMaterial class
*/


IroMatteBackgroundEffect::IroMatteBackgroundEffect(Qt3DCore::QNode *parent)
    : GLTF2MaterialEffect(parent)
{
    m_gl3Technique = new IroMatteBackgroundTechnique(IroMatteBackgroundTechnique::GL3, this);
    m_es3Technique = new IroMatteBackgroundTechnique(IroMatteBackgroundTechnique::ES3, this);
    m_es2Technique = new IroMatteBackgroundTechnique(IroMatteBackgroundTechnique::ES2, this);

    addTechnique(m_gl3Technique);
    addTechnique(m_es3Technique);
    addTechnique(m_es2Technique);
}

IroMatteBackgroundEffect::~IroMatteBackgroundEffect() = default;


void IroMatteBackgroundEffect::updateDoubleSided(bool doubleSided)
{
    const auto cullingMode = doubleSided ? QCullFace::NoCulling : QCullFace::Back;
    m_gl3Technique->setCullingMode(cullingMode);
    m_es3Technique->setCullingMode(cullingMode);
    m_es2Technique->setCullingMode(cullingMode);
}

void IroMatteBackgroundEffect::updateSkinning(bool useSkinning)
{
    // Set Layers on zFill and opaque/Transparent shader builders
    auto layers = m_gl3Technique->enabledLayers();
    if (useSkinning) {
        layers.removeAll(QStringLiteral("no-skinning"));
        layers.append(QStringLiteral("skinning"));
    } else {
        layers.removeAll(QStringLiteral("skinning"));
        layers.append(QStringLiteral("no-skinning"));
    }

    m_gl3Technique->setEnabledLayers(layers);
    m_es3Technique->setEnabledLayers(layers);
    m_es2Technique->setEnabledLayers(layers);
    m_gl3Technique->setAllowCulling(!useSkinning);
    m_es3Technique->setAllowCulling(!useSkinning);
    m_es2Technique->setAllowCulling(!useSkinning);
}

void IroMatteBackgroundEffect::updateOpaque(bool opaque)
{
    m_gl3Technique->setOpaque(opaque);
    m_es3Technique->setOpaque(opaque);
    m_es2Technique->setOpaque(opaque);
}

void IroMatteBackgroundEffect::updateAlphaCutoffEnabled(bool enabled)
{
    auto layers = m_gl3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noHasAlphaCutoff"));
        layers.append(QStringLiteral("hasAlphaCutoff"));
    } else {
        layers.removeAll(QStringLiteral("hasAlphaCutoff"));
        layers.append(QStringLiteral("noHasAlphaCutoff"));
    }
    m_gl3Technique->setEnabledLayers(layers);
    m_es3Technique->setEnabledLayers(layers);
    m_es2Technique->setEnabledLayers(layers);
}

} // namespace Kuesa

QT_END_NAMESPACE
