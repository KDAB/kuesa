/*
    view.h

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

#ifndef KUESA_VIEW_H
#define KUESA_VIEW_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <QFlags>
#include <QRectF>
#include <QVector4D>
#include <Qt3DCore/QEntity>

class tst_View;
class tst_ForwardRenderer;

QT_BEGIN_NAMESPACE

namespace Kuesa {

class AbstractPostProcessingEffect;
class SceneStages;
class ReflectionStages;
class EffectsStages;
class ReflectionPlane;

using SceneStagesPtr = QSharedPointer<SceneStages>;
using ReflectionStagesPtr = QSharedPointer<ReflectionStages>;
using EffectsStagesPtr = QSharedPointer<EffectsStages>;

class KUESASHARED_EXPORT View : public Qt3DRender::QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(Qt3DCore::QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(bool frustumCulling READ frustumCulling WRITE setFrustumCulling NOTIFY frustumCullingChanged)
    Q_PROPERTY(bool skinning READ skinning WRITE setSkinning NOTIFY skinningChanged)
    Q_PROPERTY(bool backToFrontSorting READ backToFrontSorting WRITE setBackToFrontSorting NOTIFY backToFrontSortingChanged)
    Q_PROPERTY(bool zFilling READ zFilling WRITE setZFilling NOTIFY zFillingChanged)
    Q_PROPERTY(bool particlesEnabled READ particlesEnabled WRITE setParticlesEnabled NOTIFY particlesEnabledChanged)

public:
    explicit View(Qt3DCore::QNode *parent = nullptr);
    ~View();

    QRectF viewportRect() const;
    Qt3DCore::QEntity *camera() const;
    bool frustumCulling() const;
    bool skinning() const;
    bool backToFrontSorting() const;
    bool zFilling() const;
    bool particlesEnabled() const;

    const std::vector<AbstractPostProcessingEffect *> &postProcessingEffects() const;
    const std::vector<Qt3DRender::QLayer *> &layers() const;
    const std::vector<ReflectionPlane *> &reflectionPlanes() const;

public Q_SLOTS:
    void setViewportRect(const QRectF &viewportRect);
    void setCamera(Qt3DCore::QEntity *camera);
    void setFrustumCulling(bool frustumCulling);
    void setSkinning(bool frustumCulling);
    void setBackToFrontSorting(bool backToFrontSorting);
    void setZFilling(bool zfilling);
    void setParticlesEnabled(bool enabled);

    void addPostProcessingEffect(AbstractPostProcessingEffect *effect);
    void removePostProcessingEffect(AbstractPostProcessingEffect *effect);

    void addLayer(Qt3DRender::QLayer *layer);
    void removeLayer(Qt3DRender::QLayer *layer);

    void addReflectionPlane(ReflectionPlane *plane);
    void removeReflectionPlane(ReflectionPlane *plane);

Q_SIGNALS:
    void viewportRectChanged(const QRectF &viewportRect);
    void cameraChanged(Qt3DCore::QEntity *camera);
    void frustumCullingChanged(bool frustumCulling);
    void skinningChanged(bool skinning);
    void backToFrontSortingChanged(bool backToFrontSorting);
    void zFillingChanged(bool zFilling);
    void particlesEnabledChanged(bool enabled);

protected:
    void scheduleFGTreeRebuild();
    void rebuildFGTree();

    virtual void reconfigureStages();
    virtual void reconfigureFrameGraph();

    View *rootView() const;

    AbstractPostProcessingEffect::FrameGraphNodePtr frameGraphSubtreeForPostProcessingEffect(AbstractPostProcessingEffect *effect) const;

    enum Feature {
        BackToFrontSorting = (1 << 0),
        Skinning           = (1 << 1),
        FrustumCulling     = (1 << 2),
        ZFilling           = (1 << 3),
        Particles          = (1 << 4),
    };
    Q_DECLARE_FLAGS(Features, Feature)

private:
    void reconfigureFrameGraphHelper(Qt3DRender::QFrameGraphNode *stageRoot);

    SceneStagesPtr m_sceneStages;
    ReflectionStagesPtr m_reflectionStages;
    EffectsStagesPtr m_fxStages;

    Qt3DCore::QEntity *m_camera = nullptr;
    QRectF m_viewport = QRectF(0.0f, 0.0f, 1.0f, 1.0f);

    std::vector<Qt3DRender::QLayer *> m_layers;
    std::vector<AbstractPostProcessingEffect *> m_fxs;

    std::vector<ReflectionPlane *> m_reflectionPlanes;

    Features m_features = Features(FrustumCulling|Skinning);
    bool m_fgTreeRebuiltScheduled = false;

    friend class ::tst_View;
    friend class ::tst_ForwardRenderer;
    friend class ForwardRenderer;
};

} // Kuesa

QT_END_NAMESPACE

#endif // KUESA_VIEW_H
