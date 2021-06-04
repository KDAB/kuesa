/*
    iro2glasssemtechnique_p.h

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

#ifndef KUESA_IRO2GLASSSEMTECHNIQUE_P_H
#define KUESA_IRO2GLASSSEMTECHNIQUE_P_H

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
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QBlendEquationArguments>

QT_BEGIN_NAMESPACE

namespace Kuesa {
class Iro2GlassSemTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2,
        RHI
    };

    explicit Iro2GlassSemTechnique(Version version, Qt3DCore::QNode *parent = nullptr)
        : QTechnique(parent)
        , m_backFaceCulling(new Qt3DRender::QCullFace(this))
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

        m_techniqueAllowFrustumCullingFilterKey->setName(QStringLiteral("allowCulling"));
        m_techniqueAllowFrustumCullingFilterKey->setValue(true);
        addFilterKey(m_techniqueAllowFrustumCullingFilterKey);

        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("renderingStyle"));
        filterKey->setValue(QStringLiteral("forward"));
        addFilterKey(filterKey);
        {
            const QUrl vertexShaderGraph[] = {
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json"))
            };

            const QUrl fragmentShaderGraph[] = {
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iro2glasssem.frag.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iro2glasssem.frag.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iro2glasssem.frag.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iro2glasssem.frag.json"))
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

            auto renderShaderBuilder = new Qt3DRender::QShaderProgramBuilder(this);
            auto renderShader = new Qt3DRender::QShaderProgram(this);
            auto transparentRenderPass = new Qt3DRender::QRenderPass(this);

            // Use default vertex shader graph if no vertex shader code was specified
            if (renderableVertexShaderCode[version].isEmpty()) {
                renderShaderBuilder->setShaderProgram(renderShader);
                renderShaderBuilder->setVertexShaderGraph(vertexShaderGraph[version]);
            } else {
                renderShader->setVertexShaderCode(renderableVertexShaderCode[version]);
            }

            if (renderableFragmentShaderCode[version].isEmpty()) {
                renderShaderBuilder->setShaderProgram(renderShader);
                renderShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph[version]);
            } else {
                renderShader->setFragmentShaderCode(renderableFragmentShaderCode[version]);
            }

            // Set geometry shader code if one was specified
            renderShader->setGeometryShaderCode(renderableGeometryShaderCode[version]);
            transparentRenderPass->setShaderProgram(renderShader);

            auto blendEquation = new Qt3DRender::QBlendEquation(this);
            blendEquation->setBlendFunction(Qt3DRender::QBlendEquation::Add);

            auto blendArguments = new Qt3DRender::QBlendEquationArguments(this);
            blendArguments->setSourceRgb(Qt3DRender::QBlendEquationArguments::Zero);
            blendArguments->setSourceAlpha(Qt3DRender::QBlendEquationArguments::One);
            blendArguments->setDestinationRgb(Qt3DRender::QBlendEquationArguments::SourceColor);
            blendArguments->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::Zero);

            transparentRenderPass->addRenderState(m_backFaceCulling);
            transparentRenderPass->addRenderState(blendEquation);
            transparentRenderPass->addRenderState(blendArguments);

            auto transparentFilterKey = new Qt3DRender::QFilterKey(this);
            transparentFilterKey->setName(QStringLiteral("KuesaDrawStage"));
            transparentFilterKey->setValue(QStringLiteral("Transparent"));
            transparentRenderPass->addFilterKey(transparentFilterKey);

            addRenderPass(transparentRenderPass);
        }
        {
            const QUrl vertexShaderGraph[] = {
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json"))
            };

            const QUrl fragmentShaderGraph[] = {
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iro2reflectionsem.frag.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iro2reflectionsem.frag.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iro2reflectionsem.frag.json")),
                QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/iro2reflectionsem.frag.json"))
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

            auto renderShaderBuilder = new Qt3DRender::QShaderProgramBuilder(this);
            auto renderShader = new Qt3DRender::QShaderProgram(this);
            auto transparentRenderPass = new Qt3DRender::QRenderPass(this);

            // Use default vertex shader graph if no vertex shader code was specified
            if (renderableVertexShaderCode[version].isEmpty()) {
                renderShaderBuilder->setShaderProgram(renderShader);
                renderShaderBuilder->setVertexShaderGraph(vertexShaderGraph[version]);
            } else {
                renderShader->setVertexShaderCode(renderableVertexShaderCode[version]);
            }

            if (renderableFragmentShaderCode[version].isEmpty()) {
                renderShaderBuilder->setShaderProgram(renderShader);
                renderShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph[version]);
            } else {
                renderShader->setFragmentShaderCode(renderableFragmentShaderCode[version]);
            }

            // Set geometry shader code if one was specified
            renderShader->setGeometryShaderCode(renderableGeometryShaderCode[version]);
            transparentRenderPass->setShaderProgram(renderShader);

            auto blendEquation = new Qt3DRender::QBlendEquation(this);
            blendEquation->setBlendFunction(Qt3DRender::QBlendEquation::Add);

            auto blendArguments = new Qt3DRender::QBlendEquationArguments(this);
            blendArguments->setSourceRgb(Qt3DRender::QBlendEquationArguments::One);
            blendArguments->setSourceAlpha(Qt3DRender::QBlendEquationArguments::Zero);
            blendArguments->setDestinationRgb(Qt3DRender::QBlendEquationArguments::One);
            blendArguments->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::One);

            transparentRenderPass->addRenderState(m_backFaceCulling);
            transparentRenderPass->addRenderState(blendEquation);
            transparentRenderPass->addRenderState(blendArguments);

            auto transparentFilterKey = new Qt3DRender::QFilterKey(this);
            transparentFilterKey->setName(QStringLiteral("KuesaDrawStage"));
            transparentFilterKey->setValue(QStringLiteral("Transparent"));
            transparentRenderPass->addFilterKey(transparentFilterKey);

            addRenderPass(transparentRenderPass);
        }
    }

    QStringList enabledLayers() const
    {
        const auto shaderBuilder = findChild<Qt3DRender::QShaderProgramBuilder *>();
        if (shaderBuilder != nullptr)
            return shaderBuilder->enabledLayers();
        return {};
    }

    void setEnabledLayers(const QStringList &layers)
    {
        const auto shaderBuilders = findChildren<Qt3DRender::QShaderProgramBuilder *>();
        for (auto shaderBuilder : shaderBuilders) {
            if (shaderBuilder != nullptr)
                shaderBuilder->setEnabledLayers(layers);
        }
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

    void setAllowCulling(bool allowCulling)
    {
        m_techniqueAllowFrustumCullingFilterKey->setValue(allowCulling);
    }

    QList<Qt3DRender::QShaderProgramBuilder *> shaderBuilders()
    {
        return findChildren<Qt3DRender::QShaderProgramBuilder *>();
    }

private:
    Qt3DRender::QCullFace *m_backFaceCulling;
    Qt3DRender::QFilterKey *m_techniqueAllowFrustumCullingFilterKey;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_IRO2GLASSSEMTECHNIQUE_P_H
