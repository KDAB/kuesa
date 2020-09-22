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

#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qparameter.h>

#include "zfillrenderstage_p.h"
#include "opaquerenderstage_p.h"
#include "transparentrenderstage_p.h"

QT_BEGIN_NAMESPACE

using namespace Kuesa;

/*!
    \internal
 */
SceneStages::SceneStages()
    : m_opaqueStage(nullptr)
    , m_transparentStage(nullptr)
    , m_zFillStage(nullptr)
{
}

SceneStages::~SceneStages()
{
    delete m_opaqueStagesConfiguration.m_root;
    delete m_transparentStagesConfiguration.m_root;
}

/*!
    \internal
 */
void SceneStages::init()
{
    m_opaqueStagesConfiguration.m_root = new Qt3DRender::QLayerFilter();
    m_opaqueStagesConfiguration.m_cameraSelector = new Qt3DRender::QCameraSelector(m_opaqueStagesConfiguration.m_root);
    m_opaqueStagesConfiguration.m_viewport = new Qt3DRender::QViewport(m_opaqueStagesConfiguration.m_cameraSelector);
    m_transparentStagesConfiguration.m_root = new Qt3DRender::QLayerFilter();
    m_transparentStagesConfiguration.m_cameraSelector = new Qt3DRender::QCameraSelector(m_transparentStagesConfiguration.m_root);
    m_transparentStagesConfiguration.m_viewport = new Qt3DRender::QViewport(m_transparentStagesConfiguration.m_cameraSelector);
    m_opaqueStage = new OpaqueRenderStage(m_opaqueStagesConfiguration.m_viewport);
    m_transparentStage = new TransparentRenderStage(m_transparentStagesConfiguration.m_viewport);
    m_opaqueStagesConfiguration.m_root->setFilterMode(Qt3DRender::QLayerFilter::AcceptAllMatchingLayers);
    m_transparentStagesConfiguration.m_root->setFilterMode(Qt3DRender::QLayerFilter::AcceptAllMatchingLayers);
}

/*!
    \internal
 */
void SceneStages::setZFilling(bool zFilling)
{
    const bool hasZillStage = m_zFillStage != nullptr;
    if (hasZillStage != zFilling) {
        if (hasZillStage) {
            QObject::disconnect(m_zFillDestroyedConnection);
            delete m_zFillStage;
            m_zFillStage = nullptr;
        } else {
            m_zFillStage = new ZFillRenderStage();
            m_zFillDestroyedConnection = QObject::connect(m_zFillStage, &Qt3DCore::QNode::nodeDestroyed,
                                                          m_zFillStage, [this] { m_zFillStage = nullptr; });
            m_opaqueStage->setParent(Q_NODE_NULLPTR);
            m_zFillStage->setParent(m_opaqueStagesConfiguration.m_viewport);
            m_opaqueStage->setParent(m_opaqueStagesConfiguration.m_viewport);
        }
        m_opaqueStage->setZFilling(zFilling);
    }
}

/*!
    \internal
 */
void SceneStages::setBackToFrontSorting(bool backToFrontSorting)
{
    if (backToFrontSorting != m_transparentStage->backToFrontSorting())
        m_transparentStage->setBackToFrontSorting(backToFrontSorting);
}

/*!
    \internal
 */
void SceneStages::reconfigure(Qt3DRender::QLayer *l, Qt3DCore::QEntity *camera, QRectF viewport)
{
    Qt3DRender::QLayer *oldLayer = layer();

    if (oldLayer) {
        m_opaqueStagesConfiguration.m_root->removeLayer(oldLayer);
        m_opaqueStagesConfiguration.m_root->setObjectName(QLatin1String("Opaque Stages Root"));
        m_transparentStagesConfiguration.m_root->removeLayer(oldLayer);
        m_transparentStagesConfiguration.m_root->setObjectName(QLatin1String("Transparent Stages Root"));
    }

    if (l) {
        m_opaqueStagesConfiguration.m_root->addLayer(l);
        m_opaqueStagesConfiguration.m_root->setObjectName(QString(QLatin1String("Opaque Stages Root (%1)")).arg(l->objectName()));
        m_transparentStagesConfiguration.m_root->addLayer(l);
        m_transparentStagesConfiguration.m_root->setObjectName(QString(QLatin1String("Transparent Stages Root (%1)")).arg(l->objectName()));
    }

    m_opaqueStagesConfiguration.m_root->setEnabled(l != nullptr);
    m_transparentStagesConfiguration.m_root->setEnabled(l != nullptr);

    m_opaqueStagesConfiguration.m_cameraSelector->setCamera(camera);
    m_transparentStagesConfiguration.m_cameraSelector->setCamera(camera);

    m_opaqueStagesConfiguration.m_viewport->setNormalizedRect(viewport);
    m_transparentStagesConfiguration.m_viewport->setNormalizedRect(viewport);
}

/*!
    \internal
 */
bool SceneStages::zFilling()
{
    return m_zFillStage != nullptr;
}

/*!
    \internal
 */
bool SceneStages::backToFrontSorting()
{
    return m_transparentStage->backToFrontSorting();
}

Qt3DRender::QLayer *SceneStages::layer() const
{
    if (m_opaqueStagesConfiguration.m_root->layers().size() > 0)
        return m_opaqueStagesConfiguration.m_root->layers().first();
    return nullptr;
}

/*!
    \internal
 */
void SceneStages::unParent()
{
    m_opaqueStagesConfiguration.m_root->setParent(Q_NODE_NULLPTR);
    m_transparentStagesConfiguration.m_root->setParent(Q_NODE_NULLPTR);
}

/*!
    \internal
 */
void SceneStages::setParent(Qt3DCore::QNode *opaqueRoot,
                            Qt3DCore::QNode *transparentRoot)
{
    m_opaqueStagesConfiguration.m_root->setParent(opaqueRoot);
    m_transparentStagesConfiguration.m_root->setParent(transparentRoot);
}

/*!
    \internal
 */
Qt3DRender::QLayerFilter *SceneStages::opaqueStagesRoot() const
{
    return m_opaqueStagesConfiguration.m_root;
}

/*!
    \internal
 */
Qt3DRender::QLayerFilter *SceneStages::transparentStagesRoot() const
{
    return m_transparentStagesConfiguration.m_root;
}

/*!
    \internal
 */
OpaqueRenderStage *SceneStages::opaqueStage() const
{
    return m_opaqueStage;
}

/*!
    \internal
 */
ZFillRenderStage *SceneStages::zFillStage() const
{
    return m_zFillStage;
}

/*!
    \internal
 */
TransparentRenderStage *SceneStages::transparentStage() const
{
    return m_transparentStage;
}

QT_END_NAMESPACE
