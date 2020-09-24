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

ReflectionStages::ReflectionStages(Qt3DRender::QFrameGraphNode *parent)
    : SceneStages(parent)
    , m_clearDepth(nullptr)
{
    // Enable Reflections on the SceneStages
    m_reflectiveEnabledParameter->setValue(true);

    m_clearDepth = new Qt3DRender::QClearBuffers();
    m_clearDepth->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
    new Qt3DRender::QNoDraw(m_clearDepth);

    // Force initial configuration
    reconfigure(SceneFeaturedRenderStageBase::features());
}

ReflectionStages::~ReflectionStages()
{
}

void ReflectionStages::reconfigure(const Features features)
{
    // We force disable frustum culling
    Features editedFeatures = features;
    editedFeatures.setFlag(FrustumCulling, false);

    // Remove Clear Node from hierarchy
    m_clearDepth->setParent(Q_NODE_NULLPTR);

    // Rebuild FG hierarchy based on set features
    SceneStages::reconfigure(editedFeatures);

    // Insert Clear Node again so as to clear Depth after having drawn reflections
    m_clearDepth->setParent(this);
}

void ReflectionStages::setReflectivePlaneEquation(const QVector4D &planeEquation)
{
    m_reflectivePlaneParameter->setValue(planeEquation);
}

QT_END_NAMESPACE
