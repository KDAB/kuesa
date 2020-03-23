/*
    particles.cpp

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

#include "particles.h"

#include "particlemesh_p.h"
#include "particlematerial_p.h"

#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QComputeCommand>

QT_BEGIN_NAMESPACE

namespace Kuesa {

namespace {
constexpr const auto WorkGroupSize = 128;
}

/*!
 \class Kuesa::Particles
 \inheaderfile Kuesa/Particles
 \inmodule Kuesa
 \since Kuesa 1.3

 \brief Particle system that controls emission and the various properties or
 particles over their lifetime.

 \badcode
 #include <Qt3DExtras/Qt3DWindow>
 #include <ForwardRenderer>
 #include <SceneEntity>
 #include <Particles>

 Qt3DExtras::Qt3DWindow win;
 Kuesa::SceneEntity *root = new Kuesa::SceneEntity();
 Kuesa::ForwardRenderer *frameGraph = new Kuesa::ForwardRenderer();
 frameGraph->setParticlesEnabled(true)

 Kuesa::Particles *particles = new Kuesa::Particles(root);
 particles->setParticleCount(128);
 particles->setMaxParticlesEmittedPerFrame(4);
 particles->setGravity(QVector3D(0.0f, -1.0f, 0.0f));
 ...

 win->setRootEntity(root);
 win->setActiveFrameGraph(forwardRenderer);
 ...
 \endcode
*/

/*!
 \qmltype Particles
 \inmodule Kuesa
 \since Kuesa 1.3

 \brief Particle system that controls emission and the various properties or
 particles over their lifetime.

 \badcode
 import Kuesa 1.3 as Kuesa

 Kuesa.SceneEnity {
     id: root
     components: [
         RenderSettings {
             Kuesa.ForwardRenderer {
                    particlesEnabled: true
                    camera: ...
             }
         }
     ]

    Kuesa.Particles {
        particleCount: 128
        maxParticlesEmittedPerFrame: 4
        gravity: Qt.vector3d(0, -1, 0)
        emitterPosition: Qt.vector3d(0, 0, 0)
        emitterPositionRandom:  Qt.vector3d(0, 0, 0)
        emitterVelocity:  Qt.vector3d(0, 1, 0)
        emitterVelocityRandom: Qt.vector3d(0, 0, 0)
        particleLifespan: 10
        particleLifespanRandom: 3
        initialSize: Qt.vector2d(0.1, 0.1)
        finalSize: Qt.vector2d(1, 1)
        initialColor: Qt.vector4d(1.0, 0.0, 0.0, 1.0)
        finalColor: Qt.vector4d(0.0, 1.0, 0.0, 1.0)
        initialAngle: 0
        initialAngleRandom: 0
        rotationRate: 0
        rotationRateRandom: 0
        spriteTexture: TextureLoader {
            source: "kuesa-logo.png"
        }
    }
 }
 \endcode
 */


Particles::Particles(Qt3DCore::QEntity *parent)
    : Qt3DCore::QEntity(parent)
    , m_computeCommand(new Qt3DRender::QComputeCommand(this))
    , m_mesh(new ParticleMesh(this))
    , m_material(new ParticleMaterial(this))
{
    connect(m_material, &ParticleMaterial::particleCountChanged, this, &Particles::particleCountChanged);
    connect(m_material, &ParticleMaterial::particleCountChanged, m_mesh, &ParticleMesh::setParticleCount);
    connect(m_material, &ParticleMaterial::particleCountChanged, this, &Particles::updateWorkGroupCount);
    connect(m_material, &ParticleMaterial::frameTimeChanged, this, &Particles::frameTimeChanged);
    connect(m_material, &ParticleMaterial::maxParticlesEmittedPerFrameChanged, this, &Particles::maxParticlesEmittedPerFrameChanged);
    connect(m_material, &ParticleMaterial::gravityChanged, this, &Particles::gravityChanged);
    connect(m_material, &ParticleMaterial::emitterPositionChanged, this, &Particles::emitterPositionChanged);
    connect(m_material, &ParticleMaterial::emitterPositionRandomChanged, this, &Particles::emitterPositionRandomChanged);
    connect(m_material, &ParticleMaterial::emitterVelocityChanged, this, &Particles::emitterVelocityChanged);
    connect(m_material, &ParticleMaterial::emitterVelocityRandomChanged, this, &Particles::emitterVelocityRandomChanged);
    connect(m_material, &ParticleMaterial::particleLifespanChanged, this, &Particles::particleLifespanChanged);
    connect(m_material, &ParticleMaterial::particleLifespanRandomChanged, this, &Particles::particleLifespanRandomChanged);
    connect(m_material, &ParticleMaterial::initialSizeChanged, this, &Particles::initialSizeChanged);
    connect(m_material, &ParticleMaterial::finalSizeChanged, this, &Particles::finalSizeChanged);
    connect(m_material, &ParticleMaterial::initialColorChanged, this, &Particles::initialColorChanged);
    connect(m_material, &ParticleMaterial::finalColorChanged, this, &Particles::finalColorChanged);
    connect(m_material, &ParticleMaterial::initialAngleChanged, this, &Particles::initialAngleChanged);
    connect(m_material, &ParticleMaterial::initialAngleRandomChanged, this, &Particles::initialAngleRandomChanged);
    connect(m_material, &ParticleMaterial::rotationRateChanged, this, &Particles::rotationRateChanged);
    connect(m_material, &ParticleMaterial::rotationRateRandomChanged, this, &Particles::rotationRateRandomChanged);
    connect(m_material, &ParticleMaterial::spriteTextureChanged, this, &Particles::spriteTextureChanged);

    m_mesh->setVertexBuffer(m_material->sortKeyBuffer());
    m_mesh->setParticleCount(m_material->particleCount());

    auto *computeEntity = new Qt3DCore::QEntity(this);
    computeEntity->addComponent(m_computeCommand);
    computeEntity->addComponent(m_material);

    auto *renderEntity = new Qt3DCore::QEntity(this);
    renderEntity->addComponent(m_mesh);
    renderEntity->addComponent(m_material);

    updateWorkGroupCount();
}

/*!
    \property Kuesa::Particles::particleCount

    Holds the maximum number of particles the system can handle.
    Currently the maximum value is 2048.

    \default 32
    \since Kuesa 1.3
 */

/*!
    \qmlproperty int Kuesa::Particles::particleCount

    Holds the maximum number of particles the system can handle.
    Currently the maximum value is 2048.

    \default 32
    \since Kuesa 1.3
 */
int Particles::particleCount() const
{
    return m_material->particleCount();
}

/*!
    \property Kuesa::Particles::frameTime

    Holds the speed at which updates need to be made in milliseconds.

    \default 1/60
    \since Kuesa 1.3
 */

/*!
    \qmlproperty real Kuesa::Particles::frameTime

    Holds the speed at which updates need to be made in milliseconds.

    \default 1/60
    \since Kuesa 1.3
 */
float Particles::frameTime() const
{
    return m_material->frameTime();
}

/*!
    \property Kuesa::Particles::maxParticlesEmittedPerFrame

    Holds the maximum number of new particles that can be created each frame.

    \default 4
    \since Kuesa 1.3
 */

/*!
    \qmlproperty int Kuesa::Particles::maxParticlesEmittedPerFrame

    Holds the maximum number of new particles that can be created each frame.

    \default 4
    \since Kuesa 1.3
 */
int Particles::maxParticlesEmittedPerFrame() const
{
    return m_material->maxParticlesEmittedPerFrame();
}

/*!
    \property Kuesa::Particles::gravity

    Holds the gravity value to apply to the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty vector3d Kuesa::Particles::gravity

    Holds the gravity value to apply to the particles.
    \since Kuesa 1.3
 */
QVector3D Particles::gravity() const
{
    return m_material->gravity();
}

/*!
    \property Kuesa::Particles::emitterPosition

    Holds the world position of the particle emitter.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty vector3d Kuesa::Particles::emitterPosition

    Holds the world position of the particle emitter.
    \since Kuesa 1.3
 */
QVector3D Particles::emitterPosition() const
{
    return m_material->emitterPosition();
}

/*!
    \property Kuesa::Particles::emitterPositionRandom

    Holds the randomness factor applied to the world position.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty vector3d Kuesa::Particles::emitterPositionRandom

    Holds the randomness factor applied to the world position.
    \since Kuesa 1.3
 */
QVector3D Particles::emitterPositionRandom() const
{
    return m_material->emitterPositionRandom();
}

/*!
    \property Kuesa::Particles::emitterVelocity

    Holds the velocity applied to the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty vector3d Kuesa::Particles::emitterVelocity

    Holds the velocity applied to the particles.
    \since Kuesa 1.3
 */
QVector3D Particles::emitterVelocity() const
{
    return m_material->emitterVelocity();
}


/*!
    \property Kuesa::Particles::emitterVelocityRandom

    Holds the velocity randomness factor applied to the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty vector3d Kuesa::Particles::emitterVelocityRandom

    Holds the velocity randomness factor applied to the particles.
    \since Kuesa 1.3
 */
QVector3D Particles::emitterVelocityRandom() const
{
    return m_material->emitterVelocityRandom();
}

/*!
    \property Kuesa::Particles::particleLifespan

    Holds the lifetime of the particles.
    \since Kuesa 1.3
    \default 1
 */

/*!
    \qmlproperty real Kuesa::Particles::particleLifespan

    Holds the lifetime of the particles.
    \since Kuesa 1.3
    \default 1
 */
float Particles::particleLifespan() const
{
    return m_material->particleLifespan();
}

/*!
    \property Kuesa::Particles::particleLifespanRandom

    Holds the lifetime randomness factor applied to the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty real Kuesa::Particles::particleLifespanRandom

    Holds the lifetime randomness factor applied to the particles.
    \since Kuesa 1.3
 */
float Particles::particleLifespanRandom() const
{
    return m_material->particleLifespanRandom();
}

/*!
    \property Kuesa::Particles::initialSize

    Holds the initial size of the particles.
    \since Kuesa 1.3
    \default 0
 */

/*!
    \qmlproperty vector2d Kuesa::Particles::initialSize

    Holds the initial size of the particles.
    \since Kuesa 1.3
    \default 0
 */
QVector2D Particles::initialSize() const
{
    return m_material->initialSize();
}

/*!
    \property Kuesa::Particles::finalSize

    Holds the final size of the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty vector2d Kuesa::Particles::finalSize

    Holds the final size of the particles.
    \since Kuesa 1.3
 */
QVector2D Particles::finalSize() const
{
    return m_material->finalSize();
}

/*!
    \property Kuesa::Particles::initialColor

    Holds the initial color of the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty vector3d Kuesa::Particles::initialColor

    Holds the initial color of the particles.
    \since Kuesa 1.3
 */
QVector4D Particles::initialColor() const
{
    return m_material->initialColor();
}

/*!
    \property Kuesa::Particles::finalColor

    Holds the final color of the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty vector3d Kuesa::Particles::finalColor

    Holds the final color of the particles.
    \since Kuesa 1.3
 */
QVector4D Particles::finalColor() const
{
    return m_material->finalColor();
}

/*!
    \property Kuesa::Particles::initialAngle

    Holds the initial rotation angle in radians of the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty real Kuesa::Particles::initialAngle

    Holds the initial rotation angle in radians of the particles.
    \since Kuesa 1.3
 */
float Particles::initialAngle() const
{
    return m_material->initialAngle();
}

/*!
    \property Kuesa::Particles::initialAngleRandom

    Holds the initial rotation randomness factor applied to the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty real Kuesa::Particles::initialAngleRandom

    Holds the initial rotation randomness factor applied to the particles.
    \since Kuesa 1.3
 */
float Particles::initialAngleRandom() const
{
    return m_material->initialAngleRandom();
}

/*!
    \property Kuesa::Particles::rotationRate

    Holds the rotation speed applied to the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty real Kuesa::Particles::rotationRate

    Holds the rotation speed applied to the particles.
    \since Kuesa 1.3
 */
float Particles::rotationRate() const
{
    return m_material->rotationRate();
}

/*!
    \property Kuesa::Particles::rotationRateRandom

    Holds the rotation speed randomness factor applied to the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty real Kuesa::Particles::rotationRateRandom

    Holds the rotation speed randomness factor applied to the particles.
    \since Kuesa 1.3
 */
float Particles::rotationRateRandom() const
{
    return m_material->rotationRateRandom();
}

/*!
    \property Kuesa::Particles::spriteTexture

    Holds the 2D sprite texture to apply to the particles.
    \since Kuesa 1.3
 */

/*!
    \qmlproperty Qt3DRender.AbstractTexture Kuesa::Particles::spriteTexture

    Holds the 2D sprite texture to apply to the particles.
    \since Kuesa 1.3
 */
Qt3DRender::QAbstractTexture *Particles::spriteTexture() const
{
    return m_material->spriteTexture();
}

void Particles::setParticleCount(int particleCount)
{
    m_material->setParticleCount(particleCount);
}

void Particles::setFrameTime(float frameTime)
{
    m_material->setFrameTime(frameTime);
}

void Particles::setMaxParticlesEmittedPerFrame(int maxParticlesEmittedPerFrame)
{
    m_material->setMaxParticlesEmittedPerFrame(maxParticlesEmittedPerFrame);
}

void Particles::setGravity(const QVector3D &gravity)
{
    m_material->setGravity(gravity);
}

void Particles::setEmitterPosition(const QVector3D &emitterPosition)
{
    m_material->setEmitterPosition(emitterPosition);
}

void Particles::setEmitterPositionRandom(const QVector3D &emitterPositionRandom)
{
    m_material->setEmitterPositionRandom(emitterPositionRandom);
}

void Particles::setEmitterVelocity(const QVector3D &emitterVelocity)
{
    m_material->setEmitterVelocity(emitterVelocity);
}

void Particles::setEmitterVelocityRandom(const QVector3D &emitterVelocityRandom)
{
    m_material->setEmitterVelocityRandom(emitterVelocityRandom);
}

void Particles::setParticleLifespan(float lifespan)
{
    m_material->setParticleLifespan(lifespan);
}

void Particles::setParticleLifespanRandom(float lifespanRandom)
{
    m_material->setParticleLifespanRandom(lifespanRandom);
}

void Particles::setInitialSize(const QVector2D &initialSize)
{
    m_material->setInitialSize(initialSize);
}

void Particles::setFinalSize(const QVector2D &finalSize)
{
    m_material->setFinalSize(finalSize);
}

void Particles::setInitialColor(const QVector4D &initialColor)
{
    m_material->setInitialColor(initialColor);
}

void Particles::setFinalColor(const QVector4D &finalColor)
{
    m_material->setFinalColor(finalColor);
}

void Particles::setInitialAngle(float initialAngle)
{
    m_material->setInitialAngle(initialAngle);
}

void Particles::setInitialAngleRandom(float initialAngleRandom)
{
    m_material->setInitialAngleRandom(initialAngleRandom);
}

void Particles::setRotationRate(float rotationRate)
{
    m_material->setRotationRate(rotationRate);
}

void Particles::setRotationRateRandom(float rotationRateRandom)
{
    m_material->setRotationRateRandom(rotationRateRandom);
}

void Particles::setSpriteTexture(Qt3DRender::QAbstractTexture *spriteTexture)
{
    m_material->setSpriteTexture(spriteTexture);
}

void Particles::updateWorkGroupCount()
{
    m_computeCommand->setWorkGroupX((particleCount() + WorkGroupSize - 1) / WorkGroupSize);
    m_computeCommand->setWorkGroupY(1);
    m_computeCommand->setWorkGroupZ(1);
}

} // namespace Kuesa

QT_END_NAMESPACE
