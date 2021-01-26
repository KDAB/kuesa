/*
    sceneconfigurationitem.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

/*!
 * \class KuesaUtils::SceneConfigurationItem
 * \internal
 */

/*!
    \qmlproperty list<Kuesa::AnimationPlayer> KuesaUtils::SceneConfigurationItem::animations

    List of \l [QML] {Kuesa::AnimationPlayer} instances referenced by the
    SceneConfiguration.
*/

/*!
    \qmlproperty list<Kuesa::TransformTracker> KuesaUtils::SceneConfigurationItem::transformTrackers

    List of \l [QML] {Kuesa::TransformTracker} instances referenced by the
    SceneConfiguration.
*/


/*!
    \qmlproperty list<Kuesa::PlaceholderTracker> KuesaUtils::SceneConfigurationItem::placeholders

    List of \l [QML] {Kuesa::PlaceholderTracker} instances referenced by the
    SceneConfiguration.
*/


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
                                                     SceneConfigurationItem::qmlAppendTransformTrackers,
                                                     SceneConfigurationItem::qmlTransformTrackersCount,
                                                     SceneConfigurationItem::qmlTransformTrackersAt,
                                                     SceneConfigurationItem::qmlClearTransformTrackers);
}

QQmlListProperty<Kuesa::PlaceholderTracker> SceneConfigurationItem::placeholderTrackers()
{
    return QQmlListProperty<Kuesa::PlaceholderTracker>(this, nullptr,
                                                       SceneConfigurationItem::qmlAppendPlaceholderTracker,
                                                       SceneConfigurationItem::qmlPlaceholderTrackersCount,
                                                       SceneConfigurationItem::qmlPlaceholderTrackerAt,
                                                       SceneConfigurationItem::qmlClearPlaceholderTrackers);
}

void SceneConfigurationItem::qmlAppendTransformTrackers(QQmlListProperty<Kuesa::TransformTracker> *list, Kuesa::TransformTracker *node)
{
    if (node == nullptr)
        return;
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    self->m_managedTransformTrackers.push_back(node);
    self->parentSceneConfiguration()->addTransformTracker(node);
}

Kuesa::TransformTracker *SceneConfigurationItem::qmlTransformTrackersAt(QQmlListProperty<Kuesa::TransformTracker> *list, qt_size_type index)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    return self->m_managedTransformTrackers.at(size_t(index));
}

qt_size_type SceneConfigurationItem::qmlTransformTrackersCount(QQmlListProperty<Kuesa::TransformTracker> *list)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    return int(self->m_managedTransformTrackers.size());
}

void SceneConfigurationItem::qmlClearTransformTrackers(QQmlListProperty<Kuesa::TransformTracker> *list)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    self->parentSceneConfiguration()->clearAnimationPlayers();
    self->m_managedTransformTrackers.clear();
}

void SceneConfigurationItem::qmlAppendPlaceholderTracker(QQmlListProperty<Kuesa::PlaceholderTracker> *list, Kuesa::PlaceholderTracker *node)
{
    if (node == nullptr)
        return;
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    self->m_managedPlaceholderTrackers.push_back(node);
    self->parentSceneConfiguration()->addPlaceholderTracker(node);
}

Kuesa::PlaceholderTracker *SceneConfigurationItem::qmlPlaceholderTrackerAt(QQmlListProperty<Kuesa::PlaceholderTracker> *list, qt_size_type index)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    return self->m_managedPlaceholderTrackers.at(size_t(index));
}

qt_size_type SceneConfigurationItem::qmlPlaceholderTrackersCount(QQmlListProperty<Kuesa::PlaceholderTracker> *list)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    return int(self->m_managedPlaceholderTrackers.size());
}

void SceneConfigurationItem::qmlClearPlaceholderTrackers(QQmlListProperty<Kuesa::PlaceholderTracker> *list)
{
    SceneConfigurationItem *self = static_cast<SceneConfigurationItem *>(list->object);
    self->parentSceneConfiguration()->clearPlaceholderTrackers();
    self->m_managedPlaceholderTrackers.clear();
}
