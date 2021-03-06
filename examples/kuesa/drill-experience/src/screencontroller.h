/*
    screencontroller.h

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

#ifndef SCREENCONTROLLER_H
#define SCREENCONTROLLER_H

#include "abstractscreencontroller.h"
#include <QHash>
#include <QPointer>
#include <QObjectList>

namespace Kuesa {
class SteppedAnimationPlayer;
} // namespace Kuesa

class ScreenController : public AbstractScreenController
{
    Q_OBJECT
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(SelectablePart selectedPart READ selectedPart WRITE setSelectedPart NOTIFY selectedPartChanged)
    Q_PROPERTY(Step guidedDrillingStep READ guidedDrillingStep NOTIFY guidedDrillingStepChanged)
    Q_PROPERTY(QList<QObject *> partLabels READ partLabels CONSTANT)
    Q_PROPERTY(float positionOnCameraOrbit READ positionOnCameraOrbit WRITE setPositionOnCameraOrbit NOTIFY positionOnCameraOrbitChanged)
public:
    enum class Mode {
        StatusMode,
        UserManualMode,
        GuidedDrillingMode
    };
    Q_ENUM(Mode)

    enum class SelectablePart {
        Trigger,
        Clutch,
        Chuck,
        DirectionSwitch,
        BatteryPack,
        NoPartSelected
    };
    Q_ENUM(SelectablePart)

    enum class DrillMode {
        None,
        Drill,
        Screw,
    };
    Q_ENUM(DrillMode)

    enum class MaterialType {
        None,
        Wood,
        Concrete,
        Metal
    };
    Q_ENUM(MaterialType)

    enum class Step : int {
        None,
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

    explicit ScreenController(Qt3DCore::QNode *parent = nullptr);
    ~ScreenController();

    void setSelectedPart(SelectablePart selectedPart);
    SelectablePart selectedPart() const;

    QObjectList partLabels() const;

    void setMode(Mode mode);
    Mode mode() const;

    void setBit(Bit bit);
    Bit bit() const;

    DrillMode drillingMode() const;
    MaterialType drillingMaterial() const;

    Step guidedDrillingStep() const;
    Q_INVOKABLE Step nextStep();
    Q_INVOKABLE Step reset();

    Q_INVOKABLE QString bitName(Bit bit);

    float positionOnCameraOrbit() const;
    void setPositionOnCameraOrbit(float p);

signals:
    void selectedPartChanged();
    void modeChanged(Mode mode);
    void guidedDrillingStepChanged();
    void drillModeChanged();
    void drillingMaterialChanged();
    void bitChanged();
    void positionOnCameraOrbitChanged();

private:
    void updateSceneConfiguration();
    void showDetailView(const QString &camera);
    void hideDetailView();
    void playAnimationBackAndForth(Kuesa::AnimationPlayer *player, int delay = 0);

    void loadDrillBit();
    void setPartLabelNames();
    void addObjectPickersOnBit();

    void setDrillMode(DrillMode mode);
    void setDrillingMaterial(MaterialType material);

    SelectablePart m_selectedPart = SelectablePart::NoPartSelected;
    QHash<SelectablePart, KuesaUtils::SceneConfiguration *> m_sceneConfigurationsTable;

    // Labels owned by the SceneConfiguration
    QObjectList m_partLabels;

    // Animations owned by the SceneConfiguration
    Kuesa::AnimationPlayer *m_cameraAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_batteryInOutAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_directionSwitchAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_triggerAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_drillAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_toolInOutAnimationPlayer = nullptr;

    // Views owned by the SceneConfiguration if parented, by us otherwise
    KuesaUtils::ViewConfiguration *m_mainViewConfiguration = nullptr;
    KuesaUtils::ViewConfiguration *m_detailViewConfiguration = nullptr;

    // Owned by the sceneEntity
    Qt3DCore::QTransform *m_insertedDrillBitTranform;

    // Weak pointers
    Qt3DCore::QEntity *m_insertedDrillBit = nullptr;
    Qt3DCore::QEntity *m_originalDrillBitParent = nullptr;
    Qt3DCore::QTransform *m_originalDrillBitTransform = nullptr;

    // Owned by the sceneConfiguration
    Kuesa::SteppedAnimationPlayer *m_steppedPlayer = nullptr;
    Qt3DAnimation::QClock *m_animationClock = nullptr;

    Mode m_mode = ScreenController::Mode::StatusMode;
    Bit m_bit = Bit::None;
    MaterialType m_drillingMaterial = MaterialType::None;
    DrillMode m_drillingMode = DrillMode::None;

    Step m_drillingStep = Step::None;
};

class CompleteAnimationRunner : public QObject
{
    Q_OBJECT
public:
    using Callback = std::function<void()>;

    explicit CompleteAnimationRunner(ScreenController *parent,
                                     Kuesa::AnimationPlayer *p,
                                     const CompleteAnimationRunner::Callback &callback,
                                     float speed = 1.0f);
};

class PartLabel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF position READ position NOTIFY positionChanged)
    Q_PROPERTY(QString labelName READ labelName NOTIFY labelNameChanged)
    Q_PROPERTY(QString nodeName READ nodeName CONSTANT)
    Q_PROPERTY(ScreenController::SelectablePart part READ part CONSTANT)

public:
    explicit PartLabel(const QString &nodeName,
                       const ScreenController::SelectablePart part,
                       Kuesa::TransformTracker *tracker,
                       QObject *parent = nullptr);

    QPointF position() const;
    QString labelName() const;
    ScreenController::SelectablePart part() const;
    QString nodeName() const;
    void setLabelName(const QString &labelName);

Q_SIGNALS:
    void labelNameChanged();
    void positionChanged();

private:
    const QString m_nodeName;
    const ScreenController::SelectablePart m_part;
    Kuesa::TransformTracker *m_tracker = nullptr;
    QString m_labelName;
};

#endif // SCREENCONTROLLER_H
