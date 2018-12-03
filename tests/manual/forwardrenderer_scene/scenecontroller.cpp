/*
    scenecontroller.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#include "scenecontroller.h"

SceneController::SceneController(QObject *parent)
    : QObject(parent)
    , m_clearColor("white")
    , m_alphaBlendingEnabled(false)
    , m_frustumCullingEnabled(true)
    , m_zFillEnabled(false)
    , m_bloomFilter(false)
    , m_bloomThreshold(0.6f)
    , m_bloomExposure(0.0f)
    , m_bloomBlurPassCount(8)
    , m_blurEffect(false)
    , m_blurPassCount(8)
    , m_thresholdEffect(false)
    , m_threshold(.5)
    , m_opacityMaskEffect(false)
    , m_opacityMaskPremultipliedAlpha(false)
{
    m_scenes << "ThreeSpheres" << "BloomTest";
    m_sceneName = m_scenes.at(0);
}

QColor SceneController::clearColor() const
{
    return m_clearColor;
}

bool SceneController::alphaBlending() const
{
    return m_alphaBlendingEnabled;
}

bool SceneController::frustumCulling() const
{
    return m_frustumCullingEnabled;
}

bool SceneController::bloomEffect() const
{
    return m_bloomFilter;
}

float SceneController::bloomThreshold() const
{
    return m_bloomThreshold;
}

float SceneController::bloomExposure() const
{
    return m_bloomExposure;
}

bool SceneController::thresholdEffect() const
{
    return m_thresholdEffect;
}

float SceneController::threshold() const
{
    return m_threshold;
}

QStringList SceneController::scenes() const
{
    return m_scenes;
}

QString SceneController::sceneName() const
{
    return m_sceneName;
}

bool SceneController::blurEffect() const
{
    return m_blurEffect;
}

int SceneController::blurPassCount() const
{
    return m_blurPassCount;
}

bool SceneController::opacityMaskEffect() const
{
    return m_opacityMaskEffect;
}

bool SceneController::opacityMaskPremultipliedAlpha() const
{
    return m_opacityMaskPremultipliedAlpha;
}

bool SceneController::zFill() const
{
    return m_zFillEnabled;
}

int SceneController::bloomBlurPassCount() const
{
    return m_bloomBlurPassCount;
}

void SceneController::setClearColor(const QColor &baseColorFactor)
{
    if (m_clearColor == baseColorFactor)
        return;

    m_clearColor = baseColorFactor;
    emit clearColorChanged(baseColorFactor);
}

void SceneController::setAlphaBlending(bool useBaseColorMap)
{
    if (m_alphaBlendingEnabled == useBaseColorMap)
        return;

    m_alphaBlendingEnabled = useBaseColorMap;
    emit alphaBlendingChanged(useBaseColorMap);
}

void SceneController::setFrustumCulling(bool frustumCulling)
{
    if (m_frustumCullingEnabled == frustumCulling)
        return;

    m_frustumCullingEnabled = frustumCulling;
    emit frustumCullingChanged(m_frustumCullingEnabled);
}

void SceneController::setBloomEffect(bool bloomFilter)
{
    if (m_bloomFilter == bloomFilter)
        return;

    m_bloomFilter = bloomFilter;
    emit bloomFilterChanged(m_bloomFilter);
}

void SceneController::setBloomThreshold(float bloomThreshold)
{
    if (qFuzzyCompare(m_bloomThreshold, bloomThreshold))
        return;

    m_bloomThreshold = bloomThreshold;
    emit bloomThresholdChanged(m_bloomThreshold);
}

void SceneController::setBloomExposure(float bloomExposure)
{
    if (qFuzzyCompare(m_bloomExposure, bloomExposure))
        return;

    m_bloomExposure = bloomExposure;
    emit bloomExposureChanged(m_bloomExposure);
}

void SceneController::setThresholdEffect(bool thresholdEffect)
{
    if (m_thresholdEffect == thresholdEffect)
        return;

    m_thresholdEffect = thresholdEffect;
    emit thresholdEffectChanged(m_thresholdEffect);
}

void SceneController::setThreshold(float threshold)
{
    if (qFuzzyCompare(m_threshold, threshold))
        return;

    m_threshold = threshold;
    emit thresholdChanged(m_threshold);
}

void SceneController::setSceneName(const QString &sceneName)
{
    if (m_sceneName == sceneName || !m_scenes.contains(sceneName))
        return;

    m_sceneName = sceneName;
    emit sceneNameChanged(m_sceneName);
}

void SceneController::setBlurEffect(bool blurEffect)
{
    if (m_blurEffect == blurEffect)
        return;

    m_blurEffect = blurEffect;
    emit blurEffectChanged(m_blurEffect);
}

void SceneController::setBlurPassCount(int blurPassCount)
{
    if (m_blurPassCount == blurPassCount)
        return;

    m_blurPassCount = blurPassCount;
    emit blurPassCountChanged(m_blurPassCount);
}

void SceneController::setOpacityMaskEffect(bool opacityMaskEffect)
{
    if (m_opacityMaskEffect == opacityMaskEffect)
        return;
    m_opacityMaskEffect = opacityMaskEffect;
    emit opacityMaskEffectChanged(m_opacityMaskEffect);
}

void SceneController::setOpacityMaskPremultipliedAlpha(bool premultipliedAlpha)
{
    if (m_opacityMaskPremultipliedAlpha == premultipliedAlpha)
        return;
    m_opacityMaskPremultipliedAlpha = premultipliedAlpha;
    emit opacityMaskPremultipliedAlphaChanged(m_opacityMaskPremultipliedAlpha);
}

void SceneController::setZFill(bool zFill)
{
    if (m_zFillEnabled == zFill)
        return;

    m_zFillEnabled = zFill;
    emit zFillChanged(m_zFillEnabled);
}

void SceneController::setBloomBlurPassCount(int bloomBlurPassCount)
{
    if (m_bloomBlurPassCount == bloomBlurPassCount)
        return;

    m_bloomBlurPassCount = bloomBlurPassCount;
    emit bloomBlurPassCountChanged(m_bloomBlurPassCount);
}
