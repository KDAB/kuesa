/*
    view3dscene.cpp

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

#include "view3dscene.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>

#include <Kuesa/sceneentity.h>
#include <Kuesa/forwardrenderer.h>
#include <Kuesa/gltf2importer.h>

#include <Kuesa/private/kuesa_utils_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qabstractnodefactory_p.h>
#include <Qt3DRender/qrendersettings.h>
#include <Qt3DInput/qinputsettings.h>

#include <Kuesa/Iro2PlanarReflectionEquiRectProperties>
#include <Kuesa/Iro2PlanarReflectionSemProperties>

using namespace Kuesa;
using namespace KuesaUtils;
using namespace Qt3DCore;
using namespace Qt3DRender;

/*!
    \class KuesaUtils::View3DScene
    \brief View3DScene is a convenience wrapper aiming at simplifying the set
    up of a 3D scene and the loading of a glTF 2 scene file.

    \inmodule Kuesa
    \since Kuesa 1.3
    \inherits Kuesa::SceneEntity

    View3DScene is a convenience wrapper aiming at simplifying the set
    up of a 3D scene and the loading of a glTF 2 scene file.

    It is a subclass of \l {Kuesa::SceneEntity} which grants access to the
    various asset collections. Aditionnaly it contains a \l
    {Kuesa::GLTF2Importer} and sets up a \l {Kuesa::ForwardRenderer} framegraph.

    The property \l {KuesaUtils::View3DScene::ready} and signal \l
    {View3DScene::readyChanged} can be used to detect when the scene is visible
    on screen and therefore synchronize with other aspect of your application.

    When used in conjunction with \l {KuesaUtils::SceneConfiguration} this
    class makes it convenient to set up a Kuesa renderer that can easily load
    and unload scenes.

    To set the scene to be loaded by the View3DScene, an instance of
    \l {KuesaUtils::SceneConfiguration} must be set as the activeScene property.

    \l {Kuesa::AnimationPlayer} and \l {Kuesa::TransformTracker} instances
    defined on the activeScene will be added. Additionally, the View3DScene
    will take care of population the camera, screenSize and sceneEntity
    properties.

    Furthermore, animation playback and control is abstracted by convenience
    methods on the View3DScene instance.

*/

/*!
    \property KuesaUtils::View3DScene::importer

    \brief Points to the \l {Kuesa::GLTF2Importer} instance wrapped around by
    the View3DScene.
 */

/*!
    \property KuesaUtils::View3DScene::frameGraph

    \brief Points to the \l {Kuesa::ForwardRenderer} frame graph instance
    wrapped around by the View3DScene.
 */

/*!
    \property KuesaUtils::View3DScene::source

    \brief The source of the glTF file to be loaded.
    \readonly
 */


/*!
    \property KuesaUtils::View3DScene::cameraName

    \brief The name of the camera asset that should be used to view the scene.
    If the name references a valid camera, the camera will automatically be
    set on the ForwardRenderer frameGraph and other internal assets such as
    \l {Kuesa::TransformTracker}.
    \readonly
 */

/*!
    \property KuesaUtils::View3DScene::showDebugOverlay

    \brief Specifies whether the Qt 3D debug overlay should be displayed.
    \note this only works when Qt is running with its OpenGL backend.
 */

/*!
    \property KuesaUtils::View3DScene::screenSize

    \brief Holds the size in pixels of the rendered area. When set, this
    automatically updates the \l {Kuesa::TransformTracker} instances referenced
    by the View3DScene with the new value for proper coodinate projection.
 */

/*!
    \property KuesaUtils::View3DScene::ready

    \brief This is true once the glTF file has been properly loaded and that
    Qt 3D GPU resources have been successfully loaded onto the GPU. The scene
    should be visible on screen when this becomes true.
 */

/*!
    \property KuesaUtils::View3DScene::loaded

    \brief This is true once the glTF file has been properly loaded. The
    scene might not be visible on screen when this becomes true.
 */

/*!
    \property KuesaUtils::View3DScene::asynchronous

    \brief If true, glTF parsing is performed in a non blocking manner from a
    secondary thread. This is false by default.
 */

/*!
    \property KuesaUtils::View3DScene::activeScene

    \brief If this points to a valid \l {KuesaUtils::SceneConfiguration} instance,
    the \l {KuesaUtils::View3DScene::source}, \l
    {KuesaUtils::View3DScene::cameraName} as well as the \l
    {Kuesa::AnimationPlayer} and \l {Kuesa::TransformTracker} instances will be
    automatically set based on the values provided by the SceneConfiguration.
    This a more convenient way of specifying content when dealing with
    multiple scenes.
 */


/*!
    \property KuesaUtils::View3DScene::reflectionPlaneName

    \brief If non empty, the View3DScene will try to load the \l
    {Kuesa::ReflectionPlane} matching the name from the collection and set it on
    the ForwardRenderer framegraph views.
 */

/*!
    \qmltype View3DScene
    \inqmlmodule KuesaUtils
    \since Kuesa 1.3
    \inherits Kuesa::SceneEntity
    \instantiates KuesaUtils::View3DScene

    \brief View3DScene is a convenience wrapper aiming at simplifying the set
    up of a 3D scene and the loading of a glTF 2 scene file.

    View3D is a convenience wrapper aiming at simplifying the set up of a 3D
    scene and the loading of a glTF 2 scene file.

    It is a subclass of \l {Kuesa::SceneEntity} which grants access to the
    various asset collections. Aditionnaly it contains a \l
    {Kuesa::GLTF2Importer} and sets up a \l {Kuesa::ForwardRenderer} framegraph.

    The property \l [QML] {KuesaUtils::View3DScene::ready} and signal \l [QML]
    {View3DScene::readyChanged} can be used to detect when the scene is visible
    on screen and therefore synchronize with other aspect of your application.

    When used in conjunction with \l [QML] {KuesaUtils::SceneConfiguration} this
    class makes it convenient to set up a Kuesa renderer that can easily load
    and unload scenes.

    To set the scene to be loaded by the View3DScene, an instance of
    \l {KuesaUtils::SceneConfiguration} must be set as the activeScene property.

    \l [QML] {Kuesa::AnimationPlayer} and \l [QML] {Kuesa::TransformTracker}
    instances defined on the activeScene will be added. Additionally, the
    View3DScene will take care of population the camera, screenSize and
    sceneEntity properties.

    Furthermore, animation playback and control is abstracted by convenience
    methods on the View3DScene instance.
*/

/*!
    \qmlproperty Kuesa::GLTF2Importer KuesaUtils::View3DScene::importer

    \brief Points to the \l {Kuesa::GLTF2Importer} instance wrapped around by
    the View3D.
 */

/*!
    \qmlproperty Kuesa::ForwardRenderer KuesaUtils::View3DScene::frameGraph

    \brief Points to the \l {Kuesa::ForwardRenderer} frame graph instance
    wrapped around by the View3DScene.
 */

/*!
    \qmlproperty url KuesaUtils::View3DScene::source

    \brief The source of the glTF file to be loaded.
    \readonly
 */

/*!
    \qmlproperty string KuesaUtils::View3DScene::cameraName

    \brief The name of the camera asset that should be used to view the scene.
    If the name references a valid camera, the camera will automatically be
    set on the ForwardRenderer frameGraph and other internal assets such as
    \l [QML] {Kuesa::TransformTracker}.
    \readonly
 */

/*!
    \qmlproperty bool KuesaUtils::View3DScene::showDebugOverlay

    \brief Specifies whether the Qt 3D debug overlay should be displayed.
    \note this only works when Qt is running with its OpenGL backend.
 */

/*!
    \qmlproperty size KuesaUtils::View3DScene::screenSize

    \brief Holds the size in pixels of the rendered area. When set, this
    automatically updates the \l [QML] {Kuesa::TransformTracker} instances
    referenced by the View3D with the new value for proper coodinate
    projection.
 */

/*!
    \qmlproperty bool KuesaUtils::View3DScene::ready

    \brief This is true once the glTF file has been properly loaded and that
    Qt 3D GPU resources have been successfully loaded onto the GPU. The scene
    should be visible on screen when this becomes true.
 */

/*!
    \qmlproperty bool KuesaUtils::View3DScene::loaded

    \brief This is true once the glTF file has been properly loaded. The
    scene might not be visible on screen when this becomes true.
 */

/*!
    \qmlproperty bool KuesaUtils::View3DScene::asynchronous

    \brief If true, glTF parsing is performed in a non blocking manner from a
    secondary thread. This is false by default.
 */

/*!
    \qmlproperty KuesaUtils::SceneConfiguration KuesaUtils::View3DScene::activeScene

    \brief If this points to a valid \l [QML] {KuesaUtils::SceneConfiguration}
    instance, the \l [QML] {KuesaUtils::View3DScene::source}, \l [QML]
    {KuesaUtils::View3DScene::cameraName} as well as the \l [QML]
    {Kuesa::AnimationPlayer} and \l [QML] {Kuesa::TransformTracker} instances
    will be automatically set based on the values provided by the
    SceneConfiguration. This offers a convenient way of specifying content when
    dealing with multiple scenes.
 */

/*!
    \qmlproperty string KuesaUtils::View3DScene::reflectionPlaneName

    \brief If non empty, the View3DScene will try to load the \l [QML]
    {Kuesa::ReflectionPlane} matching the name from the collection and set it on
    the ForwardRenderer framegraph views.
 */


View3DScene::View3DScene(Qt3DCore::QNode *parent)
    : Kuesa::SceneEntity(parent)
    , m_importer(new GLTF2Importer(this))
    , m_frameGraph(nullptr)
    , m_clock(nullptr)
    , m_activeScene(nullptr)
    , m_ready(false)
    , m_frameCount(0)
{
    m_frameGraph = Qt3DCore::QAbstractNodeFactory::createNode<Kuesa::ForwardRenderer>("ForwardRenderer");
    m_importer->setSceneEntity(this);

    auto renderSettings = new QRenderSettings;
    renderSettings->setActiveFrameGraph(m_frameGraph);
    renderSettings->setRenderPolicy(QRenderSettings::OnDemand);
    addComponent(renderSettings);
    addComponent(new Qt3DInput::QInputSettings);

    m_frameAction = new Qt3DLogic::QFrameAction;
    addComponent(m_frameAction);
    connect(m_frameAction, &Qt3DLogic::QFrameAction::triggered, this, &View3DScene::updateFrame);
    m_frameAction->setEnabled(false);

    connect(m_importer, &GLTF2Importer::sourceChanged, this, &View3DScene::sourceChanged);
    connect(m_importer, &GLTF2Importer::asynchronousChanged, this, &View3DScene::asynchronousChanged);
    connect(m_frameGraph, &ForwardRenderer::showDebugOverlayChanged, this, &View3DScene::showDebugOverlayChanged);
    connect(m_frameGraph, &ForwardRenderer::cameraChanged, this, &View3DScene::updateTransformTrackers);
    connect(m_frameGraph, &ForwardRenderer::cameraChanged, this, &View3DScene::updatePlaceholderTrackers);

    QObject::connect(m_importer, &GLTF2Importer::statusChanged,
                     this, [this] () {
        if (m_importer->status() == GLTF2Importer::Ready)
            onSceneLoaded();
    });
}

View3DScene::~View3DScene() = default;

QUrl View3DScene::source() const
{
    return m_importer->source();
}

void View3DScene::setSource(const QUrl &source)
{
    if (source != m_importer->source()) {
        // Clear assets from the collections
        // This avoids name collisions if new scene defines an element with the
        // same name as one from the previous scene.
        clearCollections();

        m_importer->setSource(source);
        m_ready = false;
        m_frameCount = 0;
        emit readyChanged(false);
        emit loadedChanged(false);
        m_frameAction->setEnabled(true);
    } else {
        // If we switch activeScene but activeScene references the same
        // source, there's no point in reloading it
        emit readyChanged(m_ready);
        if (m_importer->status() == GLTF2Importer::Ready)
            onSceneLoaded();
    }
}

QString View3DScene::cameraName() const
{
    return m_cameraName;
}

void View3DScene::setCameraName(const QString &cameraName)
{
    if (cameraName != m_cameraName) {
        m_cameraName = cameraName;
        emit cameraNameChanged(m_cameraName);
    }
}

bool View3DScene::showDebugOverlay() const
{
    return m_frameGraph->showDebugOverlay();
}

QSize View3DScene::screenSize() const
{
    return m_screenSize;
}

bool View3DScene::asynchronous() const
{
    return m_importer->asynchronous();
}

QString View3DScene::reflectionPlaneName() const
{
    return m_reflectionPlaneName;
}

void View3DScene::setScreenSize(const QSize &screenSize)
{
    if (m_screenSize != screenSize) {
        m_screenSize = screenSize;
        emit screenSizeChanged(m_screenSize);
        updateTransformTrackers();
        updatePlaceholderTrackers();
    }
}

void View3DScene::setAsynchronous(bool asynchronous)
{
    m_importer->setAsynchronous(asynchronous);
}

void View3DScene::setReflectionPlaneName(const QString &reflectionPlaneName)
{
    if (m_reflectionPlaneName == reflectionPlaneName)
        return;
    m_reflectionPlaneName = reflectionPlaneName;
    emit reflectionPlaneNameChanged(reflectionPlaneName);

    if (isLoaded())
        loadReflections();
}

void View3DScene::setShowDebugOverlay(bool showDebugOverlay)
{
    m_frameGraph->setShowDebugOverlay(showDebugOverlay);
}

void View3DScene::adoptNode(QObject *object)
{
    auto *node = qobject_cast<Qt3DCore::QNode *>(object);
    if (node && node->parentNode() == nullptr)
        node->setParent(this);
}

void View3DScene::onSceneLoaded()
{
    if (!m_cameraName.isEmpty()) {
        auto camera = SceneEntity::camera(m_cameraName);
        if (camera)
            m_frameGraph->setCamera(camera);
    }

    if (m_activeScene) {
        // Add resources from the ActiveScene
        QObject::connect(m_activeScene, &SceneConfiguration::animationPlayerAdded, this, &View3DScene::addAnimationPlayer);
        QObject::connect(m_activeScene, &SceneConfiguration::animationPlayerRemoved, this, &View3DScene::removeAnimationPlayer);
        QObject::connect(m_activeScene, &SceneConfiguration::transformTrackerAdded, this, &View3DScene::addTransformTracker);
        QObject::connect(m_activeScene, &SceneConfiguration::transformTrackerRemoved, this, &View3DScene::removeTransformTracker);
        QObject::connect(m_activeScene, &SceneConfiguration::placeholderTrackerAdded, this, &View3DScene::addPlaceholderTracker);
        QObject::connect(m_activeScene, &SceneConfiguration::placeholderTrackerRemoved, this, &View3DScene::removePlaceholderTracker);

        const auto &newAnimations = m_activeScene->animationPlayers();
        for (auto a : newAnimations)
            addAnimationPlayer(a);

        const auto newTrackers = m_activeScene->transformTrackers();
        for (auto t : newTrackers)
            addTransformTracker(t);

        const auto newPlaceholderTrackers = m_activeScene->placeholderTrackers();
        for (auto p : newPlaceholderTrackers)
            addPlaceholderTracker(p);

        emit m_activeScene->loadingDone();
    }
    loadReflections();

    emit loadedChanged(true);
}

void View3DScene::updateTransformTrackers()
{
    for (auto t : m_transformTrackers) {
        t->setScreenSize(m_screenSize);
        t->setCamera(m_frameGraph->camera());
    }
}

void View3DScene::updatePlaceholderTrackers()
{
    for (Kuesa::PlaceholderTracker *t : m_placeholderTrackers) {
        t->setScreenSize(m_screenSize);
        t->setCamera(m_frameGraph->camera());
    }
}

/*!
    \internal
*/
void View3DScene::updateFrame(float dt)
{
    Q_UNUSED(dt);

    if (m_ready)
        return;

    if (m_frameCount == 0) {
        bool allReady = true;
        const auto &textures = textureImages()->names();
        for (const auto &textureName : textures) {
            auto texture = textureImage(textureName);
            auto textureImage = qobject_cast<Qt3DRender::QTextureImage *>(texture);
            if (!textureImage)
                continue;

            if (textureImage->status() != Qt3DRender::QTextureImage::Ready) {
                allReady = false;
                break;
            }
        }

        if (allReady)
            m_frameCount = 1;
    } else {
        m_frameCount++;
    }

    if (m_frameCount == 2) {
        m_ready = true;
        emit readyChanged(true);
        m_frameAction->setEnabled(false);
    }
}

void View3DScene::loadReflections()
{
    if (reflectionPlanes()->size() == 0 || m_reflectionPlaneName.isEmpty())
        return;

    // Find matching ReflectionPlane
    Kuesa::ReflectionPlane *p = reflectionPlane(m_reflectionPlaneName);

    auto setReflectionPlaneOnView = [&] (View *v) {
        if (p && !Utils::contains(v->reflectionPlanes(), p))
            v->addReflectionPlane(p);
    };

    if (m_frameGraph->views().size() > 0) {
        for (View *v : m_frameGraph->views())
            setReflectionPlaneOnView(v);
    } else {
        setReflectionPlaneOnView(m_frameGraph);
    }
}

/*!
    \internal
    \brief Adds \a tracker to the list of managed \l {Kuesa::TransformTracker}
    instances of the View3DScene.
 */
void View3DScene::addTransformTracker(TransformTracker *tracker)
{
    if (std::find(std::begin(m_transformTrackers), std::end(m_transformTrackers), tracker) == std::end(m_transformTrackers)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(tracker, &View3DScene::removeTransformTracker, m_transformTrackers);
        if (tracker->parentNode() == nullptr)
            tracker->setParent(this);
        if (tracker->sceneEntity() == nullptr)
            tracker->setSceneEntity(this);
        m_transformTrackers.push_back(tracker);
        updateTransformTrackers();
    }
}

/*!
    \internal
    \brief Removes \a tracker from the list of managed \l
    {Kuesa::TransformTracker} instances of the View3DScene.
 */
void View3DScene::removeTransformTracker(TransformTracker *tracker)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(tracker);
    m_transformTrackers.erase(std::remove(std::begin(m_transformTrackers), std::end(m_transformTrackers), tracker),
                     std::end(m_transformTrackers));
}

/*!
    \internal
    \brief Clears the list of managed \l {Kuesa::TransformTracker} instances of
    the View3DScene.
 */
void View3DScene::clearTransformTrackers()
{
    const std::vector<Kuesa::TransformTracker *> trackersCopy = m_transformTrackers;
    for (Kuesa::TransformTracker *t : trackersCopy)
        removeTransformTracker(t);
}

/*!
    \internal
    \brief Adds \a placeholderTracker to the list of managed \l {Kuesa::PlaceholderTracker}
    instances of the View3DScene.
 */
void View3DScene::addPlaceholderTracker(PlaceholderTracker *placeholderTracker)
{
    if (std::find(std::begin(m_placeholderTrackers), std::end(m_placeholderTrackers), placeholderTracker) == std::end(m_placeholderTrackers)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(placeholderTracker, &View3DScene::removePlaceholderTracker, m_placeholderTrackers);

        if (placeholderTracker->parentNode() == nullptr)
            placeholderTracker->setParent(this);
        if (placeholderTracker->sceneEntity() == nullptr)
            placeholderTracker->setSceneEntity(this);

        m_placeholderTrackers.push_back(placeholderTracker);
        updatePlaceholderTrackers();
    }
}

/*!
    \internal
    \brief Removes \a placeholderTracker from the list of managed \l
    {Kuesa::PlaceholderTracker} instances of the View3DScene.
 */
void View3DScene::removePlaceholderTracker(PlaceholderTracker *placeholderTracker)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(placeholderTracker);
    m_placeholderTrackers.erase(std::remove(std::begin(m_placeholderTrackers), std::end(m_placeholderTrackers), placeholderTracker),
                         std::end(m_placeholderTrackers));
}

/*!
    \internal
    \brief Clears the list of managed \l {Kuesa::PlaceholderTracker} instances of
    the View3DScene.
 */
void View3DScene::clearPlaceholderTrackers()
{
    const std::vector<Kuesa::PlaceholderTracker *> placeholderTrackersCopy = m_placeholderTrackers;
    for (Kuesa::PlaceholderTracker *p : placeholderTrackersCopy)
        removePlaceholderTracker(p);
}

/*!
    \brief Returns the \l {Kuesa::TransformTracker} instances referenced by the
    View3DScene instance.
 */
const std::vector<TransformTracker *> &View3DScene::transformTrackers() const
{
    return m_transformTrackers;
}

/*!
    \brief Returns the \l {Kuesa::PlaceholderTracker} instances referenced by the
    View3DScene instance.
 */
const std::vector<PlaceholderTracker *> &View3DScene::placeholderTrackers() const
{
    return m_placeholderTrackers;
}

/*!
    \internal
    \brief Adds \a animation to the list of managed \l {Kuesa::AnimationPlayer}
    instances of the View3DScene.
 */
void View3DScene::addAnimationPlayer(AnimationPlayer *animation)
{
    if (m_clock == nullptr)
        m_clock = new Qt3DAnimation::QClock(this);
    if (std::find(std::begin(m_animations), std::end(m_animations), animation) == std::end(m_animations)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(animation, &View3DScene::removeAnimationPlayer, m_animations);

        if (animation->parentNode() == nullptr)
            animation->setParent(this);
        if (animation->sceneEntity() == nullptr)
            animation->setSceneEntity(this);

        m_animations.push_back(animation);

        //if Animation has no Clock, set one
        if (animation->clock() == nullptr)
            animation->setClock(m_clock);
    }
}

/*!
    \internal
    \brief Removes \a animation from the list of managed \l {Kuesa::AnimationPlayer}
    instances of the View3DScene.
 */
void View3DScene::removeAnimationPlayer(AnimationPlayer *animation)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(animation);
    auto it = std::remove_if(std::begin(m_animations), std::end(m_animations), [animation](AnimationPlayer *a) {
            return a == animation;
    });
    if (it != m_animations.end()) {
        if (animation->clock() == m_clock)
            animation->setClock(nullptr);

        m_animations.erase(it, std::end(m_animations));
    }
}


/*!
    \internal
    \brief Clears the list of managed \l {Kuesa::AnimationPlayer} instances of
    the View3DScene.
 */
void View3DScene::clearAnimationPlayers()
{
    const std::vector<Kuesa::AnimationPlayer *> animCopy = m_animations;
    for (Kuesa::AnimationPlayer *a : animCopy)
        removeAnimationPlayer(a);
}

/*!
    \brief Returns the \l {Kuesa::AnimationPlayer} instances referenced by the
    View3DScene instance.
 */
const std::vector<AnimationPlayer *> &View3DScene::animationPlayers() const
{
    return m_animations;
}

SceneConfiguration *View3DScene::activeScene() const
{
    return m_activeScene;
}

/*!
    \brief Sets the active scene configuration to \a scene. If \a scene is a valid
    non null instance, the \l [QML] {KuesaUtils::View3DScene::source}, \l
    {KuesaUtils::View3DScene::cameraName} as well as the \l {Kuesa::AnimationPlayer}
    and \l {Kuesa::TransformTracker} instances will be automatically set based on
    the values provided by the SceneConfiguration. If null, all of these will
    be cleared.
 */
void View3DScene::setActiveScene(SceneConfiguration *scene)
{
    if (m_activeScene != scene) {

        if (m_activeScene) {
            // Unregister previous resources
            const auto &oldAnimations = m_activeScene->animationPlayers();
            const auto &oldTrackers = m_activeScene->transformTrackers();

            for (auto a : oldAnimations)
                removeAnimationPlayer(a);
            for (auto a : oldTrackers)
                removeTransformTracker(a);

            Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
            d->unregisterDestructionHelper(m_activeScene);

            QObject::disconnect(m_activeScene);

            // If we took ownership of the SceneConfiguration -> restore parent
            if (m_activeSceneOwner) {
                // Make sure we unset the QNode * parent if it was overridden
                QNode *originalNodeParent = qobject_cast<QNode *>(m_activeSceneOwner.data());
                // In case parent is a QNode *
                if (originalNodeParent) {
                    m_activeScene->setParent(originalNodeParent);
                } else {// Parent was not a QNode
                    // Reset QNode parent
                    m_activeScene->setParent(Q_NODE_NULLPTR);
                    static_cast<QObject *>(m_activeScene)->setParent(m_activeSceneOwner.data());
                }
            }
        }

        m_activeScene = scene;
        m_activeSceneOwner.clear();
        emit activeSceneChanged(m_activeScene);

        if (m_activeScene) {
            // Ensure we parent the scene to a valid QNode so that resources
            // are properly added to the scene
            m_activeSceneOwner = scene->parent();

            if (!m_activeScene->parentNode())
                m_activeScene->setParent(this);

            if (!m_activeScene->sceneEntity())
                m_activeScene->setSceneEntity(this);

            Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
            d->registerDestructionHelper(m_activeScene, &View3DScene::setActiveScene, m_activeScene);

            setSource(m_activeScene->source());
            setCameraName(m_activeScene->cameraName());

            QObject::connect(m_activeScene, &SceneConfiguration::sourceChanged, this, &View3DScene::setSource);
            QObject::connect(m_activeScene, &SceneConfiguration::cameraNameChanged, this, &View3DScene::setCameraName);

            // ActiveScene resources (animations, transformTrackers ....) will be loaded
            // Once the scene will have been loaded
            // Otherwise we could end up having an AnimationPlayer that references a yet to be loaded
            // AnimationClip, which Qt3D might complain about
        } else {
            // Set empty sources / camera
            setSource(QUrl());
            setCameraName(QString());
        }
    }
}

bool View3DScene::isReady() const
{
    return m_ready;
}

bool View3DScene::isLoaded() const
{
    return m_importer->status() == GLTF2Importer::Ready;
}

/*!
    \brief Starts all the \l {Kuesa::AnimationPlayer} instances referenced by
    the View3DScene instance.

     \sa View3DScene::stop, View3DScene::restart
 */
void View3DScene::start()
{
    for (auto a : m_animations)
        a->start();
}

/*!
    \brief Restarts all the \l {Kuesa::AnimationPlayer} instances referenced by
    the View3DScene instance.

    \sa View3DScene::stop, View3DScene::start
 */
void View3DScene::restart()
{
    for (auto a : m_animations)
        a->restart();
}

/*!
    \brief Stops all the \l {Kuesa::AnimationPlayer} instances referenced by
    the View3DScene instance.

    \sa View3DScene::start
 */
void View3DScene::stop()
{
    for (auto a : m_animations)
        a->stop();
}

/*!
    \brief Set the normalized time of all the \l {Kuesa::AnimationPlayer}
    instances referenced by the View3DScene instance to \a time.
 */
void View3DScene::gotoNormalizedTime(float time)
{
    for (auto a : m_animations)
        a->setNormalizedTime(time);
}

/*!
    \brief Set the normalized time of all the \l {Kuesa::AnimationPlayer}
    instances referenced by the View3DScene instance to 0.
 */
void View3DScene::gotoStart()
{
    gotoNormalizedTime(0.f);
}

/*!
    \brief Set the normalized time of all the \l {Kuesa::AnimationPlayer}
    instances referenced by the View3DScene instance to 1.
 */
void View3DScene::gotoEnd()
{
    gotoNormalizedTime(1.f);
}
