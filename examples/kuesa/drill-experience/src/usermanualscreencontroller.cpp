/*
    usermanualscreencontroller.cpp

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

#include "usermanualscreencontroller.h"
#include <Kuesa/View>
#include <Kuesa/TransformTracker>
#include <KuesaUtils/viewconfiguration.h>

/*
    Controller for the UserManual screen.

    We store a dedicated scene configuration for each seletable part of the drill.

    When selecting a new part, we will switch the active scene configuration
    instance to match the one associated with the part.

    This makes it convenient to store the assets and watchers we need when
    selecting different parts.
 */
UserManualScreenController::UserManualScreenController(QObject *parent)
    : AbstractScreenController(parent)
{
    QObject::connect(this, &UserManualScreenController::selectedPartChanged,
                     this, &UserManualScreenController::updateSceneConfiguration);

    KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
    configuration->setSource(QUrl(QStringLiteral("qrc:/drill/drill.gltf")));
    setSceneConfiguration(configuration);

    // Views
    m_mainViewConfiguration = new KuesaUtils::ViewConfiguration;
    m_mainViewConfiguration->setClearColor(QColor(Qt::transparent));
    m_mainViewConfiguration->setCameraName(QStringLiteral("CamOrbitCenter.CamOrbit"));
    m_mainViewConfiguration->setLayerNames({ QStringLiteral("LayerDevice") });
    configuration->addViewConfiguration(m_mainViewConfiguration);

    m_detailViewConfiguration = new KuesaUtils::ViewConfiguration;
    m_detailViewConfiguration->setViewportRect({ 0.7f, 0.0f, 0.3f, 0.3f });
    m_detailViewConfiguration->setClearColor(QColor(Qt::gray));
    m_detailViewConfiguration->setLayerNames({ QStringLiteral("LayerDevice") });

    // Trackers
    {
        m_triggerTracker = new Kuesa::TransformTracker();
        m_triggerTracker->setName(QStringLiteral("Drill.LABEL_Trigger"));
        m_mainViewConfiguration->addTransformTracker(m_triggerTracker);
        QObject::connect(m_triggerTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &UserManualScreenController::triggerPositionChanged);
    }

    {
        m_clutchTracker = new Kuesa::TransformTracker();
        m_clutchTracker->setName(QStringLiteral("Drill.LABEL_Clutch"));
        m_mainViewConfiguration->addTransformTracker(m_clutchTracker);
        QObject::connect(m_clutchTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &UserManualScreenController::clutchPositionChanged);
    }

    {
        m_chuckTracker = new Kuesa::TransformTracker();
        m_chuckTracker->setName(QStringLiteral("Drill.LABEL_Chuck"));
        m_mainViewConfiguration->addTransformTracker(m_chuckTracker);
        QObject::connect(m_chuckTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &UserManualScreenController::chuckPositionChanged);
    }

    {
        m_directionSwitchTracker = new Kuesa::TransformTracker();
        m_directionSwitchTracker->setName(QStringLiteral("Drill.LABEL_DirSwitch"));
        m_mainViewConfiguration->addTransformTracker(m_directionSwitchTracker);
        QObject::connect(m_directionSwitchTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &UserManualScreenController::directionSwitchPositionChanged);
    }

    {
        m_batteryPackTracker = new Kuesa::TransformTracker();
        m_batteryPackTracker->setName(QStringLiteral("Drill.LABEL_Battery"));
        m_mainViewConfiguration->addTransformTracker(m_batteryPackTracker);
        QObject::connect(m_batteryPackTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &UserManualScreenController::batteryPackPositionChanged);
    }

    // Animations
    {
        m_cameraAnimationPlayer = new Kuesa::AnimationPlayer;
        m_cameraAnimationPlayer->setClip(QStringLiteral("AnimCamOrbit"));
        m_cameraAnimationPlayer->setLoopCount(Kuesa::AnimationPlayer::Infinite);
        m_cameraAnimationPlayer->setRunning(true);

        configuration->addAnimationPlayer(m_cameraAnimationPlayer);

        m_batteryInOutAnimationPlayer = new Kuesa::AnimationPlayer;
        m_batteryInOutAnimationPlayer->setClip(QStringLiteral("AnimBatteryOut"));
        configuration->addAnimationPlayer(m_batteryInOutAnimationPlayer);

        m_directionSwitchAnimationPlayer = new Kuesa::AnimationPlayer;
        m_directionSwitchAnimationPlayer->setClip(QStringLiteral("AnimChangeDirectionLR"));
        configuration->addAnimationPlayer(m_directionSwitchAnimationPlayer);

        m_triggerAnimationPlayer = new Kuesa::AnimationPlayer;
        m_triggerAnimationPlayer->setClip(QStringLiteral("AnimTriggerPress"));
        configuration->addAnimationPlayer(m_triggerAnimationPlayer);

        m_drillAnimationPlayer = new Kuesa::AnimationPlayer;
        m_drillAnimationPlayer->setClip(QStringLiteral("AnimDrillCW"));
        configuration->addAnimationPlayer(m_drillAnimationPlayer);

        m_toolInOutAnimationPlayer = new Kuesa::AnimationPlayer;
        m_toolInOutAnimationPlayer->setClip(QStringLiteral("AnimToolIn"));
        configuration->addAnimationPlayer(m_toolInOutAnimationPlayer);
    }

    QObject::connect(configuration, &KuesaUtils::SceneConfiguration::unloadingDone, this, [this] {
        setSelectedPart(NoPartSelected);
    });

    // Set default scene configuration
    updateSceneConfiguration();
}

UserManualScreenController::~UserManualScreenController()
{
    // We own the view configurations if they are not parented
    if (m_mainViewConfiguration && !m_mainViewConfiguration->parent())
        delete m_mainViewConfiguration;
    if (m_detailViewConfiguration && !m_detailViewConfiguration->parent())
        delete m_detailViewConfiguration;
}

void UserManualScreenController::playAnimationBackAndForth(Kuesa::AnimationPlayer *player, int delay)
{
    Qt3DAnimation::QClock *c = player->clock();

    // Make sure player has a clock
    const bool hasClock = c != nullptr;
    if (!hasClock) {
        c = new Qt3DAnimation::QClock;
        player->setClock(c);
    }

    // Connect to running changed
    auto connection = std::make_shared<QMetaObject::Connection>();
    *connection = QObject::connect(player, &Kuesa::AnimationPlayer::runningChanged, player, [this, player, connection] {
        // We only want to do something if we are not running (meaning we were done playing)
        if (player->isRunning())
            return;

        Qt3DAnimation::QClock *c = player->clock();
        const bool wasReversed = c->playbackRate() < 0.0;

        // Reverse playback speed and set normalized time based on playback direction
        c->setPlaybackRate(c->playbackRate() * -1.0f);
        player->setNormalizedTime(c->playbackRate() > 0.0f ? 0.0f : 1.0f);

        // If we were done playing in reverse, then return early
        if (wasReversed) {
            QObject::disconnect(*connection);
            setSelectedPart(NoPartSelected);
            return;
        }

        // Otherwise, play animation backward
        player->start();
    });

    player->restart(delay);
}

void UserManualScreenController::setSelectedPart(UserManualScreenController::SelectablePart selectedPart)
{
    if (selectedPart == m_selectedPart)
        return;
    m_selectedPart = selectedPart;
    emit selectedPartChanged();
}

UserManualScreenController::SelectablePart UserManualScreenController::selectedPart() const
{
    return m_selectedPart;
}

QPointF UserManualScreenController::triggerPosition() const
{
    return m_triggerTracker->screenPosition();
}

QPointF UserManualScreenController::clutchPosition() const
{
    return m_clutchTracker->screenPosition();
}

QPointF UserManualScreenController::chuckPosition() const
{
    return m_chuckTracker->screenPosition();
}

QPointF UserManualScreenController::directionSwitchPosition() const
{
    return m_directionSwitchTracker->screenPosition();
}

QPointF UserManualScreenController::batteryPackPosition() const
{
    return m_batteryPackTracker->screenPosition();
}

void UserManualScreenController::updateSceneConfiguration()
{
    hideDetailView();

    switch (m_selectedPart) {
    case Trigger: {
        showDetailView(QStringLiteral("CamTrigger"));
        playAnimationBackAndForth(m_triggerAnimationPlayer, 750);
        break;
    }
    case Clutch: {
        showDetailView(QStringLiteral("CamChuck"));
        break;
    }
    case Chuck: {
        showDetailView(QStringLiteral("CamChuck"));
        playAnimationBackAndForth(m_toolInOutAnimationPlayer);
        break;
    }
    case DirectionSwitch: {
        showDetailView(QStringLiteral("CamDirectionSwitch"));
        playAnimationBackAndForth(m_directionSwitchAnimationPlayer, 750);
        break;
    }
    case BatteryPack: {
        showDetailView(QStringLiteral("CamBattery"));
        playAnimationBackAndForth(m_batteryInOutAnimationPlayer, 750);
        break;
    }
    case NoPartSelected: {
        break;
    }
    default:
        Q_UNREACHABLE();
    };
}

void UserManualScreenController::showDetailView(const QString &cameraName)
{
    KuesaUtils::SceneConfiguration *conf = sceneConfiguration();
    m_detailViewConfiguration->setCameraName(cameraName);
    conf->addViewConfiguration(m_detailViewConfiguration);
}

void UserManualScreenController::hideDetailView()
{
    KuesaUtils::SceneConfiguration *conf = sceneConfiguration();
    conf->removeViewConfiguration(m_detailViewConfiguration);
}
