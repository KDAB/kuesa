/*
    reflectionstages.cpp

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

#include "reflectionstages_p.h"

#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qnodraw.h>
#include <QVector4D>

#include "zfillrenderstage_p.h"
#include "opaquerenderstage_p.h"
#include "transparentrenderstage_p.h"

QT_BEGIN_NAMESPACE

using namespace Kuesa;

ReflectionStages::ReflectionStages()
    : m_opaqueStage(nullptr)
    , m_transparentStage(nullptr)
    , m_zFillStage(nullptr)
    , m_reflectiveEnabledParameter(nullptr)
    , m_reflectivePlaneParameter(nullptr)
    , m_clearDepth(nullptr)
{
}

ReflectionStages::~ReflectionStages()
{
    delete m_opaqueStagesConfiguration.m_reflectiveTechniqueFilter;
    delete m_transparentStagesConfiguration.m_reflectiveTechniqueFilter;
}

void ReflectionStages::init()
{
    m_opaqueStagesConfiguration.m_reflectiveTechniqueFilter = new Qt3DRender::QTechniqueFilter();
    m_opaqueStagesConfiguration.m_layerFilter = new Qt3DRender::QLayerFilter(m_opaqueStagesConfiguration.m_reflectiveTechniqueFilter);
    m_opaqueStagesConfiguration.m_cameraSelector = new Qt3DRender::QCameraSelector(m_opaqueStagesConfiguration.m_layerFilter);
    m_opaqueStagesConfiguration.m_viewport = new Qt3DRender::QViewport(m_opaqueStagesConfiguration.m_cameraSelector);

    m_transparentStagesConfiguration.m_reflectiveTechniqueFilter = new Qt3DRender::QTechniqueFilter();
    m_transparentStagesConfiguration.m_layerFilter = new Qt3DRender::QLayerFilter(m_transparentStagesConfiguration.m_reflectiveTechniqueFilter);
    m_transparentStagesConfiguration.m_cameraSelector = new Qt3DRender::QCameraSelector(m_transparentStagesConfiguration.m_layerFilter);
    m_transparentStagesConfiguration.m_viewport = new Qt3DRender::QViewport(m_transparentStagesConfiguration.m_cameraSelector);

    m_opaqueStage = new OpaqueRenderStage(m_opaqueStagesConfiguration.m_viewport);
    m_transparentStage = new TransparentRenderStage(m_transparentStagesConfiguration.m_viewport);

    m_opaqueStagesConfiguration.m_layerFilter->setFilterMode(Qt3DRender::QLayerFilter::AcceptAllMatchingLayers);
    m_transparentStagesConfiguration.m_layerFilter->setFilterMode(Qt3DRender::QLayerFilter::AcceptAllMatchingLayers);

    m_reflectiveEnabledParameter = new Qt3DRender::QParameter(QStringLiteral("isReflective"), true);
    m_reflectivePlaneParameter = new Qt3DRender::QParameter(QStringLiteral("reflectionPlane"), QVector4D());

    m_transparentStagesConfiguration.m_reflectiveTechniqueFilter->addParameter(m_reflectiveEnabledParameter);
    m_transparentStagesConfiguration.m_reflectiveTechniqueFilter->addParameter(m_reflectivePlaneParameter);

    m_opaqueStagesConfiguration.m_reflectiveTechniqueFilter->addParameter(m_reflectiveEnabledParameter);
    m_opaqueStagesConfiguration.m_reflectiveTechniqueFilter->addParameter(m_reflectivePlaneParameter);

    m_clearDepth = new Qt3DRender::QClearBuffers();
    m_clearDepth->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
    new Qt3DRender::QNoDraw(m_clearDepth);
}

void ReflectionStages::setZFilling(bool zFilling)
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

void ReflectionStages::setBackToFrontSorting(bool backToFrontSorting)
{
    if (backToFrontSorting != m_transparentStage->backToFrontSorting())
        m_transparentStage->setBackToFrontSorting(backToFrontSorting);
}

void ReflectionStages::reconfigure(Qt3DRender::QLayer *l, Qt3DCore::QEntity *camera, QRectF viewport)
{
    Qt3DRender::QLayer *oldLayer = layer();

    qDebug() << Q_FUNC_INFO << l;

    if (oldLayer) {
        m_opaqueStagesConfiguration.m_reflectiveTechniqueFilter->setObjectName(QLatin1String("Opaque Stages Root"));
        m_transparentStagesConfiguration.m_reflectiveTechniqueFilter->setObjectName(QLatin1String("Transparent Stages Root"));
        m_opaqueStagesConfiguration.m_layerFilter->removeLayer(oldLayer);
        m_transparentStagesConfiguration.m_layerFilter->removeLayer(oldLayer);
    }

    if (l) {
        m_opaqueStagesConfiguration.m_reflectiveTechniqueFilter->setObjectName(QString(QLatin1String("Opaque Stages Root (%1)")).arg(l->objectName()));
        m_transparentStagesConfiguration.m_reflectiveTechniqueFilter->setObjectName(QString(QLatin1String("Transparent Stages Root (%1)")).arg(l->objectName()));
        m_opaqueStagesConfiguration.m_layerFilter->addLayer(l);
        m_transparentStagesConfiguration.m_layerFilter->addLayer(l);

        qDebug() << Q_FUNC_INFO << l;

    }

    m_opaqueStagesConfiguration.m_cameraSelector->setCamera(camera);
    m_transparentStagesConfiguration.m_cameraSelector->setCamera(camera);

    m_opaqueStagesConfiguration.m_viewport->setNormalizedRect(viewport);
    m_transparentStagesConfiguration.m_viewport->setNormalizedRect(viewport);
}

bool ReflectionStages::zFilling()
{
    return m_zFillStage != nullptr;
}

bool ReflectionStages::backToFrontSorting()
{
    return m_transparentStage->backToFrontSorting();
}

Qt3DRender::QLayer *ReflectionStages::layer() const
{
    if (m_opaqueStagesConfiguration.m_layerFilter->layers().size() > 0)
        return m_opaqueStagesConfiguration.m_layerFilter->layers().first();
    return nullptr;
}

void ReflectionStages::unParent()
{
    m_opaqueStagesConfiguration.m_reflectiveTechniqueFilter->setParent(Q_NODE_NULLPTR);
    m_transparentStagesConfiguration.m_reflectiveTechniqueFilter->setParent(Q_NODE_NULLPTR);
    m_clearDepth->setParent(Q_NODE_NULLPTR);
}

void ReflectionStages::setParent(Qt3DCore::QNode *stageRoot)
{
    m_opaqueStagesConfiguration.m_reflectiveTechniqueFilter->setParent(stageRoot);
    m_transparentStagesConfiguration.m_reflectiveTechniqueFilter->setParent(stageRoot);
    m_clearDepth->setParent(stageRoot);
}

Qt3DRender::QFrameGraphNode *ReflectionStages::opaqueStagesRoot() const
{
    return m_opaqueStagesConfiguration.m_reflectiveTechniqueFilter;
}

Qt3DRender::QFrameGraphNode *ReflectionStages::transparentStagesRoot() const
{
    return m_transparentStagesConfiguration.m_reflectiveTechniqueFilter;
}

OpaqueRenderStage *ReflectionStages::opaqueStage() const
{
    return m_opaqueStage;
}

ZFillRenderStage *ReflectionStages::zFillStage() const
{
    return m_zFillStage;
}

TransparentRenderStage *ReflectionStages::transparentStage() const
{
    return m_transparentStage;
}

void ReflectionStages::setReflectivePlaneEquation(const QVector4D &planeEquation)
{
    m_reflectivePlaneParameter->setValue(planeEquation);
}

QT_END_NAMESPACE
