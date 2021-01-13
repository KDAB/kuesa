/*
    viewscene3ditem.cpp

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
    \readonly
 */
QQmlListProperty<Kuesa::AnimationPlayer> View3DSceneItem::animations()
{
    return QQmlListProperty<Kuesa::AnimationPlayer>(this, nullptr,
                                                    View3DSceneItem::qmlAnimationCount,
                                                    View3DSceneItem::qmlAnimationAt);
}

Kuesa::AnimationPlayer *View3DSceneItem::qmlAnimationAt(QQmlListProperty<Kuesa::AnimationPlayer> *list, qt_size_type index)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    return self->parent3DScene()->animationPlayers().at(size_t(index));
}

qt_size_type View3DSceneItem::qmlAnimationCount(QQmlListProperty<Kuesa::AnimationPlayer> *list)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    return int(self->parent3DScene()->animationPlayers().size());
}

/*!
    \qmlproperty list<Kuesa.TransformTracker> View3DScene::transformTrackers

    \brief Holds the list of \l {Kuesa.TransformTracker} instances referenced by
    the View3DScene.
    \readonly
 */
QQmlListProperty<Kuesa::TransformTracker> View3DSceneItem::transformTrackers()
{
    return QQmlListProperty<Kuesa::TransformTracker>(this, nullptr,
                                                     View3DSceneItem::qmlTrackersCount,
                                                     View3DSceneItem::qmlTrackersAt);
}

Kuesa::TransformTracker *View3DSceneItem::qmlTrackersAt(QQmlListProperty<Kuesa::TransformTracker> *list, qt_size_type index)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    return self->parent3DScene()->transformTrackers().at(size_t(index));
}

qt_size_type View3DSceneItem::qmlTrackersCount(QQmlListProperty<Kuesa::TransformTracker> *list)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    return int(self->parent3DScene()->transformTrackers().size());
}
