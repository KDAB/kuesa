/*
    view.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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

#include "view.h"
#include <Kuesa/reflectionplane.h>
#include <private/kuesa_utils_p.h>
#include <private/kuesa_p.h>
#include <private/scenestages_p.h>
#include <private/effectsstages_p.h>
#include <private/reflectionstages_p.h>
#include <Qt3DCore/private/qnode_p.h>

#include <Qt3DRender/QLayer>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::View
    \brief View allows to specify the rendering description for a rendered view
    of the scene. They are to be used along with the QForwardRenderer
    FrameGraph.
    \inmodule Kuesa
    \since Kuesa 1.3
    \inherits Qt3DRender::QFrameGraphNode

    View allows to specify the description of a rendered view of the scene.
    They are to be used along with the QForwardRenderer FrameGraph.

    This is especially useful to render on the same window multiple views of
    different subset of content coming from a glTF file and scene from
    different view points.

    Views are rendered in the order in which they were added to the ForwardRenderer.

    \badcode
    Kuesa::ForwardRenderer *frameGraph = new Kuesa::ForwardRenderer();

    Kuesa::View *view1 = new Kuesa::View();
    view1->setCamera(sceneCamera1);
    view1->setViewport(QRectF(0, 0, 0.5, 1);

    Kuesa::View *view2 = new Kuesa::View();
    view2->setCamera(sceneCamera2);
    view2->setViewport(QRectF(0.5, 0, 0.5, 1);
    view2->addLayer(groundLayer);

    frameGraph->addView(view1);
    frameGraph->addView(view2);
    \endcode

*/

/*!
    \qmltype View
    \brief View allows to specify the description of a rendered view of the scene.
    They are to be used along with the ForwardRenderer FrameGraph.
    \inqmlmodule Kuesa
    \since Kuesa 1.3
    \instantiates Kuesa::View

    View allows to specify the rendering description for a rendered view
    of the scene. They are to be used along with the ForwardRenderer FrameGraph.

    This is especially useful to render on the same window multiple views of
    different subset of content coming from a glTF file and scene from
    different view points.

    Views are rendered in the order in which they were added to the ForwardRenderer.

    \badcode
    ForwardRenderer {
        views: [
            View {
                camera: sceneCamera1
                viewport: Qt.rectf(0, 0, 0.5, 1)
            },
            View {
                camera: sceneCamera2
                viewport: Qt.rectf(0.5, 0, 0.5, 1)
                layers: [ groundLayer ]
            }
        ]
    }
    \endcode
*/

View::View(Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(parent)
    , m_sceneStages(SceneStagesPtr::create())
    , m_reflectionStages(ReflectionStagesPtr::create())
    , m_fxStages(EffectsStagesPtr::create())
{
    rebuildFGTree();
}

View::~View()
{
}

/*!
    \property Kuesa::View::viewportRect

    Holds the viewport rectangle from within which the rendering will occur.
    Rectangle is in normalized coordinates.
*/

/*!
    \qmlproperty rect Kuesa::View::viewportRect

    Holds the viewport rectangle from within which the rendering will occur.
    Rectangle is in normalized coordinates.
*/

QRectF View::viewportRect() const
{
    return m_viewport;
}

/*!
    \property Kuesa::View::camera

    Holds the camera used to view the scene.
*/

/*!
    \qmlproperty Entity Kuesa::View::camera

    Holds the camera used to view the scene.
*/

Qt3DCore::QEntity *View::camera() const
{
    return m_camera;
}

/*!
    \property Kuesa::View::frustumCulling

    Holds whether frustum culling is enabled or not. Enabled by default.
*/

/*!
    \qmlproperty bool Kuesa::View::frustumCulling

    Holds whether frustum culling is enabled or not. Enabled by default.
*/
bool View::frustumCulling() const
{
    return m_features & FrustumCulling;
}

/*!
    \property Kuesa::View::skinning

    Holds whether skinned mesh support is required.
    Disabled by default.
*/

/*!
    \qmlproperty bool Kuesa::View::skinning

    Holds whether skinned mesh support is required.
    Disabled by default.
*/
bool View::skinning() const
{
    return m_features & Skinning;
}

/*!
    \property Kuesa::View::backToFrontSorting

    Holds whether back to front sorting to render objects in back-to-front
    order is enabled. This is required for proper alpha blending rendering.
    Disabled by default.
*/

/*!
    \qmlproperty bool Kuesa::View::backToFrontSorting

    Holds whether back to front sorting to render objects in back-to-front
    order is enabled. This is required for proper alpha blending rendering.
    Disabled by default.
*/
bool View::backToFrontSorting() const
{
    return m_features & BackToFrontSorting;
}

/*!
    \property Kuesa::View::zFilling

    Holds whether multi-pass zFilling support is enabled. Disabled by default.
*/

/*!
    \qmlproperty bool Kuesa::View::zFilling

    Holds whether multi-pass zFilling support is enabled. Disabled by default.
*/
bool View::zFilling() const
{
    return m_features & ZFilling;
}

/*!
    \property Kuesa::View::particlesEnabled

    Holds whether particles support is enabled. Disabled by default.
*/

/*!
    \qmlproperty bool Kuesa::View::particlesEnabled

    Holds whether particles support is enabled. Disabled by default.
*/
bool View::particlesEnabled() const
{
    return m_features & Particles;
}


/*!
    \qmlproperty list<AbstractPostProcessingEffect> Kuesa::ForwardRenderer::postProcessingEffects

    Holds the list of post processing effects.

    In essence this will complete the FrameGraph tree with a
    dedicated subtree provided by the effect.

    Lifetime of the subtree will be entirely managed by the ForwardRenderer.

    Be aware that registering several effects of the same type might have
    unexpected behavior. It is advised against unless explicitly documented in
    the effect.

    The FrameGraph tree is reconfigured upon replacing the list of effects.
*/
const std::vector<AbstractPostProcessingEffect *> &View::postProcessingEffects() const
{
    return m_fxs;
}

/*!
 * Returns all layers used by the view.
 */
const std::vector<Qt3DRender::QLayer *> &View::layers() const
{
    return m_layers;
}

const std::vector<ReflectionPlane *> &View::reflectionPlanes() const
{
    return m_reflectionPlanes;
}

/*!
 * Registers a new post processing effect \a effect with the View.
 * In essence this will complete the FrameGraph tree with a
 * dedicated subtree provided by the effect.
 *
 * Lifetime of the subtree will be entirely managed by the View.
 *
 * Be aware that registering several effects of the same type might have
 * unexpected behavior. It is advised against unless explicitly documented in
 * the effect.
 *
 * The FrameGraph tree is reconfigured upon insertion of a new effect.
 *
 * Effects added to a view, contrary to effects added to the ForwardRenderer,
 * only affect the view.
 */
void View::addPostProcessingEffect(AbstractPostProcessingEffect *effect)
{
    if (Utils::contains(m_fxs, effect))
        return;

    // Add effect to vector of registered effects
    m_fxs.push_back(effect);

    // Handle destruction of effect
    auto d = Qt3DCore::QNodePrivate::get(this);
    d->registerDestructionHelper(effect, &View::removePostProcessingEffect, m_fxs);

    // Request FG Tree rebuild
    rebuildFGTree();
}

/*!
 * Unregisters \a effect from the current View. This will destroy the
 * FrameGraph subtree associated with the effect.
 *
 * The FrameGraph tree is reconfigured upon removal of an effect.
 */
void View::removePostProcessingEffect(AbstractPostProcessingEffect *effect)
{
    // Remove effect entry
    if (Utils::removeAll(m_fxs, effect) <= 0)
        return;

    // Stop watching for effect destruction
    auto d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(effect);

    // Request FG Tree rebuild
    rebuildFGTree();
}

void View::addLayer(Qt3DRender::QLayer *layer)
{
    if (layer) {
        // Parent it to the View if they have no parent
        // We want to avoid them ever being parented to the SceneStages
        // As those might be destroyed when rebuilding the FG
        if (!layer->parent())
            layer->setParent(this);

        auto d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(layer, &View::removeLayer, m_layers);
        m_layers.push_back(layer);
        reconfigureStages();
    }
}

void View::removeLayer(Qt3DRender::QLayer *layer)
{
    auto it = std::find(std::begin(m_layers), std::end(m_layers), layer);
    if (it != std::end(m_layers)) {
        auto d = Qt3DCore::QNodePrivate::get(this);
        d->unregisterDestructionHelper(layer);
        m_layers.erase(it);
        reconfigureStages();
    }
}

void View::addReflectionPlane(ReflectionPlane *plane)
{
    if (plane) {
        // Parent it to the View if they have no parent
        // We want to avoid them ever being parented to the SceneStages
        // As those might be destroyed when rebuilding the FG
        if (!plane->parent())
            plane->setParent(this);

        auto d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(plane, &View::removeReflectionPlane, m_reflectionPlanes);
        m_reflectionPlanes.push_back(plane);
        rebuildFGTree();

        QObject::connect(plane, &ReflectionPlane::equationChanged,
                         this, &View::rebuildFGTree);
        QObject::connect(plane, &ReflectionPlane::layersChanged,
                         this, &View::rebuildFGTree);
    }
}

void View::removeReflectionPlane(ReflectionPlane *plane)
{
    auto it = std::find(std::begin(m_reflectionPlanes), std::end(m_reflectionPlanes), plane);
    if (it != std::end(m_reflectionPlanes)) {
        auto d = Qt3DCore::QNodePrivate::get(this);
        d->unregisterDestructionHelper(plane);
        m_reflectionPlanes.erase(it);
        rebuildFGTree();
        plane->disconnect(this);
    }
}

void View::setViewportRect(const QRectF &viewportRect)
{
    if (m_viewport == viewportRect)
        return;
     m_viewport = viewportRect;
     emit viewportRectChanged(viewportRect);
     reconfigureStages();
}

void View::setCamera(Qt3DCore::QEntity *camera)
{
    if (m_camera == camera)
        return;

    auto d = Qt3DCore::QNodePrivate::get(this);
    if (m_camera)
        d->unregisterDestructionHelper(m_camera);

    m_camera = camera;

    if (m_camera) {
        if (!m_camera->parent())
            m_camera->setParent(this);
        d->registerDestructionHelper(m_camera, &View::setCamera, m_camera);
    }

    const bool blocked = blockNotifications(true);
    emit cameraChanged(m_camera);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::setFrustumCulling(bool frustumCulling)
{
    if (frustumCulling == bool(m_features & FrustumCulling))
        return;
    m_features.setFlag(FrustumCulling, frustumCulling);
    const bool blocked = blockNotifications(true);
    emit frustumCullingChanged(frustumCulling);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::setSkinning(bool skinning)
{
    if (skinning == bool(m_features & Skinning))
        return;
    m_features.setFlag(Skinning, skinning);
    const bool blocked = blockNotifications(true);
    emit skinningChanged(skinning);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::setBackToFrontSorting(bool backToFrontSorting)
{
    if (backToFrontSorting == bool(m_features & BackToFrontSorting))
        return;
    m_features.setFlag(BackToFrontSorting, backToFrontSorting);
    const bool blocked = blockNotifications(true);
    emit backToFrontSortingChanged(backToFrontSorting);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::setZFilling(bool zfilling)
{
    if (zfilling == bool(m_features & ZFilling))
        return;
    m_features.setFlag(ZFilling, zfilling);
    const bool blocked = blockNotifications(true);
    emit zFillingChanged(zfilling);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::setParticlesEnabled(bool enabled)
{
    if (enabled == bool(m_features & Particles))
        return;
    m_features.setFlag(Particles, enabled);
    const bool blocked = blockNotifications(true);
    emit particlesEnabledChanged(enabled);
    blockNotifications(blocked);
    reconfigureStages();
}

void View::scheduleFGTreeRebuild()
{
    if (!m_fgTreeRebuiltScheduled) {
        m_fgTreeRebuiltScheduled = true;
        QMetaObject::invokeMethod(this, &View::rebuildFGTree, Qt::QueuedConnection);
    }
}

void View::rebuildFGTree()
{
    m_fgTreeRebuiltScheduled = false;
    // Reconfigure FrameGraph Tree
    reconfigureStages();
    reconfigureFrameGraph();
}

/*!
 * \internal
 *
 * Returns the root pointer of the FrameGraph subtree registered by the \a
 * effect. nullptr will be returned if no subtree has been registered or if the
 * effect is invalid.
 */
AbstractPostProcessingEffect::FrameGraphNodePtr View::frameGraphSubtreeForPostProcessingEffect(AbstractPostProcessingEffect *effect) const
{
    return m_effectFGSubtrees.value(effect, nullptr);
}

void View::reconfigureStages()
{
    const bool useSkinning = bool(m_features & Skinning);
    const bool useZFilling = bool(m_features & ZFilling);
    const bool sortBackToFront = bool(m_features & BackToFrontSorting);
    const bool useFrustumCulling = bool(m_features & FrustumCulling);

    // Scene Stages
    m_sceneStages->setBackToFrontSorting(sortBackToFront);
    m_sceneStages->setSkinning(useSkinning);
    m_sceneStages->setZFilling(useZFilling);
    m_sceneStages->setFrustumCulling(useFrustumCulling);
    m_sceneStages->setCamera(m_camera);
    m_sceneStages->setViewport(m_viewport);

    // Update layers on sceneStages
    // Remove old layers
    const QVector<Qt3DRender::QLayer *> oldLayers = m_sceneStages->layers();
    for (Qt3DRender::QLayer *l : oldLayers)
        m_sceneStages->removeLayer(l);

    // Add new ones
    for (Qt3DRender::QLayer *l : m_layers)
        m_sceneStages->addLayer(l);

    // FX
    m_fxStages->setCamera(m_camera);
    m_fxStages->setViewport(m_viewport);

    // Update fxs on effectsStages
    const std::vector<Kuesa::AbstractPostProcessingEffect *> oldFXs = m_fxStages->effects();
    for (Kuesa::AbstractPostProcessingEffect *fx : oldFXs)
        m_fxStages->removeEffect(fx);

    for (Kuesa::AbstractPostProcessingEffect *fx : m_fxs) {
        m_fxStages->addEffect(fx);
        fx->setViewportRect(m_viewport);
    }

    // Reflections Stages
    m_reflectionStages->setBackToFrontSorting(sortBackToFront);
    m_reflectionStages->setSkinning(useSkinning);
    m_reflectionStages->setZFilling(useZFilling);
    m_reflectionStages->setFrustumCulling(useFrustumCulling);
    m_reflectionStages->setCamera(m_camera);
    m_reflectionStages->setViewport(m_viewport);

    const bool needsReflections = m_reflectionPlanes.size() > 0;
    if (needsReflections) {
        // Remove layers
        const QVector<Qt3DRender::QLayer *> oldReflectionLayers = m_reflectionStages->layers();
        for (Qt3DRender::QLayer *l : oldLayers)
            m_reflectionStages->removeLayer(l);
        const std::vector<Qt3DRender::QLayer *> &reflectionVisibleLayers =
                m_reflectionPlanes[0]->layers().size() > 0 ? m_reflectionPlanes[0]->layers() : m_layers;
        // Readd layers
        for (Qt3DRender::QLayer *l : reflectionVisibleLayers)
            m_reflectionStages->addLayer(l);
        // Set equation
        m_reflectionStages->setReflectivePlaneEquation(m_reflectionPlanes[0]->equation());
    }
}

void View::reconfigureFrameGraph()
{
    m_sceneStages->setParent(Q_NODE_NULLPTR);
    m_reflectionStages->setParent(Q_NODE_NULLPTR);

    // We draw reflections prior to drawing the scene
    const bool needsReflections = m_reflectionPlanes.size() > 0;
    if (needsReflections)
        m_reflectionStages->setParent(this);

    m_sceneStages->setParent(this);

    // FXs
    // Those are handled by the ForwardRenderer
    // which takes care of adding the EffectStage FG node
    // at the proper place in the FG tree
}

View *View::rootView() const
{
    Qt3DCore::QNode *parent = parentNode();
    View *parentView = qobject_cast<View *>(parent);

    // Find nearest parent that is a parent view
    while (parent != nullptr && parentView == nullptr) {
        parent = parentNode();
        parentView = qobject_cast<View *>(parent);
    }

    if (parentView)
        return parentView->rootView();
    return const_cast<View *>(this);
}

} // Kuesa

QT_END_NAMESPACE
