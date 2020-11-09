/*
    particlematerial.cpp

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mauro Persano <mauro.persano@kdab.com>

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

#include "particlematerial_p.h"

#include "noisetextureimage_p.h"

#include <QUrl>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QShaderProgramBuilder>

#include <numeric>

using namespace Qt3DRender;

QT_BEGIN_NAMESPACE

namespace Kuesa {

namespace {
struct Particle {
    float velocity[4];
    float position[4];
    float age;
    float lifespan;
    float rotation;
    float rotationRate;
};
static_assert(sizeof(Particle) % (4 * sizeof(float)) == 0, "Particle struct must be aligned to a vec4 boundary");
} // namespace

ParticleMaterial::ParticleMaterial(Qt3DCore::QNode *parent)
    : QMaterial(parent)
    , m_particleCount(new QParameter(QStringLiteral("particleCount"), 32))
    , m_particleBuffer(new QParameter(QStringLiteral("particleBuffer"), QVariant::fromValue(new Qt3DGeometry::QBuffer(this))))
    , m_deadListBuffer(new QParameter(QStringLiteral("deadListBuffer"), QVariant::fromValue(new Qt3DGeometry::QBuffer(this))))
    , m_sortKeyBuffer(new QParameter(QStringLiteral("sortKeyBuffer"), QVariant::fromValue(new Qt3DGeometry::QBuffer(this))))
    , m_randomBuffer(new QParameter(QStringLiteral("randomBuffer"), QVariant::fromValue(new QTexture2D)))
    , m_frameTime(new QParameter(QStringLiteral("frameTime"), 1.0f / 60.f))
    , m_maxParticlesEmittedPerFrame(new QParameter(QStringLiteral("maxParticlesEmittedPerFrame"), 4))
    , m_gravity(new QParameter(QStringLiteral("gravity"), QVector3D(0.0f, 0.0f, 0.0f)))
    , m_emitterPosition(new QParameter(QStringLiteral("emitterPosition"), QVector3D(0.0f, 0.0f, 0.0f)))
    , m_emitterPositionRandom(new QParameter(QStringLiteral("emitterPositionRandom"), QVector3D(0.0f, 0.0f, 0.0f)))
    , m_emitterVelocity(new QParameter(QStringLiteral("emitterVelocity"), QVector3D(0.0f, 1.0f, 0.0f)))
    , m_emitterVelocityRandom(new QParameter(QStringLiteral("emitterVelocityRandom"), QVector3D(0.0f, 0.0f, 0.0f)))
    , m_particleLifespan(new QParameter(QStringLiteral("particleLifespan"), 1.0f))
    , m_particleLifespanRandom(new QParameter(QStringLiteral("particleLifespanRandom"), 0.0f))
    , m_initialSize(new QParameter(QStringLiteral("initialSize"), QVector2D(1.0f, 1.0f)))
    , m_finalSize(new QParameter(QStringLiteral("finalSize"), QVector2D(1.0f, 1.0f)))
    , m_initialColor(new QParameter(QStringLiteral("initialColor"), QVector4D(1.0f, 1.0f, 1.0f, 1.0f)))
    , m_finalColor(new QParameter(QStringLiteral("finalColor"), QVector4D(1.0f, 1.0f, 1.0f, 1.0f)))
    , m_initialAngle(new QParameter(QStringLiteral("initialAngle"), 0.0f))
    , m_initialAngleRandom(new QParameter(QStringLiteral("initialAngleRandom"), 0.0f))
    , m_rotationRate(new QParameter(QStringLiteral("rotationRate"), 0.0f))
    , m_rotationRateRandom(new QParameter(QStringLiteral("rotationRateRandom"), 0.0f))
    , m_spriteTexture(new QParameter(QStringLiteral("spriteTexture"), QVariant::fromValue(new QTexture2D)))
    , m_alignMode(Particles::AlignMode::FaceCamera)
{
    connect(m_particleCount, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit particleCountChanged(v.toInt()); });
    connect(m_particleCount, &QParameter::valueChanged, this, &ParticleMaterial::updateBuffers);
    connect(m_frameTime, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit frameTimeChanged(v.toFloat()); });
    connect(m_maxParticlesEmittedPerFrame, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit maxParticlesEmittedPerFrameChanged(v.toInt()); });
    connect(m_gravity, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit gravityChanged(v.value<QVector3D>()); });
    connect(m_emitterPosition, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit emitterPositionChanged(v.value<QVector3D>()); });
    connect(m_emitterPositionRandom, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit emitterPositionRandomChanged(v.value<QVector3D>()); });
    connect(m_emitterVelocity, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit emitterVelocityChanged(v.value<QVector3D>()); });
    connect(m_emitterVelocityRandom, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit emitterVelocityRandomChanged(v.value<QVector3D>()); });
    connect(m_particleLifespan, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit particleLifespanChanged(v.toFloat()); });
    connect(m_particleLifespanRandom, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit particleLifespanRandomChanged(v.toFloat()); });
    connect(m_initialSize, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit initialSizeChanged(v.value<QVector2D>()); });
    connect(m_finalSize, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit finalSizeChanged(v.value<QVector2D>()); });
    connect(m_initialColor, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit initialColorChanged(v.value<QVector4D>()); });
    connect(m_finalColor, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit finalColorChanged(v.value<QVector4D>()); });
    connect(m_initialAngle, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit initialAngleChanged(v.toFloat()); });
    connect(m_initialAngleRandom, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit initialAngleRandomChanged(v.toFloat()); });
    connect(m_rotationRate, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit rotationRateChanged(v.toFloat()); });
    connect(m_rotationRateRandom, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit rotationRateRandomChanged(v.toFloat()); });
    connect(m_spriteTexture, &QParameter::valueChanged, this,
            [this](const QVariant &v) { emit spriteTextureChanged(v.value<QAbstractTexture *>()); });

    auto *randomBuffer = m_randomBuffer->value().value<QAbstractTexture *>();
    randomBuffer->setMinificationFilter(QAbstractTexture::Nearest);
    randomBuffer->setMagnificationFilter(QAbstractTexture::Nearest);
    randomBuffer->setWrapMode(QTextureWrapMode(QTextureWrapMode::Repeat));
    randomBuffer->setGenerateMipMaps(false);
    randomBuffer->addTextureImage(new NoiseTextureImage);

    const auto addFilterKey = [](auto *parent, const QString &name, const QVariant &value) {
        auto filterKey = new QFilterKey(parent);
        filterKey->setName(name);
        filterKey->setValue(value);
        parent->addFilterKey(filterKey);
    };

    auto emitShaderProgram = new QShaderProgram;
    emitShaderProgram->setComputeShaderCode(
            QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/particleemit.comp"))));

    auto emitPass = new QRenderPass;
    addFilterKey(emitPass, QStringLiteral("KuesaComputeStage"), QStringLiteral("ParticleEmit"));
    emitPass->setShaderProgram(emitShaderProgram);
    emitPass->addParameter(m_maxParticlesEmittedPerFrame);
    emitPass->addParameter(m_randomBuffer);
    emitPass->addParameter(m_emitterPosition);
    emitPass->addParameter(m_emitterPositionRandom);
    emitPass->addParameter(m_emitterVelocity);
    emitPass->addParameter(m_emitterVelocityRandom);
    emitPass->addParameter(m_particleLifespan);
    emitPass->addParameter(m_particleLifespanRandom);
    emitPass->addParameter(m_initialAngle);
    emitPass->addParameter(m_initialAngleRandom);
    emitPass->addParameter(m_rotationRate);
    emitPass->addParameter(m_rotationRateRandom);

    auto simulateShaderProgram = new QShaderProgram;
    simulateShaderProgram->setComputeShaderCode(
            QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/particlesimulate.comp"))));

    auto simulatePass = new QRenderPass;
    addFilterKey(simulatePass, QStringLiteral("KuesaComputeStage"), QStringLiteral("ParticleSimulate"));
    simulatePass->setShaderProgram(simulateShaderProgram);
    simulatePass->addParameter(m_frameTime);
    simulatePass->addParameter(m_gravity);

    auto sortShaderProgram = new QShaderProgram;
    sortShaderProgram->setComputeShaderCode(
            QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/particlesort.comp"))));

    auto sortPass = new QRenderPass;
    addFilterKey(sortPass, QStringLiteral("KuesaComputeStage"), QStringLiteral("ParticleSort"));
    sortPass->setShaderProgram(sortShaderProgram);

    auto renderShaderProgram = new QShaderProgram;
    renderShaderProgram->setVertexShaderCode(
            QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/particle.vert"))));
    renderShaderProgram->setFragmentShaderCode(
            QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl3/particle.frag"))));

    m_renderShaderProgramBuilder = new QShaderProgramBuilder(this);
    m_renderShaderProgramBuilder->setShaderProgram(renderShaderProgram);
    m_renderShaderProgramBuilder->setGeometryShaderGraph(
            QUrl(QStringLiteral("qrc:/kuesa/shaders/graphs/particle.geom.json")));
    m_renderShaderProgramBuilder->setEnabledLayers({ QStringLiteral("alignModeFaceCamera") });

    auto renderPass = new QRenderPass;
    addFilterKey(renderPass, QStringLiteral("KuesaDrawStage"), QStringLiteral("ParticleRender"));
    renderPass->setShaderProgram(renderShaderProgram);
    renderPass->addParameter(m_initialSize);
    renderPass->addParameter(m_finalSize);
    renderPass->addParameter(m_initialColor);
    renderPass->addParameter(m_finalColor);
    renderPass->addParameter(m_spriteTexture);

    auto cullFace = new QCullFace();
    cullFace->setMode(QCullFace::NoCulling);
    renderPass->addRenderState(cullFace);

    renderPass->addRenderState(new QNoDepthMask);

    auto blendEquationArguments = new QBlendEquationArguments();
    blendEquationArguments->setSourceRgb(QBlendEquationArguments::SourceAlpha);
    blendEquationArguments->setSourceAlpha(QBlendEquationArguments::SourceAlpha);
    blendEquationArguments->setDestinationRgb(QBlendEquationArguments::OneMinusSourceAlpha);
    blendEquationArguments->setDestinationAlpha(QBlendEquationArguments::One);
    renderPass->addRenderState(blendEquationArguments);

    auto blendEquation = new QBlendEquation;
    blendEquation->setBlendFunction(QBlendEquation::Add);
    renderPass->addRenderState(blendEquation);

    auto technique = new QTechnique;
    technique->addRenderPass(emitPass);
    technique->addRenderPass(simulatePass);
    technique->addRenderPass(sortPass);
    technique->addRenderPass(renderPass);

    auto graphicsApiFilter = technique->graphicsApiFilter();
    graphicsApiFilter->setApi(QGraphicsApiFilter::OpenGL);
    graphicsApiFilter->setProfile(QGraphicsApiFilter::CoreProfile);
    graphicsApiFilter->setMajorVersion(4);
    graphicsApiFilter->setMinorVersion(3);

    auto effect = new QEffect;
    effect->addTechnique(technique);
    effect->addParameter(m_particleCount);
    effect->addParameter(m_particleBuffer);
    effect->addParameter(m_deadListBuffer);
    effect->addParameter(m_sortKeyBuffer);

    setEffect(effect);

    updateBuffers();
}

int ParticleMaterial::particleCount() const
{
    return m_particleCount->value().toInt();
}

void ParticleMaterial::setParticleCount(int particleCount)
{
    m_particleCount->setValue(particleCount);
}

float ParticleMaterial::frameTime() const
{
    return m_frameTime->value().toFloat();
}

void ParticleMaterial::setFrameTime(float frameTime)
{
    m_frameTime->setValue(frameTime);
}

int ParticleMaterial::maxParticlesEmittedPerFrame() const
{
    return m_maxParticlesEmittedPerFrame->value().toInt();
}

void ParticleMaterial::setMaxParticlesEmittedPerFrame(int maxParticlesEmittedPerFrame)
{
    m_maxParticlesEmittedPerFrame->setValue(maxParticlesEmittedPerFrame);
}

QVector3D ParticleMaterial::gravity() const
{
    return m_gravity->value().value<QVector3D>();
}

void ParticleMaterial::setGravity(const QVector3D &gravity)
{
    m_gravity->setValue(gravity);
}

QVector3D ParticleMaterial::emitterPosition() const
{
    return m_emitterPosition->value().value<QVector3D>();
}

void ParticleMaterial::setEmitterPosition(const QVector3D &emitterPosition)
{
    m_emitterPosition->setValue(emitterPosition);
}

QVector3D ParticleMaterial::emitterPositionRandom() const
{
    return m_emitterPositionRandom->value().value<QVector3D>();
}

void ParticleMaterial::setEmitterPositionRandom(const QVector3D &emitterPositionRandom)
{
    m_emitterPositionRandom->setValue(emitterPositionRandom);
}

QVector3D ParticleMaterial::emitterVelocity() const
{
    return m_emitterVelocity->value().value<QVector3D>();
}

void ParticleMaterial::setEmitterVelocity(const QVector3D &emitterVelocity)
{
    m_emitterVelocity->setValue(emitterVelocity);
}

QVector3D ParticleMaterial::emitterVelocityRandom() const
{
    return m_emitterVelocityRandom->value().value<QVector3D>();
}

void ParticleMaterial::setEmitterVelocityRandom(const QVector3D &emitterVelocityRandom)
{
    m_emitterVelocityRandom->setValue(emitterVelocityRandom);
}

float ParticleMaterial::particleLifespan() const
{
    return m_particleLifespan->value().toFloat();
}

void ParticleMaterial::setParticleLifespan(float particleLifespan)
{
    m_particleLifespan->setValue(particleLifespan);
}

float ParticleMaterial::particleLifespanRandom() const
{
    return m_particleLifespanRandom->value().toFloat();
}

void ParticleMaterial::setParticleLifespanRandom(float particleLifespanRandom)
{
    m_particleLifespanRandom->setValue(particleLifespanRandom);
}

QVector2D ParticleMaterial::initialSize() const
{
    return m_initialSize->value().value<QVector2D>();
}

void ParticleMaterial::setInitialSize(const QVector2D &initialSize)
{
    m_initialSize->setValue(initialSize);
}

QVector2D ParticleMaterial::finalSize() const
{
    return m_finalSize->value().value<QVector2D>();
}

void ParticleMaterial::setFinalSize(const QVector2D &finalSize)
{
    m_finalSize->setValue(finalSize);
}

QVector4D ParticleMaterial::initialColor() const
{
    return m_initialColor->value().value<QVector4D>();
}

void ParticleMaterial::setInitialColor(const QVector4D &initialColor)
{
    m_initialColor->setValue(initialColor);
}

QVector4D ParticleMaterial::finalColor() const
{
    return m_finalColor->value().value<QVector4D>();
}

void ParticleMaterial::setFinalColor(const QVector4D &finalColor)
{
    m_finalColor->setValue(finalColor);
}

float ParticleMaterial::initialAngle() const
{
    return m_initialAngle->value().toFloat();
}

void ParticleMaterial::setInitialAngle(float initialAngle)
{
    m_initialAngle->setValue(initialAngle);
}

float ParticleMaterial::initialAngleRandom() const
{
    return m_initialAngleRandom->value().toFloat();
}

void ParticleMaterial::setInitialAngleRandom(float initialAngleRandom)
{
    m_initialAngleRandom->setValue(initialAngleRandom);
}

float ParticleMaterial::rotationRate() const
{
    return m_rotationRate->value().toFloat();
}

void ParticleMaterial::setRotationRate(float rotationRate)
{
    m_rotationRate->setValue(rotationRate);
}

float ParticleMaterial::rotationRateRandom() const
{
    return m_rotationRateRandom->value().toFloat();
}

void ParticleMaterial::setRotationRateRandom(float rotationRateRandom)
{
    m_rotationRateRandom->setValue(rotationRateRandom);
}

QAbstractTexture *ParticleMaterial::spriteTexture() const
{
    return m_spriteTexture->value().value<QAbstractTexture *>();
}

void ParticleMaterial::setSpriteTexture(QAbstractTexture *spriteTexture)
{
    m_spriteTexture->setValue(QVariant::fromValue(spriteTexture));
}

Particles::AlignMode ParticleMaterial::alignMode() const
{
    return m_alignMode;
}

void ParticleMaterial::setAlignMode(Particles::AlignMode alignMode)
{
    if (m_alignMode == alignMode)
        return;
    const auto layerName = [alignMode] {
        switch (alignMode) {
        case Particles::AlignMode::FaceCamera:
            return QStringLiteral("alignModeFaceCamera");
        case Particles::AlignMode::Velocity:
            return QStringLiteral("alignModeVelocity");
        default:
            break;
        }
        Q_UNREACHABLE();
    }();
    m_renderShaderProgramBuilder->setEnabledLayers({ layerName });
    m_alignMode = alignMode;
    emit alignModeChanged(alignMode);
}

Qt3DGeometry::QBuffer *ParticleMaterial::particleBuffer() const
{
    return m_particleBuffer->value().value<Qt3DGeometry::QBuffer *>();
}

Qt3DGeometry::QBuffer *ParticleMaterial::deadListBuffer() const
{
    return m_deadListBuffer->value().value<Qt3DGeometry::QBuffer *>();
}

Qt3DGeometry::QBuffer *ParticleMaterial::sortKeyBuffer() const
{
    return m_sortKeyBuffer->value().value<Qt3DGeometry::QBuffer *>();
}

void ParticleMaterial::updateBuffers()
{
    const auto particleCount = this->particleCount();

    auto particleData = [&particleCount] {
        // fill initial particleBuffer with inactive particles (age < 0)
        QByteArray data(particleCount * sizeof(Particle), 0);
        auto *p = reinterpret_cast<Particle *>(data.data());
        for (int i = 0; i < particleCount; ++i)
            p->age = -1.0f;
        return data;
    }();
    particleBuffer()->setData(particleData);

    auto deadListData = [&particleCount] {
        // fill initial deadListBuffer with indexes of all particles
        QByteArray data((particleCount + 2) * sizeof(int), 0);
        auto *p = reinterpret_cast<int *>(data.data());
        *p++ = particleCount; // count
        *p++ = 0; // head
        std::iota(p, p + particleCount, 0); // particle indices
        return data;
    }();
    deadListBuffer()->setData(deadListData);

    sortKeyBuffer()->setData(QByteArray(particleCount * 2 * sizeof(float), 0));
}

} // namespace Kuesa

QT_END_NAMESPACE
