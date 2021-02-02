/*
    controllerwidget.cpp

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

#include "controllerwidget.h"

#include "scenecontroller.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QMetaEnum>

namespace {
QDoubleSpinBox *makeVectorComponentSpinBox(double minimum, double maximum)
{
    auto *spinBox = new QDoubleSpinBox;
    spinBox->setSingleStep(0.1);
    spinBox->setMinimum(minimum);
    spinBox->setMaximum(maximum);
    spinBox->setDecimals(2);
    return spinBox;
}

class Vector2DWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Vector2DWidget(QWidget *parent = nullptr)
        : QWidget(parent)
        , m_x(makeVectorComponentSpinBox(-5.0, 5.0))
        , m_y(makeVectorComponentSpinBox(-5.0, 5.0))
    {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_x);
        layout->addWidget(m_y);

        const auto valueChanged = [this] {
            emit this->valueChanged(value());
        };
        connect(m_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, valueChanged);
        connect(m_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, valueChanged);
    }

    QVector2D value() const
    {
        return QVector2D(m_x->value(), m_y->value());
    }

    void setValue(const QVector2D &value)
    {
        m_x->setValue(value.x());
        m_y->setValue(value.y());
    }

signals:
    void valueChanged(const QVector2D &value);

private:
    QDoubleSpinBox *m_x;
    QDoubleSpinBox *m_y;
};

class Vector3DWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Vector3DWidget(QWidget *parent = nullptr)
        : QWidget(parent)
        , m_x(makeVectorComponentSpinBox(-5.0, 5.0))
        , m_y(makeVectorComponentSpinBox(-5.0, 5.0))
        , m_z(makeVectorComponentSpinBox(-5.0, 5.0))
    {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_x);
        layout->addWidget(m_y);
        layout->addWidget(m_z);

        const auto valueChanged = [this] {
            emit this->valueChanged(value());
        };
        connect(m_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, valueChanged);
        connect(m_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, valueChanged);
        connect(m_z, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, valueChanged);
    }

    QVector3D value() const
    {
        return QVector3D(m_x->value(), m_y->value(), m_z->value());
    }

    void setValue(const QVector3D &value)
    {
        m_x->setValue(value.x());
        m_y->setValue(value.y());
        m_z->setValue(value.z());
    }

signals:
    void valueChanged(const QVector3D &value);

private:
    QDoubleSpinBox *m_x;
    QDoubleSpinBox *m_y;
    QDoubleSpinBox *m_z;
};

class Vector4DWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Vector4DWidget(QWidget *parent = nullptr)
        : QWidget(parent)
        , m_x(makeVectorComponentSpinBox(-5.0, 5.0))
        , m_y(makeVectorComponentSpinBox(-5.0, 5.0))
        , m_z(makeVectorComponentSpinBox(-5.0, 5.0))
        , m_w(makeVectorComponentSpinBox(-5.0, 5.0))
    {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_x);
        layout->addWidget(m_y);
        layout->addWidget(m_z);
        layout->addWidget(m_w);

        const auto valueChanged = [this] {
            emit this->valueChanged(value());
        };
        connect(m_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, valueChanged);
        connect(m_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, valueChanged);
        connect(m_z, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, valueChanged);
        connect(m_w, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, valueChanged);
    }

    QVector4D value() const
    {
        return QVector4D(m_x->value(), m_y->value(), m_z->value(), m_w->value());
    }

    void setValue(const QVector4D &value)
    {
        m_x->setValue(value.x());
        m_y->setValue(value.y());
        m_z->setValue(value.z());
        m_w->setValue(value.w());
    }

signals:
    void valueChanged(const QVector4D &value);

private:
    QDoubleSpinBox *m_x;
    QDoubleSpinBox *m_y;
    QDoubleSpinBox *m_z;
    QDoubleSpinBox *m_w;
};
} // namespace

ControllerWidget::ControllerWidget(SceneController *controller, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
{
    setWindowTitle(tr("Scene Controls"));

    auto *particleCount = new QSpinBox(this);
    particleCount->setMinimum(1);
    particleCount->setMaximum(2000);
    particleCount->setValue(m_controller->particleCount());
    connect(particleCount, QOverload<int>::of(&QSpinBox::valueChanged),
            m_controller, &SceneController::setParticleCount);

    auto *maxParticlesEmittedPerFrame = new QSpinBox(this);
    maxParticlesEmittedPerFrame->setMinimum(0);
    maxParticlesEmittedPerFrame->setMaximum(2000);
    maxParticlesEmittedPerFrame->setValue(m_controller->maxParticlesEmittedPerFrame());
    connect(maxParticlesEmittedPerFrame, QOverload<int>::of(&QSpinBox::valueChanged),
            m_controller, &SceneController::setMaxParticlesEmittedPerFrame);

    auto *gravity = new Vector3DWidget(this);
    gravity->setValue(m_controller->gravity());
    connect(gravity, &Vector3DWidget::valueChanged, m_controller, &SceneController::setGravity);

    auto *emitterPosition = new Vector3DWidget(this);
    emitterPosition->setValue(m_controller->emitterPosition());
    connect(emitterPosition, &Vector3DWidget::valueChanged, m_controller, &SceneController::setEmitterPosition);

    auto *emitterPositionRandom = new Vector3DWidget(this);
    emitterPositionRandom->setValue(m_controller->emitterPositionRandom());
    connect(emitterPositionRandom, &Vector3DWidget::valueChanged, m_controller, &SceneController::setEmitterPositionRandom);

    auto *emitterVelocity = new Vector3DWidget(this);
    emitterVelocity->setValue(m_controller->emitterVelocity());
    connect(emitterVelocity, &Vector3DWidget::valueChanged, m_controller, &SceneController::setEmitterVelocity);

    auto *emitterVelocityRandom = new Vector3DWidget(this);
    emitterVelocityRandom->setValue(m_controller->emitterVelocityRandom());
    connect(emitterVelocityRandom, &Vector3DWidget::valueChanged, m_controller, &SceneController::setEmitterVelocityRandom);

    auto *particleLifespan = new QDoubleSpinBox(this);
    particleLifespan->setValue(m_controller->particleLifespan());
    connect(particleLifespan, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_controller, &SceneController::setParticleLifespan);

    auto *particleLifespanRandom = new QDoubleSpinBox(this);
    particleLifespanRandom->setValue(m_controller->particleLifespanRandom());
    connect(particleLifespanRandom, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_controller, &SceneController::setParticleLifespanRandom);

    auto *initialSize = new Vector2DWidget(this);
    initialSize->setValue(m_controller->initialSize());
    connect(initialSize, &Vector2DWidget::valueChanged, m_controller, &SceneController::setInitialSize);

    auto *finalSize = new Vector2DWidget(this);
    finalSize->setValue(m_controller->finalSize());
    connect(finalSize, &Vector2DWidget::valueChanged, m_controller, &SceneController::setFinalSize);

    auto *initialColor = new Vector4DWidget(this);
    initialColor->setValue(m_controller->initialColor());
    connect(initialColor, &Vector4DWidget::valueChanged, m_controller, &SceneController::setInitialColor);

    auto *finalColor = new Vector4DWidget(this);
    finalColor->setValue(m_controller->finalColor());
    connect(finalColor, &Vector4DWidget::valueChanged, m_controller, &SceneController::setFinalColor);

    auto *initialAngle = new QDoubleSpinBox(this);
    initialAngle->setValue(m_controller->initialAngle());
    connect(initialAngle, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_controller, &SceneController::setInitialAngle);

    auto *initialAngleRandom = new QDoubleSpinBox(this);
    initialAngleRandom->setValue(m_controller->initialAngleRandom());
    connect(initialAngleRandom, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_controller, &SceneController::setInitialAngleRandom);

    auto *rotationRate = new QDoubleSpinBox(this);
    rotationRate->setValue(m_controller->rotationRate());
    connect(rotationRate, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_controller, &SceneController::setRotationRate);

    auto *rotationRateRandom = new QDoubleSpinBox(this);
    rotationRateRandom->setValue(m_controller->rotationRateRandom());
    connect(rotationRateRandom, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_controller, &SceneController::setRotationRateRandom);

    auto *alignMode = new QComboBox(this);
    const auto alignModes = QMetaEnum::fromType<Kuesa::Particles::AlignMode>();
    for (int i = 0; i < alignModes.keyCount(); ++i) {
        alignMode->addItem(alignModes.key(i), QVariant::fromValue(static_cast<Kuesa::Particles::AlignMode>(alignModes.value(i))));
    }
    alignMode->setCurrentIndex(alignMode->findData(QVariant::fromValue(m_controller->alignMode())));
    connect(alignMode, QOverload<int>::of(&QComboBox::currentIndexChanged), m_controller, [this, alignMode](int index) {
        const auto mode = alignMode->itemData(index).value<Kuesa::Particles::AlignMode>();
        m_controller->setAlignMode(mode);
    });

    auto *layout = new QFormLayout(this);
    layout->addRow(tr("Particle count:"), particleCount);
    layout->addRow(tr("Max particles emitted per frame:"), maxParticlesEmittedPerFrame);
    layout->addRow(tr("Gravity:"), gravity);
    layout->addRow(tr("Emitter position:"), emitterPosition);
    layout->addRow(tr("Emitter position random:"), emitterPositionRandom);
    layout->addRow(tr("Emitter velocity:"), emitterVelocity);
    layout->addRow(tr("Emitter velocity random:"), emitterVelocityRandom);
    layout->addRow(tr("Particle lifespan:"), particleLifespan);
    layout->addRow(tr("Particle lifespan random:"), particleLifespanRandom);
    layout->addRow(tr("Initial size:"), initialSize);
    layout->addRow(tr("Final size:"), finalSize);
    layout->addRow(tr("Initial color:"), initialColor);
    layout->addRow(tr("Final color:"), finalColor);
    layout->addRow(tr("Initial angle:"), initialAngle);
    layout->addRow(tr("Initial angle random:"), initialAngleRandom);
    layout->addRow(tr("Rotation rate:"), rotationRate);
    layout->addRow(tr("Rotation rate random:"), rotationRateRandom);
    layout->addRow(tr("Align mode:"), alignMode);
}

#include "controllerwidget.moc"
