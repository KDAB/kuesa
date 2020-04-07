#!/usr/bin/env python3

import sys
import re
import os
import json
import shutil

class CustomMaterialGenerator:

    propertyTypesToQtType = { 'float': 'float',
                              'int'  : 'int',
                              'bool' : 'bool',
                              'color': 'QColor',
                              'vec2' : 'QVector2D',
                              'vec3' : 'QVector3D',
                              'vec4' : 'QVector4D',
                              'texture2d' : 'Qt3DRender::QAbstractTexture *' }

    includesForPropertyType = {
                              'vec2' : '#include <QVector2D>',
                              'vec3' : '#include <QVector3D>',
                              'vec4' : '#include <QVector4D>',
                              'color' : '#include <QColor>',
                              'texture2d' : '#include <Qt3DRender/QAbstractTexture>' }

    headerPrivateWarning = """
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

"""

    headerContent = """
/*
    %s.h

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

#ifndef KUESA_%s_H
#define KUESA_%s_H

%s%s

QT_BEGIN_NAMESPACE

namespace Kuesa {
%s
} // namespace Kuesa

QT_END_NAMESPACE
%s
#endif // KUESA_%s_H
"""

    cppContent = """
/*
    %s.cpp

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

%s

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {
%s
} // namespace Kuesa

QT_END_NAMESPACE
"""

    propertiesClassHeaderContent = """
class %sShaderData;

class KUESASHARED_EXPORT %sProperties : public GLTF2MaterialProperties
{
    Q_OBJECT
%s
public:
    Q_INVOKABLE explicit %sProperties(Qt3DCore::QNode *parent = nullptr);
    ~%sProperties();

    Qt3DRender::QShaderData *shaderData() const override;
%s
public Q_SLOTS:
%s
Q_SIGNALS:
%s
private:
    %sShaderData *m_shaderData;
};"""

    propertiesClassCppContent = """
%s
%sProperties::%sProperties(Qt3DCore::QNode *parent)
    : GLTF2MaterialProperties(parent)
    , m_shaderData(new %sShaderData(this))
{
%s
}

%sProperties::~%sProperties() = default;

Qt3DRender::QShaderData *%sProperties::shaderData() const
{
    return m_shaderData;
}

%s
%s"""


    shaderDataClassHeaderContent = """
class %sShaderData : public Qt3DRender::QShaderData
{
    Q_OBJECT
%s
public:
    explicit %sShaderData(Qt3DCore::QNode *parent = nullptr);
    ~%sShaderData();
%s
public Q_SLOTS:
%s
Q_SIGNALS:
%s
private:
%s
};
"""

    shaderDataClassCppContent = """
%sShaderData::%sShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)%s
{}

%sShaderData::~%sShaderData() = default;

%s
%s
"""

    effectClassHeaderContent = """
class %sTechnique;

class KUESASHARED_EXPORT %sEffect : public GLTF2MaterialEffect
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit %sEffect(Qt3DCore::QNode *parent = nullptr);
    ~%sEffect();

private:
    %sTechnique *m_gl3Technique;
    %sTechnique *m_es3Technique;
    %sTechnique *m_es2Technique;

    void updateDoubleSided(bool doubleSided) override;
    void updateSkinning(bool useSkinning) override;
    void updateOpaque(bool opaque) override;
    void updateAlphaCutoffEnabled(bool enabled) override;
};
"""

    techniqueOpaqueAndTransparent = """class %sTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2
    };

    explicit %sTechnique(Version version, Qt3DCore::QNode *parent = nullptr)
        : QTechnique(parent)
        , m_backFaceCulling(new QCullFace(this))
        , m_blendEquation(new Qt3DRender::QBlendEquation(this))
        , m_blendArguments(new Qt3DRender::QBlendEquationArguments(this))
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

        const QUrl vertexShaderGraph[] = {
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s"))
        };

        const QUrl fragmentShaderGraph[] = {
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s"))
        };

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
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
        };

        const QByteArray renderableFragmentShaderCode[] = {
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
        };

         const QByteArray renderableGeometryShaderCode[] = {
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
        };

        // Use default vertex shader graph if no vertex shader code was specified
        if (renderableVertexShaderCode[version].isEmpty()) {
            m_renderShaderBuilder->setShaderProgram(m_renderShader);
            m_renderShaderBuilder->setVertexShaderGraph(vertexShaderGraph[version]);

            m_zfillShaderBuilder->setShaderProgram(m_zfillShader);
            m_zfillShaderBuilder->setVertexShaderGraph(vertexShaderGraph[version]);
        } else {
            m_renderShader->setVertexShaderCode(renderableVertexShaderCode[version]);
            m_zfillShader->setVertexShaderCode(renderableVertexShaderCode[version]);
        }

        if (renderableFragmentShaderCode[version].isEmpty()) {
            m_renderShaderBuilder->setShaderProgram(m_renderShader);
            m_renderShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph[version]);
        } else {
            m_renderShader->setFragmentShaderCode(renderableFragmentShaderCode[version]);
        }
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

        m_blendEquation->setBlendFunction(Qt3DRender::QBlendEquation::%s);
        m_blendArguments->setSourceRgb(Qt3DRender::QBlendEquationArguments::%s);
        m_blendArguments->setSourceAlpha(Qt3DRender::QBlendEquationArguments::%s);
        m_blendArguments->setDestinationRgb(Qt3DRender::QBlendEquationArguments::%s);
        m_blendArguments->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::%s);

        m_transparentRenderPass->setShaderProgram(m_renderShader);
        m_transparentRenderPass->addRenderState(m_backFaceCulling);
        m_transparentRenderPass->addRenderState(m_blendEquation);
        m_transparentRenderPass->addRenderState(m_blendArguments);
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
    Qt3DRender::QBlendEquation *m_blendEquation;
    Qt3DRender::QBlendEquationArguments *m_blendArguments;
    Qt3DRender::QShaderProgramBuilder *m_renderShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_zfillShaderBuilder;
    Qt3DRender::QShaderProgram *m_renderShader;
    Qt3DRender::QShaderProgram *m_zfillShader;
    Qt3DRender::QRenderPass *m_zfillRenderPass;
    Qt3DRender::QRenderPass *m_opaqueRenderPass;
    Qt3DRender::QRenderPass *m_transparentRenderPass;
    Qt3DRender::QFilterKey *m_techniqueAllowFrustumCullingFilterKey;
};
"""

    techniqueOpaqueOnly = """class %sTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2
    };

    explicit %sTechnique(Version version, Qt3DCore::QNode *parent = nullptr)
        : QTechnique(parent)
        , m_backFaceCulling(new QCullFace(this))
        , m_renderShaderBuilder(new QShaderProgramBuilder(this))
        , m_zfillShaderBuilder(new QShaderProgramBuilder(this))
        , m_renderShader(new QShaderProgram(this))
        , m_zfillShader(new QShaderProgram(this))
        , m_zfillRenderPass(new QRenderPass(this))
        , m_opaqueRenderPass(new QRenderPass(this))
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
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s"))
        };

        const QUrl fragmentShaderGraph[] = {
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s"))
        };

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
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
        };

        const QByteArray renderableFragmentShaderCode[] = {
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
        };

         const QByteArray renderableGeometryShaderCode[] = {
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
        };

        // Use default vertex shader graph if no vertex shader code was specified
        if (renderableVertexShaderCode[version].isEmpty()) {
            m_renderShaderBuilder->setShaderProgram(m_renderShader);
            m_renderShaderBuilder->setVertexShaderGraph(vertexShaderGraph[version]);

            m_zfillShaderBuilder->setShaderProgram(m_zfillShader);
            m_zfillShaderBuilder->setVertexShaderGraph(vertexShaderGraph[version]);
        } else {
            m_renderShader->setVertexShaderCode(renderableVertexShaderCode[version]);
            m_zfillShader->setVertexShaderCode(renderableVertexShaderCode[version]);
        }

        if (renderableFragmentShaderCode[version].isEmpty()) {
            m_renderShaderBuilder->setShaderProgram(m_renderShader);
            m_renderShaderBuilder->setFragmentShaderGraph(fragmentShaderGraph[version]);
        } else {
            m_renderShader->setFragmentShaderCode(renderableFragmentShaderCode[version]);
        }
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
    Qt3DRender::QFilterKey *m_techniqueAllowFrustumCullingFilterKey;
};
"""

    techniqueBackgroundOnly = """class %sTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2
    };

    explicit %sTechnique(Version version, Qt3DCore::QNode *parent = nullptr)
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
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s"))
        };

        const QUrl fragmentShaderGraph[] = {
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s"))
        };

        const QByteArray renderableVertexShaderCode[] = {
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
        };

        const QByteArray renderableFragmentShaderCode[] = {
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
        };

         const QByteArray renderableGeometryShaderCode[] = {
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
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
"""

    techniqueTransparentOnly = """class %sTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2
    };

    explicit %sTechnique(Version version, Qt3DCore::QNode *parent = nullptr)
        : QTechnique(parent)
        , m_backFaceCulling(new QCullFace(this))
        , m_blendEquation(new Qt3DRender::QBlendEquation(this))
        , m_blendArguments(new Qt3DRender::QBlendEquationArguments(this))
        , m_renderShaderBuilder(new QShaderProgramBuilder(this))
        , m_renderShader(new QShaderProgram(this))
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

        const QUrl vertexShaderGraph[] = {
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s"))
        };

        const QUrl fragmentShaderGraph[] = {
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s")),
            QUrl(QStringLiteral("%s"))
        };

        const QByteArray renderableVertexShaderCode[] = {
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
        };

        const QByteArray renderableFragmentShaderCode[] = {
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
        };

         const QByteArray renderableGeometryShaderCode[] = {
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)"),
            QByteArray(R"(%s)")
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
        m_techniqueAllowFrustumCullingFilterKey->setValue(true);
        addFilterKey(m_techniqueAllowFrustumCullingFilterKey);

        auto transparentFilterKey = new Qt3DRender::QFilterKey(this);
        transparentFilterKey->setName(QStringLiteral("KuesaDrawStage"));
        transparentFilterKey->setValue(QStringLiteral("Transparent"));

        m_blendEquation->setBlendFunction(Qt3DRender::QBlendEquation::%s);
        m_blendArguments->setSourceRgb(Qt3DRender::QBlendEquationArguments::%s);
        m_blendArguments->setSourceAlpha(Qt3DRender::QBlendEquationArguments::%s);
        m_blendArguments->setDestinationRgb(Qt3DRender::QBlendEquationArguments::%s);
        m_blendArguments->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::%s);

        m_transparentRenderPass->setShaderProgram(m_renderShader);
        m_transparentRenderPass->addRenderState(m_backFaceCulling);
        m_transparentRenderPass->addRenderState(m_blendEquation);
        m_transparentRenderPass->addRenderState(m_blendArguments);
        m_transparentRenderPass->addFilterKey(transparentFilterKey);
        addRenderPass(m_transparentRenderPass);
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

    void setAllowCulling(bool allowCulling)
    {
        m_techniqueAllowFrustumCullingFilterKey->setValue(allowCulling);
    }

private:
    Qt3DRender::QCullFace *m_backFaceCulling;
    Qt3DRender::QBlendEquation *m_blendEquation;
    Qt3DRender::QBlendEquationArguments *m_blendArguments;
    Qt3DRender::QShaderProgramBuilder *m_renderShaderBuilder;
    Qt3DRender::QShaderProgram *m_renderShader;
    Qt3DRender::QRenderPass *m_transparentRenderPass;
    Qt3DRender::QFilterKey *m_techniqueAllowFrustumCullingFilterKey;
};
"""

    techniqueMultiTransparentInnerPass = """
        {
            const QUrl vertexShaderGraph[] = {
                QUrl(QStringLiteral("%s")),
                QUrl(QStringLiteral("%s")),
                QUrl(QStringLiteral("%s"))
            };

            const QUrl fragmentShaderGraph[] = {
                QUrl(QStringLiteral("%s")),
                QUrl(QStringLiteral("%s")),
                QUrl(QStringLiteral("%s"))
            };

            const QByteArray renderableVertexShaderCode[] = {
                QByteArray(R"(%s)"),
                QByteArray(R"(%s)"),
                QByteArray(R"(%s)")
            };

            const QByteArray renderableFragmentShaderCode[] = {
                QByteArray(R"(%s)"),
                QByteArray(R"(%s)"),
                QByteArray(R"(%s)")
            };

            const QByteArray renderableGeometryShaderCode[] = {
                QByteArray(R"(%s)"),
                QByteArray(R"(%s)"),
                QByteArray(R"(%s)")
            };

            auto renderShaderBuilder = new QShaderProgramBuilder(this);
            auto renderShader = new QShaderProgram(this);
            auto transparentRenderPass = new QRenderPass(this);

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
            blendEquation->setBlendFunction(Qt3DRender::QBlendEquation::%s);

            auto blendArguments = new Qt3DRender::QBlendEquationArguments(this);
            blendArguments->setSourceRgb(Qt3DRender::QBlendEquationArguments::%s);
            blendArguments->setSourceAlpha(Qt3DRender::QBlendEquationArguments::%s);
            blendArguments->setDestinationRgb(Qt3DRender::QBlendEquationArguments::%s);
            blendArguments->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::%s);

            transparentRenderPass->addRenderState(m_backFaceCulling);
            transparentRenderPass->addRenderState(blendEquation);
            transparentRenderPass->addRenderState(blendArguments);

            auto transparentFilterKey = new Qt3DRender::QFilterKey(this);
            transparentFilterKey->setName(QStringLiteral("KuesaDrawStage"));
            transparentFilterKey->setValue(QStringLiteral("Transparent"));
            transparentRenderPass->addFilterKey(transparentFilterKey);

            addRenderPass(transparentRenderPass);
        }"""

    techniqueMultiTransparent = """class %sTechnique : public Qt3DRender::QTechnique
{
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2
    };

    explicit %sTechnique(Version version, Qt3DCore::QNode *parent = nullptr)
        : QTechnique(parent)
        , m_backFaceCulling(new QCullFace(this))
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

        m_techniqueAllowFrustumCullingFilterKey->setName(QStringLiteral("allowCulling"));
        m_techniqueAllowFrustumCullingFilterKey->setValue(true);
        addFilterKey(m_techniqueAllowFrustumCullingFilterKey);

        auto filterKey = new QFilterKey(this);
        filterKey->setName(QStringLiteral("renderingStyle"));
        filterKey->setValue(QStringLiteral("forward"));
        addFilterKey(filterKey);%s
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
    Qt3DRender::QFilterKey *m_techniqueAllowFrustumCullingFilterKey;
};
"""


    effectClassCppContent = """
%s
%s

%sEffect::%sEffect(Qt3DCore::QNode *parent)
    : GLTF2MaterialEffect(parent)
{
    m_gl3Technique = new %sTechnique(%sTechnique::GL3, this);
    m_es3Technique = new %sTechnique(%sTechnique::ES3, this);
    m_es2Technique = new %sTechnique(%sTechnique::ES2, this);

    addTechnique(m_gl3Technique);
    addTechnique(m_es3Technique);
    addTechnique(m_es2Technique);
}

%sEffect::~%sEffect() = default;


void %sEffect::updateDoubleSided(bool doubleSided)
{
    const auto cullingMode = doubleSided ? QCullFace::NoCulling : QCullFace::Back;
    m_gl3Technique->setCullingMode(cullingMode);
    m_es3Technique->setCullingMode(cullingMode);
    m_es2Technique->setCullingMode(cullingMode);
}

void %sEffect::updateSkinning(bool useSkinning)
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

void %sEffect::updateOpaque(bool opaque)
{
    m_gl3Technique->setOpaque(opaque);
    m_es3Technique->setOpaque(opaque);
    m_es2Technique->setOpaque(opaque);
}

void %sEffect::updateAlphaCutoffEnabled(bool enabled)
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
"""


    materialClassHeaderContent = """
class %sProperties;
class GLTF2MaterialProperties;

class KUESASHARED_EXPORT %sMaterial : public GLTF2Material
{
    Q_OBJECT
    Q_PROPERTY(Kuesa::%sProperties *materialProperties READ materialProperties WRITE setMaterialProperties NOTIFY materialPropertiesChanged)

public:
    Q_INVOKABLE explicit %sMaterial(Qt3DCore::QNode *parent = nullptr);
    ~%sMaterial();

    %sProperties *materialProperties() const;

public Q_SLOTS:
    void setMaterialProperties(Kuesa::%sProperties *materialProperties);
    void setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties);

Q_SIGNALS:
    void materialPropertiesChanged(Kuesa::%sProperties *materialProperties);

private:
    %sProperties *m_materialProperties = nullptr;
    Qt3DRender::QParameter *m_shaderDataParameter;
};
"""

    materialClassCppContent = """
%s
%sMaterial::%sMaterial(Qt3DCore::QNode *parent)
    : GLTF2Material(parent),
    m_shaderDataParameter(new Qt3DRender::QParameter(QStringLiteral(\"properties\"), {}))
{
    addParameter(m_shaderDataParameter);
}

%sMaterial::~%sMaterial() = default;

/*!
    \property %sMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

/*!
    \qmlproperty %sProperties %sMaterial::materialProperties

    The properties defining the appearance of the material.

    \since Kuesa 1.2
 */

Kuesa::%sProperties *%sMaterial::materialProperties() const
{
    return m_materialProperties;
}

void %sMaterial::setMaterialProperties(Kuesa::%sProperties *materialProperties)
{
    if (m_materialProperties != materialProperties) {
        m_materialProperties = materialProperties;
        emit materialPropertiesChanged(materialProperties);

        if (m_materialProperties) {
            m_shaderDataParameter->setValue(QVariant::fromValue(m_materialProperties->shaderData()));
            m_materialProperties->addClientMaterial(this);
        }
    }
}

void %sMaterial::setMaterialProperties(Kuesa::GLTF2MaterialProperties *materialProperties)
{
    setMaterialProperties(qobject_cast<Kuesa::%sProperties *>(materialProperties));
}
"""

    priContent = """
# %s.pri
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Paul Lemire <paul.lemire@kdab.com>
#
# Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
# accordance with the Kuesa Enterprise License Agreement provided with the Software in the
# LICENSE.KUESA.ENTERPRISE file.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

INCLUDEPATH += $$PWD

QT += 3dcore-private

SOURCES += \\
    $$PWD/%smaterial.cpp \\
    $$PWD/%seffect.cpp \\
    $$PWD/%sproperties.cpp \\
    $$PWD/%sshaderdata.cpp

HEADERS += \\
    $$PWD/%smaterial.h \\
    $$PWD/%seffect.h \\
    $$PWD/%sproperties.h \\
    $$PWD/%sshaderdata_p.h
"""

    def __init__(self, jsonDescriptionFilePath):
        self.jsonDescriptionFilePath = jsonDescriptionFilePath

    def parseDescription(self):
        try:
            with open(self.jsonDescriptionFilePath) as f:
                fileContent = f.read()
                self.rawJson = json.loads(fileContent)
                print(json.dumps(self.rawJson))
        except IOError as e:
            print("Couldn't open file (%s)." % e)

    def propertySetters(self, properties, declarationOnly = True, className = ""):
        content = ""

        doNotConvertToRefTypes = ["float", "int", "bool", "texture2d"]

        for prop in properties:
            rawPropType = prop.get("type", "")
            propType = CustomMaterialGenerator.propertyTypesToQtType[rawPropType]
            paramType = ("%s " if rawPropType in doNotConvertToRefTypes else "const %s &") % (propType)
            propName = prop.get("name", "")
            if declarationOnly:
                content += " " * 4 + "void set%s(%s%s);\n" % (propName[0].upper() + propName[1:],
                                                              paramType,
                                                              propName)
            else:
                setterName = "%s::set%s" % (className, propName[0].upper() + propName[1:])
                contentStr = "void %s(%s%s)\n{\n    if (m_%s == %s)\n        return;\n"
                content += contentStr % (setterName,
                                         paramType,
                                         propName,
                                         propName,
                                         propName)
                if rawPropType.startswith("texture"):
                    contentStr = """
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_%s != nullptr)
        d->unregisterDestructionHelper(m_%s);
    m_%s = %s;
    if (m_%s != nullptr) {
        if (m_%s->parent() == nullptr)
            m_%s->setParent(this);
        d->registerDestructionHelper(m_%s, &%s, m_%s);
    }
    emit %sChanged(m_%s);
}

"""
                    content += contentStr % (propName,
                                             propName,
                                             propName,
                                             propName,
                                             propName,
                                             propName,
                                             propName,
                                             propName,
                                             setterName,
                                             propName,
                                             propName,
                                             propName)

                else:
                    contentStr = " " * 4 + "m_%s = %s;\n    emit %sChanged(%s);\n}\n\n"
                    content += contentStr % (propName, propName, propName, propName)
        return content

    def propertySettersShaderDataForwarding(self, properties, className = ""):
            content = ""

            doNotConvertToRefTypes = ["float", "int", "bool", "texture2d"]

            for prop in properties:
                rawPropType = prop.get("type", "")
                propType = CustomMaterialGenerator.propertyTypesToQtType[rawPropType]
                paramType = ("%s " if rawPropType in doNotConvertToRefTypes else "const %s &") % (propType)
                propName = prop.get("name", "")
                setterSig = "set%s" % (propName[0].upper() + propName[1:])
                content += "void %s::%s(%s%s)\n{\n    m_shaderData->%s(%s);\n}\n\n" % (className,
                                                                                       setterSig,
                                                                                       paramType,
                                                                                       propName,
                                                                                       setterSig,
                                                                                       propName)
            return content

    def propertyGetters(self, properties, declarationOnly = True, className = ""):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            propType = CustomMaterialGenerator.propertyTypesToQtType.get(prop.get("type", ""), "")
            if declarationOnly:
                content += " " * 4 + "%s %s() const;\n" % (propType, propName)
            else:
                contentStr = "%s %s::%s() const\n{\n    return m_%s;\n}\n\n"
                content += contentStr % (propType, className, propName, propName)
        return content

    def propertyGettersShaderDataForwarding(self, properties, className):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            propType = CustomMaterialGenerator.propertyTypesToQtType.get(prop.get("type", ""), "")
            doc = prop.get("doc", "")
            qmlDocStr = "/*!\n    \\qmlproperty %s %s::%s\n    %s\n*/\n" % (propType,
                                                                            className,
                                                                            propName,
                                                                            doc)
            cppDocStr = "/*!\n    \\property %s::%s\n    %s\n*/\n" % (className,
                                                                      propName,
                                                                      doc)
            contentStr = "%s%s%s %s::%s() const\n{\n    return m_shaderData->%s();\n}\n\n"
            content += contentStr % (qmlDocStr,
                                     cppDocStr,
                                     propType,
                                     className,
                                     propName,
                                     propName)
        return content


    def memberInitializations(self, properties):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            propType = prop.get("type", "")
            content += "\n    , m_%s(%s)" % (propName,
                                            "nullptr" if propType.startswith("texture") else "")
        return content

    def propertySignals(self, properties):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            propType = CustomMaterialGenerator.propertyTypesToQtType.get(prop.get("type", ""), "")
            content += " " * 4 + "void %sChanged(%s);\n" % (propName, propType)
        return content

    def propertyMembers(self, properties):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            propType = CustomMaterialGenerator.propertyTypesToQtType.get(prop.get("type", ""), "")
            content += " " * 4 + "%s m_%s;\n" % (propType, propName)
        return content

    def propertyDeclarations(self, properties):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            propType = CustomMaterialGenerator.propertyTypesToQtType.get(prop.get("type", ""), "")
            content += " " * 4 + "Q_PROPERTY(%s %s READ %s WRITE %s NOTIFY %s)\n" % (propType,
                                                                                     propName,
                                                                                     propName,
                                                                                     "set" + propName[0].upper() + propName[1:],
                                                                                     propName + "Changed")
        return content

    def includesForProperties(self, properties):
        uniqueIncludes = set()
        content = ""
        for prop in properties:
            jsonPropType = prop.get("type", "")
            incl = CustomMaterialGenerator.includesForPropertyType[jsonPropType] if jsonPropType in CustomMaterialGenerator.includesForPropertyType else ""
            if len(incl) > 0:
                uniqueIncludes.add(incl)
        for inc in sorted(uniqueIncludes):
            content += "%s\n" % (inc)
        return content

    def docForClass(self, className, inheritedClassName, docContent):
        docStr = """
/*!
    \class Kuesa::%s
    \inheaderfile Kuesa/%s
    \inherits %s
    \inmodule Kuesa
    \since Kuesa 1.2

    \\brief Kuesa::%s %s
*/

/*!
    \qmltype %s
    \instantiates Kuesa::%s
    \inqmlmodule Kuesa
    \since Kuesa 1.2

    \\brief Kuesa::%s %s
*/
"""
        return docStr % (className,
                         className,
                         inheritedClassName,
                         className,
                         docContent,
                         className,
                         className,
                         className,
                         docContent)

    def generateHeaderFile(self, content, className, includes = "", privateHeader = False, declare_metatype = False):
        headerFileName = className.lower() + ("_p" if privateHeader else "")
        includeGuardName = className.upper() + ("_P" if privateHeader else "")
        declareMetaType = "Q_DECLARE_METATYPE(Kuesa::%s*)" % (className)
        headerFileContent = CustomMaterialGenerator.headerContent % (headerFileName,
                                                                     includeGuardName,
                                                                     includeGuardName,
                                                                     CustomMaterialGenerator.headerPrivateWarning if privateHeader else "",
                                                                     includes,
                                                                     content,
                                                                     declareMetaType if declare_metatype else "",
                                                                     includeGuardName)
        with open(headerFileName + ".h", 'w') as f:
            f.write(headerFileContent)

    def generateCppFile(self, content, className, includes = ""):
        cppFileName = className.lower()
        headerFileContent = CustomMaterialGenerator.cppContent % (cppFileName,
                                                                  includes,
                                                                  content)
        with open(cppFileName + ".cpp", 'w') as f:
            f.write(headerFileContent)


    def generateShaderData(self):
        props = self.rawJson.get("properties", [])
        matName = self.rawJson.get("type", "")
        className = matName + "ShaderData"

        def generateHeader():
            setters = self.propertySetters(props, True)
            getters = self.propertyGetters(props, True)
            signals = self.propertySignals(props)
            members = self.propertyMembers(props)
            propDeclarations = self.propertyDeclarations(props)
            content = CustomMaterialGenerator.shaderDataClassHeaderContent % (matName,
                                                                              propDeclarations,
                                                                              matName,
                                                                              matName,
                                                                              getters,
                                                                              setters,
                                                                              signals,
                                                                              members)
            includes = self.includesForProperties(props)
            includes += "#include <Qt3DRender/QShaderData>"
            self.generateHeaderFile(content,
                                    className,
                                    includes,
                                    True)
        generateHeader()

        def generateCpp():
            initializations = self.memberInitializations(props)
            setters = self.propertySetters(props, False, className)
            getters = self.propertyGetters(props, False, className)
            includes = "#include \"%s\"\n" % (className.lower() + "_p.h")
            includes += "#include <Qt3DCore/private/qnode_p.h>\n"
            content = CustomMaterialGenerator.shaderDataClassCppContent % (matName,
                                                                           matName,
                                                                           initializations,
                                                                           matName,
                                                                           matName,
                                                                           getters,
                                                                           setters)
            self.generateCppFile(content,
                                 className,
                                 includes)
        generateCpp()

    def generateMaterialProperties(self):
        props = self.rawJson.get("properties", [])
        matName = self.rawJson.get("type", "")
        className = matName + "Properties"

        def generateHeader():
            setters = self.propertySetters(props, True)
            getters = self.propertyGetters(props, True)
            signals = self.propertySignals(props)
            propDeclarations = self.propertyDeclarations(props)
            content = CustomMaterialGenerator.propertiesClassHeaderContent % (matName,
                                                                              matName,
                                                                              propDeclarations,
                                                                              matName,
                                                                              matName,
                                                                              getters,
                                                                              setters,
                                                                              signals,
                                                                              matName)
            includes = self.includesForProperties(props)
            includes += "#include <Kuesa/GLTF2MaterialProperties>\n"
            includes += "#include <Kuesa/kuesa_global.h>\n"

            self.generateHeaderFile(content,
                                    className,
                                    includes,
                                    declare_metatype=True)
        generateHeader()

        def generateCpp():
            setters = self.propertySettersShaderDataForwarding(props, className)
            getters = self.propertyGettersShaderDataForwarding(props, className)

            connectionStatements = ""
            connectionStatementsStr = "    QObject::connect(m_shaderData, &%sShaderData::%sChanged, this, &%sProperties::%sChanged);\n"
            for p in props:
                propName = p.get("name", "")
                connectionStatements += connectionStatementsStr % (matName,
                                                                   propName,
                                                                   matName,
                                                                   propName)

            doc = self.docForClass(className,
                                   "Kuesa::GLTF2MaterialProperties",
                                   "holds the properties controlling the visual appearance of a %sMaterial instance" % (matName))

            content = CustomMaterialGenerator.propertiesClassCppContent % (doc,
                                                                           matName,
                                                                           matName,
                                                                           matName,
                                                                           connectionStatements,
                                                                           matName,
                                                                           matName,
                                                                           matName,
                                                                           setters,
                                                                           getters)
            includes = "#include \"%sproperties.h\"\n" % (matName.lower())
            includes += "#include \"%sshaderdata_p.h\"\n" % (matName.lower())

            self.generateCppFile(content,
                                 className,
                                 includes)
        generateCpp()

    def generateEffect(self):
        matName = self.rawJson.get("type", "")
        className = matName + "Effect"

        def generateHeader():
            content = CustomMaterialGenerator.effectClassHeaderContent % (matName,
                                                                          matName,
                                                                          matName,
                                                                          matName,
                                                                          matName,
                                                                          matName,
                                                                          matName)
            self.generateHeaderFile(content,
                                    className,
                                    "#include <Kuesa/gltf2materialeffect.h>\n#include <Kuesa/kuesa_global.h>\n")
        generateHeader()


        def generateCpp():
            doc = self.docForClass(className,
                                   "Qt3DRender::QEffect",
                                   "is the effect for the %sMaterial class" % (matName))
            passes = self.rawJson.get("passes", [])
            passes_info = []

            class PassInfo:
                gl3FragCode = ""
                es3FragCode = ""
                es2FragCode = ""
                gl3VertCode = ""
                es3VertCode = ""
                es2VertCode = ""
                gl3GeometryCode = ""
                es3GeometryCode = ""
                es2GeometryCode = ""
                es2VertexShaderGraph = ""
                es2FragmentShaderGraph = ""
                es3VertexShaderGraph = ""
                es3FragmentShaderGraph = ""
                gl3VertexShaderGraph = ""
                gl3FragmentShaderGraph = ""
                blendFunction = ""
                blendSourceRGB = ""
                blendSourceAlpha = ""
                blendDestinationRGB = ""
                blendDestinationAlpha = ""
                pass_type = ""

            # Retrieve Pass information
            for render_pass in passes:
                pass_info = PassInfo()

                # Blending
                blending_obj = render_pass.get("blending", {})
                pass_info.blendFunction = blending_obj.get("function", "Add")
                pass_info.blendSourceRGB = blending_obj.get("sourceRGB", "SourceAlpha")
                pass_info.blendSourceAlpha = blending_obj.get("sourceAlpha", "SourceAlpha")
                pass_info.blendDestinationRGB = blending_obj.get("destinationRGB", "OneMinusSourceAlpha")
                pass_info.blendDestinationAlpha = blending_obj.get("destinationAlpha", "One")

                # Pass Type
                pass_info.pass_type = render_pass.get("type", "TransparentAndOpaque")

                # Shaders
                shaders =render_pass.get("shaders", [])
                for shader in shaders:
                    shaderType = shader.get("type", "")
                    shaderFormat = shader.get("format", {})
                    majorVersion = shaderFormat.get("major", 0)
                    api = shaderFormat.get("api", "")
                    code = shader.get("code", "")
                    graph = shader.get("graph", "")
                    if shaderType == "Fragment":
                        if api == "OpenGLES":
                            if majorVersion == 3:
                                pass_info.es3FragCode = code
                                pass_info.es3FragmentShaderGraph = graph
                            elif majorVersion == 2:
                                pass_info.es2FragCode = code
                                pass_info.es2FragmentShaderGraph = graph
                        elif api == "OpenGL":
                            if majorVersion == 3:
                                pass_info.gl3FragCode = code
                                pass_info.gl3FragmentShaderGraph = graph
                    elif shaderType == "Vertex":
                        if api == "OpenGLES":
                            if majorVersion == 3:
                                pass_info.es3VertCode = code
                                pass_info.es3VertexShaderGraph = graph
                            elif majorVersion == 2:
                                pass_info.es2VertCode = code
                                pass_info.es2VertexShaderGraph = graph
                        elif api == "OpenGL":
                            if majorVersion == 3:
                                pass_info.gl3VertCode = code
                                pass_info.gl3VertexShaderGraph = graph
                    elif shaderType == "Geometry":
                        if api == "OpenGLES":
                            if majorVersion == 3:
                                pass_info.es3GeometryCode = code
                            # ES2 has no geometry shader support
                        elif api == "OpenGL":
                            if majorVersion == 3:
                                pass_info.gl3GeometryCode = code
                passes_info.append(pass_info)


            # Find out what type of technique we need to create based on our passes info

            if len(passes_info) == 0:
                print("No passes found in Material description")
                return
            if len(passes_info) != 1:
                for pass_info in passes_info:
                    if pass_info.pass_type != "MultiPassTransparent":
                        print("Generator only handle Multiple Passes of type MultiPassTransparent")
                        return

            def transparentAndOpaqueTechnique(passes_info):
                pass_info = passes_info[0]
                return CustomMaterialGenerator.techniqueOpaqueAndTransparent % (matName,
                                                                                matName,
                                                                                pass_info.gl3VertexShaderGraph,
                                                                                pass_info.es3VertexShaderGraph,
                                                                                pass_info.es2VertexShaderGraph,
                                                                                pass_info.gl3FragmentShaderGraph,
                                                                                pass_info.es3FragmentShaderGraph,
                                                                                pass_info.es2FragmentShaderGraph,
                                                                                pass_info.gl3VertCode,
                                                                                pass_info.es3VertCode,
                                                                                pass_info.es2VertCode,
                                                                                pass_info.gl3FragCode,
                                                                                pass_info.es3FragCode,
                                                                                pass_info.es2FragCode,
                                                                                pass_info.gl3GeometryCode,
                                                                                pass_info.es3GeometryCode,
                                                                                pass_info.es2GeometryCode,
                                                                                pass_info.blendFunction,
                                                                                pass_info.blendSourceRGB,
                                                                                pass_info.blendSourceAlpha,
                                                                                pass_info.blendDestinationRGB,
                                                                                pass_info.blendDestinationAlpha)

            def transparentOnlyTechnique(passes_info):
                pass_info = passes_info[0]
                return CustomMaterialGenerator.techniqueTransparentOnly % (matName,
                                                                           matName,
                                                                           pass_info.gl3VertexShaderGraph,
                                                                           pass_info.es3VertexShaderGraph,
                                                                           pass_info.es2VertexShaderGraph,
                                                                           pass_info.gl3FragmentShaderGraph,
                                                                           pass_info.es3FragmentShaderGraph,
                                                                           pass_info.es2FragmentShaderGraph,
                                                                           pass_info.gl3VertCode,
                                                                           pass_info.es3VertCode,
                                                                           pass_info.es2VertCode,
                                                                           pass_info.gl3FragCode,
                                                                           pass_info.es3FragCode,
                                                                           pass_info.es2FragCode,
                                                                           pass_info.gl3GeometryCode,
                                                                           pass_info.es3GeometryCode,
                                                                           pass_info.es2GeometryCode,
                                                                           pass_info.blendFunction,
                                                                           pass_info.blendSourceRGB,
                                                                           pass_info.blendSourceAlpha,
                                                                           pass_info.blendDestinationRGB,
                                                                           pass_info.blendDestinationAlpha)


            def opaqueOnlyTechnique(passes_info):
                pass_info = passes_info[0]
                return CustomMaterialGenerator.techniqueOpaqueOnly % (matName,
                                                                      matName,
                                                                      pass_info.gl3VertexShaderGraph,
                                                                      pass_info.es3VertexShaderGraph,
                                                                      pass_info.es2VertexShaderGraph,
                                                                      pass_info.gl3FragmentShaderGraph,
                                                                      pass_info.es3FragmentShaderGraph,
                                                                      pass_info.es2FragmentShaderGraph,
                                                                      pass_info.gl3VertCode,
                                                                      pass_info.es3VertCode,
                                                                      pass_info.es2VertCode,
                                                                      pass_info.gl3FragCode,
                                                                      pass_info.es3FragCode,
                                                                      pass_info.es2FragCode,
                                                                      pass_info.gl3GeometryCode,
                                                                      pass_info.es3GeometryCode,
                                                                      pass_info.es2GeometryCode)


            def multiTransparentTechnique(passes_info):
                innerPassContent = ""

                for idx, pass_info in enumerate(passes_info):
                    innerPassContent +=  CustomMaterialGenerator.techniqueMultiTransparentInnerPass % (pass_info.gl3VertexShaderGraph,
                                                                                                       pass_info.es3VertexShaderGraph,
                                                                                                       pass_info.es2VertexShaderGraph,
                                                                                                       pass_info.gl3FragmentShaderGraph,
                                                                                                       pass_info.es3FragmentShaderGraph,
                                                                                                       pass_info.es2FragmentShaderGraph,
                                                                                                       pass_info.gl3VertCode,
                                                                                                       pass_info.es3VertCode,
                                                                                                       pass_info.es2VertCode,
                                                                                                       pass_info.gl3FragCode,
                                                                                                       pass_info.es3FragCode,
                                                                                                       pass_info.es2FragCode,
                                                                                                       pass_info.gl3GeometryCode,
                                                                                                       pass_info.es3GeometryCode,
                                                                                                       pass_info.es2GeometryCode,
                                                                                                       pass_info.blendFunction,
                                                                                                       pass_info.blendSourceRGB,
                                                                                                       pass_info.blendSourceAlpha,
                                                                                                       pass_info.blendDestinationRGB,
                                                                                                       pass_info.blendDestinationAlpha)
                return CustomMaterialGenerator.techniqueMultiTransparent % (matName,
                                                                            matName,
                                                                            innerPassContent)

            def backgroundTechnique(passes_info):
                pass_info = passes_info[0]
                return CustomMaterialGenerator.techniqueBackgroundOnly % (matName,
                                                                          matName,
                                                                          pass_info.gl3VertexShaderGraph,
                                                                          pass_info.es3VertexShaderGraph,
                                                                          pass_info.es2VertexShaderGraph,
                                                                          pass_info.gl3FragmentShaderGraph,
                                                                          pass_info.es3FragmentShaderGraph,
                                                                          pass_info.es2FragmentShaderGraph,
                                                                          pass_info.gl3VertCode,
                                                                          pass_info.es3VertCode,
                                                                          pass_info.es2VertCode,
                                                                          pass_info.gl3FragCode,
                                                                          pass_info.es3FragCode,
                                                                          pass_info.es2FragCode,
                                                                          pass_info.gl3GeometryCode,
                                                                          pass_info.es3GeometryCode,
                                                                          pass_info.es2GeometryCode)


            def generateTechnique(passes_info):
                switcher = {
                    "Background": backgroundTechnique,
                    "Opaque": opaqueOnlyTechnique,
                    "Transparent": transparentOnlyTechnique,
                    "TransparentAndOpaque": transparentAndOpaqueTechnique,
                    "MultiPassTransparent": multiTransparentTechnique
                }
                pass_type = passes_info[0].pass_type
                if pass_type not in switcher:
                    print("Unhandled pass type")
                    return
                return switcher[pass_type](passes_info), pass_type


            technique_content, technique_name = generateTechnique(passes_info)
            content = CustomMaterialGenerator.effectClassCppContent % (technique_content,
                                                                       doc,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName,
                                                                       matName)
            includes = "#include <Qt3DRender/QEffect>\n"
            includes += "#include <Qt3DRender/QTechnique>\n"
            includes += "#include <Qt3DRender/QCullFace>\n"
            includes += "#include <Qt3DRender/QFilterKey>\n"
            includes += "#include <Qt3DRender/QParameter>\n"
            includes += "#include <Qt3DRender/QRenderPass>\n"
            includes += "#include <Qt3DRender/QShaderProgram>\n"
            includes += "#include <Qt3DRender/QShaderProgramBuilder>\n"
            includes += "#include <Qt3DRender/QGraphicsApiFilter>\n"

            if technique_name in ["Transparent", "TransparentAndOpaque", "MultiPassTransparent"]:
                includes += "#include <Qt3DRender/QBlendEquation>\n"
                includes += "#include <Qt3DRender/QBlendEquationArguments>\n"

            if technique_name == "Background":
                includes += "#include <Qt3DRender/QNoDepthMask>\n"
                includes += "#include <Qt3DRender/QDepthTest>\n"

            includes += "#include \"%s.h\"\n" % (className.lower())

            self.generateCppFile(content,
                                 className,
                                 includes)
        generateCpp()

    def generateMaterial(self):
        matName = self.rawJson.get("type", "")
        className = matName + "Material"

        def generateHeader():
            content = CustomMaterialGenerator.materialClassHeaderContent % (matName,
                                                                            matName,
                                                                            matName,
                                                                            matName,
                                                                            matName,
                                                                            matName,
                                                                            matName,
                                                                            matName,
                                                                            matName)
            self.generateHeaderFile(content,
                                    className,
                                    "#include <Kuesa/GLTF2Material>\n#include <Kuesa/kuesa_global.h>\n")
        generateHeader()

        def generateCpp():
            matName = self.rawJson.get("type", "")
            doc = self.docForClass(className, "Kuesa::GLTF2Material", self.rawJson.get("doc", ""))
            content = CustomMaterialGenerator.materialClassCppContent % (doc,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName)
            includes = "#include \"%smaterial.h\"\n" % (matName.lower())
            includes += "#include \"%sproperties.h\"\n" % (matName.lower())
            includes += "#include \"%sshaderdata_p.h\"\n" % (matName.lower())
            includes += "#include <Qt3DRender/qparameter.h>\n"
            self.generateCppFile(content,
                                 className,
                                 includes)
        generateCpp()

    def generatePri(self):
        matName = self.rawJson.get("type", "").lower()
        content = CustomMaterialGenerator.priContent % (matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName)
        with open(matName + ".pri", 'w') as f:
            f.write(content)

    def generate(self):
        # Parse JSON description file
        self.parseDescription()

        # Generate a new directory to contain all classes
        # and a .pri file
        matName = self.rawJson.get("type", "").lower()
        if os.path.isdir(matName):
            shutil.rmtree(matName)
        os.mkdir(matName)
        os.chdir(matName)

        # Generate and write the different classes and files
        self.generateMaterial()
        self.generateEffect()
        self.generateMaterialProperties()
        self.generateShaderData()
        self.generatePri()

        # Return to previous path
        os.chdir("..")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage " + sys.argv[0] + " material description file")
        sys.exit()

    generator = CustomMaterialGenerator(sys.argv[1])
    generator.generate()
