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
#include <QTimer>

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
    configuration->setCameraName(QStringLiteral("CamOrbiteCenter.CamOrbit"));
    setSceneConfiguration(configuration);

    {
        m_triggerTracker = new Kuesa::TransformTracker();
        m_triggerTracker->setName(QStringLiteral("Drill.LocTrigger"));
        configuration->addTransformTracker(m_triggerTracker);
        QObject::connect(m_triggerTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &UserManualScreenController::triggerPositionChanged);
    }

    {
        m_clutchTracker = new Kuesa::TransformTracker();
        m_clutchTracker->setName(QStringLiteral("Drill.Ring1"));
        configuration->addTransformTracker(m_clutchTracker);
        QObject::connect(m_clutchTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &UserManualScreenController::clutchPositionChanged);
    }

    {
        m_chuckTracker = new Kuesa::TransformTracker();
        m_chuckTracker->setName(QStringLiteral("Drill.LABEL_Chuck"));
        configuration->addTransformTracker(m_chuckTracker);
        QObject::connect(m_chuckTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &UserManualScreenController::chuckPositionChanged);
    }

    {
        m_directionSwitchTracker = new Kuesa::TransformTracker();
        m_directionSwitchTracker->setName(QStringLiteral("Drill.LocDirectionKnobs"));
        configuration->addTransformTracker(m_directionSwitchTracker);
        QObject::connect(m_directionSwitchTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &UserManualScreenController::directionSwitchPositionChanged);
    }

    {
        m_batteryPackTracker = new Kuesa::TransformTracker();
        m_batteryPackTracker->setName(QStringLiteral("Drill.LABEL_Battery"));
        configuration->addTransformTracker(m_batteryPackTracker);
        QObject::connect(m_batteryPackTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &UserManualScreenController::batteryPackPositionChanged);
    }

    // Animations
    {
        m_cameraAnimationPlayer = new Kuesa::AnimationPlayer;
        m_cameraAnimationPlayer->setClip(QStringLiteral("AnimCamOrbit"));
        m_cameraAnimationPlayer->setLoopCount(Kuesa::AnimationPlayer::Infinite);
        m_cameraAnimationPlayer->setRunning(true);
        {
            Qt3DAnimation::QClock *cameraAnimClock = new Qt3DAnimation::QClock;
            cameraAnimClock->setPlaybackRate(0.1f);
            m_cameraAnimationPlayer->setClock(cameraAnimClock);
        }

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

    // Views
    {
        m_mainView = new Kuesa::View;
        m_mainView->setClearColor(QColor(Qt::transparent));
        addView(m_mainView);

        m_detailView = new Kuesa::View;
        m_detailView->setViewportRect({0.7f, 0.0f, 0.3f, 0.3f});
        m_detailView->setClearColor(QColor(Qt::gray));
    }

    // Scene Set up once SceneEnity is available for collection lookups
    QObject::connect(configuration, &KuesaUtils::SceneConfiguration::loadingDone, this, [this] {
        KuesaUtils::SceneConfiguration *configuration = sceneConfiguration();
        Kuesa::SceneEntity *sceneEntity = configuration->sceneEntity();
        if (sceneEntity && m_mainView->layers().empty()) {
            Qt3DRender::QLayer *drillLayer = sceneEntity->layer(QStringLiteral("LayerDevice"));
            m_mainView->addLayer(drillLayer);
            m_detailView->addLayer(drillLayer);
        }
    });


    // Set default scene configuration
    updateSceneConfiguration();
}

UserManualScreenController::~UserManualScreenController()
{
    // We own the views if they are not parented to the FrameGraph
    if (m_mainView && !m_mainView->parent())
        delete m_mainView.data();
    if (m_detailView && !m_detailView->parent())
        delete m_detailView.data();
}

namespace {

void playAnimationBackAndForth(Kuesa::AnimationPlayer *player, int delay = 0)
{
    Qt3DAnimation::QClock *c = player->clock();
    const bool hasClock = c != nullptr;
    if (!hasClock) {
        c = new Qt3DAnimation::QClock;
        player->setClock(c);

        QObject::connect(player, &Kuesa::AnimationPlayer::runningChanged, player, [player] {
            qDebug() << Q_FUNC_INFO;
            if (player->isRunning())
                return;
            Qt3DAnimation::QClock *c = player->clock();
            // Player animation backward once completed
            c->setPlaybackRate(c->playbackRate() * -1.0f);
            if (c->playbackRate() < 0.0f)
                player->setNormalizedTime(1.0f);
            player->start();
        });
    }

    QTimer::singleShot(delay, player, &Kuesa::AnimationPlayer::restart);
}

} // anonymous

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
    case Clutch:{
        showDetailView(QStringLiteral("CamChuck"));
        break;
    }
    case Chuck:{
        showDetailView(QStringLiteral("CamChuck"));
        playAnimationBackAndForth(m_toolInOutAnimationPlayer);
        break;
    }
    case DirectionSwitch:{
        showDetailView(QStringLiteral("CamDirectionSwitch"));
        playAnimationBackAndForth(m_directionSwitchAnimationPlayer, 750);
        break;
    }
    case BatteryPack:{
        showDetailView(QStringLiteral("CamBattery"));
        playAnimationBackAndForth(m_batteryInOutAnimationPlayer, 750);
        break;
    }
    case NoPartSelected:{
        break;
    }
    default:
        Q_UNREACHABLE();
    };
}

void UserManualScreenController::showDetailView(const QString &cameraName)
{
    KuesaUtils::SceneConfiguration *conf = sceneConfiguration();
    Kuesa::SceneEntity *sceneEntity = conf->sceneEntity();

    if (sceneEntity) {
        Qt3DRender::QCamera *camera = sceneEntity->camera(cameraName);
        if (camera)
            m_detailView->setCamera(camera);
    }

    addView(m_detailView);
}

void UserManualScreenController::hideDetailView()
{
    removeView(m_detailView);
}
