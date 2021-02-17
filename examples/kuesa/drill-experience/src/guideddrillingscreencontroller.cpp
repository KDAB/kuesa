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
#include <Kuesa/View>

GuidedDrillingScreenController::GuidedDrillingScreenController(QObject *parent)
    : AbstractScreenController(parent)
{
    KuesaUtils::SceneConfiguration *configuration = new KuesaUtils::SceneConfiguration();
    configuration->setSource(QUrl(QStringLiteral("qrc:/drill/drill.gltf")));
    configuration->setCameraName(QStringLiteral("|CamCenter|OrbitCam"));
    setSceneConfiguration(configuration);

    Kuesa::View *view = new Kuesa::View;
    view->setViewportRect({0.5f, 0.25f, 0.5f, 0.5f});

    addView(view);
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

void GuidedDrillingScreenController::nextStep()
{
    auto findNextStep = [this] () -> Step {
        // Note: maybe we later will need to skip some steps based on what
        // previous steps were selected

        // We can also update the sceneConfiguration
        return Step(m_currentStep + 1);
    };

    if (m_currentStep < CompletionStep) {
        m_history.push_back(findNextStep());
        m_currentStep = m_history.back();
        emit currentStepChanged();
    }
}

void GuidedDrillingScreenController::previousStep()
{
    if (m_history.size() > 0) {
        m_currentStep = m_history.back();
        m_history.pop_back();
        emit currentStepChanged();
    }
}

void GuidedDrillingScreenController::reset()
{
    m_history.clear();
    m_currentStep = ModeSelection;
    emit currentStepChanged();
}
