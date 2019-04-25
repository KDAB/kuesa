/*
    scenecontroller.h

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef SCENECONTROLLER_H
#define SCENECONTROLLER_H

#include <QObject>
#include <QColor>
#include <QStringList>

class SceneController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList scenes READ scenes CONSTANT)
    Q_PROPERTY(QString sceneName READ sceneName WRITE setSceneName NOTIFY sceneNameChanged)
    Q_PROPERTY(QColor clearColor READ clearColor WRITE setClearColor NOTIFY clearColorChanged)
    Q_PROPERTY(bool alphaBlending READ alphaBlending WRITE setAlphaBlending NOTIFY alphaBlendingChanged)
    Q_PROPERTY(bool frustumCulling READ frustumCulling WRITE setFrustumCulling NOTIFY frustumCullingChanged)
    Q_PROPERTY(bool zFill READ zFill WRITE setZFill NOTIFY zFillChanged)

    Q_PROPERTY(bool bloomEffect READ bloomEffect WRITE setBloomEffect NOTIFY bloomFilterChanged)
    Q_PROPERTY(float bloomThreshold READ bloomThreshold WRITE setBloomThreshold NOTIFY bloomThresholdChanged)
    Q_PROPERTY(float bloomExposure READ bloomExposure WRITE setBloomExposure NOTIFY bloomExposureChanged)
    Q_PROPERTY(int bloomBlurPassCount READ bloomBlurPassCount WRITE setBloomBlurPassCount NOTIFY bloomBlurPassCountChanged)
    Q_PROPERTY(bool thresholdEffect READ thresholdEffect WRITE setThresholdEffect NOTIFY thresholdEffectChanged)
    Q_PROPERTY(float threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)

    Q_PROPERTY(bool blurEffect READ blurEffect WRITE setBlurEffect NOTIFY blurEffectChanged)
    Q_PROPERTY(int blurPassCount READ blurPassCount WRITE setBlurPassCount NOTIFY blurPassCountChanged)

    Q_PROPERTY(bool opacityMaskEffect READ opacityMaskEffect WRITE setOpacityMaskEffect NOTIFY opacityMaskEffectChanged)
    Q_PROPERTY(bool opacityMaskPremultipliedAlpha READ opacityMaskPremultipliedAlpha WRITE setOpacityMaskPremultipliedAlpha NOTIFY opacityMaskPremultipliedAlphaChanged)

public:
    explicit SceneController(QObject *parent = nullptr);

    QString sceneName() const;
    QStringList scenes() const;
    QColor clearColor() const;
    bool alphaBlending() const;
    bool frustumCulling() const;
    bool bloomEffect() const;
    float bloomThreshold() const;
    float bloomExposure() const;
    int bloomBlurPassCount() const;
    bool thresholdEffect() const;
    float threshold() const;
    bool blurEffect() const;
    int blurPassCount() const;
    bool opacityMaskEffect() const;
    bool opacityMaskPremultipliedAlpha() const;
    bool zFill() const;

public slots:
    void setSceneName(const QString &sceneName);
    void setClearColor(const QColor &clearColor);
    void setAlphaBlending(bool alphaBlending);
    void setFrustumCulling(bool frustumCulling);
    void setBloomEffect(bool bloomEffect);
    void setBloomThreshold(float bloomThreshold);
    void setBloomExposure(float bloomExposure);
    void setBloomBlurPassCount(int bloomBlurPassCount);
    void setThresholdEffect(bool thresholdEffect);
    void setThreshold(float threshold);
    void setBlurEffect(bool blurEffect);
    void setBlurPassCount(int blurPassCount);
    void setOpacityMaskEffect(bool opacityMaskEffect);
    void setOpacityMaskPremultipliedAlpha(bool premultipliedAlpha);
    void setZFill(bool zFill);

signals:
    void sceneNameChanged(const QString &sceneName);
    void clearColorChanged(const QColor &clearColor);
    void alphaBlendingChanged(bool alphaBlending);
    void frustumCullingChanged(bool frustumCulling);
    void bloomFilterChanged(bool bloomEffect);
    void bloomThresholdChanged(float bloomThreshold);
    void bloomExposureChanged(float bloomExposure);
    void bloomBlurPassCountChanged(int bloomBlurPassCount);
    void thresholdEffectChanged(bool thresholdEffect);
    void thresholdChanged(float threshold);
    void blurEffectChanged(bool blurEffect);
    void blurPassCountChanged(int blurPassCount);
    void opacityMaskEffectChanged(bool opacityMaskEffect);
    void opacityMaskPremultipliedAlphaChanged(bool premultipliedAlpha);
    void zFillChanged(bool zFill);

private:
    QStringList m_scenes;
    QString m_sceneName;

    QColor m_clearColor;
    bool m_alphaBlendingEnabled;
    bool m_frustumCullingEnabled;
    bool m_zFillEnabled;

    bool m_bloomFilter;
    float m_bloomThreshold;
    float m_bloomExposure;
    int m_bloomBlurPassCount;

    bool m_blurEffect;
    int m_blurPassCount;

    bool m_thresholdEffect;
    float m_threshold;

    bool m_opacityMaskEffect;
    bool m_opacityMaskPremultipliedAlpha;
};

#endif // SCENECONTROLLER_H
