/*
    opaquerenderstage.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#include "opaquerenderstage_p.h"
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QRenderStateSet>

QT_USE_NAMESPACE

using namespace Kuesa;

OpaqueRenderStage::OpaqueRenderStage(Qt3DCore::QNode *parent)
    : AbstractRenderStage(parent)
{
    setObjectName(QStringLiteral("KuesaOpaqueRenderStage"));
    auto filterKey = new Qt3DRender::QFilterKey(this);
    filterKey->setName(QStringLiteral("KuesaDrawStage"));
    filterKey->setValue(QStringLiteral("Opaque"));
    addMatch(filterKey);

    m_states = new Qt3DRender::QRenderStateSet(this);
    m_depthTest = new Qt3DRender::QDepthTest;
    m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::Less);
    m_states->addRenderState(m_depthTest);
    m_noDepthWrite = new Qt3DRender::QNoDepthMask(m_states);
}

OpaqueRenderStage::~OpaqueRenderStage()
{
}

void OpaqueRenderStage::setZFilling(bool zFill)
{
    if (zFill != zFilling()) {
        if (zFill) {
            m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::Equal);
            m_states->addRenderState(m_noDepthWrite);
        } else {
            m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::Less);
            if (m_states->renderStates().contains(m_noDepthWrite))
                m_states->removeRenderState(m_noDepthWrite);
        }
    }
}

bool OpaqueRenderStage::zFilling() const
{
    return m_depthTest->depthFunction() == Qt3DRender::QDepthTest::Equal;
}
