/*
    statusscreencontroller.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef STATUSSCREENCONTROLLER_H
#define STATUSSCREENCONTROLLER_H

#include "abstractscreencontroller.h"

namespace Kuesa {
    class AnimationPlayer;
}

class DrillStatus : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(float rpm READ rpm WRITE setRPM NOTIFY rpmChanged)
    Q_PROPERTY(float currentDraw READ currentDraw WRITE setCurrentDraw NOTIFY currentDrawChanged)
    Q_PROPERTY(float torque READ torque WRITE setTorque NOTIFY torqueChanged)
    Q_PROPERTY(float batteryLife READ batteryLife WRITE setBatteryLife NOTIFY batteryLifeChanged)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)

public:
    explicit DrillStatus(Qt3DCore::QNode *parent = nullptr);

    enum Direction {
        Clockwise,
        CounterClockwise
    };
    Q_ENUM(Direction)

    enum Mode {
        Drill,
        HammerDrill,
        ScrewDriving
    };
    Q_ENUM(Mode)

    void setRPM(float rpm);
    void setCurrentDraw(float currentDraw);
    void setTorque(float torque);
    void setBatteryLife(float batteryLife);
    void setDirection(Direction direction);
    void setMode(Mode mode);

    float rpm() const;
    float currentDraw() const;
    float torque() const;
    float batteryLife() const;
    Direction direction() const;
    Mode mode() const;

    signals:
    void rpmChanged();
    void currentDrawChanged();
    void torqueChanged();
    void batteryLifeChanged();
    void directionChanged();
    void modeChanged();

private:
    float m_rpm = 0.0f;
    float m_currentDraw = 0.0f;
    float m_torque = 0.0f;
    float m_batteryLife = 100.0f;
    Direction m_direction = Clockwise;
    Mode m_mode = Drill;
};

class StatusScreenController : public AbstractScreenController
{
    Q_OBJECT
    Q_PROPERTY(DrillStatus *drillStatus READ drillStatus CONSTANT)
public:
    explicit StatusScreenController(QObject *parent = nullptr);

    DrillStatus *drillStatus() const;


private:
    void createDrillStatusSimulationAnimation(Qt3DCore::QNode *parentNode);

    DrillStatus *m_status = nullptr;
    Kuesa::AnimationPlayer *m_cameraAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_runningDrillPlayer = nullptr;
};

#endif // STATUSSCREENCONTROLLER_H
