/*
    screencontroller.cpp

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

#include "screencontroller.h"
#include <Kuesa/View>
#include <Kuesa/TransformTracker>
#include <Kuesa/SteppedAnimationPlayer>
#include <KuesaUtils/viewconfiguration.h>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <array>

namespace {
using Bit = ScreenController::Bit;
constexpr std::array<Bit, 20> bits = {
    Bit::Drill1,
    Bit::Drill2,
    Bit::Drill3,
    Bit::Drill4,
    Bit::Drill5,
    Bit::Drill6,

    Bit::ScrewHex,
    Bit::ScrewHexMedium,
    Bit::ScrewHexSmall,
    Bit::ScrewHexTiny,

    Bit::ScrewTorx,
    Bit::ScrewTorxMedium,
    Bit::ScrewTorxSmall,
    Bit::ScrewTorxTiny,

    Bit::ScrewPhilips,
    Bit::ScrewPhilipsMedium,
    Bit::ScrewPhilipsSmall,

    Bit::ScrewFlat,
    Bit::ScrewFlatMedium,
    Bit::ScrewFlatSmall,
};

// By Driving Mode
constexpr std::array<Bit, 14> screwDriverBits = {
    Bit::ScrewHex,
    Bit::ScrewHexMedium,
    Bit::ScrewHexSmall,
    Bit::ScrewHexTiny,

    Bit::ScrewTorx,
    Bit::ScrewTorxMedium,
    Bit::ScrewTorxSmall,
    Bit::ScrewTorxTiny,

    Bit::ScrewPhilips,
    Bit::ScrewPhilipsMedium,
    Bit::ScrewPhilipsSmall,

    Bit::ScrewFlat,
    Bit::ScrewFlatMedium,
    Bit::ScrewFlatSmall,
};
constexpr std::array<Bit, 6> drillingBits = {
    Bit::Drill1,
    Bit::Drill2,
    Bit::Drill3,
    Bit::Drill4,
    Bit::Drill5,
    Bit::Drill6,
};

// By Drilling Material
constexpr std::array<Bit, 2> metalDrillingBits = {
    Bit::Drill1,
    Bit::Drill2,
};
constexpr std::array<Bit, 2> concreteDrillingBits = {
    Bit::Drill5,
    Bit::Drill6,
};
constexpr std::array<Bit, 2> woodDrillingBits = {
    Bit::Drill3,
    Bit::Drill4,
};

// Must be kept in sync between the gltf file and the Bit enum
QString gltfBitName(Bit bit)
{
    switch (bit) {
    case Bit::None:
        Q_UNREACHABLE();

    case Bit::Drill1:
        return QStringLiteral("Tool_Drill1");
    case Bit::Drill2:
        return QStringLiteral("Tool_Drill2");
    case Bit::Drill3:
        return QStringLiteral("Tool_Drill3");
    case Bit::Drill4:
        return QStringLiteral("Tool_Drill4");
    case Bit::Drill5:
        return QStringLiteral("Tool_Drill5");
    case Bit::Drill6:
        return QStringLiteral("Tool_Drill6");

    case Bit::ScrewHex:
        return QStringLiteral("Tool_Hex");
    case Bit::ScrewHexMedium:
        return QStringLiteral("Tool_HexMed");
    case Bit::ScrewHexSmall:
        return QStringLiteral("Tool_HexSmall");
    case Bit::ScrewHexTiny:
        return QStringLiteral("Tool_HexTiny");

    case Bit::ScrewTorx:
        return QStringLiteral("Tool_Torx");
    case Bit::ScrewTorxMedium:
        return QStringLiteral("Tool_TorxMed");
    case Bit::ScrewTorxSmall:
        return QStringLiteral("Tool_TorxSmall");
    case Bit::ScrewTorxTiny:
        return QStringLiteral("Tool_TorxTiny");

    case Bit::ScrewPhilips:
        return QStringLiteral("Tool_Philips");
    case Bit::ScrewPhilipsMedium:
        return QStringLiteral("Tool_PhilipsMed");
    case Bit::ScrewPhilipsSmall:
        return QStringLiteral("Tool_PhilipsSmall");

    case Bit::ScrewFlat:
        return QStringLiteral("Tool_Flat");
    case Bit::ScrewFlatMedium:
        return QStringLiteral("Tool_FlatMed");
    case Bit::ScrewFlatSmall:
        return QStringLiteral("Tool_FlatSmall");
    }

    Q_UNREACHABLE();
}

void ensureClockOnAnimationPlayer(Kuesa::AnimationPlayer *player)
{
    Qt3DAnimation::QClock *c = player->clock();

    // Make sure player has a clock
    const bool hasClock = c != nullptr;
    if (!hasClock) {
        c = new Qt3DAnimation::QClock;
        player->setClock(c);
    }
}

} // anonymous namespace

/*
    Controller for the UserManual screen.

    We store a dedicated scene configuration for each seletable part of the drill.

    When selecting a new part, we will switch the active scene configuration
    instance to match the one associated with the part.

    This makes it convenient to store the assets and watchers we need when
    selecting different parts.
 */
ScreenController::ScreenController(Qt3DCore::QNode *parent)
    : AbstractScreenController(parent)
{
    QObject::connect(this, &ScreenController::selectedPartChanged,
                     this, &ScreenController::updateSceneConfiguration);

    KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
    configuration->setSource(QUrl(QStringLiteral("qrc:/drill/drill.gltf")));
    setSceneConfiguration(configuration);

    // Views
    m_mainViewConfiguration = new KuesaUtils::ViewConfiguration;
    m_mainViewConfiguration->setClearColor(QColor(Qt::transparent));
    m_mainViewConfiguration->setCameraName(QStringLiteral("CamOrbitCenter.CamOrbit"));
    m_mainViewConfiguration->setLayerNames({ QStringLiteral("LayerDevice"), QStringLiteral("LayerEnv") });
    m_mainViewConfiguration->setSkinning(true); // To view cubemap
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
                         this, &ScreenController::triggerPositionChanged);
    }

    {
        m_clutchTracker = new Kuesa::TransformTracker();
        m_clutchTracker->setName(QStringLiteral("Drill.LABEL_Clutch"));
        m_mainViewConfiguration->addTransformTracker(m_clutchTracker);
        QObject::connect(m_clutchTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &ScreenController::clutchPositionChanged);
    }

    {
        m_chuckTracker = new Kuesa::TransformTracker();
        m_chuckTracker->setName(QStringLiteral("Drill.LABEL_Chuck"));
        m_mainViewConfiguration->addTransformTracker(m_chuckTracker);
        QObject::connect(m_chuckTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &ScreenController::chuckPositionChanged);
    }

    {
        m_directionSwitchTracker = new Kuesa::TransformTracker();
        m_directionSwitchTracker->setName(QStringLiteral("Drill.LABEL_DirSwitch"));
        m_mainViewConfiguration->addTransformTracker(m_directionSwitchTracker);
        QObject::connect(m_directionSwitchTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &ScreenController::directionSwitchPositionChanged);
    }

    {
        m_batteryPackTracker = new Kuesa::TransformTracker();
        m_batteryPackTracker->setName(QStringLiteral("Drill.LABEL_Battery"));
        m_mainViewConfiguration->addTransformTracker(m_batteryPackTracker);
        QObject::connect(m_batteryPackTracker, &Kuesa::TransformTracker::screenPositionChanged,
                         this, &ScreenController::batteryPackPositionChanged);
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

        // We have two animations. One with all the steps of the tutorial one after the other
        // The other animation is a pulse animation that we used to encode events
        // We create a timeline which emits an "up" signal when the pulse animation goes from 0 to 1
        // Similarly, the timeline emits a "down" signal when the signal animation goes from 1 to 0
        // This allows us to stop the tutorial animation when we detect the "up" signal
        m_steppedPlayer = new Kuesa::SteppedAnimationPlayer();
        m_steppedPlayer->setClip(QStringLiteral("AnimGuideSteps"));
        m_steppedPlayer->setAnimationNames({ QStringLiteral("AnimGuideAnim") });

        // Add a clock so we can control forward and backwards
        m_animationClock = new Qt3DAnimation::QClock;
        m_steppedPlayer->setClock(m_animationClock);
        configuration->addAnimationPlayer(m_steppedPlayer);
    }

    // Guided Drilling
    {
        m_insertedDrillBitTranform = new Qt3DCore::QTransform();
        m_insertedDrillBitTranform->setParent(sceneConfiguration()->sceneEntity());
        m_insertedDrillBitTranform->setRotationX(90);
    }

    QObject::connect(configuration, &KuesaUtils::SceneConfiguration::loadingDone,
                     this, &ScreenController::addObjectPickersOnBit);

    QObject::connect(this, &ScreenController::bitChanged,
                     this, &ScreenController::loadDrillBit);
    QObject::connect(m_cameraAnimationPlayer, &Kuesa::AnimationPlayer::normalizedTimeChanged,
                     this, &ScreenController::positionOnCameraOrbitChanged);

    QObject::connect(configuration, &KuesaUtils::SceneConfiguration::unloadingDone, this, [this] {
        setSelectedPart(SelectablePart::NoPartSelected);
        m_originalDrillBitParent = nullptr;
    });

    // Set default scene configuration
    updateSceneConfiguration();
}

ScreenController::~ScreenController()
{
    // We own the view configurations if they are not parented
    if (m_mainViewConfiguration && !m_mainViewConfiguration->parent())
        delete m_mainViewConfiguration;
    if (m_detailViewConfiguration && !m_detailViewConfiguration->parent())
        delete m_detailViewConfiguration;
}

void ScreenController::playAnimationBackAndForth(Kuesa::AnimationPlayer *player, int delay)
{
    ensureClockOnAnimationPlayer(player);

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
            setSelectedPart(SelectablePart::NoPartSelected);
            return;
        }

        // Otherwise, play animation backward
        player->start();
    });

    player->restart(delay);
}

void ScreenController::setSelectedPart(ScreenController::SelectablePart selectedPart)
{
    if (selectedPart == m_selectedPart)
        return;
    m_selectedPart = selectedPart;
    emit selectedPartChanged();
}

ScreenController::SelectablePart ScreenController::selectedPart() const
{
    return m_selectedPart;
}

CompleteAnimationRunner::CompleteAnimationRunner(ScreenController *parent,
                                                 Kuesa::AnimationPlayer *p,
                                                 const CompleteAnimationRunner::Callback &callback,
                                                 float speed)
    : QObject(parent)
{
    if (!p->isRunning()) {
        callback();
        return;
    }

    p->setLoopCount(p->currentLoop() + 1);
    ensureClockOnAnimationPlayer(p);
    p->clock()->setPlaybackRate(speed);

    QMetaObject::Connection *c = new QMetaObject::Connection;
    auto singleShot = [=](bool running) {
        if (!running) {
            disconnect(*c);
            callback();
            deleteLater();
        }
    };
    *c = QObject::connect(p, &Kuesa::AnimationPlayer::runningChanged, this, singleShot);
}

void ScreenController::setMode(ScreenController::Mode mode)
{
    if (m_mode == mode)
        return;
    m_mode = mode;
    emit modeChanged(mode);

    // Update Scene Content based on selected mode
    switch (m_mode) {
    case Mode::GuidedDrillingMode: {
        // Run until end of current loop
        new CompleteAnimationRunner(
                this,
                m_cameraAnimationPlayer,
                [this]() {
                    m_mainViewConfiguration->setLayerNames({ QStringLiteral("LayerDevice"), QStringLiteral("LayerTools"), QStringLiteral("LayerEnv") });
                    m_mainViewConfiguration->setCameraName(QStringLiteral("CamTransition"));
                    reset();
                    nextStep();
                },
                10.0f);
        break;
    }
    case Mode::UserManualMode: {
        // Run until end of current loop
        new CompleteAnimationRunner(
                this,
                m_cameraAnimationPlayer,
                [this]() {
                    m_mainViewConfiguration->setCameraName(QStringLiteral("CamOrbitCenter.CamOrbit"));
                    m_mainViewConfiguration->setLayerNames({ QStringLiteral("LayerDevice"), QStringLiteral("LayerTools"), QStringLiteral("LayerEnv") });
                    reset();
                },
                10.0f);
        break;
    }
    case Mode::StatusMode: {
        m_cameraAnimationPlayer->setLoopCount(Kuesa::AnimationPlayer::Infinite);
        m_cameraAnimationPlayer->clock()->setPlaybackRate(1.0f);
        m_cameraAnimationPlayer->restart(true);
        m_mainViewConfiguration->setCameraName(QStringLiteral("CamOrbitCenter.CamOrbit"));
        m_mainViewConfiguration->setLayerNames({ QStringLiteral("LayerDevice"), QStringLiteral("LayerEnv") });
        break;
    }
    default:
        break;
    }
}

ScreenController::Mode ScreenController::mode() const
{
    return m_mode;
}

void ScreenController::setBit(ScreenController::Bit bit)
{
    if (bit == m_bit)
        return;
    m_bit = bit;
    emit bitChanged();

    // Were we reset ?
    if (m_bit == Bit::None) {
        setDrillMode(DrillMode::None);
        setDrillingMaterial(MaterialType::None);
        return;
    }

    // Deduce Drilling Mode and Material for the bit

    // Screw driving
    const bool isScrewDriving = std::find(std::begin(screwDriverBits), std::end(screwDriverBits), bit) != std::end(screwDriverBits);
    if (isScrewDriving) {
        setDrillMode(ScreenController::DrillMode::Screw);
        setDrillingMaterial(ScreenController::MaterialType::None);
        return;
    }

    // Drilling
    setDrillMode(ScreenController::DrillMode::Drill);
    const bool isMetalBit = std::find(std::begin(metalDrillingBits), std::end(metalDrillingBits), bit) != std::end(metalDrillingBits);
    if (isMetalBit) {
        setDrillingMaterial(ScreenController::MaterialType::Metal);
    } else {
        const bool isWoodBit = std::find(std::begin(woodDrillingBits), std::end(woodDrillingBits), bit) != std::end(woodDrillingBits);
        if (isWoodBit) {
            setDrillingMaterial(ScreenController::MaterialType::Wood);
        } else { // Masonry
            setDrillingMaterial(ScreenController::MaterialType::Concrete);
        }
    }
}

ScreenController::Bit ScreenController::bit() const
{
    return m_bit;
}

void ScreenController::setDrillMode(ScreenController::DrillMode mode)
{
    if (mode == m_drillingMode)
        return;
    m_drillingMode = mode;
    emit drillModeChanged();
}

ScreenController::DrillMode ScreenController::drillingMode() const
{
    return m_drillingMode;
}

void ScreenController::setDrillingMaterial(ScreenController::MaterialType material)
{
    if (material == m_drillingMaterial)
        return;
    m_drillingMaterial = material;
    emit drillingMaterialChanged();
}

ScreenController::MaterialType ScreenController::drillingMaterial() const
{
    return m_drillingMaterial;
}

ScreenController::Step ScreenController::guidedDrillingStep() const
{
    return m_drillingStep;
}

ScreenController::Step ScreenController::nextStep()
{
    if (mode() != Mode::GuidedDrillingMode || m_steppedPlayer->isRunning())
        return m_drillingStep;

    auto findNextStep = [this]() -> Step {
        // Note: maybe we later will need to skip some steps based on what
        // previous steps were selected

        // We can also update the sceneConfiguration
        return Step(int(m_drillingStep) + 1);
    };

    if (m_drillingStep < Step::CompletionStep) {
        m_drillingStep = findNextStep();
        emit guidedDrillingStepChanged();

        m_animationClock->setPlaybackRate(1.0);
        m_steppedPlayer->start();
    } else {
        m_steppedPlayer->setNormalizedTime(0.0f);
        setMode(Mode::UserManualMode);
    }
    return m_drillingStep;
}

ScreenController::Step ScreenController::reset()
{
    setBit(Bit::None);
    m_drillingStep = Step::None;
    emit guidedDrillingStepChanged();

    m_steppedPlayer->setNormalizedTime(0.0);

    return m_drillingStep;
}

QString ScreenController::bitName(ScreenController::Bit bit)
{
    switch (bit) {
    case Bit::None:
        return QLatin1String("");

    case Bit::Drill1:
        return QStringLiteral("Metal drill 1");
    case Bit::Drill2:
        return QStringLiteral("Metal drill 2");
    case Bit::Drill3:
        return QStringLiteral("Wood drill 1");
    case Bit::Drill4:
        return QStringLiteral("Wood drill 2");
    case Bit::Drill5:
        return QStringLiteral("Concrete drill 1");
    case Bit::Drill6:
        return QStringLiteral("Concrete drill 2");

    case Bit::ScrewHex:
        return QStringLiteral("Hex");
    case Bit::ScrewHexMedium:
        return QStringLiteral("Medium hex");
    case Bit::ScrewHexSmall:
        return QStringLiteral("Small hex");
    case Bit::ScrewHexTiny:
        return QStringLiteral("Tiny hex");

    case Bit::ScrewTorx:
        return QStringLiteral("Torx");
    case Bit::ScrewTorxMedium:
        return QStringLiteral("Medium Torx");
    case Bit::ScrewTorxSmall:
        return QStringLiteral("Small Torx");
    case Bit::ScrewTorxTiny:
        return QStringLiteral("Tiny Torx");

    case Bit::ScrewPhilips:
        return QStringLiteral("Philips");
    case Bit::ScrewPhilipsMedium:
        return QStringLiteral("Medium Philips");
    case Bit::ScrewPhilipsSmall:
        return QStringLiteral("Small Philips");

    case Bit::ScrewFlat:
        return QStringLiteral("Flat");
    case Bit::ScrewFlatMedium:
        return QStringLiteral("Medium flat");
    case Bit::ScrewFlatSmall:
        return QStringLiteral("Small flat");
    }

    Q_UNREACHABLE();
}

float ScreenController::positionOnCameraOrbit() const
{
    return m_cameraAnimationPlayer->normalizedTime();
}

void ScreenController::setPositionOnCameraOrbit(float p)
{
    m_cameraAnimationPlayer->setNormalizedTime(p);
}

QPointF ScreenController::triggerPosition() const
{
    return m_triggerTracker->screenPosition();
}

QPointF ScreenController::clutchPosition() const
{
    return m_clutchTracker->screenPosition();
}

QPointF ScreenController::chuckPosition() const
{
    return m_chuckTracker->screenPosition();
}

QPointF ScreenController::directionSwitchPosition() const
{
    return m_directionSwitchTracker->screenPosition();
}

QPointF ScreenController::batteryPackPosition() const
{
    return m_batteryPackTracker->screenPosition();
}

void ScreenController::updateSceneConfiguration()
{
    hideDetailView();

    switch (m_selectedPart) {
    case SelectablePart::Trigger: {
        showDetailView(QStringLiteral("CamTrigger"));
        playAnimationBackAndForth(m_triggerAnimationPlayer, 750);
        break;
    }
    case SelectablePart::Clutch: {
        showDetailView(QStringLiteral("CamChuck"));
        break;
    }
    case SelectablePart::Chuck: {
        showDetailView(QStringLiteral("CamChuck"));
        playAnimationBackAndForth(m_toolInOutAnimationPlayer);
        break;
    }
    case SelectablePart::DirectionSwitch: {
        showDetailView(QStringLiteral("CamDirectionSwitch"));
        playAnimationBackAndForth(m_directionSwitchAnimationPlayer, 750);
        break;
    }
    case SelectablePart::BatteryPack: {
        showDetailView(QStringLiteral("CamBattery"));
        playAnimationBackAndForth(m_batteryInOutAnimationPlayer, 750);
        break;
    }
    case SelectablePart::NoPartSelected: {
        break;
    }
    default:
        Q_UNREACHABLE();
    };
}

void ScreenController::showDetailView(const QString &cameraName)
{
    KuesaUtils::SceneConfiguration *conf = sceneConfiguration();
    m_detailViewConfiguration->setCameraName(cameraName);
    conf->addViewConfiguration(m_detailViewConfiguration);
}

void ScreenController::hideDetailView()
{
    KuesaUtils::SceneConfiguration *conf = sceneConfiguration();
    conf->removeViewConfiguration(m_detailViewConfiguration);
}

void ScreenController::loadDrillBit()
{
    Kuesa::SceneEntity *sceneEntity = sceneConfiguration()->sceneEntity();
    if (!sceneEntity)
        return;
    Qt3DCore::QEntity *drillBitHolder = sceneEntity->entity(QStringLiteral("Drill.DrillAxis.DrillHelper.ToolHelper"));
    if (drillBitHolder) {
        if (m_insertedDrillBit) {
            m_insertedDrillBit->setParent(m_originalDrillBitParent);
            m_insertedDrillBit->removeComponent(m_insertedDrillBitTranform);
            m_insertedDrillBit->addComponent(m_originalDrillBitTransform);

            m_insertedDrillBit = nullptr;
        }

        if (m_bit == Bit::None)
            return;

        // Retrieve Drill given its name and parent it
        m_insertedDrillBit = sceneEntity->entity(gltfBitName(m_bit));
        m_insertedDrillBit->setParent(drillBitHolder);
        m_originalDrillBitTransform = m_insertedDrillBit->componentsOfType<Qt3DCore::QTransform>().at(0);
        m_insertedDrillBit->removeComponent(m_originalDrillBitTransform);
        m_insertedDrillBit->addComponent(m_insertedDrillBitTranform);
    }
}

void ScreenController::addObjectPickersOnBit()
{
    Kuesa::SceneEntity *sceneEntity = sceneConfiguration()->sceneEntity();
    if (!sceneEntity)
        return;

    for (const auto bit : bits) {
        Qt3DCore::QEntity *drillBit = sceneEntity->entity(gltfBitName(bit));

        // Record Drill Bits original parent to restore parenting
        // when switching between bits
        if (!m_originalDrillBitParent)
            m_originalDrillBitParent = drillBit->parentEntity();

        if (drillBit->componentsOfType<Qt3DRender::QObjectPicker>().empty()) {
            Qt3DRender::QObjectPicker *picker = new Qt3DRender::QObjectPicker();
            picker->setHoverEnabled(true);
            QObject::connect(picker, &Qt3DRender::QObjectPicker::clicked, this, [this, bit] {
                if (mode() != Mode::GuidedDrillingMode)
                    setMode(Mode::GuidedDrillingMode);
                else
                    setBit(bit);
            });
            drillBit->addComponent(picker);
        }
    }
}
