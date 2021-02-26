/*
    viewconfiguration.h

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

#ifndef KUESA_KUESA_UTILS_VIEWCONFIGURATION_H
#define KUESA_KUESA_UTILS_VIEWCONFIGURATION_H

#include <KuesaUtils/kuesa_utils_global.h>

#include <Qt3DCore/QNode>
#include <QUrl>

#include <Kuesa/animationplayer.h>
#include <Kuesa/transformtracker.h>
#include <Kuesa/placeholdertracker.h>
#include <Kuesa/kuesanode.h>
#include <Kuesa/ToneMappingAndGammaCorrectionEffect>

QT_BEGIN_NAMESPACE

namespace KuesaUtils {

class KUESAUTILS_SHARED_EXPORT ViewConfiguration : public Kuesa::KuesaNode
{
    Q_OBJECT
    Q_PROPERTY(QString cameraName READ cameraName WRITE setCameraName NOTIFY cameraNameChanged)
    Q_PROPERTY(QStringList layerNames READ layerNames WRITE setLayerNames NOTIFY layerNamesChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(bool frustumCulling READ frustumCulling WRITE setFrustumCulling NOTIFY frustumCullingChanged)
    Q_PROPERTY(bool skinning READ skinning WRITE setSkinning NOTIFY skinningChanged)
    Q_PROPERTY(bool backToFrontSorting READ backToFrontSorting WRITE setBackToFrontSorting NOTIFY backToFrontSortingChanged)
    Q_PROPERTY(bool zFilling READ zFilling WRITE setZFilling NOTIFY zFillingChanged)
    Q_PROPERTY(bool particlesEnabled READ particlesEnabled WRITE setParticlesEnabled NOTIFY particlesEnabledChanged)
    Q_PROPERTY(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm READ toneMappingAlgorithm WRITE setToneMappingAlgorithm NOTIFY toneMappingAlgorithmChanged)
    Q_PROPERTY(bool usesStencilMask READ usesStencilMask WRITE setUsesStencilMask NOTIFY usesStencilMaskChanged)
    Q_PROPERTY(float exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
    Q_PROPERTY(QColor clearColor READ clearColor WRITE setClearColor NOTIFY clearColorChanged)

public:
    explicit ViewConfiguration(Qt3DCore::QNode *parent = nullptr);

    QString cameraName() const;
    QStringList layerNames() const;
    QRectF viewportRect() const;
    bool frustumCulling() const;
    bool skinning() const;
    bool backToFrontSorting() const;
    bool zFilling() const;
    bool particlesEnabled() const;
    Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm() const;
    bool usesStencilMask() const;
    float exposure() const;
    float gamma() const;
    QColor clearColor() const;

    const std::vector<Kuesa::TransformTracker *> &transformTrackers() const;
    void addTransformTracker(Kuesa::TransformTracker *tracker);
    void removeTransformTracker(Kuesa::TransformTracker *tracker);
    void clearTransformTrackers();

    const std::vector<Kuesa::PlaceholderTracker *> &placeholderTrackers() const;
    void addPlaceholderTracker(Kuesa::PlaceholderTracker *tracker);
    void removePlaceholderTracker(Kuesa::PlaceholderTracker *tracker);
    void clearPlaceholderTrackers();

public Q_SLOTS:
    void setCameraName(const QString &cameraName);
    void setLayerNames(const QStringList &layerNames);
    void setViewportRect(const QRectF &viewportRect);
    void setFrustumCulling(bool frustumCulling);
    void setSkinning(bool skinning);
    void setBackToFrontSorting(bool backToFrontSorting);
    void setZFilling(bool zFilling);
    void setParticlesEnabled(bool particlesEnabled);
    void setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping algorithm);
    void setUsesStencilMask(bool usesStencilMask);
    void setExposure(float exposure);
    void setGamma(float gamma);
    void setClearColor(QColor clearColor);

Q_SIGNALS:
    void cameraNameChanged(const QString &cameraName);
    void layerNamesChanged(const QStringList &layerNames);
    void viewportRectChanged(QRectF viewportRect);
    void frustumCullingChanged(bool frustumCulling);
    void skinningChanged(bool skinning);
    void backToFrontSortingChanged(bool backToFrontSorting);
    void zFillingChanged(bool zFillingChanged);
    void particlesEnabledChanged(bool particlesEnabled);
    void toneMappingAlgorithmChanged(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping algorithm);
    void usesStencilMaskChanged(bool usesStencilMask);
    void exposureChanged(float exposure);
    void gammaChanged(float gamma);
    void clearColorChanged(QColor clearColor);

    void transformTrackerAdded(Kuesa::TransformTracker *tracker);
    void transformTrackerRemoved(Kuesa::TransformTracker *tracker);

    void placeholderTrackerAdded(Kuesa::PlaceholderTracker *tracker);
    void placeholderTrackerRemoved(Kuesa::PlaceholderTracker *tracker);

    void loadingDone();
    void unloadingDone();

private:
    QString m_cameraName;
    QStringList m_layerNames;
    QRectF m_viewportRect = { 0, 0, 1, 1 };
    bool m_frustumCulling = true;
    bool m_skinning = false;
    bool m_backToFrontSorting = false;
    bool m_zFilling = false;
    bool m_particlesEnabled = false;
    Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping m_toneMappingAlgorithm = Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping::None;
    bool m_usesStencilMask = false;
    float m_exposure = 0.0f;
    float m_gamma = 2.2f;
    QColor m_clearColor = Qt::black;

    std::vector<Kuesa::TransformTracker *> m_transformTrackers;
    std::vector<Kuesa::PlaceholderTracker *> m_placeholderTrackers;
};
} // namespace KuesaUtils

#endif // VIEWCONFIGURATION_H
