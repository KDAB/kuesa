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
    , m_ready(false)
    , m_frameCount(0)
{
    m_frameGraph = Qt3DCore::QAbstractNodeFactory::createNode<Kuesa::ForwardRenderer>("ForwardRenderer");
    m_importer->setSceneEntity(this);

    auto renderSettings = new QRenderSettings;
    renderSettings->setActiveFrameGraph(m_frameGraph);
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
    connect(this, &SceneEntity::loadingDone, this, &View3DScene::onSceneLoaded);
}

View3DScene::~View3DScene() = default;

QUrl View3DScene::source() const
{
    return m_importer->source();
}

void View3DScene::setSource(const QUrl &source)
{
    m_importer->setSource(source);
    m_ready = false;
    m_frameCount = 0;
    emit readyChanged(false);
    emit loadedChanged(false);
    m_frameAction->setEnabled(true);
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
        const auto& textures = textureImages()->names();
        for (const auto &textureName: textures) {
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

bool View3DScene::isReady() const
{
    return m_ready;
}

bool View3DScene::isLoaded() const
{
    return m_importer->status() == GLTF2Importer::Ready;
}

void View3DScene::addAnimationPlayer(AnimationPlayer *animation)
{
    if (nullptr == m_clock)
        m_clock = new Qt3DAnimation::QClock(this);
    if (std::find(std::begin(m_animations), std::end(m_animations), animation) == std::end(m_animations)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(animation, &View3DScene::removeAnimationPlayer, animation);
        if (animation->parentNode() == nullptr)
            animation->setParent(this);
        m_animations.push_back(animation);
        animation->setClock(m_clock);
    }
}

void View3DScene::removeAnimationPlayer(AnimationPlayer *animation)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(animation);
    m_animations.erase(std::remove_if(std::begin(m_animations), std::end(m_animations), [animation](AnimationPlayer *a) {
                           return a == animation;
                       }),
                       std::end(m_animations));
}

void View3DScene::clearAnimationPlayers()
{
    m_animations.clear();
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
