/*
    sceneconfigurationitem.cpp

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

#include "sceneconfigurationitem.h"

using namespace KuesaUtils;

SceneConfigurationItem::SceneConfigurationItem(QObject *parent)
    : QObject(parent)
{
}

SceneConfigurationItem::~SceneConfigurationItem() = default;

QQmlListProperty<Kuesa::AnimationPlayer> SceneConfigurationItem::animations()
{
    return QQmlListProperty<Kuesa::AnimationPlayer>(this, nullptr,
                                                    SceneConfigurationItem::qmlAppendAnimation,
                                                    SceneConfigurationItem::qmlAnimationCount,
                                                    SceneConfigurationItem::qmlAnimationAt,
                                                    SceneConfigurationItem::qmlClearAnimations);
}

void SceneConfigurationItem::qmlAppendAnimation(QQmlListProperty<Kuesa::AnimationPlayer> *list, Kuesa::AnimationPlayer *node)
{
    if (node == nullptr)
        return;
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    self->m_managedAnimations.push_back(node);
    self->parentSceneConfiguration()->addAnimationPlayer(node);
}

Kuesa::AnimationPlayer *SceneConfigurationItem::qmlAnimationAt(QQmlListProperty<Kuesa::AnimationPlayer> *list, qt_size_type index)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    return self->m_managedAnimations.at(size_t(index));
}

qt_size_type SceneConfigurationItem::qmlAnimationCount(QQmlListProperty<Kuesa::AnimationPlayer> *list)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    return int(self->m_managedAnimations.size());
}

void SceneConfigurationItem::qmlClearAnimations(QQmlListProperty<Kuesa::AnimationPlayer> *list)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    self->parentSceneConfiguration()->clearAnimationPlayers();
    self->m_managedAnimations.clear();
}

QQmlListProperty<Kuesa::TransformTracker> SceneConfigurationItem::transformTrackers()
{
    return QQmlListProperty<Kuesa::TransformTracker>(this, nullptr,
                                                     SceneConfigurationItem::qmlAppendTrackers,
                                                     SceneConfigurationItem::qmlTrackersCount,
                                                     SceneConfigurationItem::qmlTrackersAt,
                                                     SceneConfigurationItem::qmlClearTrackers);
}

void SceneConfigurationItem::qmlAppendTrackers(QQmlListProperty<Kuesa::TransformTracker> *list, Kuesa::TransformTracker *node)
{
    if (node == nullptr)
        return;
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    self->m_managedTrackers.push_back(node);
    self->parentSceneConfiguration()->addTransformTracker(node);
}

Kuesa::TransformTracker *SceneConfigurationItem::qmlTrackersAt(QQmlListProperty<Kuesa::TransformTracker> *list, qt_size_type index)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    return self->m_managedTrackers.at(size_t(index));
}

qt_size_type SceneConfigurationItem::qmlTrackersCount(QQmlListProperty<Kuesa::TransformTracker> *list)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    return int(self->m_managedTrackers.size());
}

void SceneConfigurationItem::qmlClearTrackers(QQmlListProperty<Kuesa::TransformTracker> *list)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    self->parentSceneConfiguration()->clearAnimationPlayers();
    self->m_managedTrackers.clear();
}
