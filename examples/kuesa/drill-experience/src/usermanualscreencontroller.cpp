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

    // Table containing the various scene configurations for each on selectable part
    {
        KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
        configuration->setSource(QUrl(QStringLiteral("qrc:/drill/drill.gltf")));
        configuration->setCameraName(QStringLiteral("CamOrbiteCenter.CamOrbit"));

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


        m_sceneConfigurationsTable[NoPartSelected] = configuration;
    }
    {
        KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
        configuration->setSource(QUrl(QStringLiteral("")));
        configuration->setCameraName(QStringLiteral(""));
        m_sceneConfigurationsTable[Trigger] = configuration;
    }
    {
        KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
        configuration->setSource(QUrl(QStringLiteral("")));
        configuration->setCameraName(QStringLiteral(""));
        m_sceneConfigurationsTable[Clutch] = configuration;
    }
    {
        KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
        configuration->setSource(QUrl(QStringLiteral("")));
        configuration->setCameraName(QStringLiteral(""));
        m_sceneConfigurationsTable[Chuck] = configuration;
    }
    {
        KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
        configuration->setSource(QUrl(QStringLiteral("")));
        configuration->setCameraName(QStringLiteral(""));
        m_sceneConfigurationsTable[DirectionSwitch] = configuration;
    }
    {
        KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
        configuration->setSource(QUrl(QStringLiteral("")));
        configuration->setCameraName(QStringLiteral(""));
        m_sceneConfigurationsTable[BatteryPack] = configuration;
    }

    // Set default scene configuration
    updateSceneConfiguration();
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
    KuesaUtils::SceneConfiguration *newConfiguration = m_sceneConfigurationsTable.value(m_selectedPart, nullptr);
    // Call base class setter method
    setSceneConfiguration(newConfiguration);
}
