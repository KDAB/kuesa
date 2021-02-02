/*
    scenecontroller.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "scenecontroller.h"

SceneController::SceneController(QObject *parent)
    : QObject(parent)
    , m_particleCount(128)
    , m_maxParticlesEmittedPerFrame(4)
    , m_gravity(QVector3D(0, -0.1, 0))
    , m_emitterPosition(QVector3D(0, 0, 0))
    , m_emitterPositionRandom(QVector3D(0.1, 0.1, 0.1))
    , m_emitterVelocity(QVector3D(0.25, 0.5, 0.25))
    , m_emitterVelocityRandom(QVector3D(0.1, 0.1, 0.1))
    , m_particleLifespan(3.0)
    , m_particleLifespanRandom(1.0)
    , m_initialSize(QVector2D(0.5, 0.5))
    , m_finalSize(QVector2D(0.125, 0.125))
    , m_initialColor(QVector4D(1, 1, 1, 1))
    , m_finalColor(QVector4D(1, 1, 1, 0))
    , m_initialAngle(0)
    , m_initialAngleRandom(3.15159265)
    , m_rotationRate(3.14159265)
    , m_rotationRateRandom(1.0)
    , m_alignMode(Kuesa::Particles::AlignMode::FaceCamera)
{
}

int SceneController::particleCount() const
{
    return m_particleCount;
}

int SceneController::maxParticlesEmittedPerFrame() const
{
    return m_maxParticlesEmittedPerFrame;
}

QVector3D SceneController::gravity() const
{
    return m_gravity;
}

QVector3D SceneController::emitterPosition() const
{
    return m_emitterPosition;
}

QVector3D SceneController::emitterPositionRandom() const
{
    return m_emitterPositionRandom;
}

QVector3D SceneController::emitterVelocity() const
{
    return m_emitterVelocity;
}

QVector3D SceneController::emitterVelocityRandom() const
{
    return m_emitterVelocityRandom;
}

float SceneController::particleLifespan() const
{
    return m_particleLifespan;
}

float SceneController::particleLifespanRandom() const
{
    return m_particleLifespanRandom;
}

QVector2D SceneController::initialSize() const
{
    return m_initialSize;
}

QVector2D SceneController::finalSize() const
{
    return m_finalSize;
}

QVector4D SceneController::initialColor() const
{
    return m_initialColor;
}

QVector4D SceneController::finalColor() const
{
    return m_finalColor;
}

float SceneController::initialAngle() const
{
    return m_initialAngle;
}

float SceneController::initialAngleRandom() const
{
    return m_initialAngleRandom;
}

float SceneController::rotationRate() const
{
    return m_rotationRate;
}

float SceneController::rotationRateRandom() const
{
    return m_rotationRateRandom;
}

Kuesa::Particles::AlignMode SceneController::alignMode() const
{
    return m_alignMode;
}

void SceneController::setParticleCount(int particleCount)
{
    if (m_particleCount == particleCount)
        return;
    m_particleCount = particleCount;
    emit particleCountChanged(particleCount);
}

void SceneController::setMaxParticlesEmittedPerFrame(int maxParticlesEmittedPerFrame)
{
    if (m_maxParticlesEmittedPerFrame == maxParticlesEmittedPerFrame)
        return;
    m_maxParticlesEmittedPerFrame = maxParticlesEmittedPerFrame;
    emit maxParticlesEmittedPerFrameChanged(maxParticlesEmittedPerFrame);
}

void SceneController::setGravity(const QVector3D &gravity)
{
    if (m_gravity == gravity)
        return;
    m_gravity = gravity;
    emit gravityChanged(gravity);
}

void SceneController::setEmitterPosition(const QVector3D &emitterPosition)
{
    if (m_emitterPosition == emitterPosition)
        return;
    m_emitterPosition = emitterPosition;
    emit emitterPositionChanged(emitterPosition);
}

void SceneController::setEmitterPositionRandom(const QVector3D &emitterPositionRandom)
{
    if (m_emitterPositionRandom == emitterPositionRandom)
        return;
    m_emitterPositionRandom = emitterPositionRandom;
    emit emitterPositionRandomChanged(emitterPositionRandom);
}

void SceneController::setEmitterVelocity(const QVector3D &emitterVelocity)
{
    if (m_emitterVelocity == emitterVelocity)
        return;
    m_emitterVelocity = emitterVelocity;
    emit emitterVelocityChanged(emitterVelocity);
}

void SceneController::setEmitterVelocityRandom(const QVector3D &emitterVelocityRandom)
{
    if (m_emitterVelocityRandom == emitterVelocityRandom)
        return;
    m_emitterVelocityRandom = emitterVelocityRandom;
    emit emitterVelocityRandomChanged(emitterVelocityRandom);
}

void SceneController::setParticleLifespan(float particleLifespan)
{
    if (qFuzzyCompare(m_particleLifespan, particleLifespan))
        return;
    m_particleLifespan = particleLifespan;
    emit particleLifespanChanged(particleLifespan);
}

void SceneController::setParticleLifespanRandom(float particleLifespanRandom)
{
    if (qFuzzyCompare(m_particleLifespanRandom, particleLifespanRandom))
        return;
    m_particleLifespanRandom = particleLifespanRandom;
    emit particleLifespanRandomChanged(particleLifespanRandom);
}

void SceneController::setInitialSize(const QVector2D &initialSize)
{
    if (m_initialSize == initialSize)
        return;
    m_initialSize = initialSize;
    emit initialSizeChanged(initialSize);
}

void SceneController::setFinalSize(const QVector2D &finalSize)
{
    if (m_finalSize == finalSize)
        return;
    m_finalSize = finalSize;
    emit finalSizeChanged(finalSize);
}

void SceneController::setInitialColor(const QVector4D &initialColor)
{
    if (m_initialColor == initialColor)
        return;
    m_initialColor = initialColor;
    emit initialColorChanged(initialColor);
}

void SceneController::setFinalColor(const QVector4D &finalColor)
{
    if (m_finalColor == finalColor)
        return;
    m_finalColor = finalColor;
    emit finalColorChanged(finalColor);
}

void SceneController::setInitialAngle(float initialAngle)
{
    if (qFuzzyCompare(m_initialAngle, initialAngle))
        return;
    m_initialAngle = initialAngle;
    emit initialAngleChanged(initialAngle);
}

void SceneController::setInitialAngleRandom(float initialAngleRandom)
{
    if (qFuzzyCompare(m_initialAngleRandom, initialAngleRandom))
        return;
    m_initialAngleRandom = initialAngleRandom;
    emit initialAngleRandomChanged(initialAngleRandom);
}

void SceneController::setRotationRate(float rotationRate)
{
    if (qFuzzyCompare(m_rotationRate, rotationRate))
        return;
    m_rotationRate = rotationRate;
    emit rotationRateChanged(rotationRate);
}

void SceneController::setRotationRateRandom(float rotationRateRandom)
{
    if (qFuzzyCompare(m_rotationRateRandom, rotationRateRandom))
        return;
    m_rotationRateRandom = rotationRateRandom;
    emit rotationRateRandomChanged(rotationRateRandom);
}

void SceneController::setAlignMode(Kuesa::Particles::AlignMode alignMode)
{
    if (m_alignMode == alignMode)
        return;
    m_alignMode = alignMode;
    emit alignModeChanged(alignMode);
}
