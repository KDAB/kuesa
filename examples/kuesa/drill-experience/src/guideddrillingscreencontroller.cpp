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

#include <array>

#include "guideddrillingscreencontroller.h"
#include <Kuesa/View>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>

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
    configuration->setCameraName(QStringLiteral("CamOrbitCenter.CamOrbit"));

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

    configuration->addAnimationPlayer(m_drillAnimation);
    configuration->addAnimationPlayer(m_triggerPressAnimation);
    configuration->addAnimationPlayer(m_directionSwitchAnimation);
    configuration->addAnimationPlayer(m_drillInsertAnimation);
    configuration->addAnimationPlayer(m_drillRemoveAnimation);

    setSceneConfiguration(configuration);

    QObject::connect(this, &GuidedDrillingScreenController::currentStepChanged,
                     this, &GuidedDrillingScreenController::syncViewToStep);
    QObject::connect(configuration, &KuesaUtils::SceneConfiguration::loadingDone,
                     this, &GuidedDrillingScreenController::addObjectPickersOnBit);
    QObject::connect(this, &GuidedDrillingScreenController::bitChanged,
                     this, &GuidedDrillingScreenController::loadDrillBit);
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

void GuidedDrillingScreenController::setMode(GuidedDrillingScreenController::Mode mode)
{
    if (mode == m_mode)
        return;
    m_mode = mode;
    emit modeChanged();
}

void GuidedDrillingScreenController::setMaterial(MaterialType material)
{
    if (material == m_material)
        return;
    m_material = material;
    emit materialChanged();
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
    auto findNextStep = [this] () -> Step {
        // Note: maybe we later will need to skip some steps based on what
        // previous steps were selected

        // We can also update the sceneConfiguration
        return Step(m_currentStep + 1);
    };

    if (m_currentStep < CompletionStep) {
        m_history.push_back(m_currentStep);
        m_currentStep = findNextStep();
        emit currentStepChanged();
    }
    return m_currentStep;
}

GuidedDrillingScreenController::Step GuidedDrillingScreenController::previousStep()
{
    if (m_history.size() > 0) {
        m_currentStep = m_history.back();
        m_history.pop_back();
        emit currentStepChanged();
    }
    return m_currentStep;
}

GuidedDrillingScreenController::Step GuidedDrillingScreenController::reset()
{
    m_history.clear();
    m_currentStep = ModeSelection;
    emit currentStepChanged();
    return m_currentStep;
}

void GuidedDrillingScreenController::loadDrillBit()
{
    Kuesa::SceneEntity *sceneEntity = sceneConfiguration()->sceneEntity();
    if (!sceneEntity)
        return;
    Qt3DCore::QEntity *drillBitHolder = sceneEntity->entity(QStringLiteral("Drill.DrillAxis.DrillHelper.ToolHelper"));
    if (drillBitHolder) {
        const auto childNodes = drillBitHolder->childNodes();
        for (Qt3DCore::QNode *c : childNodes)
            c->setParent(m_originalDrillBitParent);

        if (m_bit == Bit::None)
            return;

        // Retrieve Drill given its name and parent it
        Qt3DCore::QEntity *drillBit = sceneEntity->entity(gltfBitName(m_bit));
        if (drillBit) {
            drillBit->setParent(drillBitHolder);
        }
    }

}

void GuidedDrillingScreenController::syncViewToStep()
{
    KuesaUtils::SceneConfiguration *configuration = sceneConfiguration();
    switch (currentStep()) {
    case ModeSelection:
        configuration->setCameraName(QStringLiteral("CamOrbitCenter.CamOrbit"));
        break;
    case MaterialSelection:
        configuration->setCameraName(QStringLiteral("CamOrbitCenter.CamOrbit"));
        break;
    case BitSelection:
        configuration->setCameraName(QStringLiteral("CamTools"));
        break;
    case SetupBit:
        configuration->setCameraName(QStringLiteral("CamChuck"));
        m_drillInsertAnimation->restart();
        break;
    case SetupClutch:
        configuration->setCameraName(QStringLiteral("CamChuck"));
        break;
    case SetupSpeed:
        configuration->setCameraName(QStringLiteral("CamChuck"));
        break;
    case SetupDirection:
        configuration->setCameraName(QStringLiteral("CamDirectionSwitch"));
        m_directionSwitchAnimation->restart();
        break;
    case CompletionStep:
        configuration->setCameraName(QStringLiteral("CamTrigger"));
        m_triggerPressAnimation->restart();
        m_drillAnimation->restart();
        break;
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
