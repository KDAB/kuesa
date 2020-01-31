
/*
    sphericalenvmapeffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "sphericalenvmapeffect.h"


QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

class SphericalEnvMapTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2
    };

    explicit SphericalEnvMapTechnique(Version version, Qt3DCore::QNode *parent = nullptr)
        : QTechnique(parent)
        , m_backFaceCulling(new QCullFace(this))
        , m_renderShaderBuilder(new QShaderProgramBuilder(this))
        , m_zfillShaderBuilder(new QShaderProgramBuilder(this))
        , m_renderShader(new QShaderProgram(this))
        , m_zfillShader(new QShaderProgram(this))
        , m_zfillRenderPass(new QRenderPass(this))
        , m_opaqueRenderPass(new QRenderPass(this))
        , m_transparentRenderPass(new QRenderPass(this))
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

        const auto vertexShaderGraph = QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/metallicroughness.vert.json"));

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

        const QByteArray renderableVertexShaderCode[] = {
            QByteArray(R"(#version 150 core

uniform mat4 mvp;
uniform mat4 modelView;

struct MaterialProperties {
    vec3 factors;
    vec2 disturbation;
    float postVertexColor;
    float postGain;
    sampler2D sem;
    float semGain;
};

uniform MaterialProperties properties;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec3 normalSem;
out vec2 texCoord;
out vec4 postColor;

vec3 semNormal()
{
    vec3 n = (modelView * vec4(vertexNormal, 0.0)).xyz;
    n *= properties.factors;
    n.xy += properties.disturbation;
    return normalize(n);
}

void main()
{
    normalSem = semNormal();
    texCoord = vertexTexCoord;
    postColor = mix(vec4(1.0), vertexColor, properties.postVertexColor) * properties.postGain;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
)"),
            QByteArray(R"(#version 300 es
precision highp float;
precision highp sampler2D;
uniform mat4 mvp;
uniform mat4 modelView;

struct MaterialProperties {
    vec3 factors;
    vec2 disturbation;
    float postVertexColor;
    float postGain;
    sampler2D sem;
    float semGain;
};

uniform MaterialProperties properties;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec3 normalSem;
out vec2 texCoord;
out vec4 postColor;

vec3 semNormal()
{
    vec3 n = (modelView * vec4(vertexNormal, 0.0)).xyz;
    n *= properties.factors;
    n.xy += properties.disturbation;
    return normalize(n);
}

void main()
{
    normalSem = semNormal();
    texCoord = vertexTexCoord;
    postColor = mix(vec4(1.0), vertexColor, properties.postVertexColor) * properties.postGain;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
)"),
            QByteArray(R"(precision highp float;
precision highp sampler2D;
uniform mat4 mvp;
uniform mat4 modelView;

struct MaterialProperties {
    vec3 factors;
    vec2 disturbation;
    float postVertexColor;
    float postGain;
    sampler2D sem;
    float semGain;
};

uniform MaterialProperties properties;

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTexCoord;
attribute vec4 vertexColor;

varying vec3 normalSem;
varying vec2 texCoord;
varying vec4 postColor;

vec3 semNormal()
{
    vec3 n = (modelView * vec4(vertexNormal, 0.0)).xyz;
    n *= properties.factors;
    n.xy += properties.disturbation;
    return normalize(n);
}

void main()
{
    normalSem = semNormal();
    texCoord = vertexTexCoord;
    postColor = mix(vec4(1.0), vertexColor, properties.postVertexColor) * properties.postGain;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
)")
        };

        const QByteArray renderableFragmentShaderCode[] = {
            QByteArray(R"(#version 150 core
vec4 sRGBToLinear(vec4 color)  { return vec4(pow(color.rgb, vec3(2.2)), color.a); }

struct MaterialProperties {
    vec3 factors;
    vec2 disturbation;
    float postVertexColor;
    float postGain;
    sampler2D sem;
    float semGain;
};

uniform MaterialProperties properties;

in vec3 normalSem;
in vec4 postColor;

out vec4 fragColor;

vec2 semS(vec3 normalSem_)
{
    vec2 s = normalSem_.xy;
    s = s * 0.5 + vec2(0.5);
    return s;
}

void main()
{
    fragColor = postColor * vec4(sRGBToLinear(texture(properties.sem, semS(normalize(normalSem)))).rgb * properties.semGain, 1.0);
})"),
            QByteArray(R"(#version 300 es
precision highp float;
precision highp sampler2D;
vec4 sRGBToLinear(vec4 color)  { return vec4(pow(color.rgb, vec3(2.2)), color.a); }

struct MaterialProperties {
    vec3 factors;
    vec2 disturbation;
    float postVertexColor;
    float postGain;
    sampler2D sem;
    float semGain;
};

uniform MaterialProperties properties;

in vec3 normalSem;
in vec4 postColor;

out vec4 fragColor;

vec2 semS(vec3 normalSem_)
{
    vec2 s = normalSem_.xy;
    s = s * 0.5 + vec2(0.5);
    return s;
}

void main()
{
    fragColor = postColor * vec4(sRGBToLinear(texture(properties.sem, semS(normalize(normalSem)))).rgb * properties.semGain, 1.0);
})"),
            QByteArray(R"(precision highp float;
precision highp sampler2D;
vec4 sRGBToLinear(vec4 color)  { return vec4(pow(color.rgb, vec3(2.2)), color.a); }

struct MaterialProperties {
    vec3 factors;
    vec2 disturbation;
    float postVertexColor;
    float postGain;
    sampler2D sem;
    float semGain;
};

uniform MaterialProperties properties;

varying vec3 normalSem;
varying vec4 postColor;

vec2 semS(vec3 normalSem_)
{
    vec2 s = normalSem_.xy;
    s = s * 0.5 + vec2(0.5);
    return s;
}

void main()
{
    gl_FragColor = postColor * vec4(sRGBToLinear(texture(properties.sem, semS(normalize(normalSem)))).rgb * properties.semGain, 1.0);
})")
        };

         const QByteArray renderableGeometryShaderCode[] = {
            QByteArray(R"()"),
            QByteArray(R"()"),
            QByteArray(R"()")
        };

        // Use default vertex shader graph if no vertex shader code was specified
        if (renderableVertexShaderCode[version].isEmpty()) {
            m_renderShaderBuilder->setShaderProgram(m_renderShader);
            m_renderShaderBuilder->setVertexShaderGraph(vertexShaderGraph);

            m_zfillShaderBuilder->setShaderProgram(m_zfillShader);
            m_zfillShaderBuilder->setVertexShaderGraph(vertexShaderGraph);
        } else {
            m_renderShader->setVertexShaderCode(renderableVertexShaderCode[version]);
            m_zfillShader->setVertexShaderCode(renderableVertexShaderCode[version]);
        }

        m_renderShader->setFragmentShaderCode(renderableFragmentShaderCode[version]);
        m_zfillShader->setFragmentShaderCode(zFillFragmentShaderCode[version]);

         // Set geometry shader code if one was specified
        m_renderShader->setGeometryShaderCode(renderableGeometryShaderCode[version]);
        m_zfillShader->setGeometryShaderCode(renderableGeometryShaderCode[version]);

        auto filterKey = new QFilterKey(this);
        filterKey->setName(QStringLiteral("renderingStyle"));
        filterKey->setValue(QStringLiteral("forward"));
        addFilterKey(filterKey);

        m_techniqueAllowFrustumCullingFilterKey->setName(QStringLiteral("allowCulling"));
        m_techniqueAllowFrustumCullingFilterKey->setValue(true);
        addFilterKey(m_techniqueAllowFrustumCullingFilterKey);

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

        m_opaqueRenderPass->setShaderProgram(m_renderShader);
        m_opaqueRenderPass->addRenderState(m_backFaceCulling);
        m_opaqueRenderPass->addFilterKey(opaqueFilterKey);
        addRenderPass(m_opaqueRenderPass);

        auto transparentFilterKey = new Qt3DRender::QFilterKey(this);
        transparentFilterKey->setName(QStringLiteral("KuesaDrawStage"));
        transparentFilterKey->setValue(QStringLiteral("Transparent"));

        m_transparentRenderPass->setShaderProgram(m_renderShader);
        m_transparentRenderPass->addRenderState(m_backFaceCulling);
        m_transparentRenderPass->addFilterKey(transparentFilterKey);
        m_transparentRenderPass->setEnabled(false);
        addRenderPass(m_transparentRenderPass);
    }

    QStringList enabledLayers() const
    {
        return m_renderShaderBuilder->enabledLayers();
    }

    void setEnabledLayers(const QStringList &layers)
    {
        m_renderShaderBuilder->setEnabledLayers(layers);
        m_zfillShaderBuilder->setEnabledLayers(layers);
    }

    void setOpaque(bool opaque)
    {
        m_zfillRenderPass->setEnabled(opaque);
        m_opaqueRenderPass->setEnabled(opaque);
        m_transparentRenderPass->setEnabled(!opaque);
    }

    void setCullingMode(QCullFace::CullingMode mode)
    {
        m_backFaceCulling->setMode(mode);
    }

    QCullFace::CullingMode cullingMode() const
    {
        return m_backFaceCulling->mode();
    }

    void setAllowCulling(bool allowCulling)
    {
        m_techniqueAllowFrustumCullingFilterKey->setValue(allowCulling);
    }

private:
    Qt3DRender::QCullFace *m_backFaceCulling;
    Qt3DRender::QShaderProgramBuilder *m_renderShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_zfillShaderBuilder;
    Qt3DRender::QShaderProgram *m_renderShader;
    Qt3DRender::QShaderProgram *m_zfillShader;
    Qt3DRender::QRenderPass *m_zfillRenderPass;
    Qt3DRender::QRenderPass *m_opaqueRenderPass;
    Qt3DRender::QRenderPass *m_transparentRenderPass;
    Qt3DRender::QFilterKey *m_techniqueAllowFrustumCullingFilterKey;
};


/*!
    \class Kuesa::SphericalEnvMapEffect
    \inheaderfile Kuesa/SphericalEnvMapEffect
    \inherits Qt3DRender::QEffect
    \inmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::SphericalEnvMapEffect is the effect for the SphericalEnvMapMaterial class
*/

/*!
    \qmltype SphericalEnvMapEffect
    \instantiates Kuesa::SphericalEnvMapEffect
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \brief Kuesa::SphericalEnvMapEffect is the effect for the SphericalEnvMapMaterial class
*/


SphericalEnvMapEffect::SphericalEnvMapEffect(Qt3DCore::QNode *parent)
    : QEffect(parent)
    , m_useSkinning(false)
    , m_opaque(true)
    , m_alphaCutoffEnabled(false)
{
    const auto enabledLayers = QStringList{
            // Vertex Shader layers
            QStringLiteral("no-skinning"),
            // Fragment Shader layers
            QStringLiteral("noHasAlphaCutoff")
    };

    m_gl3Technique = new SphericalEnvMapTechnique(SphericalEnvMapTechnique::GL3, this);
    m_gl3Technique->setEnabledLayers(enabledLayers);

    m_es3Technique = new SphericalEnvMapTechnique(SphericalEnvMapTechnique::ES3, this);
    m_es3Technique->setEnabledLayers(enabledLayers);

    m_es2Technique = new SphericalEnvMapTechnique(SphericalEnvMapTechnique::ES2, this);
    m_es2Technique->setEnabledLayers(enabledLayers);

    addTechnique(m_gl3Technique);
    addTechnique(m_es3Technique);
    addTechnique(m_es2Technique);
}

SphericalEnvMapEffect::~SphericalEnvMapEffect() = default;

bool SphericalEnvMapEffect::isDoubleSided() const
{
    return m_gl3Technique->cullingMode() == QCullFace::NoCulling;
}

bool SphericalEnvMapEffect::useSkinning() const
{
    return m_useSkinning;
}

bool SphericalEnvMapEffect::isOpaque() const
{
    return m_opaque;
}

bool SphericalEnvMapEffect::isAlphaCutoffEnabled() const
{
    return m_alphaCutoffEnabled;
}

void SphericalEnvMapEffect::setDoubleSided(bool doubleSided)
{
    const auto cullingMode = doubleSided ? QCullFace::NoCulling : QCullFace::Back;
    m_gl3Technique->setCullingMode(cullingMode);
    m_es3Technique->setCullingMode(cullingMode);
    m_es2Technique->setCullingMode(cullingMode);
}

void SphericalEnvMapEffect::setUseSkinning(bool useSkinning)
{
    if (useSkinning == m_useSkinning)
        return;
    m_useSkinning = useSkinning;
    emit useSkinningChanged(m_useSkinning);

    // Set Layers on zFill and opaque/Transparent shader builders
    auto layers = m_gl3Technique->enabledLayers();
    if (m_useSkinning) {
        layers.removeAll(QStringLiteral("no-skinning"));
        layers.append(QStringLiteral("skinning"));
    } else {
        layers.removeAll(QStringLiteral("no-skinning"));
        layers.append(QStringLiteral("skinning"));
    }

    m_gl3Technique->setEnabledLayers(layers);
    m_es3Technique->setEnabledLayers(layers);
    m_es2Technique->setEnabledLayers(layers);
    m_gl3Technique->setAllowCulling(!m_useSkinning);
    m_es3Technique->setAllowCulling(!m_useSkinning);
    m_es2Technique->setAllowCulling(!m_useSkinning);

}

void SphericalEnvMapEffect::setOpaque(bool opaque)
{
    if (opaque == m_opaque)
        return;
    m_opaque = opaque;
    m_gl3Technique->setOpaque(opaque);
    m_es3Technique->setOpaque(opaque);
    m_es2Technique->setOpaque(opaque);

    if (opaque)
        setAlphaCutoffEnabled(false);

    emit opaqueChanged(opaque);
}

void SphericalEnvMapEffect::setAlphaCutoffEnabled(bool enabled)
{
    if (m_alphaCutoffEnabled == enabled)
        return;

    auto layers = m_gl3Technique->enabledLayers();
    if (enabled) {
        layers.removeAll(QStringLiteral("noHasAlphaCutoff"));
        layers.append(QStringLiteral("hasAlphaCutoff"));
    } else {
        layers.removeAll(QStringLiteral("hasAlphaCutoff"));
        layers.append(QStringLiteral("noHasAlphaCutoff"));
    }
    m_alphaCutoffEnabled = enabled;
    m_gl3Technique->setEnabledLayers(layers);
    m_es3Technique->setEnabledLayers(layers);
    m_es2Technique->setEnabledLayers(layers);
    emit alphaCutoffEnabledChanged(enabled);
}

} // namespace Kuesa

QT_END_NAMESPACE
