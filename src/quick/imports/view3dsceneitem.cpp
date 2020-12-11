/*
    viewscene3ditem.cpp

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

#include "view3dsceneitem.h"

using namespace KuesaUtils;

View3DSceneItem::View3DSceneItem(QObject *parent)
    : QObject(parent)
{
}

View3DSceneItem::~View3DSceneItem() = default;

/*!
    \qmlproperty list<Kuesa.AnimationPlayer> View3DScene::aninations

    \brief Holds the list of \l [QML] {Kuesa::AnimationPlayer} instances referenced by
    the View3DScene.
 */
QQmlListProperty<Kuesa::AnimationPlayer> View3DSceneItem::animations()
{
    return QQmlListProperty<Kuesa::AnimationPlayer>(this, nullptr,
                                                    View3DSceneItem::qmlAppendAnimation,
                                                    View3DSceneItem::qmlAnimationCount,
                                                    View3DSceneItem::qmlAnimationAt,
                                                    View3DSceneItem::qmlClearAnimations);
}

void View3DSceneItem::qmlAppendAnimation(QQmlListProperty<Kuesa::AnimationPlayer> *list, Kuesa::AnimationPlayer *node)
{
    if (node == nullptr)
        return;
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    self->m_managedAnimations.push_back(node);
    self->parent3DScene()->addAnimationPlayer(node);
}

Kuesa::AnimationPlayer *View3DSceneItem::qmlAnimationAt(QQmlListProperty<Kuesa::AnimationPlayer> *list, qt_size_type index)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    return self->m_managedAnimations.at(size_t(index));
}

qt_size_type View3DSceneItem::qmlAnimationCount(QQmlListProperty<Kuesa::AnimationPlayer> *list)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    return int(self->m_managedAnimations.size());
}

void View3DSceneItem::qmlClearAnimations(QQmlListProperty<Kuesa::AnimationPlayer> *list)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    self->parent3DScene()->clearAnimationPlayers();
    self->m_managedAnimations.clear();
}

/*!
    \qmlproperty list<Kuesa.TransformTracker> View3DScene::transformTrackers

    \brief Holds the list of \l {Kuesa.TransformTracker} instances referenced by
    the View3DScene.
 */
QQmlListProperty<Kuesa::TransformTracker> View3DSceneItem::transformTrackers()
{
    return QQmlListProperty<Kuesa::TransformTracker>(this, nullptr,
                                                     View3DSceneItem::qmlAppendTrackers,
                                                     View3DSceneItem::qmlTrackersCount,
                                                     View3DSceneItem::qmlTrackersAt,
                                                     View3DSceneItem::qmlClearTrackers);
}

void View3DSceneItem::qmlAppendTrackers(QQmlListProperty<Kuesa::TransformTracker> *list, Kuesa::TransformTracker *node)
{
    if (node == nullptr)
        return;
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    self->m_managedTrackers.push_back(node);
    self->parent3DScene()->addTransformTracker(node);
}

Kuesa::TransformTracker *View3DSceneItem::qmlTrackersAt(QQmlListProperty<Kuesa::TransformTracker> *list, qt_size_type index)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    return self->m_managedTrackers.at(size_t(index));
}

qt_size_type View3DSceneItem::qmlTrackersCount(QQmlListProperty<Kuesa::TransformTracker> *list)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    return int(self->m_managedTrackers.size());
}

void View3DSceneItem::qmlClearTrackers(QQmlListProperty<Kuesa::TransformTracker> *list)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    self->parent3DScene()->clearAnimationPlayers();
    self->m_managedTrackers.clear();
}
