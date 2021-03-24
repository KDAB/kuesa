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
#include <Kuesa/AnimationPulse>

class GuidedDrillingScreenController : public AbstractScreenController
{
    Q_OBJECT
    Q_PROPERTY(Step currentStep READ currentStep NOTIFY currentStepChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(MaterialType material READ material WRITE setMaterial NOTIFY materialChanged)
    Q_PROPERTY(Bit bit READ bit WRITE setBit NOTIFY bitChanged)
    Q_PROPERTY(std::vector<int> filteredBits READ filteredBits NOTIFY filteredBitsChanged)

public:
    enum class Mode {
        None,
        Drill,
        Screw,
    };
    Q_ENUM(Mode)

    enum class MaterialType {
        None,
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
        SetupDirection,
        CompletionStep
    };
    Q_ENUM(Step)

    enum class Bit {
        None,

        Drill1,
        Drill2,
        Drill3,
        Drill4,
        Drill5,
        Drill6,

        ScrewHex,
        ScrewHexMedium,
        ScrewHexSmall,
        ScrewHexTiny,

        ScrewTorx,
        ScrewTorxMedium,
        ScrewTorxSmall,
        ScrewTorxTiny,

        ScrewPhilips,
        ScrewPhilipsMedium,
        ScrewPhilipsSmall,

        ScrewFlat,
        ScrewFlatMedium,
        ScrewFlatSmall,
    };
    Q_ENUM(Bit)

    explicit GuidedDrillingScreenController(QObject *parent = nullptr);

    Step currentStep() const;
    Mode mode() const;
    MaterialType material() const;
    Bit bit() const;
    std::vector<int> filteredBits() const;

    void setMode(Mode mode);
    void setMaterial(MaterialType material);
    void setBit(Bit bit);

    Q_INVOKABLE Step nextStep();
    Q_INVOKABLE Step previousStep();
    Q_INVOKABLE Step reset();

    Q_INVOKABLE QString bitName(Bit bit);

signals:
    void currentStepChanged();
    void modeChanged();
    void materialChanged();
    void bitChanged();
    void filteredBitsChanged();

private:
    Step m_currentStep = ModeSelection;
    Mode m_mode = Mode::None;
    MaterialType m_material = MaterialType::None;
    Bit m_bit = Bit::None;
    std::vector<Bit> m_filteredBits;

    std::vector<Step> m_history;

    // Owned by the sceneEntity
    Qt3DCore::QTransform *m_insertedDrillBitTranform;

    // Owned by the sceneConfiguration
    Kuesa::AnimationPlayer *m_stepsAnimation = nullptr;
    Kuesa::AnimationPulse *m_pulse = nullptr;
    Qt3DAnimation::QClock *m_animationClock = nullptr;

    // Weak pointers
    Qt3DCore::QEntity *m_insertedDrillBit = nullptr;
    Qt3DCore::QEntity *m_originalDrillBitParent = nullptr;
    Qt3DCore::QTransform *m_originalDrillBitTransform = nullptr;

    void loadDrillBit();
    void addObjectPickersOnBit();
    void filterBits();
};

#endif // GUIDEDDRILLINGSCREENCONTROLLER_H
