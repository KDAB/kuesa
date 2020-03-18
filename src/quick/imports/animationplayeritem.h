/*
    animationplayeritem.h

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

#ifndef ANIMATIONPLAYERITEM_H
#define ANIMATIONPLAYERITEM_H

#include <QObject>
#include <QtQml/QQmlListProperty>
#include <Qt3DCore/QNode>
#include <Kuesa/AnimationPlayer>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class AnimationPlayerItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DCore::QNode> targets READ targetList CONSTANT)
public:
    AnimationPlayerItem(QObject *parent = nullptr);

    QQmlListProperty<Qt3DCore::QNode> targetList();

    AnimationPlayer *parentAnimationPlayer() const { return qobject_cast<AnimationPlayer *>(parent()); }

private:
    static void qmlAppendTarget(QQmlListProperty<Qt3DCore::QNode> *list, Qt3DCore::QNode *node);
    static Qt3DCore::QNode *qmlTargetAt(QQmlListProperty<Qt3DCore::QNode> *list, int index);
    static int qmlTargetCount(QQmlListProperty<Qt3DCore::QNode> *list);
    static void qmlClearTargets(QQmlListProperty<Qt3DCore::QNode> *list);

    QVector<Qt3DCore::QNode *> m_managedTargets;
};
} // namespace Kuesa

QT_END_NAMESPACE

#endif // ANIMATIONPLAYERITEM_H
