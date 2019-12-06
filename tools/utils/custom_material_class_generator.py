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

    headerContent = """
/*
    %s.h

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

#ifndef KUESA_%s_H
#define KUESA_%s_H

%s

QT_BEGIN_NAMESPACE

namespace Kuesa {
%s
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_%s_H
"""

    cppContent = """
/*
    %s.cpp

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

class KUESASHARED_EXPORT %sEffect : public Qt3DRender::QEffect
{
    Q_OBJECT
    Q_PROPERTY(bool doubleSided READ isDoubleSided WRITE setDoubleSided NOTIFY doubleSidedChanged)
    Q_PROPERTY(bool useSkinning READ useSkinning WRITE setUseSkinning NOTIFY useSkinningChanged)
    Q_PROPERTY(bool opaque READ isOpaque WRITE setOpaque NOTIFY opaqueChanged)
    Q_PROPERTY(bool alphaCutoffEnabled READ isAlphaCutoffEnabled WRITE setAlphaCutoffEnabled NOTIFY alphaCutoffEnabledChanged)

public:
    Q_INVOKABLE explicit %sEffect(Qt3DCore::QNode *parent = nullptr);
    ~%sEffect();

    bool isDoubleSided() const;
    bool useSkinning() const;
    bool isOpaque() const;
    bool isAlphaCutoffEnabled() const;

public Q_SLOTS:
    void setDoubleSided(bool doubleSided);
    void setUseSkinning(bool useSkinning);
    void setOpaque(bool opaque);
    void setAlphaCutoffEnabled(bool enabled);

Q_SIGNALS:
    void doubleSidedChanged(bool doubleSided);
    void useSkinningChanged(bool useSkinning);
    void opaqueChanged(bool opaque);
    void alphaCutoffEnabledChanged(bool enabled);

private:
    bool m_useSkinning;
    bool m_opaque;
    bool m_alphaCutoffEnabled;
    %sTechnique *m_gl3Technique;
    %sTechnique *m_es3Technique;
    %sTechnique *m_es2Technique;
};
"""

    effectClassCppContent = """
class %sTechnique : public Qt3DRender::QTechnique
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

%s

%sEffect::%sEffect(Qt3DCore::QNode *parent)
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

    m_gl3Technique = new %sTechnique(%sTechnique::GL3, this);
    m_gl3Technique->setEnabledLayers(enabledLayers);

    m_es3Technique = new %sTechnique(%sTechnique::ES3, this);
    m_es3Technique->setEnabledLayers(enabledLayers);

    m_es2Technique = new %sTechnique(%sTechnique::ES2, this);
    m_es2Technique->setEnabledLayers(enabledLayers);

    addTechnique(m_gl3Technique);
    addTechnique(m_es3Technique);
    addTechnique(m_es2Technique);
}

%sEffect::~%sEffect() = default;

bool %sEffect::isDoubleSided() const
{
    return m_gl3Technique->cullingMode() == QCullFace::NoCulling;
}

bool %sEffect::useSkinning() const
{
    return m_useSkinning;
}

bool %sEffect::isOpaque() const
{
    return m_opaque;
}

bool %sEffect::isAlphaCutoffEnabled() const
{
    return m_alphaCutoffEnabled;
}

void %sEffect::setDoubleSided(bool doubleSided)
{
    const auto cullingMode = doubleSided ? QCullFace::NoCulling : QCullFace::Back;
    m_gl3Technique->setCullingMode(cullingMode);
    m_es3Technique->setCullingMode(cullingMode);
    m_es2Technique->setCullingMode(cullingMode);
}

void %sEffect::setUseSkinning(bool useSkinning)
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

void %sEffect::setOpaque(bool opaque)
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

void %sEffect::setAlphaCutoffEnabled(bool enabled)
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
        for inc in uniqueIncludes:
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

    def generateHeaderFile(self, content, className, includes = "", privateHeader = False):
        headerFileName = className.lower() + ("_p" if privateHeader else "")
        includeGuardName = className.upper() + ("_P" if privateHeader else "")
        headerFileContent = CustomMaterialGenerator.headerContent % (headerFileName,
                                                                     includeGuardName,
                                                                     includeGuardName,
                                                                     includes,
                                                                     content,
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
                                    includes)
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
                                    "#include <Qt3DRender/QEffect>\n#include <Kuesa/kuesa_global.h>\n")
        generateHeader()


        def generateCpp():
            doc = self.docForClass(className,
                                   "Qt3DRender::QEffect",
                                   "is the effect for the %sMaterial class" % (matName))

            gl3FragCode = ""
            es3FragCode = ""
            es2FragCode = ""
            gl3VertCode = ""
            es3VertCode = ""
            es2VertCode = ""
            gl3GeometryCode = ""
            es3GeometryCode = ""
            es2GeometryCode = ""

            shaders = self.rawJson.get("shaders", [])
            for shader in shaders:
                shaderType = shader.get("type", "")
                shaderFormat = shader.get("format", {})
                majorVersion = shaderFormat.get("major", 0)
                api = shaderFormat.get("api", "")
                code = shader.get("code", "")
                if shaderType == "Fragment":
                    if api == "OpenGLES":
                        if majorVersion == 3:
                            es3FragCode = code
                        elif majorVersion == 2:
                            es2FragCode = code
                    elif api == "OpenGL":
                        if majorVersion == 3:
                            gl3FragCode = code
                elif shaderType == "Vertex":
                    if api == "OpenGLES":
                        if majorVersion == 3:
                            es3VertCode = code
                        elif majorVersion == 2:
                            es2VertCode = code
                    elif api == "OpenGL":
                        if majorVersion == 3:
                            gl3VertCode = code
                elif shaderType == "Geometry":
                    if api == "OpenGLES":
                        if majorVersion == 3:
                            es3GeometryCode = code
                        # ES2 has no geometry shader support
                    elif api == "OpenGL":
                        if majorVersion == 3:
                            gl3GeometryCode = code

            content = CustomMaterialGenerator.effectClassCppContent % (matName,
                                                                       matName,
                                                                       gl3VertCode,
                                                                       es3VertCode,
                                                                       es2VertCode,
                                                                       gl3FragCode,
                                                                       es3FragCode,
                                                                       es2FragCode,
                                                                       gl3GeometryCode,
                                                                       es3GeometryCode,
                                                                       es2GeometryCode,
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
