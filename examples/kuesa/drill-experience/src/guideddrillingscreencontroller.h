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

#ifndef GUIDEDDRILLINGSCREENCONTROLLER_H
#define GUIDEDDRILLINGSCREENCONTROLLER_H

#include "abstractscreencontroller.h"

class GuidedDrillingScreenController : public AbstractScreenController
{
    Q_OBJECT
    Q_PROPERTY(Step currentStep READ currentStep NOTIFY currentStepChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(MaterialType material READ material WRITE setMaterial NOTIFY materialChanged)
    Q_PROPERTY(QString bitName READ bitName WRITE setBitName NOTIFY bitNameChanged)

public:

    enum Mode {
        Drill,
        Screw,
        Unscrew
    };
    Q_ENUM(Mode)

    enum MaterialType {
        Wood,
        Concrete,
        Metal
    };
    Q_ENUM(MaterialType)

    enum Step {
        ModeSelection,
        MaterialSelection,
        BitSelection,
        SetupBit,
        SetupClutch,
        SetupSpeed,
        SetupDirection,
        CompletionStep
    };
    Q_ENUM(Step)

    explicit GuidedDrillingScreenController(QObject *parent = nullptr);

    Step currentStep() const;
    Mode mode() const;
    MaterialType material() const;
    QString bitName() const;

    void setMode(Mode mode);
    void setMaterial(MaterialType material);
    void setBitName(const QString &bitName);

    Q_INVOKABLE Step nextStep();
    Q_INVOKABLE Step previousStep();
    Q_INVOKABLE Step reset();

signals:
    void currentStepChanged();
    void modeChanged();
    void materialChanged();
    void bitNameChanged();

private:
    Step m_currentStep = ModeSelection;
    Mode m_mode = Drill;
    MaterialType m_material = Wood;
    QString m_bitName;

    std::vector<Step> m_history;

    // Owned by the sceneConfiguration
    Kuesa::AnimationPlayer *m_drillAnimation = nullptr;
    Kuesa::AnimationPlayer *m_triggerPressAnimation = nullptr;
    Kuesa::AnimationPlayer *m_directionSwitchAnimation = nullptr;
    Kuesa::AnimationPlayer *m_drillInsertAnimation = nullptr;
    Kuesa::AnimationPlayer *m_drillRemoveAnimation = nullptr;

    Qt3DCore::QEntity *m_originalDrillBitParent = nullptr;

    void loadDrillBit();
    void addObjectPickersOnBit();
    void syncViewToStep();
};

#endif // GUIDEDDRILLINGSCREENCONTROLLER_H
