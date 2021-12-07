/*
    steppedanimationplayer.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include "steppedanimationplayer.h"

/*!
 * \class Kuesa::SteppedAnimationPlayer
 * \inheaderfile Kuesa/SteppedAnimationPlayer
 * \inherits Kuesa::AnimationPulse
 * \inmodule Kuesa
 * \since Kuesa 1.4
 * \brief SteppedAnimationPlayer is an utility class that automatically stops a set of
 * animations when the "up" signal of an AnimationPulse is emitted.
 *
 * Given a list of animation names, it will create a Kuesa::AnimationPlayer for
 * each. The running state of each of these Kuesa::AnimationPlayer will match
 * the running state of the Kuesa::SteppedAnimationPlayer. Therefore, when the
 * Kuesa::SteppedAnimationPlayer starts, all the animations will start.
 *
 * When the "up" signal of the Kuesa::SteppedAnimationPlayer is triggered,
 * the Kuesa::SteppedAnimationPlayer and all the animations will stop.
 *
 */

/*!
    \qmltype SteppedAnimationPlayer
    \inherits Kuesa::SteppedAnimationPlayer
    \inqmlmodule Kuesa
    \since Kuesa 1.4
    \instantiates Kuesa::SteppedAnimationPlayer
    \brief SteppedAnimationPlayer is an utility class that automatically stops a set of
    animations when the "up" signal of an AnimationPulse is emitted.

    Given a list of animation names, it will create a Kuesa::AnimationPlayer for
    each. The running state of each of these Kuesa::AnimationPlayer will match
    the running state of the Kuesa::SteppedAnimationPlayer. Therefore, when the
    Kuesa::SteppedAnimationPlayer starts, all the animations will start.

    When the "up" signal of the Kuesa::SteppedAnimationPlayer is triggered,
    the Kuesa::SteppedAnimationPlayer and all the animations will stop.
 */

QT_BEGIN_NAMESPACE

namespace Kuesa {
SteppedAnimationPlayer::SteppedAnimationPlayer(Qt3DCore::QNode *parent)
    : Kuesa::AnimationPulse(parent)
{
    QObject::connect(this, &SteppedAnimationPlayer::animationNamesChanged,
                     this, &SteppedAnimationPlayer::updateAnimationPlayers);

    QObject::connect(this, &SteppedAnimationPlayer::up,
                     this, &SteppedAnimationPlayer::stop);
}

/*!
    \property Kuesa::SteppedAnimationPlayer::animationNames

    Holds the names of the animations that are controlled by this
    Kuesa::SteppedAnimationPlayer
 */

/*!
    \qmlproperty Kuesa::SteppedAnimationPlayer::animationNames

    Holds the names of the animations that are controlled by this
    Kuesa::SteppedAnimationPlayer
 */
QStringList SteppedAnimationPlayer::animationNames() const
{
    return m_animationNames;
}

void SteppedAnimationPlayer::setAnimationNames(const QStringList &animationNames)
{
    if (m_animationNames != animationNames) {
        m_animationNames = animationNames;
        emit animationNamesChanged();
    }
}

void SteppedAnimationPlayer::updateAnimationPlayers()
{
    // When the animationNames changed, we update all the animationPlayers
    // If the animations in the animations collection change, we don't need to update again,
    // as each animationPlayer will be updated independently
    for (AnimationPlayer *aPlayer : m_animationPlayers)
        delete aPlayer;
    m_animationPlayers.clear();

    m_animationPlayers.reserve(m_animationNames.size());
    std::transform(m_animationNames.begin(), m_animationNames.end(), std::back_inserter(m_animationPlayers), [this](const QString &name) {
        AnimationPlayer *aPlayer = new AnimationPlayer(this);
        aPlayer->setClip(name);

        // set properties
        aPlayer->setSceneEntity(this->sceneEntity());
        aPlayer->setRunning(this->isRunning());
        aPlayer->setLoopCount(this->loopCount());
        aPlayer->setClock(this->clock());
        aPlayer->setNormalizedTime(this->normalizedTime());

        QObject::connect(this, &Kuesa::KuesaNode::sceneEntityChanged, aPlayer, &Kuesa::KuesaNode::setSceneEntity);
        QObject::connect(this, &Kuesa::AnimationPlayer::runningChanged, aPlayer, &Kuesa::AnimationPlayer::setRunning);
        QObject::connect(this, &Kuesa::AnimationPlayer::loopCountChanged, aPlayer, &Kuesa::AnimationPlayer::setLoopCount);
        QObject::connect(this, &Kuesa::AnimationPlayer::clockChanged, aPlayer, &Kuesa::AnimationPlayer::setClock);
        QObject::connect(this, &Kuesa::AnimationPlayer::normalizedTimeChanged, aPlayer, &Kuesa::AnimationPlayer::setNormalizedTime);

        return aPlayer;
    });
}

} // namespace Kuesa

QT_END_NAMESPACE
