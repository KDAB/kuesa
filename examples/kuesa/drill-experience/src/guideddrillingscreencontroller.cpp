/*
    guideddrillingscreencontroller.h

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

#include "guideddrillingscreencontroller.h"
#include <array>
#include <Kuesa/View>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <KuesaUtils/viewconfiguration.h>

namespace {
using Bit = GuidedDrillingScreenController::Bit;
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
} // anonymous namespace

GuidedDrillingScreenController::GuidedDrillingScreenController(QObject *parent)
    : AbstractScreenController(parent)
{
    KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
    configuration->setSource(QUrl(QStringLiteral("qrc:/drill/drill.gltf")));

    KuesaUtils::ViewConfiguration *mainViewConfiguration = new KuesaUtils::ViewConfiguration;
    mainViewConfiguration->setCameraName(QStringLiteral("CamTransition"));
    mainViewConfiguration->setClearColor(QColor(Qt::transparent));
    configuration->addViewConfiguration(mainViewConfiguration);

    m_drillAnimation = new Kuesa::AnimationPlayer();
    m_drillAnimation->setClip(QStringLiteral("AnimDrillCW"));

    m_triggerPressAnimation = new Kuesa::AnimationPlayer();
    m_triggerPressAnimation->setClip(QStringLiteral("AnimTriggerPress"));

    m_directionSwitchAnimation = new Kuesa::AnimationPlayer();
    m_directionSwitchAnimation->setClip(QStringLiteral("AnimChangeDirectionLR"));

    m_drillInsertAnimation = new Kuesa::AnimationPlayer();
    m_drillInsertAnimation->setClip(QStringLiteral("AnimToolIn"));

    m_drillRemoveAnimation = new Kuesa::AnimationPlayer();
    m_drillRemoveAnimation->setClip(QStringLiteral("AnimToolOut"));

    m_cameraTransitionAnimationSideToBit = new Kuesa::AnimationPlayer();
    m_cameraTransitionAnimationSideToBit->setClip(QStringLiteral("AnimTransition01"));

    m_cameraTransitionAnimationBitToChuck = new Kuesa::AnimationPlayer();
    m_cameraTransitionAnimationBitToChuck->setClip(QStringLiteral("AnimTransition02"));

    m_cameraTransitionAnimationChuckToRear = new Kuesa::AnimationPlayer();
    m_cameraTransitionAnimationChuckToRear->setClip(QStringLiteral("AnimTransition03"));

    m_cameraTransitionAnimationRearToTrigger = new Kuesa::AnimationPlayer();
    m_cameraTransitionAnimationRearToTrigger->setClip(QStringLiteral("AnimTransition04"));

    m_cameraTransitionAnimationTriggerToSide = new Kuesa::AnimationPlayer();
    m_cameraTransitionAnimationTriggerToSide->setClip(QStringLiteral("AnimTransition05"));

    configuration->addAnimationPlayer(m_drillAnimation);
    configuration->addAnimationPlayer(m_triggerPressAnimation);
    configuration->addAnimationPlayer(m_directionSwitchAnimation);
    configuration->addAnimationPlayer(m_drillInsertAnimation);
    configuration->addAnimationPlayer(m_drillRemoveAnimation);
    configuration->addAnimationPlayer(m_cameraTransitionAnimationSideToBit);
    configuration->addAnimationPlayer(m_cameraTransitionAnimationBitToChuck);
    configuration->addAnimationPlayer(m_cameraTransitionAnimationChuckToRear);
    configuration->addAnimationPlayer(m_cameraTransitionAnimationRearToTrigger);
    configuration->addAnimationPlayer(m_cameraTransitionAnimationTriggerToSide);

    setSceneConfiguration(configuration);

    m_insertedDrillBitTranform = new Qt3DCore::QTransform();
    m_insertedDrillBitTranform->setParent(sceneConfiguration()->sceneEntity());
    m_insertedDrillBitTranform->setRotationX(90);

    QObject::connect(configuration, &KuesaUtils::SceneConfiguration::loadingDone,
                     this, &GuidedDrillingScreenController::addObjectPickersOnBit);
    QObject::connect(configuration, &KuesaUtils::SceneConfiguration::unloadingDone,
                     this, [this] { m_originalDrillBitParent = nullptr; });
    QObject::connect(this, &GuidedDrillingScreenController::bitChanged,
                     this, &GuidedDrillingScreenController::loadDrillBit);
    QObject::connect(this, &GuidedDrillingScreenController::modeChanged,
                     this, &GuidedDrillingScreenController::filterBits);
    QObject::connect(this, &GuidedDrillingScreenController::materialChanged,
                     this, &GuidedDrillingScreenController::filterBits);
}

GuidedDrillingScreenController::Step GuidedDrillingScreenController::currentStep() const
{
    return m_currentStep;
}

GuidedDrillingScreenController::Mode GuidedDrillingScreenController::mode() const
{
    return m_mode;
}

GuidedDrillingScreenController::MaterialType GuidedDrillingScreenController::material() const
{
    return m_material;
}

GuidedDrillingScreenController::Bit GuidedDrillingScreenController::bit() const
{
    return m_bit;
}

std::vector<int> GuidedDrillingScreenController::filteredBits() const
{
    std::vector<int> bits;
    std::transform(std::begin(m_filteredBits), std::end(m_filteredBits), std::back_inserter(bits), [](Bit bit) { return int(bit); });
    return bits;
}

void GuidedDrillingScreenController::setMode(GuidedDrillingScreenController::Mode mode)
{
    if (mode == m_mode)
        return;
    m_mode = mode;
    emit modeChanged();
    setMaterial(MaterialType::None);
    setBit(Bit::None);
}

void GuidedDrillingScreenController::setMaterial(MaterialType material)
{
    if (material == m_material)
        return;
    m_material = material;
    emit materialChanged();
    setBit(Bit::None);
}

void GuidedDrillingScreenController::setBit(Bit bit)
{
    if (bit == m_bit)
        return;
    m_bit = bit;
    emit bitChanged();
}

GuidedDrillingScreenController::Step GuidedDrillingScreenController::nextStep()
{
    auto findNextStep = [this]() -> Step {
        // Note: maybe we later will need to skip some steps based on what
        // previous steps were selected

        // We can also update the sceneConfiguration
        return Step(m_currentStep + 1);
    };

    if (m_currentStep < CompletionStep) {
        m_history.push_back(m_currentStep);
        m_currentStep = findNextStep();
        emit currentStepChanged();
        syncViewToStep(m_history.back());
    }
    return m_currentStep;
}

GuidedDrillingScreenController::Step GuidedDrillingScreenController::previousStep()
{
    if (m_history.size() > 0) {
        const Step lastStep = m_currentStep;
        m_currentStep = m_history.back();
        m_history.pop_back();
        emit currentStepChanged();
        syncViewToStep(lastStep);
    }
    return m_currentStep;
}

GuidedDrillingScreenController::Step GuidedDrillingScreenController::reset()
{
    m_history.clear();

    setBit(Bit::None);
    setMode(Mode::None);
    setMaterial(MaterialType::None);

    const Step lastStep = m_currentStep;
    m_currentStep = ModeSelection;
    emit currentStepChanged();
    syncViewToStep(lastStep);

    return m_currentStep;
}

QString GuidedDrillingScreenController::bitName(Bit bit)
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

void GuidedDrillingScreenController::loadDrillBit()
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

namespace {

void launchCameraAnimation(Kuesa::AnimationPlayer *player, bool reversed)
{
    if (!player)
        return;

    Qt3DAnimation::QClock *c = player->clock();

    // Make sure player has a clock
    const bool hasClock = c != nullptr;
    if (!hasClock) {
        c = new Qt3DAnimation::QClock;
        player->setClock(c);
    }

    if (reversed) {
        c->setPlaybackRate(-1.0f);
        player->setNormalizedTime(1.0f);
    } else {
        c->setPlaybackRate(1.0f);
        player->setNormalizedTime(0.0f);
    }
    player->start();
}

} // namespace

void GuidedDrillingScreenController::syncViewToStep(Step previousStep)
{
    const bool forward = previousStep < m_currentStep || previousStep == CompletionStep;

    auto forwardAnimationForStep = [this](Step step) -> Kuesa::AnimationPlayer * {
        switch (step) {
        case ModeSelection:
            return m_cameraTransitionAnimationTriggerToSide;
        case BitSelection:
            return m_cameraTransitionAnimationSideToBit;
        case SetupBit:
            return m_cameraTransitionAnimationBitToChuck;
        case SetupDirection:
            return m_cameraTransitionAnimationChuckToRear;
        case CompletionStep:
            return m_cameraTransitionAnimationRearToTrigger;
        default:
            return nullptr;
        }
    };

    auto backwardAnimationForStep = [this](Step step) -> Kuesa::AnimationPlayer * {
        switch (step) {
        case MaterialSelection:
            return m_cameraTransitionAnimationSideToBit;
        case BitSelection:
            return m_cameraTransitionAnimationBitToChuck;
        case SetupBit:
            return m_cameraTransitionAnimationChuckToRear;
        case SetupDirection:
            return m_cameraTransitionAnimationRearToTrigger;
        default:
            return nullptr;
        }
    };

    auto animationForStep = [&](Step step) {
        if (forward)
            return forwardAnimationForStep(step);
        return backwardAnimationForStep(step);
    };

    // Camera Animations
    if (m_currentStep != previousStep)
        launchCameraAnimation(animationForStep(m_currentStep), !forward);

    // Handling of Initial Starting case
    if (m_currentStep == ModeSelection && previousStep == m_currentStep)
        m_cameraTransitionAnimationTriggerToSide->run(0.99f, 1.0f);

    // Other animations
    switch (m_currentStep) {
    case ModeSelection:
        break;
    case MaterialSelection:
        break;
    case BitSelection:
        break;
    case SetupBit: {
        constexpr int delay = 500;
        m_drillInsertAnimation->restart(delay);
        break;
    }
    case SetupDirection: {
        constexpr int delay = 1250;
        m_directionSwitchAnimation->restart(delay);
        break;
    }
    case CompletionStep: {
        constexpr int delay = 1250;
        m_triggerPressAnimation->restart(delay);
        m_drillAnimation->restart(delay);
        break;
    }
    }
}

void GuidedDrillingScreenController::addObjectPickersOnBit()
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
                setBit(bit);
            });
            drillBit->addComponent(picker);
        }
    }
}

void GuidedDrillingScreenController::filterBits()
{
    m_filteredBits = [&]() -> std::vector<Bit> {
        switch (m_mode) {
        case Mode::None:
            return { bits.cbegin(), bits.cend() };
        case Mode::Screw:
            return { screwDriverBits.cbegin(), screwDriverBits.cend() };
        case Mode::Drill:
            switch (m_material) {
            case MaterialType::None:
                return { drillingBits.cbegin(), drillingBits.cend() };
            case MaterialType::Wood:
                return { woodDrillingBits.cbegin(), woodDrillingBits.cend() };
            case MaterialType::Concrete:
                return { concreteDrillingBits.cbegin(), concreteDrillingBits.cend() };
            case MaterialType::Metal:
                return { metalDrillingBits.cbegin(), metalDrillingBits.cend() };
            }

            Q_UNREACHABLE();
        }

        Q_UNREACHABLE();
    }();
    Q_EMIT filteredBitsChanged();

    Kuesa::SceneEntity *sceneEntity = sceneConfiguration()->sceneEntity();
    if (!sceneEntity)
        return;

    for (const auto bit : bits) {
        Qt3DCore::QEntity *drillBit = sceneEntity->entity(gltfBitName(bit));
        drillBit->setEnabled(false);
    }

    for (const auto bit : m_filteredBits) {
        Qt3DCore::QEntity *drillBit = sceneEntity->entity(gltfBitName(bit));
        drillBit->setEnabled(true);
    }
}
