/*
    particles.h

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

#ifndef KUESA_PARTICLES_H
#define KUESA_PARTICLES_H

#include <Kuesa/kuesa_global.h>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QAbstractTexture>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QComputeCommand;
} // namespace Qt3DRender

namespace Kuesa {

class ParticleMesh;
class ParticleMaterial;

class KUESASHARED_EXPORT Particles : public Qt3DCore::QEntity
{
public:
    Q_OBJECT
    Q_PROPERTY(int particleCount READ particleCount WRITE setParticleCount NOTIFY particleCountChanged)
    Q_PROPERTY(float frameTime READ frameTime WRITE setFrameTime NOTIFY frameTimeChanged)
    Q_PROPERTY(int maxParticlesEmittedPerFrame READ maxParticlesEmittedPerFrame WRITE setMaxParticlesEmittedPerFrame NOTIFY maxParticlesEmittedPerFrameChanged)
    Q_PROPERTY(QVector3D gravity READ gravity WRITE setGravity NOTIFY gravityChanged)
    Q_PROPERTY(QVector3D emitterPosition READ emitterPosition WRITE setEmitterPosition NOTIFY emitterPositionChanged)
    Q_PROPERTY(QVector3D emitterPositionRandom READ emitterPositionRandom WRITE setEmitterPositionRandom NOTIFY emitterPositionRandomChanged)
    Q_PROPERTY(QVector3D emitterVelocity READ emitterVelocity WRITE setEmitterVelocity NOTIFY emitterVelocityChanged)
    Q_PROPERTY(QVector3D emitterVelocityRandom READ emitterVelocityRandom WRITE setEmitterVelocityRandom NOTIFY emitterVelocityRandomChanged)
    Q_PROPERTY(float particleLifespan READ particleLifespan WRITE setParticleLifespan NOTIFY particleLifespanChanged)
    Q_PROPERTY(float particleLifespanRandom READ particleLifespanRandom WRITE setParticleLifespanRandom NOTIFY particleLifespanRandomChanged)
    Q_PROPERTY(QVector2D initialSize READ initialSize WRITE setInitialSize NOTIFY initialSizeChanged)
    Q_PROPERTY(QVector2D finalSize READ finalSize WRITE setFinalSize NOTIFY finalSizeChanged)
    Q_PROPERTY(QVector4D initialColor READ initialColor WRITE setInitialColor NOTIFY initialColorChanged)
    Q_PROPERTY(QVector4D finalColor READ finalColor WRITE setFinalColor NOTIFY finalColorChanged)
    Q_PROPERTY(float initialAngle READ initialAngle WRITE setInitialAngle NOTIFY initialAngleChanged)
    Q_PROPERTY(float initialAngleRandom READ initialAngleRandom WRITE setInitialAngleRandom NOTIFY initialAngleRandomChanged)
    Q_PROPERTY(float rotationRate READ rotationRate WRITE setRotationRate NOTIFY rotationRateChanged)
    Q_PROPERTY(float rotationRateRandom READ rotationRateRandom WRITE setRotationRateRandom NOTIFY rotationRateRandomChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *spriteTexture READ spriteTexture WRITE setSpriteTexture NOTIFY spriteTextureChanged)
    Q_PROPERTY(AlignMode alignMode READ alignMode WRITE setAlignMode NOTIFY alignModeChanged)

public:
    enum class AlignMode {
        FaceCamera,
        Velocity
    };
    Q_ENUM(AlignMode);

    explicit Particles(Qt3DCore::QEntity *parent = nullptr);

    int particleCount() const;
    float frameTime() const;
    int maxParticlesEmittedPerFrame() const;
    QVector3D gravity() const;
    QVector3D emitterPosition() const;
    QVector3D emitterPositionRandom() const;
    QVector3D emitterVelocity() const;
    QVector3D emitterVelocityRandom() const;
    float particleLifespan() const;
    float particleLifespanRandom() const;
    QVector2D initialSize() const;
    QVector2D finalSize() const;
    QVector4D initialColor() const;
    QVector4D finalColor() const;
    float initialAngle() const;
    float initialAngleRandom() const;
    float rotationRate() const;
    float rotationRateRandom() const;
    Qt3DRender::QAbstractTexture *spriteTexture() const;
    AlignMode alignMode() const;

public Q_SLOTS:
    void setParticleCount(int particleCount);
    void setFrameTime(float frameTime);
    void setMaxParticlesEmittedPerFrame(int maxParticlesEmittedPerFrame);
    void setGravity(const QVector3D &gravity);
    void setEmitterPosition(const QVector3D &emitterPosition);
    void setEmitterPositionRandom(const QVector3D &emitterPositionRandom);
    void setEmitterVelocity(const QVector3D &emitterVelocity);
    void setEmitterVelocityRandom(const QVector3D &emitterVelocityRandom);
    void setParticleLifespan(float lifespan);
    void setParticleLifespanRandom(float lifespanRandom);
    void setInitialSize(const QVector2D &initialSize);
    void setFinalSize(const QVector2D &finalSize);
    void setInitialColor(const QVector4D &initialColor);
    void setFinalColor(const QVector4D &finalColor);
    void setInitialAngle(float initialAngle);
    void setInitialAngleRandom(float initialAngleRandom);
    void setRotationRate(float rotationRate);
    void setRotationRateRandom(float rotationRateRandom);
    void setSpriteTexture(Qt3DRender::QAbstractTexture *spriteTexture);
    void setAlignMode(AlignMode alignMode);

Q_SIGNALS:
    void particleCountChanged(int particleCount);
    void frameTimeChanged(float frameTime);
    void maxParticlesEmittedPerFrameChanged(int maxParticlesEmittedPerFrame);
    void gravityChanged(const QVector3D &gravity);
    void emitterPositionChanged(const QVector3D &emitterPosition);
    void emitterPositionRandomChanged(const QVector3D &emitterPositionRandom);
    void emitterVelocityChanged(const QVector3D &emitterVelocity);
    void emitterVelocityRandomChanged(const QVector3D &emitterVelocityRandom);
    void particleLifespanChanged(float particleLifespan);
    void particleLifespanRandomChanged(float particleLifespanRandom);
    void initialSizeChanged(const QVector2D &initialSize);
    void finalSizeChanged(const QVector2D &finalSize);
    void initialColorChanged(const QVector4D &initialColor);
    void finalColorChanged(const QVector4D &finalColor);
    void initialAngleChanged(float initialAngle);
    void initialAngleRandomChanged(float initialAngleRandom);
    void rotationRateChanged(float rotationRate);
    void rotationRateRandomChanged(float rotationRateRandom);
    void spriteTextureChanged(const Qt3DRender::QAbstractTexture *spriteTexture);
    void alignModeChanged(AlignMode alignMode);

private:
    void updateWorkGroupCount();

    Qt3DRender::QComputeCommand *m_computeCommand;
    ParticleMesh *m_mesh;
    ParticleMaterial *m_material;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_PARTICLES_H
