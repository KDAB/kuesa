/*
    viewconfiguration.cpp

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

#include "viewconfiguration.h"

#include <Qt3DCore/private/qnode_p.h>

/*!
    \class KuesaUtils::ViewConfiguration
    \brief ViewConfiguration provides a way of conveniently specifying
    different views of the same scene.

    \inmodule Kuesa
    \since Kuesa 1.4
    \inherits Kuesa::KuesaNode
*/

/*!
    \property KuesaUtils::ViewConfiguration::cameraName

    \brief The name of the camera asset that should be used to view the scene.
    If the name references a valid camera, the camera will automatically be
    set on the ForwardRenderer frameGraph and other internal assets such as
    \l {Kuesa::TransformTracker}.
 */

/*!
    \property KuesaUtils::ViewConfiguration::layerNames

    \brief The list of KDAB_layers layer names to be selected for rendering.
    This allows selecting only a sub part of a glTF 2 model to only render parts
    that are linked to a specific layer.
 */

/*!
    \property KuesaUtils::ViewConfiguration::usesStencilMask

    Enables/disables stencil buffers. If true, stencil operations be used during the render phase to modify the stencil buffer.
    The resulting stencil buffer can later be used to apply post process effect to only part of the scene

    \default False
 */

/*!
    \property KuesaUtils::ViewConfiguration::toneMappingAlgorithm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \default ToneMappingAndGammaCorrectionEffect::None
 */

/*!
    \property KuesaUtils::ViewConfiguration::exposure
    Exposure correction factor used before the linear to sRGB conversion.
    \default 0
 */

/*!
    \property KuesaUtils::ViewConfiguration::gamma

    Holds the gamma value to use for gamma correction that brings linear colors
    to sRGB colors.
    \default 2.2
 */

/*!
    \property KuesaUtils::ViewConfiguration::particlesEnabled

    Holds whether particles support is enabled. Disabled by default.
*/

/*!
    \property KuesaUtils::ViewConfiguration::zFilling

    Holds whether multi-pass zFilling support is enabled. Disabled by default.
*/

/*!
    \property KuesaUtils::ViewConfiguration::backToFrontSorting

    Holds whether back to front sorting to render objects in back-to-front
    order is enabled. This is required for proper alpha blending rendering.
    Enabled by default.
*/

/*!
    \property KuesaUtils::ViewConfiguration::::skinning

    Holds whether skinned mesh support is required.
    Disabled by default.
*/

/*!
    \property KuesaUtils::ViewConfiguration::frustumCulling

    Holds whether frustum culling is enabled or not. Enabled by default.
*/

/*!
    \property KuesaUtils::ViewConfiguration::viewportRect

    Holds the viewport rectangle from within which the rendering will occur.
    Rectangle is in normalized coordinates.
*/

/*!
    \property KuesaUtils::ViewConfiguration::clearColor

    Holds the color used to clear the screen at the start of each frame.
*/

/*!
    \qmltype ViewConfiguration
    \instantiates KuesaUtils::ViewConfiguration
    \inqmlmodule KuesaUtils
    \since Kuesa 1.4
    \inherits Kuesa::KuesaNode

    \brief ViewConfiguration provides a way of conveniently specifying
    different views of the same scene.
*/

/*!
    \qmlproperty string KuesaUtils::ViewConfiguration::cameraName

    \brief The name of the camera asset that should be used to view the scene.
    If the name references a valid camera, the camera will automatically be
    set on the ForwardRenderer frameGraph and other internal assets such as
    \l [QML] {Kuesa::TransformTracker}.
 */

/*!
    \qmlproperty list<string> KuesaUtils::ViewConfiguration::layerNames

    \brief The list of KDAB_layers layer names to be selected for rendering.
    This allows selecting only a sub part of a glTF 2 model to only render parts
    that are linked to a specific layer.
 */

/*!
    \qmlproperty bool KuesaUtils::ViewConfiguration::usesStencilMask

    Enables/disables stencil buffers. If true, stencil operations be used during the render phase to modify the stencil buffer.
    The resulting stencil buffer can later be used to apply post process effect to only part of the scene

    \default False
 */

/*!
    \qmlproperty ToneMappingAndGammaCorrectionEffect.ToneMapping KuesaUtils::ViewConfiguration::toneMappingAlgorithm

    Tone mapping specifies how we perform color conversion from HDR (high
    dynamic range) content to LDR (low dynamic range) content which our monitor
    displays.

    \default ToneMappingAndGammaCorrectionEffect.None
 */

/*!
    \qmlproperty real KuesaUtils::ViewConfiguration::exposure
    Exposure correction factor used before the linear to sRGB conversion.
    \default 0
 */

/*!
    \qmlproperty real KuesaUtils::ViewConfiguration::gamma

    Holds the gamma value to use for gamma correction that brings linear colors
    to sRGB colors.
    \default 2.2
 */

/*!
    \qmlproperty bool KuesaUtils::ViewConfiguration::particlesEnabled

    Holds whether particles support is enabled. Disabled by default.
*/

/*!
    \qmlproperty bool KuesaUtils::ViewConfiguration::zFilling

    Holds whether multi-pass zFilling support is enabled. Disabled by default.
*/

/*!
    \qmlproperty bool KuesaUtils::ViewConfiguration::backToFrontSorting

    Holds whether back to front sorting to render objects in back-to-front
    order is enabled. This is required for proper alpha blending rendering.
    Enabled by default.
*/

/*!
    \qmlproperty bool KuesaUtils::ViewConfiguration::::skinning

    Holds whether skinned mesh support is required.
    Disabled by default.
*/

/*!
    \qmlproperty bool KuesaUtils::ViewConfiguration::frustumCulling

    Holds whether frustum culling is enabled or not. Enabled by default.
*/

/*!
    \qmlproperty rect KuesaUtils::ViewConfiguration::viewportRect

    Holds the viewport rectangle from within which the rendering will occur.
    Rectangle is in normalized coordinates.
*/

/*!
    \qmlproperty color KuesaUtisl::ViewConfiguration::clearColor

    Holds the color used to clear the screen at the start of each frame.
*/

QT_BEGIN_NAMESPACE

KuesaUtils::ViewConfiguration::ViewConfiguration(Qt3DCore::QNode *parent)
    : Kuesa::KuesaNode(parent)
{
}

QString KuesaUtils::ViewConfiguration::cameraName() const
{
    return m_cameraName;
}

QStringList KuesaUtils::ViewConfiguration::layerNames() const
{
    return m_layerNames;
}

QRectF KuesaUtils::ViewConfiguration::viewportRect() const
{
    return m_viewportRect;
}

bool KuesaUtils::ViewConfiguration::frustumCulling() const
{
    return m_frustumCulling;
}

bool KuesaUtils::ViewConfiguration::skinning() const
{
    return m_skinning;
}

bool KuesaUtils::ViewConfiguration::backToFrontSorting() const
{
    return m_backToFrontSorting;
}

bool KuesaUtils::ViewConfiguration::zFilling() const
{
    return m_zFilling;
}

bool KuesaUtils::ViewConfiguration::particlesEnabled() const
{
    return m_particlesEnabled;
}

Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping KuesaUtils::ViewConfiguration::toneMappingAlgorithm() const
{
    return m_toneMappingAlgorithm;
}

bool KuesaUtils::ViewConfiguration::usesStencilMask() const
{
    return m_usesStencilMask;
}

float KuesaUtils::ViewConfiguration::exposure() const
{
    return m_exposure;
}

float KuesaUtils::ViewConfiguration::gamma() const
{
    return m_gamma;
}

QColor KuesaUtils::ViewConfiguration::clearColor() const
{
    return m_clearColor;
}

const std::vector<Kuesa::TransformTracker *> &KuesaUtils::ViewConfiguration::transformTrackers() const
{
    return m_transformTrackers;
}

void KuesaUtils::ViewConfiguration::addTransformTracker(Kuesa::TransformTracker *tracker)
{
    if (std::find(std::begin(m_transformTrackers), std::end(m_transformTrackers), tracker) == std::end(m_transformTrackers)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(tracker, &ViewConfiguration::removeTransformTracker, m_transformTrackers);
        if (tracker->parentNode() == nullptr)
            tracker->setParent(this);
        m_transformTrackers.push_back(tracker);
        emit transformTrackerAdded(tracker);
    }
}

void KuesaUtils::ViewConfiguration::removeTransformTracker(Kuesa::TransformTracker *tracker)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(tracker);
    auto it = std::remove(std::begin(m_transformTrackers), std::end(m_transformTrackers), tracker);
    if (it != std::end(m_transformTrackers)) {
        m_transformTrackers.erase(it);
        emit transformTrackerRemoved(tracker);
    }
}

void KuesaUtils::ViewConfiguration::clearTransformTrackers()
{
    const std::vector<Kuesa::TransformTracker *> trackersCopy = m_transformTrackers;
    for (Kuesa::TransformTracker *t : trackersCopy)
        removeTransformTracker(t);
}

const std::vector<Kuesa::PlaceholderTracker *> &KuesaUtils::ViewConfiguration::placeholderTrackers() const
{
    return m_placeholderTrackers;
}

void KuesaUtils::ViewConfiguration::addPlaceholderTracker(Kuesa::PlaceholderTracker *tracker)
{
    if (std::find(std::begin(m_placeholderTrackers), std::end(m_placeholderTrackers), tracker) == std::end(m_placeholderTrackers)) {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(tracker, &ViewConfiguration::removePlaceholderTracker, m_placeholderTrackers);
        if (tracker->parentNode() == nullptr)
            tracker->setParent(this);
        m_placeholderTrackers.push_back(tracker);
        emit placeholderTrackerAdded(tracker);
    }
}

void KuesaUtils::ViewConfiguration::removePlaceholderTracker(Kuesa::PlaceholderTracker *tracker)
{
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(tracker);
    auto it = std::remove(std::begin(m_placeholderTrackers), std::end(m_placeholderTrackers), tracker);
    if (it != std::end(m_placeholderTrackers)) {
        m_placeholderTrackers.erase(it);
        emit placeholderTrackerRemoved(tracker);
    }
}

void KuesaUtils::ViewConfiguration::clearPlaceholderTrackers()
{
    const std::vector<Kuesa::PlaceholderTracker *> trackersCopy = m_placeholderTrackers;
    for (Kuesa::PlaceholderTracker *t : trackersCopy)
        removePlaceholderTracker(t);
}

void KuesaUtils::ViewConfiguration::setCameraName(const QString &cameraName)
{
    if (m_cameraName != cameraName) {
        m_cameraName = cameraName;
        emit cameraNameChanged(cameraName);
    }
}

void KuesaUtils::ViewConfiguration::setLayerNames(const QStringList &layerNames)
{
    if (m_layerNames != layerNames) {
        m_layerNames = layerNames;
        emit layerNamesChanged(layerNames);
    }
}

void KuesaUtils::ViewConfiguration::setViewportRect(const QRectF &viewportRect)
{
    if (m_viewportRect != viewportRect) {
        m_viewportRect = viewportRect;
        emit viewportRectChanged(m_viewportRect);
    }
}

void KuesaUtils::ViewConfiguration::setFrustumCulling(bool frustumCulling)
{
    if (frustumCulling != m_frustumCulling) {
        m_frustumCulling = frustumCulling;
        emit frustumCullingChanged(m_frustumCulling);
    }
}

void KuesaUtils::ViewConfiguration::setSkinning(bool skinning)
{
    if (m_skinning != skinning) {
        m_skinning = skinning;
        emit skinningChanged(skinning);
    }
}

void KuesaUtils::ViewConfiguration::setBackToFrontSorting(bool backToFrontSorting)
{
    if (m_backToFrontSorting != backToFrontSorting) {
        m_backToFrontSorting = backToFrontSorting;
        emit backToFrontSortingChanged(m_backToFrontSorting);
    }
}

void KuesaUtils::ViewConfiguration::setZFilling(bool zFilling)
{
    if (m_zFilling != zFilling) {
        m_zFilling = zFilling;
        emit zFillingChanged(zFilling);
    }
}

void KuesaUtils::ViewConfiguration::setParticlesEnabled(bool particlesEnabled)
{
    if (m_particlesEnabled != particlesEnabled) {
        m_particlesEnabled = particlesEnabled;
        emit particlesEnabledChanged(particlesEnabled);
    }
}

void KuesaUtils::ViewConfiguration::setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping algorithm)
{
    if (m_toneMappingAlgorithm != algorithm) {
        m_toneMappingAlgorithm = algorithm;
        emit toneMappingAlgorithmChanged(algorithm);
    }
}

void KuesaUtils::ViewConfiguration::setUsesStencilMask(bool usesStencilMask)
{
    if (m_usesStencilMask != usesStencilMask) {
        m_usesStencilMask = usesStencilMask;
        emit usesStencilMaskChanged(usesStencilMask);
    }
}

void KuesaUtils::ViewConfiguration::setExposure(float exposure)
{
    if (m_exposure != exposure) {
        m_exposure = exposure;
        emit exposureChanged(exposure);
    }
}

void KuesaUtils::ViewConfiguration::setGamma(float gamma)
{
    if (m_gamma != gamma) {
        m_gamma = gamma;
        emit gammaChanged(gamma);
    }
}

void KuesaUtils::ViewConfiguration::setClearColor(QColor clearColor)
{
    if (m_clearColor != clearColor) {
        m_clearColor = clearColor;
        emit clearColorChanged(clearColor);
    }
}

QT_END_NAMESPACE
