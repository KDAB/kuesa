/*
    texturepreviewmaterial.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "texturepreviewmaterial.h"
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QShaderProgram>

TexturePreviewMaterial::TexturePreviewMaterial(Qt3DCore::QNode *parent)
    : Qt3DRender::QMaterial(parent)
    , m_textureParameter(new Qt3DRender::QParameter())
    , m_texture(nullptr)
{
    struct TechniqueDescription
    {
        Qt3DRender::QGraphicsApiFilter::Api api;
        Qt3DRender::QGraphicsApiFilter::OpenGLProfile profile;
        int majorV, minorV;
        QUrl vertexShaderUrl;
        QUrl fragmentShaderUrl;
    };

    static const TechniqueDescription descriptions[] = {
        {
            Qt3DRender::QGraphicsApiFilter::OpenGL,
            Qt3DRender::QGraphicsApiFilter::CoreProfile,
            3, 3,
            QUrl(QStringLiteral("qrc:/shaders/gl3/screen_quad.vert")),
            QUrl(QStringLiteral("qrc:/shaders/gl3/screen_quad.frag"))
        },
        {
            Qt3DRender::QGraphicsApiFilter::OpenGL,
            Qt3DRender::QGraphicsApiFilter::NoProfile,
            2, 0,
            QUrl(QStringLiteral("qrc:/shaders/es2/screen_quad.vert")),
            QUrl(QStringLiteral("qrc:/shaders/es2/screen_quad.frag"))
        },
        {
            Qt3DRender::QGraphicsApiFilter::OpenGLES,
            Qt3DRender::QGraphicsApiFilter::NoProfile,
            2, 0,
            QUrl(QStringLiteral("qrc:/shaders/es2/screen_quad.vert")),
            QUrl(QStringLiteral("qrc:/shaders/es2/screen_quad.frag"))
        }
    };

    auto effect = new Qt3DRender::QEffect();

    auto passFilterKey = new Qt3DRender::QFilterKey(effect);
    passFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    passFilterKey->setValue(QStringLiteral("Opaque"));

    auto techniqueFilterKey = new Qt3DRender::QFilterKey(effect);
    techniqueFilterKey->setName(QStringLiteral("renderingStyle"));
    techniqueFilterKey->setValue(QStringLiteral("forward"));

    for (int i = 0, m = sizeof(descriptions) / sizeof(TechniqueDescription); i < m; ++i) {
        const TechniqueDescription description = descriptions[i];
        auto shader = new Qt3DRender::QShaderProgram();
        auto technique = new Qt3DRender::QTechnique();
        auto renderPass = new Qt3DRender::QRenderPass();

        shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(description.vertexShaderUrl));
        shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(description.fragmentShaderUrl));
        technique->graphicsApiFilter()->setApi(description.api);
        technique->graphicsApiFilter()->setProfile(description.profile);
        technique->graphicsApiFilter()->setMajorVersion(description.majorV);
        technique->graphicsApiFilter()->setMinorVersion(description.minorV);
        technique->addFilterKey(techniqueFilterKey);
        renderPass->addFilterKey(passFilterKey);
        technique->addRenderPass(renderPass);
        renderPass->setShaderProgram(shader);
        effect->addTechnique(technique);
    }

    setEffect(effect);

    m_textureParameter->setName(QStringLiteral("tex"));
    addParameter(m_textureParameter);
}

TexturePreviewMaterial::~TexturePreviewMaterial()
{
}

void TexturePreviewMaterial::setTexture(Qt3DRender::QAbstractTexture *texture)
{
    if (texture == m_texture)
        return;
    m_texture = texture;
    m_textureParameter->setValue(QVariant::fromValue(texture));
    emit textureChanged();
}

Qt3DRender::QAbstractTexture *TexturePreviewMaterial::texture() const
{
    return m_texture;
}
