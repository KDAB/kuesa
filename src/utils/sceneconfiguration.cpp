/*
    sceneconfiguration.cpp

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

#include "sceneconfiguration.h"

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

using namespace Kuesa;
using namespace KuesaUtils;

SceneConfiguration::SceneConfiguration(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
{
}

QUrl SceneConfiguration::source() const
{
    return m_source;
}

void SceneConfiguration::setSource(const QUrl &source)
{
    if (m_source != source) {
        m_source = source;
        emit sourceChanged(m_source);
    }
}

QString SceneConfiguration::cameraName() const
{
    return m_cameraName;
}

void SceneConfiguration::setCameraName(const QString &cameraName)
{
    if (cameraName != m_cameraName) {
        m_cameraName = cameraName;
        emit cameraNameChanged(m_cameraName);
    }
}

void SceneConfiguration::addTransformTracker(TransformTracker *tracker)
{
    if (std::find(std::begin(m_trackers), std::end(m_trackers), tracker) == std::end(m_trackers)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(tracker, &SceneConfiguration::removeTransformTracker, tracker);
        if (tracker->parentNode() == nullptr)
            tracker->setParent(this);
        m_trackers.push_back(tracker);
    }
}

void SceneConfiguration::removeTransformTracker(TransformTracker *tracker)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(tracker);
    m_trackers.erase(std::remove(std::begin(m_trackers), std::end(m_trackers), tracker),
                     std::end(m_trackers));
}

void SceneConfiguration::clearTransformTrackers()
{
    m_trackers.clear();
}

void SceneConfiguration::addAnimationPlayer(AnimationPlayer *animation)
{
    if (std::find(std::begin(m_animations), std::end(m_animations), animation) == std::end(m_animations)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(animation, &SceneConfiguration::removeAnimationPlayer, animation);
        if (animation->parentNode() == nullptr)
            animation->setParent(this);
        m_animations.push_back(animation);
    }
}

void SceneConfiguration::removeAnimationPlayer(AnimationPlayer *animation)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(animation);
    m_animations.erase(std::remove_if(std::begin(m_animations), std::end(m_animations), [animation](AnimationPlayer *a) {
                           return a == animation;
                       }),
                       std::end(m_animations));
}

void SceneConfiguration::clearAnimationPlayers()
{
    m_animations.clear();
}

QT_END_NAMESPACE
