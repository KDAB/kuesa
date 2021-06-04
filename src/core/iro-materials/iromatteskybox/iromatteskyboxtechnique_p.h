/*
    iromatteskyboxtechnique_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    This file was auto-generated

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

#ifndef KUESA_IROMATTESKYBOXTECHNIQUE_P_H
#define KUESA_IROMATTESKYBOXTECHNIQUE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QShaderProgramBuilder>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QDepthTest>

QT_BEGIN_NAMESPACE

namespace Kuesa {
class IroMatteSkyboxTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2,
        RHI
    };

    explicit IroMatteSkyboxTechnique(Version version, Qt3DCore::QNode *parent = nullptr)
        : QTechnique(parent)
        , m_backFaceCulling(new Qt3DRender::QCullFace(this))
        , m_noDepthMask(new Qt3DRender::QNoDepthMask(this))
        , m_depthTest(new Qt3DRender::QDepthTest(this))
        , m_renderShaderBuilder(new Qt3DRender::QShaderProgramBuilder(this))
        , m_renderShader(new Qt3DRender::QShaderProgram(this))
        , m_backgroundRenderPass(new Qt3DRender::QRenderPass(this))
        , m_techniqueAllowFrustumCullingFilterKey(new Qt3DRender::QFilterKey(this))
    {
        struct ApiFilterInfo {
            int major;
            int minor;
            Qt3DRender::QGraphicsApiFilter::Api api;
            Qt3DRender::QGraphicsApiFilter::OpenGLProfile profile;
        };

        const ApiFilterInfo apiFilterInfos[] = {
            { 3, 1, Qt3DRender::QGraphicsApiFilter::OpenGL, Qt3DRender::QGraphicsApiFilter::CoreProfile },
            { 3, 0, Qt3DRender::QGraphicsApiFilter::OpenGLES, Qt3DRender::QGraphicsApiFilter::NoProfile },
            { 2, 0, Qt3DRender::QGraphicsApiFilter::OpenGLES, Qt3DRender::QGraphicsApiFilter::NoProfile },
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            { 1, 0, Qt3DRender::QGraphicsApiFilter::RHI, Qt3DRender::QGraphicsApiFilter::NoProfile },
#endif
        };

        graphicsApiFilter()->setApi(apiFilterInfos[version].api);
        graphicsApiFilter()->setProfile(apiFilterInfos[version].profile);
        graphicsApiFilter()->setMajorVersion(apiFilterInfos[version].major);
        graphicsApiFilter()->setMinorVersion(apiFilterInfos[version].minor);

        const QUrl vertexShaderGraph[] = {
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteskybox.vert.json")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteskybox.vert.json")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteskybox.vert.json")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteskybox.vert.json"))
        };

        const QUrl fragmentShaderGraph[] = {
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteopaque.frag.json")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteopaque.frag.json")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteopaque.frag.json")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iromatteopaque.frag.json"))
        };

        const QByteArray renderableVertexShaderCode[] = {
            QByteArray(R"()"),
            QByteArray(R"()"),
            QByteArray(R"()"),
            QByteArray(R"()")
        };

        const QByteArray renderableFragmentShaderCode[] = {
            QByteArray(R"()"),
            QByteArray(R"()"),
            QByteArray(R"()"),
            QByteArray(R"()")
        };

        const QByteArray renderableGeometryShaderCode[] = {
            QByteArray(R"()"),
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

        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("renderingStyle"));
        filterKey->setValue(QStringLiteral("forward"));
        addFilterKey(filterKey);

        m_techniqueAllowFrustumCullingFilterKey->setName(QStringLiteral("allowCulling"));
        m_techniqueAllowFrustumCullingFilterKey->setValue(false);
        addFilterKey(m_techniqueAllowFrustumCullingFilterKey);

        auto opaqueFilterKey = new Qt3DRender::QFilterKey(this);
        opaqueFilterKey->setName(QStringLiteral("KuesaDrawStage"));
        opaqueFilterKey->setValue(QStringLiteral("Opaque"));

        m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);

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

    void setCullingMode(Qt3DRender::QCullFace::CullingMode mode)
    {
        m_backFaceCulling->setMode(mode);
    }

    Qt3DRender::QCullFace::CullingMode cullingMode() const
    {
        return m_backFaceCulling->mode();
    }

    void setAllowCulling(bool)
    {
    }

    QList<Qt3DRender::QShaderProgramBuilder *> shaderBuilders()
    {
        return { m_renderShaderBuilder };
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

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_IROMATTESKYBOXTECHNIQUE_P_H
