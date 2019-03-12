/*
    animationplayer.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "animationplayer.h"

#include "sceneentity.h"
#include "kuesa_p.h"

#include <Qt3DAnimation/QClipAnimator>
#include <Qt3DAnimation/QChannelMapping>
#include <Qt3DAnimation/QAnimationClip>
#include <Qt3DAnimation/QClock>

QT_USE_NAMESPACE
using namespace Kuesa;
using namespace Qt3DAnimation;

/*!
 * \class Kuesa::AnimationPlayer
 * \inheaderfile Kuesa/AnimationPlayer
 * \inmodule Kuesa
 * \since 1.0
 * \brief Play animations defined in glTF files
 *
 * AnimationPlayer is a utility class designed to easily control animations
 * where the clip and mapping data is stored in a collections (typically loaded
 * from glTF files).
 *
 * Clip and mapper data are referenced by name. These will be used to look up
 * actual assets in the appropriate collection.
 *
 * AnimationPlayer internally uses an instance of Qt3DAnimation::QClipAnimator
 * and mirrors it's api.
 *
 * Additionally, AnimationPlayer can be used to map clip data to other target objects
 * than those specified in a mapper. For example, an animation where the clip
 * data affect transformation properties, can be applied to any Qt3DCore::QTransform
 * instance, not just the one specified in the mapper data.
 */

/*!
 * \qmltype AnimationPlayer
 * \inqmlmodule Kuesa
 * \since 1.0
 * \instantiates Kuesa::AnimationPlayer
 * \brief Play animations defined in glTF files
 *
 * AnimationPlayer is a utility class designed to easily control animations
 * where the clip and mapping data is stored in a collections (typically loaded
 * from glTF files).
 *
 * Clip and mapper data are referenced by name. These will be used to look up
 * actual assets in the appropriate collection.
 *
 * AnimationPlayer internally uses an instance of Qt3DAnimation::QClipAnimator
 * and mirrors it's api.
 *
 * Additionally, AnimationPlayer can be used to map clip data to other target objects
 * than those specified in a mapper. For example, an animation where the clip
 * data affect transformation properties, can be applied to any Qt3DCore::QTransform
 * instance, not just the one specified in the mapper data.
*/

/*!
    \enum AnimationPlayer::Status

    This enum type describes state of the player.

    \value None  Unknown state (default).
    \value Ready  Both clip and mapper data have been found and are valid (included target nodes
                    if they have been defined).
    \value Error  An error occurred when looking for assets or trying to match clip and mapper properties.
*/

/*!
    \property AnimationPlayer::sceneEntity
    \brief pointer to the SceneEntity which contains the collections used to look up clip and mapper data

    \sa GLTF2Importer::sceneEntity()
 */

/*!
    \qmlproperty Entity AnimationPlayer::sceneEntity
    \brief pointer to the SceneEntity which contains the collections used to look up clip and mapper data
 */

/*!
    \property AnimationPlayer::status
    \brief the current status of the player

    \sa GLTF2Importer::status()
 */

/*!
    \qmlproperty enumeration AnimationPlayer::status
    \brief the current status of the player

    \list
    \li None   Unknown state (default).
    \li Ready  Both clip and mapper data have been found and are valid (included target nodes
               if they have been defined).
    \li Error  An error occurred when looking for assets or trying to match clip and mapper properties.
    \endlist
 */

/*!
    \property AnimationPlayer::clip
    \brief the name of the clip asset.

    This will be used to lookup the asset in the AnimationClipCollection instance of the SceneEntity.

    \sa GLTF2Importer::clip()
 */

/*!
    \qmlproperty string AnimationPlayer::clip
    \brief the name of the clip asset.

    This will be used to lookup the asset in the AnimationClipCollection instance of the SceneEntity.
 */

/*!
    \property AnimationPlayer::mapper
    \brief the name of the mapper asset.

    This will be used to lookup the asset in the AnimationMappingCollection instance of the SceneEntity.

    If empty (default), the name of the clip will be used as it is a common convention that they should match.

    \sa GLTF2Importer::mapper()
 */

/*!
    \qmlproperty string AnimationPlayer::mapper
    \brief the name of the mapper asset.

    This will be used to lookup the asset in the AnimationMappingCollection instance of the SceneEntity.

    If empty (default), the name of the clip will be used as it is a common convention that they should match.
 */

/*!
    \property AnimationPlayer::running
    \brief controls if the animation is running or not

    This reflects the state of the internal Qt3DAnimation::QClipAnimator instance.

    \sa GLTF2Importer::isRunning()
 */

/*!
    \qmlproperty bool AnimationPlayer::running
    \brief controls if the animation is running or not

    This reflects the state of the internal Qt3DAnimation::QClipAnimator instance.
 */

/*!
    \property AnimationPlayer::loopCount
    \brief controls the number of time the animation should repeat.

    If the value is 0 (default), the animation will run only once.

    If the value is AnimationPlayer::Infinite, the animation will loop indefinitly until explicitly stopped.

    Otherwise, it will repeat the specified number of times.

    This reflects the state of the internal Qt3DAnimation::QClipAnimator instance.

    \sa GLTF2Importer::loopCount()
 */

/*!
    \qmlproperty int AnimationPlayer::loopCount
    \brief controls the number of time the animation should repeat.

    If the value is 0 (default), the animation will run only once.

    If the value is AnimationPlayer::Infinite, the animation will loop indefinitly until explicitly stopped.

    Otherwise, it will repeat the specified number of times.

    This reflects the state of the internal Qt3DAnimation::QClipAnimator instance.
 */

/*!
    \property AnimationPlayer::clock
    \brief intense of Qt3DAnimation::QClock used to control animation speed and direction

    A Qt3DAnimation::QClock can be used to control the speed (by setting the playBackRate) and
    direction (like reversing by setting negative playBackRate values) of the animation.

    This reflects the state of the internal Qt3DAnimation::QClipAnimator instance.

    \sa GLTF2Importer::clock()
 */

/*!
    \qmlproperty Clock AnimationPlayer::clock
    \brief intense of Qt3DAnimation::QClock used to control animation speed and direction

    A Qt3DAnimation::QClock can be used to control the speed (by setting the playBackRate) and
    direction (like reversing by setting negative playBackRate values) of the animation.

    This reflects the state of the internal Qt3DAnimation::QClipAnimator instance.
 */

/*!
    \property AnimationPlayer::normalizedTime
    \brief progress of the animation as a uniform value between 0. and 1.

    This reflects the state of the internal Qt3DAnimation::QClipAnimator instance.

    \sa GLTF2Importer::normalizedTime()
 */

/*!
    \qmlproperty float AnimationPlayer::normalizedTime
    \brief progress of the animation as a uniform value between 0. and 1.

    This reflects the state of the internal Qt3DAnimation::QClipAnimator instance.
 */

AnimationPlayer::AnimationPlayer(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
    , m_sceneEntity(nullptr)
    , m_status(None)
    , m_animator(new Qt3DAnimation::QClipAnimator(this))
    , m_running(false)
{
    updateSceneFromParent(parent);
    connect(this, &Qt3DCore::QNode::parentChanged, this, [this](QObject *parent) {
        auto parentNode = qobject_cast<Qt3DCore::QNode *>(parent);
        this->updateSceneFromParent(parentNode);
    });

    connect(m_animator, &QClipAnimator::runningChanged, this, &AnimationPlayer::runningChanged);
    connect(m_animator, &QClipAnimator::loopCountChanged, this, &AnimationPlayer::loopCountChanged);
    connect(m_animator, &QClipAnimator::clockChanged, this, &AnimationPlayer::clockChanged);
    connect(m_animator, &QClipAnimator::normalizedTimeChanged, this, &AnimationPlayer::normalizedTime);
}

AnimationPlayer::~AnimationPlayer()
{
}

AnimationPlayer::Status AnimationPlayer::status() const
{
    return m_status;
}

void AnimationPlayer::setStatus(AnimationPlayer::Status status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged(m_status);
    }
}

void AnimationPlayer::updateSceneFromParent(Qt3DCore::QNode *parent)
{
    if (m_sceneEntity)
        return;

    while (parent) {
        auto scene = qobject_cast<SceneEntity *>(parent);
        if (scene) {
            setSceneEntity(scene);
            break;
        }
        parent = parent->parentNode();
    }
}

SceneEntity *AnimationPlayer::sceneEntity() const
{
    return m_sceneEntity;
}

void AnimationPlayer::setSceneEntity(SceneEntity *sceneEntity)
{
    if (sceneEntity == m_sceneEntity)
        return;

    if (m_sceneEntity)
        disconnect(m_sceneEntity, &SceneEntity::loadingDone, this, &AnimationPlayer::matchClipAndTargets);

    m_sceneEntity = sceneEntity;
    connect(m_sceneEntity, &SceneEntity::loadingDone, this, &AnimationPlayer::matchClipAndTargets);

    emit sceneEntityChanged(sceneEntity);
    matchClipAndTargets();
}

QString AnimationPlayer::clip() const
{
    return m_clip;
}

void AnimationPlayer::setClip(const QString &clip)
{
    if (m_clip == clip)
        return;

    m_clip = clip;
    emit clipChanged(m_clip);
    matchClipAndTargets();
}

QString AnimationPlayer::mapper() const
{
    return m_mapper;
}

void AnimationPlayer::setMapper(const QString &mapper)
{
    if (mapper == m_mapper)
        return;

    m_mapper = mapper;
    emit mapperChanged(m_mapper);
    matchClipAndTargets();
}

/*!
 * \brief AnimationPlayer::targets returns the list of currently defined targets for animations
 *
 * \sa AnimationPlayer::addTarget, AnimationPlayer::removeTarget
 */
QVector<Qt3DCore::QNode *> AnimationPlayer::targets() const
{
    return m_targets;
}

/*!
 * \brief AnimationPlayer::addTarget adds a node as a target for the animation
 *
 * If targets are added to the list, the mapping defined by the AnimationPlayer::mapper
 * property will be ignored and the clip data will be mapped to the specified target nodes.
 *
 * The target node needs to have properties which match the ones defined by the clips.
 *
 * \note When specifying targets, a valid object matching the specified AnimationPlayer::mapper
 * property must exist (even though it won't be affected by the animation) as it will contain
 * information required to establish valid mappings for the new targets.
 *
 * \note Kuesa::SceneEntity::transformForEntity can be useful to map animations that control
 * transformations from one entity to another. The returned transform can be used as a
 * target for the animation.
 *
 * \sa AnimationPlayer::removeTarget
 */
void AnimationPlayer::addTarget(Qt3DCore::QNode *target)
{
    if (m_targets.contains(target))
        return;

    m_targets.push_back(target);
    matchClipAndTargets();
}

/*!
 * \brief AnimationPlayer::removeTarget removes a node from the list of targets
 *
 * \sa AnimationPlayer::addTarget
 */
void AnimationPlayer::removeTarget(Qt3DCore::QNode *target)
{
    if (!m_targets.contains(target))
        return;

    m_targets.removeOne(target);
    matchClipAndTargets();
}

bool AnimationPlayer::isRunning() const
{
    return m_animator->isRunning();
}

void AnimationPlayer::setRunning(bool running)
{
    m_animator->setRunning(running);
    m_running = running;
}

int AnimationPlayer::loopCount() const
{
    return m_animator->loopCount();
}

void AnimationPlayer::setLoopCount(int loops)
{
    m_animator->setLoopCount(loops);
}

QClock *AnimationPlayer::clock() const
{
    return m_animator->clock();
}

void AnimationPlayer::setClock(QClock *clock)
{
    m_animator->setClock(clock);
}

float AnimationPlayer::normalizedTime() const
{
    return m_animator->normalizedTime();
}

void AnimationPlayer::setNormalizedTime(float timeFraction)
{
    m_animator->setNormalizedTime(timeFraction);
}

/*!
 * \brief Starts the animation
 */
void AnimationPlayer::start()
{
    m_animator->start();
}

/*!
 * \brief Stops the animation
 */
void AnimationPlayer::stop()
{
    m_animator->stop();
}

void AnimationPlayer::matchClipAndTargets()
{
    if (m_sceneEntity == nullptr) {
        setStatus(Error);
        return;
    }

    if (m_clip.isEmpty() && m_mapper.isEmpty()) {
        setStatus(Error);
        return;
    }

    Qt3DAnimation::QAnimationClip *clip = qobject_cast<Qt3DAnimation::QAnimationClip *>(m_sceneEntity->animationClip(m_clip));
    Qt3DAnimation::QChannelMapper *mapper = m_sceneEntity->animationMapping(m_mapper.isEmpty() ? m_clip : m_mapper);

    if (!clip || !mapper) {
        qCWarning(kuesa, "Undefined clip or mapper in AnimationPlayer");
        setStatus(Error);
        return;
    }

    const QVector<QAbstractChannelMapping *> mappings = mapper->mappings();
    const auto numMappings = mappings.size();

    // Check that the mapping is using a channel name that exists in the clip data
    for (int mappingId = 0; mappingId < mappings.size(); ++mappingId) {
        // mappings contains either QChannelMappings or QSkeletonMappings
        Qt3DAnimation::QChannelMapping *mapping = qobject_cast<Qt3DAnimation::QChannelMapping *>(mappings.at(mappingId));

        if (mapping != nullptr) {
            // Verify the channel name matches in clip and mapper
            const QString mappingChannel = mapping->channelName();
            const auto it = std::find_if(clip->clipData().begin(), clip->clipData().end(), [mappingChannel](const Qt3DAnimation::QChannel &value) {
                return (value.name() == mappingChannel);
            });
            if (it == clip->clipData().end()) {
                setStatus(Error);
                qCWarning(kuesa, "Mapped property %i does not match any clip", mappingId);
                return;
            }
        }
    }

    m_animator->setClip(clip);

    if (m_targets.isEmpty()) {
        m_animator->setChannelMapper(mapper);
    } else {
        // If we have different number of targets than of mapping, disable, since we can't know which target we want for each mapping
        if (m_targets.size() != numMappings) {
            setStatus(Error);
            qCWarning(kuesa, "Number of targets and mappings need to match");
            return;
        }

        // Check that the mappings try to animate a valid property of the target
        for (int mappingId = 0; mappingId < mappings.size(); ++mappingId) {
            Qt3DAnimation::QChannelMapping *mapping = qobject_cast<Qt3DAnimation::QChannelMapping *>(mappings.at(mappingId));
            Qt3DCore::QNode *targetNode = m_targets.at(mappingId);

            // Verify the target node has the property the mapping is animating
            if (mapping != nullptr && !targetNode->property(mapping->property().toStdString().c_str()).isValid()) {
                setStatus(Error);
                qCWarning(kuesa, "Mapped property %i does not match any property on target node", mappingId);
                return;
            }
        }

        // If everything matches and we can animate the targets using the mapping and the clip,
        // create a new mapper to use those targets
        Qt3DAnimation::QChannelMapper *newMapper = new Qt3DAnimation::QChannelMapper;
        for (int mappingId = 0; mappingId < mappings.size(); ++mappingId) {
            Qt3DAnimation::QChannelMapping *oldMapping = qobject_cast<Qt3DAnimation::QChannelMapping *>(mappings.at(mappingId));
            if (!oldMapping)
                continue;
            Qt3DAnimation::QChannelMapping *newMapping = new Qt3DAnimation::QChannelMapping;
            newMapping->setChannelName(oldMapping->channelName());
            newMapping->setProperty(oldMapping->property());
            newMapping->setTarget(m_targets.at(mappingId));
            newMapper->addMapping(newMapping);
        }

        m_animator->setChannelMapper(newMapper);
    }

    m_animator->setRunning(m_running);
    setStatus(Ready);
}
