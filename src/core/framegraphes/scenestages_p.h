/*
    scenestages_p.h

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

#ifndef KUESA_SCENESTAGES_P_H
#define KUESA_SCENESTAGES_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Kuesa/private/kuesa_global_p.h>
#include <QRectF>
#include <QSharedPointer>
#include <private/abstractrenderstage_p.h>
#include <Qt3DRender/QCullFace>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QNode;
class QEntity;
}

namespace Qt3DRender {
class QTechniqueFilter;
class QRenderPassFilter;
class QViewport;
class QCameraSelector;
class QNoDraw;
class QSortPolicy;
class QRenderTarget;
class QBlitFramebuffer;
class QLayerFilter;
class QClearBuffers;
class QLayer;
class QFrustumCulling;
class QAbstractTexture;
} // namespace Qt3DRender

namespace Kuesa {

class AbstractRenderStage;
class OpaqueRenderStage;
class TransparentRenderStage;
class ZFillRenderStage;
class ParticleRenderStage;

using ParticleRenderStagePtr = QSharedPointer<ParticleRenderStage>;

class KUESA_PRIVATE_EXPORT SceneFeaturedRenderStageBase : public AbstractRenderStage
{
    Q_OBJECT
public:
    explicit SceneFeaturedRenderStageBase(Qt3DRender::QFrameGraphNode *parent = nullptr);

    enum Feature {
        BackToFrontSorting = (1 << 0),
        Skinning           = (1 << 1),
        FrustumCulling     = (1 << 2),
        ZFilling           = (1 << 3),
        Particles          = (1 << 4),
        CubeShadowMap      = (1 << 5),
    };
    Q_DECLARE_FLAGS(Features, Feature)

    Features features() const;

    void setSkinning(bool skinning);
    bool skinning() const;

    void setZFilling(bool zFilling);
    bool zFilling() const;

    void setFrustumCulling(bool culling);
    bool frustumCulling() const;

    void setBackToFrontSorting(bool backToFrontSorting);
    bool backToFrontSorting() const;

    void setParticlesEnabled(bool enabled);
    bool particlesEnabled() const;

    void setCubeShadowMap(bool cubeShadowMap);
    bool cubeShadowMap() const;

private:
    Features m_features;

private:
    virtual void reconfigure(const Features features) = 0;
};

class KUESA_PRIVATE_EXPORT ScenePass : public SceneFeaturedRenderStageBase
{
    Q_OBJECT
public:
    enum SceneStageType {
        ZFill,
        Opaque,
        Transparent,
        ShadowMap
    };
    Q_ENUM(SceneStageType)

    explicit ScenePass(SceneStageType type, Qt3DRender::QFrameGraphNode *parent = nullptr);
    ~ScenePass();

    SceneStageType type() const;

    void addParameter(Qt3DRender::QParameter *parameter);
    void removeParameter(Qt3DRender::QParameter *parameter);

    void setCullingMode(Qt3DRender::QCullFace::CullingMode cullingMode);
    Qt3DRender::QCullFace::CullingMode cullingMode() const;

private:
    void reconfigure(const Features features) override;

    Qt3DRender::QTechniqueFilter *m_nonSkinnedTechniqueFilter = nullptr;
    Qt3DRender::QTechniqueFilter *m_skinnedTechniqueFilter = nullptr;
    Qt3DRender::QFrustumCulling *m_frustumCulling = nullptr;

    AbstractRenderStage *m_nonSkinnedStage = nullptr;
    AbstractRenderStage *m_skinnedStage = nullptr;

    const SceneStageType m_type;
    Qt3DRender::QCullFace::CullingMode m_cullingMode = Qt3DRender::QCullFace::NoCulling;
};
using ScenePassPtr = QSharedPointer<ScenePass>;

class KUESA_PRIVATE_EXPORT SceneStages : public SceneFeaturedRenderStageBase
{
    Q_OBJECT
public:
    explicit SceneStages(Qt3DRender::QFrameGraphNode *parent = nullptr);
    ~SceneStages();

    void setCamera(Qt3DCore::QEntity *camera);
    Qt3DCore::QEntity *camera() const;

    void setViewport(const QRectF &vp);
    QRectF viewport() const;

    QVector<Qt3DRender::QLayer *> layers() const;

    void setCullingMode(Qt3DRender::QCullFace::CullingMode cullingMode);
    Qt3DRender::QCullFace::CullingMode cullingMode() const;

    void setReflectivePlaneEquation(const QVector4D &planeEquation);
    QVector4D reflectivePlaneEquation() const;

    void setReflectivePlaneTexture(Qt3DRender::QAbstractTexture *t);
    Qt3DRender::QAbstractTexture *reflectivePlaneTexture() const;

public Q_SLOTS:
    void addLayer(Qt3DRender::QLayer *layer);
    void removeLayer(Qt3DRender::QLayer *layer);

protected:
    void reconfigure(const Features features) override;

    Qt3DRender::QCameraSelector *m_cameraSelector = nullptr;
    Qt3DRender::QLayerFilter *m_layerFilter = nullptr;

    ScenePassPtr m_zFillStage;
    ScenePassPtr m_opaqueStage;
    ScenePassPtr m_transparentStage;
    ParticleRenderStagePtr m_particleRenderStage;

    Qt3DRender::QParameter *m_reflectiveEnabledParameter = nullptr;
    Qt3DRender::QParameter *m_reflectivePlaneParameter = nullptr;
    Qt3DRender::QParameter *m_reflectivePlaneTextureParameter = nullptr;
    Qt3DRender::QAbstractTexture *m_defaultReflectivePlaneTexture = nullptr;
};
using SceneStagesPtr = QSharedPointer<SceneStages>;


} // namespace Kuesa
QT_END_NAMESPACE

#endif // KUESA_SCENESTAGES_P_H
