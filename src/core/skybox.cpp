/*
    skybox.cpp

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

#include "skybox.h"

#include <QtCore/qtimer.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DExtras/qcuboidmesh.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qseamlesscubemap.h>
#include <Qt3DRender/qshaderprogram.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;
using namespace Qt3DRender;

namespace Kuesa {

/*!
 * \class Skybox
 * \inheaderfile Kuesa/Skybox
 * \inmodule  Kuesa
 * \since 1.0
 * \brief Kuesa::SkyBox is a convenience Qt3DCore::QEntity subclass used to
 * insert a skybox in a 3D scene. Unlike Qt3DExtras::SkyboxEntity,
 * its implementation is compatible with the Kuesa provided default framegraph.
 *
 * Be sure to disable frustum culling in the FrameGraph through which the skybox rendering happens.
 *
 * \note Please note that you shouldn't try to render a skybox with an orthographic projection.
 *
 * Kuesa::Skybox uses a baseName and an extension for the textures that will be used for the skybox.
 * Depending on the skybox extension, Kuesa will create different textures to be used as skybox.
 * \list
 * \li *.dds Kuesa tries to load a texture called baseName + extension
 * and uses a Qt3DRender::QTextureLoader to generate the texture
 * \li Anything else: Kuesa tries to load 6 different textures and creates
 * a Qt3DRender::QTextureCubeMap from them
 *     \list
 *     \li baseName + _posx + extension
 *     \li baseName + _negx + extension
 *     \li baseName + _posy + extension
 *     \li baseName + _negy + extension
 *     \li baseName + _posz + extension
 *     \li baseName + _negz + extension
 *
 * \sa extension
 * \sa baseName
 */

/*!
 * \qmltype Skybox
 * \instantiates Kuesa::Skybox
 * \inmodule Kuesa
 * \since 1.0
 * \brief Kuesa.SkyBox is a convenience Qt3D.Core.Entity subclass used to
 * insert a skybox in a 3D scene. Unlike SkyboxEntity from Qt3D.Extras,
 * its implementation is compatible with the Kuesa provided default framegraph.
 *
 * Be sure to disable frustum culling in the FrameGraph through which the skybox rendering happens.
 *
 * \note Please note that you shouldn't try to render a skybox with an orthographic projection.
 *
 * Kuesa.Skybox uses a baseName and an extension for the textures that will be used for the skybox.
 * Depending on the skybox extension, Kuesa will create different textures to be used as skybox.
 * \list
 * \li *.dds Kuesa tries to load a texture called baseName + extension
 * and uses a Qt3D.Render.QTextureLoader to generate the texture
 * \li Anything else: Kuesa tries to load 6 different textures and creates
 * a Qt3D.Render.TextureCubeMap from them
 *     \list
 *     \li baseName + _posx + extension
 *     \li baseName + _negx + extension
 *     \li baseName + _posy + extension
 *     \li baseName + _negy + extension
 *     \li baseName + _posz + extension
 *     \li baseName + _negz + extension
 *
 * \sa extension
 * \sa baseName
 */

Skybox::Skybox(QNode *parent)
    : QEntity(parent)
    , m_gammaStrengthParameter(new QParameter(QStringLiteral("gammaStrength"), 0.0f))
    , m_textureParameter(new QParameter())
    , m_texture(nullptr)
    , m_extension(QStringLiteral(".png"))
    , m_hasPendingReloadTextureCall(false)
{
    m_textureParameter->setName(QStringLiteral("skyboxTexture"));

    struct TechniqueDescription
    {
        QGraphicsApiFilter::Api api;
        QGraphicsApiFilter::OpenGLProfile profile;
        int majorV, minorV;
        QUrl vertexShaderUrl;
        QUrl fragmentShaderUrl;
    };

    static const TechniqueDescription descriptions[] = {
        {
            QGraphicsApiFilter::OpenGL,
            QGraphicsApiFilter::CoreProfile,
            3, 3,
            QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/skybox.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/skybox.frag"))
        },
        {
            QGraphicsApiFilter::OpenGL,
            QGraphicsApiFilter::NoProfile,
            2, 0,
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/skybox.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/skybox.frag"))
        },
        {
            QGraphicsApiFilter::OpenGLES,
            QGraphicsApiFilter::NoProfile,
            2, 0,
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/skybox.vert")),
            QUrl(QStringLiteral("qrc:/kuesa/shaders/es2/skybox.frag"))
        }
    };

    auto effect = new QEffect();

    auto techniqueFilterKey = new QFilterKey(effect);
    techniqueFilterKey->setName(QStringLiteral("renderingStyle"));
    techniqueFilterKey->setValue(QStringLiteral("forward"));

    auto opaquePassFilterKey = new Qt3DRender::QFilterKey(effect);
    opaquePassFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    opaquePassFilterKey->setValue(QStringLiteral("Opaque"));

    auto defaultPassFilterKey = new Qt3DRender::QFilterKey(effect);
    defaultPassFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    defaultPassFilterKey->setValue(QStringLiteral("Default"));

    QCullFace *cullFront = new QCullFace();
    cullFront->setMode(QCullFace::Front);
    QDepthTest *depthTest = new QDepthTest();
    depthTest->setDepthFunction(QDepthTest::LessOrEqual);
    QSeamlessCubemap *seamlessCubemap = new QSeamlessCubemap();

    for (int i = 0, m = sizeof(descriptions) / sizeof(TechniqueDescription); i < m; ++i) {
        const TechniqueDescription description = descriptions[i];
        auto shader = new QShaderProgram();
        auto technique = new QTechnique();
        auto renderPass = new QRenderPass();

        shader->setVertexShaderCode(QShaderProgram::loadSource(description.vertexShaderUrl));
        shader->setFragmentShaderCode(QShaderProgram::loadSource(description.fragmentShaderUrl));
        technique->graphicsApiFilter()->setApi(description.api);
        technique->graphicsApiFilter()->setProfile(description.profile);
        technique->graphicsApiFilter()->setMajorVersion(description.majorV);
        technique->graphicsApiFilter()->setMinorVersion(description.minorV);
        technique->addFilterKey(techniqueFilterKey);
        technique->addRenderPass(renderPass);
        renderPass->setShaderProgram(shader);
        renderPass->addRenderState(cullFront);
        renderPass->addRenderState(depthTest);
        renderPass->addRenderState(seamlessCubemap);
        renderPass->addFilterKey(defaultPassFilterKey);
        renderPass->addFilterKey(opaquePassFilterKey);
        effect->addTechnique(technique);
    }

    auto material = new QMaterial();
    material->setEffect(effect);
    material->addParameter(m_gammaStrengthParameter);
    material->addParameter(m_textureParameter);

    auto mesh = new Qt3DExtras::QCuboidMesh();
    mesh->setXYMeshResolution(QSize(2, 2));
    mesh->setXZMeshResolution(QSize(2, 2));
    mesh->setYZMeshResolution(QSize(2, 2));

    addComponent(mesh);
    addComponent(material);
}

Skybox::~Skybox()
{
}

/*!
 * \qmlproperty string Skybox::baseName
 * Base name for the texture(s) that will be used as skybox
 */

/*!
 * \brief Skybox::setBaseName Sets the base name for the texture(s) that will be used as skybox
 */
void Skybox::setBaseName(const QString &baseName)
{
    if (baseName != m_baseName) {
        m_baseName = baseName;
        emit baseNameChanged(baseName);
        reloadTexture();
    }
}

/*!
 * \brief Skybox::baseName The skybox base name
 */
QString Skybox::baseName() const
{
    return m_baseName;
}

/*!
 * \qmlproperty string Skybox::extension
 * The extension of the skybox texture.
 */

/*!
 * \brief Skybox::setExtension Sets the extension for the texture(s) that will be used as skybox
 */
void Skybox::setExtension(const QString &extension)
{
    if (extension != m_extension) {
        m_extension = extension;
        emit extensionChanged(extension);
        reloadTexture();
    }
}

/*!
 * \brief Skybox::extension The skybox extension
 */
QString Skybox::extension() const
{
    return m_extension;
}

/*!
 * \qmlproperty bool Skybox::gammaCorrect
 *
 * True is gamma correction is enabled. False otherwise.
 */

/*!
 * \brief Skybox::setGammaCorrectEnabled Enables/disables gamma correction
 */
void Skybox::setGammaCorrectEnabled(bool enabled)
{
    if (enabled != isGammaCorrectEnabled()) {
        m_gammaStrengthParameter->setValue(enabled ? 1.0f : 0.0f);
        emit gammaCorrectEnabledChanged(enabled);
    }
}

/*!
 * \brief Skybox::isGammaCorrectEnabled True is gamma correction is enabled. False otherwise
 */
bool Skybox::isGammaCorrectEnabled() const
{
    return !qFuzzyIsNull(m_gammaStrengthParameter->value().toFloat());
}

void Skybox::reloadTexture()
{
    if (!m_hasPendingReloadTextureCall) {
        m_hasPendingReloadTextureCall = true;
        QTimer::singleShot(0, this, [this] {
            if (m_extension == QStringLiteral(".dds")) {
                QTextureLoader *textureLoader = qobject_cast<QTextureLoader *>(m_texture);
                if (textureLoader == nullptr) {
                    delete m_texture;
                    textureLoader = new QTextureLoader(this);
                    m_texture = textureLoader;
                }
                textureLoader->setSource(QUrl(m_baseName + m_extension));
            } else {
                QTextureCubeMap *cubeMapTexture = qobject_cast<QTextureCubeMap *>(m_texture);
                if (cubeMapTexture == nullptr) {
                    delete m_texture;
                    cubeMapTexture = new QTextureCubeMap(this);
                    for (int i = 0; i < 6; ++i) {
                        QTextureImage *faceImage = new QTextureImage();
                        faceImage->setMirrored(false);
                        faceImage->setFace(static_cast<QTextureCubeMap::CubeMapFace>(QTextureCubeMap::CubeMapPositiveX + i));
                        cubeMapTexture->addTextureImage(faceImage);
                    }
                    m_texture = cubeMapTexture;
                }
                static const QString faceSuffixes[] = {
                    QStringLiteral("_posx"),
                    QStringLiteral("_negx"),
                    QStringLiteral("_posy"),
                    QStringLiteral("_negy"),
                    QStringLiteral("_posz"),
                    QStringLiteral("_negz")
                };
                for (int i = 0; i < 6; ++i)
                    static_cast<QTextureImage *>(m_texture->textureImages().at(i))->setSource(QUrl(m_baseName + faceSuffixes[i] + m_extension));
            }
            m_texture->setMagnificationFilter(QTextureCubeMap::Linear);
            m_texture->setMinificationFilter(QTextureCubeMap::Linear);
            m_texture->setGenerateMipMaps(false);
            m_texture->setWrapMode(QTextureWrapMode(QTextureWrapMode::ClampToEdge));
            m_texture->setGenerateMipMaps(false);
            m_textureParameter->setValue(QVariant::fromValue(m_texture));
            m_hasPendingReloadTextureCall = false;
        });
    }
}

} // namespace Kuesa

QT_END_NAMESPACE
