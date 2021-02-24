/*
    usermanualscreencontroller.h

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

#ifndef USERMANUALSCREENCONTROLLER_H
#define USERMANUALSCREENCONTROLLER_H

#include "abstractscreencontroller.h"
#include <QHash>
#include <QPointer>

class UserManualScreenController : public AbstractScreenController
{
    Q_OBJECT
    Q_PROPERTY(SelectablePart selectedPart READ selectedPart WRITE setSelectedPart NOTIFY selectedPartChanged)
    Q_PROPERTY(QPointF triggerPosition READ triggerPosition NOTIFY triggerPositionChanged)
    Q_PROPERTY(QPointF clutchPosition READ clutchPosition NOTIFY clutchPositionChanged)
    Q_PROPERTY(QPointF chuckPosition READ chuckPosition NOTIFY chuckPositionChanged)
    Q_PROPERTY(QPointF directionSwitchPosition READ directionSwitchPosition NOTIFY directionSwitchPositionChanged)
    Q_PROPERTY(QPointF batteryPackPosition READ batteryPackPosition NOTIFY batteryPackPositionChanged)
public:
    enum SelectablePart {
        Trigger,
        Clutch,
        Chuck,
        DirectionSwitch,
        BatteryPack,
        NoPartSelected
    };
    Q_ENUM(SelectablePart)

    explicit UserManualScreenController(QObject *parent = nullptr);
    ~UserManualScreenController();

    void setSelectedPart(SelectablePart selectedPart);
    SelectablePart selectedPart() const;

    QPointF triggerPosition() const;
    QPointF clutchPosition() const;
    QPointF chuckPosition() const;
    QPointF directionSwitchPosition() const;
    QPointF speedSwitchPosition() const;
    QPointF batteryPackPosition() const;

signals:
    void selectedPartChanged();
    void triggerPositionChanged(const QPointF &screenPosition);
    void clutchPositionChanged(const QPointF &screenPosition);
    void chuckPositionChanged(const QPointF &screenPosition);
    void directionSwitchPositionChanged(const QPointF &screenPosition);
    void batteryPackPositionChanged(const QPointF &screenPosition);

private:
    void updateSceneConfiguration();
    void showDetailView(const QString &camera);
    void hideDetailView();
    void playAnimationBackAndForth(Kuesa::AnimationPlayer *player, int delay = 0);

    SelectablePart m_selectedPart = NoPartSelected;
    QHash<SelectablePart, KuesaUtils::SceneConfiguration *> m_sceneConfigurationsTable;

    // Trackers owned by the SceneConfiguration
    Kuesa::TransformTracker *m_triggerTracker = nullptr;
    Kuesa::TransformTracker *m_clutchTracker = nullptr;
    Kuesa::TransformTracker *m_chuckTracker = nullptr;
    Kuesa::TransformTracker *m_directionSwitchTracker = nullptr;
    Kuesa::TransformTracker *m_batteryPackTracker = nullptr;

    // Animations owned by the SceneConfiguration
    Kuesa::AnimationPlayer *m_cameraAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_batteryInOutAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_directionSwitchAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_triggerAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_drillAnimationPlayer = nullptr;
    Kuesa::AnimationPlayer *m_toolInOutAnimationPlayer = nullptr;

    // Views owned by the FrameGraph if parented, by us otherwise
    QPointer<Kuesa::View> m_mainView;
    QPointer<Kuesa::View> m_detailView;
};

#endif // USERMANUALSCREENCONTROLLER_H
