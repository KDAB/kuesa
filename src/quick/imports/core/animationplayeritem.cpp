/*
    animationplayeritem.cpp

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

#include "animationplayeritem.h"

QT_USE_NAMESPACE
using namespace Kuesa;

AnimationPlayerItem::AnimationPlayerItem(QObject *parent)
    : QObject(parent)
{
    Q_ASSERT(parentAnimationPlayer());
    connect(parentAnimationPlayer(), &AnimationPlayer::clipChanged, this, &AnimationPlayerItem::nameChanged);
}

AnimationPlayerItem::~AnimationPlayerItem() = default;

QString AnimationPlayerItem::name() const
{
    return parentAnimationPlayer()->clip();
}

void AnimationPlayerItem::setName(const QString &name)
{
    parentAnimationPlayer()->setClip(name);
}

QQmlListProperty<Qt3DCore::QNode> AnimationPlayerItem::targetList()
{
    return QQmlListProperty<Qt3DCore::QNode>(this, nullptr,
                                             AnimationPlayerItem::qmlAppendTarget,
                                             AnimationPlayerItem::qmlTargetCount,
                                             AnimationPlayerItem::qmlTargetAt,
                                             AnimationPlayerItem::qmlClearTargets);
}

void AnimationPlayerItem::qmlAppendTarget(QQmlListProperty<Qt3DCore::QNode> *list, Qt3DCore::QNode *node)
{
    if (node == nullptr)
        return;
    AnimationPlayerItem *self = static_cast<AnimationPlayerItem *>(list->object);
    self->m_managedTargets.push_back(node);
    self->parentAnimationPlayer()->addTarget(node);
}

Qt3DCore::QNode *AnimationPlayerItem::qmlTargetAt(QQmlListProperty<Qt3DCore::QNode> *list, qt_size_type index)
{
    AnimationPlayerItem *self = static_cast<AnimationPlayerItem *>(list->object);
    return self->parentAnimationPlayer()->targets().at(index);
}

qt_size_type AnimationPlayerItem::qmlTargetCount(QQmlListProperty<Qt3DCore::QNode> *list)
{
    AnimationPlayerItem *self = static_cast<AnimationPlayerItem *>(list->object);
    return self->parentAnimationPlayer()->targets().size();
}

void AnimationPlayerItem::qmlClearTargets(QQmlListProperty<Qt3DCore::QNode> *list)
{
    AnimationPlayerItem *self = static_cast<AnimationPlayerItem *>(list->object);
    for (Qt3DCore::QNode *target : qAsConst(self->m_managedTargets))
        self->parentAnimationPlayer()->removeTarget(target);
    self->m_managedTargets.clear();
}
