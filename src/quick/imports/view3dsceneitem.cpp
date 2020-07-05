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

Kuesa::AnimationPlayer *View3DSceneItem::qmlAnimationAt(QQmlListProperty<Kuesa::AnimationPlayer> *list, int index)
{
    View3DSceneItem *self = static_cast<View3DSceneItem *>(list->object);
    return self->m_managedAnimations.at(size_t(index));
}

int View3DSceneItem::qmlAnimationCount(QQmlListProperty<Kuesa::AnimationPlayer> *list)
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
