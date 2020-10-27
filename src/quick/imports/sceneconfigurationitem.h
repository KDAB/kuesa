/*
    sceneconfigurationitem.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#ifndef SCENECONFIGURATIONITEM_H
#define SCENECONFIGURATIONITEM_H

#include <QObject>
#include <QtQml/QQmlListProperty>
#include <KuesaUtils/sceneconfiguration.h>
#include <Kuesa/animationplayer.h>
#include <Kuesa/transformtracker.h>

#include <vector>

QT_BEGIN_NAMESPACE

namespace KuesaUtils {

class SceneConfigurationItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Kuesa::AnimationPlayer> animations READ animations CONSTANT)
    Q_PROPERTY(QQmlListProperty<Kuesa::TransformTracker> transformTrackers READ transformTrackers CONSTANT)
public:
    explicit SceneConfigurationItem(QObject *parent = nullptr);
    ~SceneConfigurationItem();

    QQmlListProperty<Kuesa::AnimationPlayer> animations();
    QQmlListProperty<Kuesa::TransformTracker> transformTrackers();

    SceneConfiguration *parentSceneConfiguration() const { return qobject_cast<SceneConfiguration *>(parent()); }

private:
    static void qmlAppendAnimation(QQmlListProperty<Kuesa::AnimationPlayer> *list, Kuesa::AnimationPlayer *node);
    static Kuesa::AnimationPlayer *qmlAnimationAt(QQmlListProperty<Kuesa::AnimationPlayer> *list, int index);
    static int qmlAnimationCount(QQmlListProperty<Kuesa::AnimationPlayer> *list);
    static void qmlClearAnimations(QQmlListProperty<Kuesa::AnimationPlayer> *list);
    static void qmlAppendTrackers(QQmlListProperty<Kuesa::TransformTracker> *list, Kuesa::TransformTracker *node);
    static Kuesa::TransformTracker *qmlTrackersAt(QQmlListProperty<Kuesa::TransformTracker> *list, int index);
    static int qmlTrackersCount(QQmlListProperty<Kuesa::TransformTracker> *list);
    static void qmlClearTrackers(QQmlListProperty<Kuesa::TransformTracker> *list);

    std::vector<Kuesa::AnimationPlayer *> m_managedAnimations;
    std::vector<Kuesa::TransformTracker *> m_managedTrackers;
};

} // namespace KuesaUtils

QT_END_NAMESPACE

#endif // SCENECONFIGURATIONITEM_H
