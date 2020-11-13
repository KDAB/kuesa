/*
    view3dscene.cpp

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

#include "view3dscene.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>

#include <Kuesa/sceneentity.h>
#include <Kuesa/forwardrenderer.h>
#include <Kuesa/gltf2importer.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qabstractnodefactory_p.h>
#include <Qt3DRender/qrendersettings.h>
#include <Qt3DInput/qinputsettings.h>

using namespace Kuesa;
using namespace KuesaUtils;
using namespace Qt3DCore;
using namespace Qt3DRender;

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
    connect(m_frameGraph, &ForwardRenderer::cameraChanged, this, &View3DScene::updateTrackers);

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

void View3DScene::setScreenSize(const QSize &screenSize)
{
    if (m_screenSize != screenSize) {
        m_screenSize = screenSize;
        emit screenSizeChanged(m_screenSize);
        updateTrackers();
    }
}

void View3DScene::setAsynchronous(bool asynchronous)
{
    m_importer->setAsynchronous(asynchronous);
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

        const auto &newAnimations = m_activeScene->animations();
        for (auto a : newAnimations)
            addAnimationPlayer(a);

        const auto newTrackers = m_activeScene->transformTrackers();
        for (auto t : newTrackers)
            addTransformTracker(t);

        emit m_activeScene->loadingDone();
    }

    emit loadedChanged(true);
}

void View3DScene::updateTrackers()
{
    for (auto t : m_trackers) {
        t->setScreenSize(m_screenSize);
        t->setCamera(m_frameGraph->camera());
    }
}

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

void View3DScene::addTransformTracker(TransformTracker *tracker)
{
    if (std::find(std::begin(m_trackers), std::end(m_trackers), tracker) == std::end(m_trackers)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(tracker, &View3DScene::removeTransformTracker, tracker);
        if (tracker->parentNode() == nullptr)
            tracker->setParent(this);
        if (tracker->sceneEntity() == nullptr)
            tracker->setSceneEntity(this);
        m_trackers.push_back(tracker);
        updateTrackers();
    }
}

void View3DScene::removeTransformTracker(TransformTracker *tracker)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(tracker);
    m_trackers.erase(std::remove(std::begin(m_trackers), std::end(m_trackers), tracker),
                     std::end(m_trackers));
}

void View3DScene::clearTransformTrackers()
{
    m_trackers.clear();
}

void View3DScene::addAnimationPlayer(AnimationPlayer *animation)
{
    if (m_clock == nullptr)
        m_clock = new Qt3DAnimation::QClock(this);
    if (std::find(std::begin(m_animations), std::end(m_animations), animation) == std::end(m_animations)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(animation, &View3DScene::removeAnimationPlayer, animation);

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

void View3DScene::clearAnimationPlayers()
{
    m_animations.clear();
}

SceneConfiguration *View3DScene::activeScene() const
{
    return m_activeScene;
}

void View3DScene::setActiveScene(SceneConfiguration *scene)
{
    if (m_activeScene != scene) {

        if (m_activeScene) {
            // Unregister previous resources
            const auto &oldAnimations = m_activeScene->animations();
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

            // ActiveScene resources (animations, transformTrackers ....) will be loaded
            // Once the scene will have been loaded
            // Otherwise we could end up having an AnimationPlayer that references a yet to be loaded
            // AnimationClip, which Qt3D might complain about
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

void View3DScene::start()
{
    for (auto a : m_animations)
        a->start();
}

void View3DScene::restart()
{
    for (auto a : m_animations)
        a->restart();
}

void View3DScene::stop()
{
    for (auto a : m_animations)
        a->stop();
}

void View3DScene::gotoNormalizedTime(float time)
{
    for (auto a : m_animations)
        a->setNormalizedTime(time);
}

void View3DScene::gotoStart()
{
    gotoNormalizedTime(0.f);
}

void View3DScene::gotoEnd()
{
    gotoNormalizedTime(1.f);
}
