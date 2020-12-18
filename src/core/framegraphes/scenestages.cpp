/*
    scenestages.cpp

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

#include "scenestages_p.h"

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qfrustumculling.h>
#include <Qt3DRender/qabstracttexture.h>
#include <QVector4D>
#include <QMetaEnum>

#include "zfillrenderstage_p.h"
#include "opaquerenderstage_p.h"
#include "transparentrenderstage_p.h"
#include <Kuesa/private/empty2dtexture_p.h>

#include <private/particlerenderstage_p.h>
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

using namespace Kuesa;

/*! \internal

    SceneFeaturedRenderStageBase is a base class that holds a set of features
    common to many render stages. Whenever one of the features is updated,
    the pure virtual reconfigure function is called
 */
SceneFeaturedRenderStageBase::SceneFeaturedRenderStageBase(Qt3DRender::QFrameGraphNode *parent)
    : AbstractRenderStage(parent)
{
}

bool SceneFeaturedRenderStageBase::skinning() const
{
    return m_features & Skinning;
}

bool SceneFeaturedRenderStageBase::zFilling() const
{
    return m_features & ZFilling;
}

bool SceneFeaturedRenderStageBase::frustumCulling() const
{
    return m_features & FrustumCulling;
}

bool SceneFeaturedRenderStageBase::backToFrontSorting() const
{
    return m_features & BackToFrontSorting;
}

bool SceneFeaturedRenderStageBase::particlesEnabled() const
{
    return m_features & Particles;
}

SceneFeaturedRenderStageBase::Features SceneFeaturedRenderStageBase::features() const
{
    return m_features;
}

void SceneFeaturedRenderStageBase::setSkinning(bool skinning)
{
    if (skinning == bool(m_features & Skinning))
        return;
    m_features.setFlag(Skinning, skinning);
    reconfigure(m_features);
}

void SceneFeaturedRenderStageBase::setZFilling(bool zFilling)
{
    if (zFilling == bool(m_features & ZFilling))
        return;
    m_features.setFlag(ZFilling, zFilling);
    reconfigure(m_features);
}

void SceneFeaturedRenderStageBase::setFrustumCulling(bool culling)
{
    if (culling == bool(m_features & FrustumCulling))
        return;
    m_features.setFlag(FrustumCulling, culling);
    reconfigure(m_features);
}

void SceneFeaturedRenderStageBase::setBackToFrontSorting(bool backToFrontSorting)
{
    if (backToFrontSorting == bool(m_features & BackToFrontSorting))
        return;
    m_features.setFlag(BackToFrontSorting, backToFrontSorting);
    reconfigure(m_features);
}

void SceneFeaturedRenderStageBase::setParticlesEnabled(bool enabled)
{
    if (enabled == bool(m_features & Particles))
        return;
    m_features.setFlag(Particles, enabled);
    reconfigure(m_features);
}

/*!
    \internal

    This represent one pass of rendering for the scene (ZFill or Opaque or Transparent)

    This takes care of performing the selected pass type for both non skinned and skinned
    meshes (if enabled)
 */
ScenePass::ScenePass(ScenePass::SceneStageType type, Qt3DRender::QFrameGraphNode *parent)
    : SceneFeaturedRenderStageBase(parent)
    , m_type(type)
{
    static const QMetaEnum metaEnum = QMetaEnum::fromType<ScenePass::SceneStageType>();

    m_nonSkinnedTechniqueFilter = new Qt3DRender::QTechniqueFilter();
    m_nonSkinnedTechniqueFilter->setObjectName(QLatin1String("KuesaNonSkinnedTechniqueFilter(%1)").arg(metaEnum.valueToKey(type)));

    m_skinnedTechniqueFilter = new Qt3DRender::QTechniqueFilter();
    m_skinnedTechniqueFilter->setObjectName(QLatin1String("KuesaSkinnedTechniqueFilter(%1)").arg(metaEnum.valueToKey(type)));

    m_frustumCulling = new Qt3DRender::QFrustumCulling();
    m_frustumCulling->setObjectName(QLatin1String("KuesaFrustrumCulling(%1)").arg(metaEnum.valueToKey(type)));

    // Both techniques are forward
    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("renderingStyle"));
        filterKey->setValue(QStringLiteral("forward"));
        m_nonSkinnedTechniqueFilter->addMatch(filterKey);
        m_skinnedTechniqueFilter->addMatch(filterKey);
    }
    // Static meshes can be culled
    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("allowCulling"));
        filterKey->setValue(true);
        m_nonSkinnedTechniqueFilter->addMatch(filterKey);
    }
    // Skinned meshes shouldn't be culled
    {
        auto filterKey = new Qt3DRender::QFilterKey(this);
        filterKey->setName(QStringLiteral("allowCulling"));
        filterKey->setValue(false);
        m_skinnedTechniqueFilter->addMatch(filterKey);
    }

    switch (type) {
    case Transparent: {
        m_nonSkinnedStage = new TransparentRenderStage();
        m_skinnedStage = new TransparentRenderStage();
        break;
    }
    case Opaque: {
        m_nonSkinnedStage = new OpaqueRenderStage();
        m_skinnedStage = new OpaqueRenderStage();
        break;
    }
    case ZFill: {
        m_nonSkinnedStage = new ZFillRenderStage();
        m_skinnedStage = new ZFillRenderStage();
        break;
    }
    }

    // Force initial configuration
    reconfigure(SceneFeaturedRenderStageBase::features());
}

ScenePass::~ScenePass()
{
    // delete unparented subtree
    if (!m_frustumCulling->parent())
        delete m_frustumCulling;

    if (!m_skinnedTechniqueFilter->parent())
        delete m_skinnedTechniqueFilter;
}

ScenePass::SceneStageType ScenePass::type() const
{
    return m_type;
}

void ScenePass::addParameter(Qt3DRender::QParameter *parameter)
{
    m_nonSkinnedTechniqueFilter->addParameter(parameter);
    m_skinnedTechniqueFilter->addParameter(parameter);
}

void ScenePass::removeParameter(Qt3DRender::QParameter *parameter)
{
    m_nonSkinnedTechniqueFilter->removeParameter(parameter);
    m_skinnedTechniqueFilter->removeParameter(parameter);
}

void ScenePass::setCullingMode(Qt3DRender::QCullFace::CullingMode cullingMode)
{
    if (cullingMode == m_cullingMode)
        return;
    m_cullingMode = cullingMode;
    reconfigure(features());
}

Qt3DRender::QCullFace::CullingMode ScenePass::cullingMode() const
{
    return m_cullingMode;
}

void ScenePass::reconfigure(const Features features)
{
    const bool useSkinning = bool(features & Skinning);
    const bool useZFilling = bool(features & ZFilling);
    const bool sortBackToFront = bool(features & BackToFrontSorting);
    const bool useFrustumCulling = bool(features & FrustumCulling);

    // Unparent all nodes
    m_nonSkinnedStage->setParent(Q_NODE_NULLPTR);
    m_skinnedStage->setParent(Q_NODE_NULLPTR);

    m_nonSkinnedTechniqueFilter->setParent(Q_NODE_NULLPTR);
    m_skinnedTechniqueFilter->setParent(Q_NODE_NULLPTR);
    m_frustumCulling->setParent(Q_NODE_NULLPTR);

    // Set Features on sub stages
    switch (m_type) {
    case Transparent: {
        static_cast<TransparentRenderStage *>(m_nonSkinnedStage)->setBackToFrontSorting(sortBackToFront);
        static_cast<TransparentRenderStage *>(m_skinnedStage)->setBackToFrontSorting(sortBackToFront);
        break;
    }
    case Opaque: {
        static_cast<OpaqueRenderStage *>(m_nonSkinnedStage)->setZFilling(useZFilling);
        static_cast<OpaqueRenderStage *>(m_nonSkinnedStage)->setCullingMode(m_cullingMode);
        static_cast<OpaqueRenderStage *>(m_skinnedStage)->setZFilling(useZFilling);
        static_cast<OpaqueRenderStage *>(m_skinnedStage)->setCullingMode(m_cullingMode);
        break;
    }
    case ZFill: {
        static_cast<ZFillRenderStage *>(m_nonSkinnedStage)->setCullingMode(m_cullingMode);
        static_cast<ZFillRenderStage *>(m_skinnedStage)->setCullingMode(m_cullingMode);
        break;
    }
    default:
        break;
    }

    // Reparent appropriately
    m_nonSkinnedTechniqueFilter->setParent(this);
    if (useFrustumCulling) {
        m_frustumCulling->setParent(m_nonSkinnedTechniqueFilter);
        m_nonSkinnedStage->setParent(m_frustumCulling);
    } else {
        m_nonSkinnedStage->setParent(m_nonSkinnedTechniqueFilter);
    }


    // Skinned Meshes are not checked against frustum culling as the skinning
    // could actually make them still be in the view frustum even if their
    // transform technically makes them out of sight
    if (useSkinning) {
        m_skinnedTechniqueFilter->setParent(this);
        m_skinnedStage->setParent(m_skinnedTechniqueFilter);
    }
}



/*!
    \internal

    SceneStages contains and manages the 3 SceneStage we need to render the scene
    - ZFill (optional)
    - Opaque
    - Transparent

    Each Stage takes care of handling skinned and non skinned meshes, zfilling,
    frustum culling and back to front sorting depending on what features are
    required.
 */
SceneStages::SceneStages(Qt3DRender::QFrameGraphNode *parent)
    : SceneFeaturedRenderStageBase(parent)
{
    m_cameraSelector = new Qt3DRender::QCameraSelector(this);
    m_layerFilter = new Qt3DRender::QLayerFilter();

    m_zFillStage = ScenePassPtr::create(ScenePass::ZFill);
    m_opaqueStage = ScenePassPtr::create(ScenePass::Opaque);
    m_transparentStage = ScenePassPtr::create(ScenePass::Transparent);
    m_particleRenderStage = ParticleRenderStagePtr::create();

    m_defaultReflectivePlaneTexture = new Empty2DTexture();
    m_reflectiveEnabledParameter = new Qt3DRender::QParameter(QStringLiteral("isReflective"), QVariant(false), this);
    m_reflectivePlaneParameter = new Qt3DRender::QParameter(QStringLiteral("reflectionPlane"), QVector4D(), this);
    m_reflectivePlaneTextureParameter = new Qt3DRender::QParameter(QStringLiteral("reflectionMap"), m_defaultReflectivePlaneTexture, this);

    // Add parameters to ScenePasses
    m_zFillStage->addParameter(m_reflectiveEnabledParameter);
    m_opaqueStage->addParameter(m_reflectiveEnabledParameter);
    m_transparentStage->addParameter(m_reflectiveEnabledParameter);

    m_zFillStage->addParameter(m_reflectivePlaneParameter);
    m_opaqueStage->addParameter(m_reflectivePlaneParameter);
    m_transparentStage->addParameter(m_reflectivePlaneParameter);

    m_zFillStage->addParameter(m_reflectivePlaneTextureParameter);
    m_opaqueStage->addParameter(m_reflectivePlaneTextureParameter);
    m_transparentStage->addParameter(m_reflectivePlaneTextureParameter);

    // Force initial configuration
    reconfigure(SceneFeaturedRenderStageBase::features());
}

SceneStages::~SceneStages()
{
    // Destroy layer filter if parentless
    if (!m_layerFilter->parent())
        delete m_layerFilter;
}

void SceneStages::reconfigure(const Features features)
{
    const bool hasLayers = m_layerFilter->layers().size() > 0;
    const bool useSkinning = bool(features & Skinning);
    const bool useZFilling = bool(features & ZFilling);
    const bool sortBackToFront = bool(features & BackToFrontSorting);
    const bool useFrustumCulling = bool(features & FrustumCulling);
    const bool useParticles = bool(features & Particles);

    // Unparent each stages to make sure they are added in correct order
    m_zFillStage->setParent(Q_NODE_NULLPTR);
    m_opaqueStage->setParent(Q_NODE_NULLPTR);
    m_transparentStage->setParent(Q_NODE_NULLPTR);
    m_particleRenderStage->setParent(Q_NODE_NULLPTR);

    // Set features on stages which will update accordingly
    const ScenePassPtr passStages[] { m_zFillStage, m_opaqueStage, m_transparentStage };
    for (const ScenePassPtr &passStage : passStages) {
        passStage->setFrustumCulling(useFrustumCulling);
        passStage->setSkinning(useSkinning);
        passStage->setZFilling(useZFilling);
        passStage->setBackToFrontSorting(sortBackToFront);
    }

    // Find stages parent root
    Qt3DRender::QFrameGraphNode *stageRoot = nullptr;
    if (hasLayers) {
        // If we have layers, then we parent the layer filter
        // and add the stages as children of it
        stageRoot = m_layerFilter;
        m_layerFilter->setParent(m_cameraSelector);
    } else {
        // Otherwise, we unparent the layer filter and
        // parent the stages to the cameraSelector
        stageRoot = m_cameraSelector;
        m_layerFilter->setParent(Q_NODE_NULLPTR);
    }

    // Reparent stages based on features
    if (useZFilling)
        m_zFillStage->setParent(stageRoot);
    m_opaqueStage->setParent(stageRoot);
    m_transparentStage->setParent(stageRoot);
    if (useParticles)
        m_particleRenderStage->setParent(stageRoot);
}

void SceneStages::addLayer(Qt3DRender::QLayer *layer)
{
    m_layerFilter->addLayer(layer);

    // We want to make sure layer is removed from SceneStages if destroyed
    auto d = Qt3DCore::QNodePrivate::get(this);
    QVector<Qt3DRender::QLayer *> fakeLayers;
    d->registerDestructionHelper(layer, &SceneStages::removeLayer, fakeLayers);

    // Reconfigure if we had no layers previously
    if (m_layerFilter->layers().size() == 1)
        reconfigure(SceneFeaturedRenderStageBase::features());
}

void SceneStages::removeLayer(Qt3DRender::QLayer *layer)
{
    m_layerFilter->removeLayer(layer);

    auto d = Qt3DCore::QNodePrivate::get(this);
    d->unregisterDestructionHelper(layer);

    // Reconfigure if we have no more layers previously
    if (m_layerFilter->layers().size() == 0)
        reconfigure(SceneFeaturedRenderStageBase::features());
}

QVector<Qt3DRender::QLayer *> SceneStages::layers() const
{
    return m_layerFilter->layers();
}

void SceneStages::setCullingMode(Qt3DRender::QCullFace::CullingMode cullingMode)
{
    m_zFillStage->setCullingMode(cullingMode);
    m_opaqueStage->setCullingMode(cullingMode);
}

Qt3DRender::QCullFace::CullingMode SceneStages::cullingMode() const
{
    return m_opaqueStage->cullingMode();
}

void SceneStages::setCamera(Qt3DCore::QEntity *camera)
{
    m_cameraSelector->setCamera(camera);
}

Qt3DCore::QEntity *SceneStages::camera() const
{
    return m_cameraSelector->camera();
}

void SceneStages::setReflectivePlaneEquation(const QVector4D &planeEquation)
{
    m_reflectivePlaneParameter->setValue(planeEquation);
}

QVector4D SceneStages::reflectivePlaneEquation() const
{
    return m_reflectivePlaneParameter->value().value<QVector4D>();
}

void SceneStages::setReflectivePlaneTexture(Qt3DRender::QAbstractTexture *t)
{
    Qt3DRender::QAbstractTexture *currentT = reflectivePlaneTexture();
    if (t == currentT)
        return;

    auto d = Qt3DCore::QNodePrivate::get(this);

    if (currentT && currentT != m_defaultReflectivePlaneTexture)
        d->unregisterDestructionHelper(currentT);

    if (t) {
        d->registerDestructionHelper(t, &SceneStages::setReflectivePlaneTexture, t);
        m_reflectivePlaneTextureParameter->setValue(QVariant::fromValue(t));
    } else {
        m_reflectivePlaneTextureParameter->setValue(QVariant::fromValue(m_defaultReflectivePlaneTexture));
    }
}

Qt3DRender::QAbstractTexture *SceneStages::reflectivePlaneTexture() const
{
    return m_reflectivePlaneTextureParameter->value().value<Qt3DRender::QAbstractTexture *>();
}

QT_END_NAMESPACE
