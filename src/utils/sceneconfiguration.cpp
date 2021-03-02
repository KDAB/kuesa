/*
    sceneconfiguration.cpp

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

#include "sceneconfiguration.h"

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

using namespace Kuesa;
using namespace KuesaUtils;

/*!
    \class KuesaUtils::SceneConfiguration
    \brief SceneConfiguration provides a way of conveniently specifying
    information about a glTF2 file and accompanying helpers to be loaded.

    \inmodule Kuesa
    \since Kuesa 1.3
    \inherits Kuesa::KuesaNode

    SceneConfiguration provides a way of conveniently specifying
    information about a glTF2 file and accompanying helpers to be loaded.

    When building an application that should display different scenes at
    different moments in time, recreating a new Qt 3D view with its FrameGraph,
    \l {Kuesa::GLTF2Importer} and other assets can be costly and unnecessary.
    Yet, that approach is still often favored as it can be seen as more
    convenient for the developer from an architecture point of view.

    The \l {KuesaUtils::SceneConfiguration} used in conjunction with \l
    {KuesaUtils::View3DScene} provides a new way to architecture such type of
    applications. The idea is to instantiate the View3DScene once and use its
    \l {KuesaUtils::View3DScene::activeScene} property to point to different \l
    {KuesaUtils::SceneConfiguration} instances. Internally, when the active
    scene is changed, the new glTF2 file will be loaded and assets needed
    created while previous glTF2 file and assets unloaded.

    The signals \l {KuesaUtils::SceneConfiguration::loadingDone} and \l
    {KuesaUtils::SceneConfiguration::unloadingDone} can be used to watch which
    state the SceneConfiguration is in and react accordingly.
*/

/*!
    \property KuesaUtils::SceneConfiguration::source

    \brief The source of the glTF file to be loaded.
 */

/*!
    \property KuesaUtils::SceneConfiguration::cameraName

    \brief The name of the camera asset that should be used to view the scene.
    If the name references a valid camera, the camera will automatically be
    set on the ForwardRenderer frameGraph and other internal assets such as
    \l {Kuesa::TransformTracker}.
 */

/*!
    \property KuesaUtils::SceneConfiguration::layerNames

    \brief The list of KDAB_layers layer names to be selected for rendering.
    This allows selecting only a sub part of a glTF 2 model to only render parts
    that are linked to a specific layer.
 */

/*!
    \qmltype SceneConfiguration
    \instantiates KuesaUtils::SceneConfiguration
    \inqmlmodule KuesaUtils
    \since Kuesa 1.3
    \inherits Kuesa::KuesaNode

    \brief SceneConfiguration provides a way of conveniently specifying
    information about a glTF2 file and accompanying helpers to be loaded.

    SceneConfiguration provides a way of conveniently specifying
    information about a glTF2 file and accompanying helpers to be loaded.

    When building an application that should display different scenes at
    different moments in time, recreating a new Qt 3D view with its FrameGraph,
    \l [QML] {Kuesa::GLTF2Importer} and other assets can be costly and
    unnecessary. Yet, that approach is still often favored as it can be seen as
    more convenient for the developer from an architecture point of view.

    The \l [QML] {KuesaUtils::SceneConfiguration} used in conjunction with \l
    [QML] {KuesaUtils::View3DScene} provides a new way to architecture such
    type of applications. The idea is to instantiate the View3D once and use
    its \l [QML] {KuesaUtils::View3DScene::activeScene} property to point to
    different \l [QML] {KuesaUtils::SceneConfiguration} instances. Internally,
    when the active scene is changed, the new glTF2 file will be loaded and
    assets needed created while previous glTF2 file and assets unloaded.

    The signals \l {KuesaUtils::SceneConfiguration::loadingDone} and \l
    {KuesaUtils::SceneConfiguration::unloadingDone} can be used to watch which
    state the SceneConfiguration is in and react accordingly.
*/

/*!
    \qmlproperty url KuesaUtils::SceneConfiguration::source

    \brief The source of the glTF file to be loaded.
 */

/*!
    \qmlproperty string KuesaUtils::SceneConfiguration::cameraName

    \brief The name of the camera asset that should be used to view the scene.
    If the name references a valid camera, the camera will automatically be
    set on the ForwardRenderer frameGraph and other internal assets such as
    \l [QML] {Kuesa::TransformTracker}.
 */

/*!
    \qmlproperty list<string> KuesaUtils::SceneConfiguration::layerNames

    \brief The list of KDAB_layers layer names to be selected for rendering.
    This allows selecting only a sub part of a glTF 2 model to only render parts
    that are linked to a specific layer.
 */

SceneConfiguration::SceneConfiguration(Qt3DCore::QNode *parent)
    : KuesaNode(parent)
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

QStringList SceneConfiguration::layerNames() const
{
    return m_layerNames;
}

void SceneConfiguration::setCameraName(const QString &cameraName)
{
    if (cameraName != m_cameraName) {
        m_cameraName = cameraName;
        emit cameraNameChanged(m_cameraName);
    }
}

void SceneConfiguration::setLayerNames(const QStringList &layerNames)
{
    if (layerNames == m_layerNames)
        return;
    m_layerNames = layerNames;
    emit layerNamesChanged(m_layerNames);
}

void SceneConfiguration::addTransformTracker(TransformTracker *tracker)
{
    if (std::find(std::begin(m_trackers), std::end(m_trackers), tracker) == std::end(m_trackers)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(tracker, &SceneConfiguration::removeTransformTracker, m_trackers);
        if (tracker->parentNode() == nullptr)
            tracker->setParent(this);
        m_trackers.push_back(tracker);
        emit transformTrackerAdded(tracker);
    }
}

void SceneConfiguration::removeTransformTracker(TransformTracker *tracker)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(tracker);
    auto it = std::remove(std::begin(m_trackers), std::end(m_trackers), tracker);
    if (it != std::end(m_trackers)) {
        m_trackers.erase(it);
        emit transformTrackerRemoved(tracker);
    }
}

void SceneConfiguration::clearTransformTrackers()
{
    const std::vector<Kuesa::TransformTracker *> trackersCopy = m_trackers;
    for (Kuesa::TransformTracker *t : trackersCopy)
        removeTransformTracker(t);
}

void SceneConfiguration::addPlaceholderTracker(PlaceholderTracker *placeholder)
{
    if (std::find(std::begin(m_placeholderTrackers), std::end(m_placeholderTrackers), placeholder) == std::end(m_placeholderTrackers)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(placeholder, &SceneConfiguration::removePlaceholderTracker, m_placeholderTrackers);
        if (placeholder->parentNode() == nullptr)
            placeholder->setParent(this);
        m_placeholderTrackers.push_back(placeholder);
        emit placeholderTrackerAdded(placeholder);
    }
}

void SceneConfiguration::removePlaceholderTracker(PlaceholderTracker *placeholder)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(placeholder);
    auto it = std::remove(std::begin(m_placeholderTrackers), std::end(m_placeholderTrackers), placeholder);
    if (it != std::end(m_placeholderTrackers)) {
        m_placeholderTrackers.erase(it,
                                    std::end(m_placeholderTrackers));
        emit placeholderTrackerRemoved(placeholder);
    }
}

void SceneConfiguration::clearPlaceholderTrackers()
{
    const std::vector<Kuesa::PlaceholderTracker *> placeholdersCopy = m_placeholderTrackers;
    for (Kuesa::PlaceholderTracker *p : placeholdersCopy)
        removePlaceholderTracker(p);
}

void SceneConfiguration::addAnimationPlayer(AnimationPlayer *animation)
{
    if (std::find(std::begin(m_animations), std::end(m_animations), animation) == std::end(m_animations)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(animation, &SceneConfiguration::removeAnimationPlayer, m_animations);
        if (animation->parentNode() == nullptr)
            animation->setParent(this);
        m_animations.push_back(animation);
        emit animationPlayerAdded(animation);
    }
}

void SceneConfiguration::removeAnimationPlayer(AnimationPlayer *animation)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(animation);
    auto it = std::remove(std::begin(m_animations), std::end(m_animations), animation);
    if (it != std::end(m_animations)) {
        m_animations.erase(it,
                           std::end(m_animations));
        emit animationPlayerRemoved(animation);
    }
}

void SceneConfiguration::clearAnimationPlayers()
{
    const std::vector<Kuesa::AnimationPlayer *> animCopy = m_animations;
    for (Kuesa::AnimationPlayer *a : animCopy)
        removeAnimationPlayer(a);
}

QT_END_NAMESPACE
