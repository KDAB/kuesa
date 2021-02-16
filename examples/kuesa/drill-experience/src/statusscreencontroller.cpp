/*
    statuscreencontroller.cpp

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

#include "statusscreencontroller.h"
#include <Qt3DAnimation/QAnimationClipData>
#include <Qt3DAnimation/QAnimationClip>
#include <Qt3DAnimation/QChannelMapping>
#include <Qt3DAnimation/QClipAnimator>
#include <Qt3DAnimation/QClock>

using namespace Qt3DAnimation;

DrillStatus::DrillStatus(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
{
}

float DrillStatus::rpm() const
{
    return m_rpm;
}

float DrillStatus::currentDraw() const
{
    return m_currentDraw;
}

float DrillStatus::torque() const
{
    return m_torque;
}

float DrillStatus::batteryLife() const
{
    return m_batteryLife;
}

DrillStatus::Direction DrillStatus::direction() const
{
    return m_direction;
}

DrillStatus::Mode DrillStatus::mode() const
{
    return m_mode;
}

void DrillStatus::setRPM(float rpm)
{
    if (rpm == m_rpm)
        return;
    m_rpm = rpm;
    emit rpmChanged();
}

void DrillStatus::setCurrentDraw(float currentDraw)
{
    if (currentDraw == m_currentDraw)
        return;
    m_currentDraw = currentDraw;
    emit currentDrawChanged();
}

void DrillStatus::setTorque(float torque)
{
    if (torque == m_torque)
        return;
    m_torque = torque;
    emit torqueChanged();
}

void DrillStatus::setBatteryLife(float batteryLife)
{
    if (batteryLife == m_batteryLife)
        return;
    m_batteryLife = batteryLife;
    emit batteryLifeChanged();
}

void DrillStatus::setDirection(DrillStatus::Direction direction)
{
    if (direction == m_direction)
        return;
    m_direction = direction;
    emit directionChanged();
}

void DrillStatus::setMode(DrillStatus::Mode mode)
{
    if (mode == m_mode)
        return;
    m_mode = mode;
    emit modeChanged();
}

void StatusScreenController::createDrillStatusSimulationAnimation(Qt3DCore::QNode *parentNode)
{
    // Drilling into wood
    QAnimationClipData drillIntoWoodClipData;
    {
        // For drilling into wood we simulate
        // -> rather constant RPM, Torque and Current Draw
        // -> linear battery life diminution

        QChannel rpmChannel;
        rpmChannel.setName(QStringLiteral("rpm"));

        QChannel torqueChannel;
        torqueChannel.setName(QStringLiteral("torque"));

        QChannel currentDrawChannel;
        currentDrawChannel.setName(QStringLiteral("torque"));

        QChannel batteryLifeChannel;
        batteryLifeChannel.setName(QStringLiteral("batteryLife"));

        QChannelComponent rpmCmp;
        // Keyframe bezier curve coord, left handle, right handle
        // T0
        rpmCmp.appendKeyFrame(QKeyFrame({0.0f, 0.0f}, {-1.0f, 0.0f}, {1.0f, 0.0f}));
        // T1 -> Starting, not yet drilling into material (high no load RPM)
        rpmCmp.appendKeyFrame(QKeyFrame({2.0f, 5000.0f}, {1.0f, 2000.0f}, {3.0f, 5000.0f}));
        // T2 -> Starting to drill into material (RPM reduces)
        rpmCmp.appendKeyFrame(QKeyFrame({3.0f, 3000.0f}, {2.0f, 5000.0f}, {4.0f, 3000.0f}));
        // T3 -> Slowly reducing RPM as we drill
        rpmCmp.appendKeyFrame(QKeyFrame({10.0f, 2000.0f}, {5.0f, 2000.0f}, {8.0f, 3000.0f}));

        QChannelComponent torqueCmp; // in NM
        // T0
        torqueCmp.appendKeyFrame(QKeyFrame({0.0f, 0.0f}, {-1.0f, 0.0f}, {1.0f, 0.0f}));
        // T1 -> Starting, not yet drilling into material (almost no torque)
        torqueCmp.appendKeyFrame(QKeyFrame({2.0f, 2.0f}, {1.0f, 3.0f}, {3.0f, 3.0f}));
        // T2 -> Starting to drill into material (torque increases)
        torqueCmp.appendKeyFrame(QKeyFrame({3.0f, 15.0f}, {2.0f, 15.0f}, {4.0f, 12.0f}));
        // T3 -> Slowly increasing torque as we drill
        torqueCmp.appendKeyFrame(QKeyFrame({10.0f, 22.0f}, {5.0f, 22.0f}, {8.0f, 15.0f}));

        QChannelComponent currentDrawCmp; // in A/h
        // T0
        currentDrawCmp.appendKeyFrame(QKeyFrame({0.0f, 0.0f}, {-1.0f, 0.0f}, {1.0f, 0.0f}));
        // T1 -> Starting, not yet drilling into material (low current draw)
        currentDrawCmp.appendKeyFrame(QKeyFrame({2.0f, 2.0f}, {1.0f, 3.0f}, {3.0f, 3.0f}));
        // T2 -> Starting to drill into material (current draw increase)
        currentDrawCmp.appendKeyFrame(QKeyFrame({3.0f, 20.0f}, {2.0f, 15.0f}, {4.0f, 12.0f}));
        // T3 -> Slowly incrasing current draw as we drill (RPM reduces)
        currentDrawCmp.appendKeyFrame(QKeyFrame({10.0f, 25.0f}, {5.0f, 22.0f}, {8.0f, 15.0f}));

        QChannelComponent batteryLifeCmp; // in %
        // Use Linear KeyFrames
        // T0
        batteryLifeCmp.appendKeyFrame(QKeyFrame({0.0f, 100.0f}));
        // T1
        batteryLifeCmp.appendKeyFrame(QKeyFrame({2.0f, 99.5f}));
        // T2
        batteryLifeCmp.appendKeyFrame(QKeyFrame({20.0f, 99.0f}));
        // T3
        batteryLifeCmp.appendKeyFrame(QKeyFrame({25.0f, 95.0f}));

        rpmChannel.appendChannelComponent(rpmCmp);
        torqueChannel.appendChannelComponent(torqueCmp);
        currentDrawChannel.appendChannelComponent(currentDrawCmp);
        batteryLifeChannel.appendChannelComponent(batteryLifeCmp);

        drillIntoWoodClipData.appendChannel(rpmChannel);
        drillIntoWoodClipData.appendChannel(torqueChannel);
        drillIntoWoodClipData.appendChannel(currentDrawChannel);
        drillIntoWoodClipData.appendChannel(batteryLifeChannel);
    }

    auto makeMapping = [this] (const QString &property) {
        QChannelMapping *mapping = new QChannelMapping;
        mapping->setTarget(m_status);
        mapping->setChannelName(property);
        mapping->setProperty(property);
        return mapping;
    };

    // Specify how to map animation clip to a target object
    QChannelMapper *animationMapper = new QChannelMapper;
    animationMapper->addMapping(makeMapping(QStringLiteral("rpm")));
    animationMapper->addMapping(makeMapping(QStringLiteral("torque")));
    animationMapper->addMapping(makeMapping(QStringLiteral("batteryLife")));
    animationMapper->addMapping(makeMapping(QStringLiteral("currentDraw")));

    QAnimationClip *animationClip = new QAnimationClip;
    animationClip->setClipData(drillIntoWoodClipData);

    // Set animator and make it run
    QClipAnimator *animator = new QClipAnimator;
    animator->setChannelMapper(animationMapper);
    animator->setClip(animationClip);
    animator->setLoopCount(QClipAnimator::Infinite);
    animator->setRunning(true);

    // Add an entity that references the animator component and parent it as a
    // child of the sceneConfiguration for it to be picked up by Qt 3D
    Qt3DCore::QEntity *e = new Qt3DCore::QEntity(parentNode);
    e->addComponent(animator);
}

StatusScreenController::StatusScreenController(QObject *parent)
    : AbstractScreenController(parent)
{
    KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
    configuration->setSource(QUrl(QStringLiteral("qrc:/drill/drill.gltf")));
    configuration->setCameraName(QStringLiteral("|CamCenter|OrbitCam"));

    m_status = new DrillStatus(configuration);

    // We simulate various RPM/Torque/Current Draw values ... over time
    // For this, we make use of the Qt 3D Animation API which makes these
    // type of things easy to materialize
    createDrillStatusSimulationAnimation(configuration);

    // Add Animations Players on SceneConfiguration
    m_cameraAnimationPlayer = new Kuesa::AnimationPlayer;
    m_cameraAnimationPlayer->setClip(QStringLiteral("AnimCamera"));
    m_cameraAnimationPlayer->setLoopCount(Kuesa::AnimationPlayer::Infinite);
    m_cameraAnimationPlayer->setRunning(true);

    // Use a custom clock to reduce animation playback speed
    QClock *cameraAnimationClock = new QClock;
    cameraAnimationClock->setPlaybackRate(0.1);
    m_cameraAnimationPlayer->setClock(cameraAnimationClock);

    m_runningDrillPlayer = new Kuesa::AnimationPlayer;
    m_runningDrillPlayer->setClip(QStringLiteral("AnimDrill"));
    m_runningDrillPlayer->setLoopCount(Kuesa::AnimationPlayer::Infinite);

    QObject::connect(m_status, &DrillStatus::rpmChanged, this, [this] () {
        const bool shouldRun = m_status->rpm() > 0.0f;
        if (m_runningDrillPlayer->isRunning() != shouldRun)
            m_runningDrillPlayer->setRunning(shouldRun);
    });

    configuration->addAnimationPlayer(m_cameraAnimationPlayer);
    configuration->addAnimationPlayer(m_runningDrillPlayer);

    // Set SceneConfiguration onController
    setSceneConfiguration(configuration);
}

DrillStatus *StatusScreenController::drillStatus() const
{
    return m_status;
}
