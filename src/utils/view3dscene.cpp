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
#include <Kuesa/private/logging_p.h>

#include <Kuesa/private/kuesa_utils_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qabstractnodefactory_p.h>
#include <Qt3DInput/qinputsettings.h>

#include <Kuesa/Iro2PlanarReflectionEquiRectProperties>
#include <Kuesa/Iro2PlanarReflectionSemProperties>

#include <KuesaUtils/viewconfiguration.h>

using namespace Kuesa;
using namespace KuesaUtils;
using namespace Qt3DCore;
using namespace Qt3DRender;

Q_LOGGING_CATEGORY(kuesa_utils, "Kuesa.Utils", QtWarningMsg)

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
    \property KuesaUtils::View3DScene::renderSettings

    \brief Points to the \l {Qt3DRender::QRenderSettings} render settings
    instance wrapped around by the View3DScene.
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
    \property KuesaUtils::View3DScene::layerNames

    \brief The names of the layer assets that should be used to select objects
    to render in the scene. If the name references a valid layer, it will
    automatically be set on the ForwardRenderer frameGraph.
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

    \brief This is true once the glTF file has been properly loaded and that Qt
    3D GPU resources have been successfully loaded onto the GPU. The scene
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
    \qmlproperty Qt3DRender::QRenderSettings KuesaUtils::View3DScene::renderSettings

    \brief Points to the \l {Qt3DRender::QRenderSettings} render settings
    instance wrapped around by the View3DScene.
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
    \qmlproperty list<string> KuesaUtils::View3DScene::layerNames

    \brief The names of the layer assets that should be used to select objects
    to render in the scene. If the name references a valid layer, it will
    automatically be set on the ForwardRenderer frameGraph.
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

    \brief This is true once the glTF file has been properly loaded and that Qt
    3D GPU resources have been successfully loaded onto the GPU. The scene
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
    , m_renderSettings(new QRenderSettings)
    , m_activeScene(nullptr)
    , m_ready(false)
    , m_frameCount(0)
{
    m_frameGraph = Qt3DCore::QAbstractNodeFactory::createNode<Kuesa::ForwardRenderer>("ForwardRenderer");
    m_importer->setSceneEntity(this);

    m_renderSettings->setActiveFrameGraph(m_frameGraph);
    m_renderSettings->setRenderPolicy(QRenderSettings::OnDemand);
    m_renderSettings->pickingSettings()->setPickMethod(QPickingSettings::TrianglePicking);
    addComponent(m_renderSettings);
    addComponent(new Qt3DInput::QInputSettings);

    m_frameAction = new Qt3DLogic::QFrameAction;
    addComponent(m_frameAction);
    connect(m_frameAction, &Qt3DLogic::QFrameAction::triggered, this, &View3DScene::updateFrame);
    m_frameAction->setEnabled(false);

    connect(m_importer, &GLTF2Importer::sourceChanged, this, &View3DScene::sourceChanged);
    connect(m_importer, &GLTF2Importer::asynchronousChanged, this, &View3DScene::asynchronousChanged);
    connect(m_frameGraph, &ForwardRenderer::showDebugOverlayChanged, this, &View3DScene::showDebugOverlayChanged);

    QObject::connect(m_importer, &GLTF2Importer::statusChanged,
                     this, [this]() {
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

void KuesaUtils::View3DScene::retrieveAndSetCamera(const QString &cameraName, Kuesa::View *view)
{
    if (!cameraName.isEmpty()) {
        auto camera = SceneEntity::camera(cameraName);
        if (camera) {
            camera->setAspectRatio((static_cast<float>(m_screenSize.width() * view->viewportRect().width())) /
                                   (static_cast<float>(m_screenSize.height() * view->viewportRect().height())));
            view->setCamera(camera);
        }
    }
}

void View3DScene::retrieveAndSetLayers(const QStringList &layers, Kuesa::View *view)
{
    std::vector<Qt3DRender::QLayer *> currentLayers = view->layers();
    std::vector<Qt3DRender::QLayer *> requiredLayers;
    for (const QString &name : layers) {
        Qt3DRender::QLayer *l = layer(name);
        if (l)
            requiredLayers.push_back(l);
    }

    std::sort(currentLayers.begin(), currentLayers.end());
    std::sort(requiredLayers.begin(), requiredLayers.end());

    // Existing layers -> intersection between currentLayers and requiredLayers
    std::vector<Qt3DRender::QLayer *> existingLayers;
    std::set_intersection(currentLayers.begin(), currentLayers.end(),
                          requiredLayers.begin(), requiredLayers.end(),
                          std::back_inserter(existingLayers));

    // Layers to remove -> difference between currentLayers and requiredLayers
    std::vector<Qt3DRender::QLayer *> layersToRemove;
    std::set_difference(currentLayers.begin(), currentLayers.end(),
                        requiredLayers.begin(), requiredLayers.end(),
                        std::back_inserter(layersToRemove));

    // Layers to add -> different between requiredLayers and existingLayers
    std::vector<Qt3DRender::QLayer *> layersToAdd;
    std::set_difference(requiredLayers.begin(), requiredLayers.end(),
                        existingLayers.begin(), existingLayers.end(),
                        std::back_inserter(layersToAdd));

    for (Qt3DRender::QLayer *layerToRemove : layersToRemove)
        view->removeLayer(layerToRemove);
    for (Qt3DRender::QLayer *layerToAdd : layersToAdd)
        view->addLayer(layerToAdd);
}

void View3DScene::onSceneLoaded()
{
    if (m_activeScene) {
        // Add resources from the ActiveScene
        QObject::connect(m_activeScene, &SceneConfiguration::animationPlayerAdded, this, &View3DScene::addAnimationPlayer);
        QObject::connect(m_activeScene, &SceneConfiguration::animationPlayerRemoved, this, &View3DScene::removeAnimationPlayer);
        QObject::connect(m_activeScene, &SceneConfiguration::viewConfigurationAdded, this, &View3DScene::addViewConfiguration);
        QObject::connect(m_activeScene, &SceneConfiguration::viewConfigurationRemoved, this, &View3DScene::removeViewConfiguration);

        const auto &newAnimations = m_activeScene->animationPlayers();
        for (auto a : newAnimations)
            addAnimationPlayer(a);

        const auto newViewConfigurations = m_activeScene->viewConfigurations();
        for (auto v : newViewConfigurations)
            addViewConfiguration(v);

        //Cameras

        emit m_activeScene->loadingDone();
    }
    loadReflections();

    emit loadedChanged(true);
}

void View3DScene::updateTransformTrackers(const std::vector<TransformTracker *> &transformTrackers, View *view)
{
    for (auto *transformTracker : transformTrackers) {
        if (!transformTracker->sceneEntity())
            transformTracker->setSceneEntity(this);
        transformTracker->setViewportRect(view->viewportRect());
        transformTracker->setCamera(view->camera());
        transformTracker->setScreenSize(m_screenSize);
    }
}

void View3DScene::updatePlaceholderTrackers(const std::vector<PlaceholderTracker *> &placeholderTrackers, View *view)
{
    for (auto *tracker : placeholderTrackers) {
        if (!tracker->sceneEntity())
            tracker->setSceneEntity(this);
        tracker->setViewportRect(view->viewportRect());
        tracker->setCamera(view->camera());
        tracker->setScreenSize(m_screenSize);
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

    auto setReflectionPlaneOnView = [&](View *v) {
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

namespace {

template<typename Emitter, typename Signal>
QMetaObject::Connection connectSignalToCallback(Emitter *emitter,
                                                std::function<void()> callback,
                                                Signal s)
{
    return QObject::connect(emitter, s, callback);
}

template<typename Emitter, typename... Signal>
std::vector<QMetaObject::Connection> connectSignalsToCallback(Emitter *emitter,
                                                              std::function<void()> callback,
                                                              Signal... sigs)
{
    return { connectSignalToCallback(emitter, callback, sigs)... };
}

template<typename T>
std::vector<T> join(const std::vector<std::vector<T>> &vecs)
{
    size_t size = 0;
    for (auto &v : vecs)
        size += v.size();

    std::vector<T> mergedVec;
    mergedVec.reserve(size);
    for (auto &v : vecs)
        mergedVec.insert(mergedVec.end(), v.begin(), v.end());
    return mergedVec;
}

} // namespace

void View3DScene::addViewConfiguration(ViewConfiguration *viewConfiguration)
{
    if (std::find_if(std::begin(m_viewConfigurationsResources),
                     std::end(m_viewConfigurationsResources),
                     [viewConfiguration](const auto &v) {
                         return v.viewConfiguration == viewConfiguration;
                     }) == std::end(m_viewConfigurationsResources)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        auto tag = std::vector<ViewConfiguration *>();
        d->registerDestructionHelper(viewConfiguration, &View3DScene::removeViewConfiguration, tag);

        // Create a framegraph View matching this ViewConfiguration
        View *view = new View();

        QObject::connect(viewConfiguration, &ViewConfiguration::viewportRectChanged, view, &View::setViewportRect);
        QObject::connect(viewConfiguration, &ViewConfiguration::frustumCullingChanged, view, &View::setFrustumCulling);
        QObject::connect(viewConfiguration, &ViewConfiguration::skinningChanged, view, &View::setSkinning);
        QObject::connect(viewConfiguration, &ViewConfiguration::backToFrontSortingChanged, view, &View::setBackToFrontSorting);
        QObject::connect(viewConfiguration, &ViewConfiguration::zFillingChanged, view, &View::setZFilling);
        QObject::connect(viewConfiguration, &ViewConfiguration::particlesEnabledChanged, view, &View::setParticlesEnabled);
        QObject::connect(viewConfiguration, &ViewConfiguration::toneMappingAlgorithmChanged, view, &View::setToneMappingAlgorithm);
        QObject::connect(viewConfiguration, &ViewConfiguration::usesStencilMaskChanged, view, &View::setUsesStencilMask);
        QObject::connect(viewConfiguration, &ViewConfiguration::exposureChanged, view, &View::setExposure);
        QObject::connect(viewConfiguration, &ViewConfiguration::gammaChanged, view, &View::setGamma);
        QObject::connect(viewConfiguration, &ViewConfiguration::clearColorChanged, view, &View::setClearColor);

        view->setViewportRect(viewConfiguration->viewportRect());
        view->setFrustumCulling(viewConfiguration->frustumCulling());
        view->setSkinning(viewConfiguration->skinning());
        view->setBackToFrontSorting(viewConfiguration->backToFrontSorting());
        view->setZFilling(viewConfiguration->zFilling());
        view->setParticlesEnabled(viewConfiguration->particlesEnabled());
        view->setToneMappingAlgorithm(viewConfiguration->toneMappingAlgorithm());
        view->setUsesStencilMask(viewConfiguration->usesStencilMask());
        view->setExposure(viewConfiguration->exposure());
        view->setGamma(viewConfiguration->gamma());
        view->setClearColor(viewConfiguration->clearColor());

        const std::vector<Kuesa::AbstractPostProcessingEffect *> &fxs = viewConfiguration->postProcessingEffects();
        for (Kuesa::AbstractPostProcessingEffect *fx : fxs)
            view->addPostProcessingEffect(fx);

        retrieveAndSetCamera(viewConfiguration->cameraName(), view);
        retrieveAndSetLayers(viewConfiguration->layerNames(), view);

        updateTransformTrackers(viewConfiguration->transformTrackers(), view);
        updatePlaceholderTrackers(viewConfiguration->placeholderTrackers(), view);

        auto triggerUpdateTransformTracker = [this, viewConfiguration, view]() {
            updateTransformTrackers(viewConfiguration->transformTrackers(), view);
        };
        auto triggerUpdatePlaceholderTracker = [this, viewConfiguration, view]() {
            updatePlaceholderTrackers(viewConfiguration->placeholderTrackers(), view);
        };
        auto triggerRetrieveAndSetCamera = [this, viewConfiguration, view]() {
            retrieveAndSetCamera(viewConfiguration->cameraName(), view);
        };
        auto triggerRetrieveAndSetLayers = [this, viewConfiguration, view]() {
            retrieveAndSetLayers(viewConfiguration->layerNames(), view);
        };
        auto removeFx = [view](Kuesa::AbstractPostProcessingEffect *fx) {
            view->removePostProcessingEffect(fx);
        };
        auto addFx = [view](Kuesa::AbstractPostProcessingEffect *fx) {
            view->addPostProcessingEffect(fx);
        };

        using ConVec = std::vector<QMetaObject::Connection>;
        const std::vector<ConVec> connections = {
            { QObject::connect(this, &View3DScene::screenSizeChanged, triggerRetrieveAndSetCamera) },
            { QObject::connect(viewConfiguration, &ViewConfiguration::cameraNameChanged, triggerRetrieveAndSetCamera) },
            { QObject::connect(viewConfiguration, &ViewConfiguration::layerNamesChanged, triggerRetrieveAndSetLayers) },
            connectSignalsToCallback(viewConfiguration, triggerUpdateTransformTracker,
                                     &ViewConfiguration::cameraNameChanged,
                                     &ViewConfiguration::viewportRectChanged,
                                     &ViewConfiguration::transformTrackerAdded,
                                     &ViewConfiguration::transformTrackerRemoved),
            { QObject::connect(this, &View3DScene::screenSizeChanged, triggerUpdateTransformTracker) },
            connectSignalsToCallback(viewConfiguration, triggerUpdatePlaceholderTracker,
                                     &ViewConfiguration::cameraNameChanged,
                                     &ViewConfiguration::viewportRectChanged,
                                     &ViewConfiguration::placeholderTrackerAdded,
                                     &ViewConfiguration::placeholderTrackerRemoved),
            { QObject::connect(this, &View3DScene::screenSizeChanged, triggerUpdatePlaceholderTracker) },
            { QObject::connect(viewConfiguration, &ViewConfiguration::postProcessingEffectAdded, view, addFx),
              QObject::connect(viewConfiguration, &ViewConfiguration::postProcessingEffectRemoved, view, removeFx) }
        };

        m_viewConfigurationsResources.emplace_back(ViewConfigurationResources{ viewConfiguration,
                                                                               view,
                                                                               join(connections) });
        // Will automatically set parent on view
        m_frameGraph->addView(view);
    }
}

void View3DScene::removeViewConfiguration(ViewConfiguration *viewConfiguration)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(viewConfiguration);

    auto it = std::find_if(std::begin(m_viewConfigurationsResources),
                           std::end(m_viewConfigurationsResources),
                           [viewConfiguration](const auto &v) {
                               return v.viewConfiguration == viewConfiguration;
                           });
    if (it != std::end(m_viewConfigurationsResources)) {
        m_frameGraph->removeView(it->view);
        delete it->view;

        for (const auto &connection : it->connections)
            QObject::disconnect(connection);

        m_viewConfigurationsResources.erase(it);
    }
}

void View3DScene::clearViewConfigurations()
{
    const auto viewConfigurationsResouces = m_viewConfigurationsResources;
    for (auto &v : viewConfigurationsResouces)
        removeViewConfiguration(v.viewConfiguration);
}

/*!
    \internal
    \brief Adds \a animation to the list of managed \l {Kuesa::AnimationPlayer}
    instances of the View3DScene.
 */
void View3DScene::addAnimationPlayer(AnimationPlayer *animation)
{
    if (std::find(std::begin(m_animations), std::end(m_animations), animation) == std::end(m_animations)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(animation, &View3DScene::removeAnimationPlayer, m_animations);

        if (animation->parentNode() == nullptr)
            animation->setParent(this);
        if (animation->sceneEntity() == nullptr)
            animation->setSceneEntity(this);

        m_animations.push_back(animation);
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
    m_animations.erase(std::remove(std::begin(m_animations), std::end(m_animations), animation),
                       std::end(m_animations));
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

    When switching between two \l {KuesaUtils::SceneConfiguration} instances,
    the collections, assets and gltf files are clear prior to being reloaded,
    even if both instances reference the same source file.
 */
void View3DScene::setActiveScene(SceneConfiguration *scene)
{
    if (m_activeScene != scene) {

        if (m_activeScene) {
            // Unregister previous resources
            const auto &oldAnimations = m_activeScene->animationPlayers();
            const auto &oldViews = m_activeScene->viewConfigurations();

            for (auto a : oldAnimations)
                removeAnimationPlayer(a);

            for (auto v : oldViews)
                removeViewConfiguration(v);

            Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
            d->unregisterDestructionHelper(m_activeScene);

            QObject::disconnect(m_activeScene);

            // If we took ownership of the SceneConfiguration -> restore parent
            // Make sure we unset the QNode * parent if it was overridden
            QNode *originalNodeParent = qobject_cast<QNode *>(m_activeSceneOwner.data());
            // In case parent is a QNode *
            if (originalNodeParent) {
                m_activeScene->setParent(originalNodeParent);
            } else { // Parent was not a QNode or our scene had no parent/owner at all when set
                // Reset QNode parent
                m_activeScene->setParent(Q_NODE_NULLPTR);
                static_cast<QObject *>(m_activeScene)->setParent(m_activeSceneOwner.data());
            }

            emit m_activeScene->unloadingDone();
        }

        m_activeScene = scene;
        m_activeSceneOwner.clear();
        emit activeSceneChanged(m_activeScene);

        // Set empty sources / camera to force a scene reset
        setSource(QUrl());

        if (m_activeScene) {
            // Ensure we parent the scene to a valid QNode so that resources
            // are properly added to the scene
            m_activeSceneOwner = scene->parent();

            if (m_activeSceneOwner && !qobject_cast<Qt3DCore::QNode *>(m_activeSceneOwner))
                qCWarning(kuesa_utils()) << "SceneConfiguration is parented by a non Qt3DCore::QNode object. View3DScene will reparent the SceneConfiguration and restore the original parent when switching back to a new activeScene";

            if (!m_activeScene->parentNode())
                m_activeScene->setParent(this);

            if (!m_activeScene->sceneEntity())
                m_activeScene->setSceneEntity(this);

            // Create

            Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
            d->registerDestructionHelper(m_activeScene, &View3DScene::setActiveScene, m_activeScene);

            setSource(m_activeScene->source());

            QObject::connect(m_activeScene, &SceneConfiguration::sourceChanged, this, &View3DScene::setSource);

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
