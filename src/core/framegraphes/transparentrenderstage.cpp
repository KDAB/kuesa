/*
    transparentrenderstage.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "transparentrenderstage_p.h"
#include <Qt3DRender/QSortPolicy>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QMultiSampleAntiAliasing>

QT_USE_NAMESPACE

using namespace Kuesa;

namespace {

QVector<Qt3DRender::QSortPolicy::SortType> sortTypes(bool backToFrontSorting)
{
    if (backToFrontSorting)
        return { Qt3DRender::QSortPolicy::BackToFront, Qt3DRender::QSortPolicy::Texture };
    return { Qt3DRender::QSortPolicy::Texture };
}

} // namespace

TransparentRenderStage::TransparentRenderStage(Qt3DRender::QFrameGraphNode *parent)
    : AbstractRenderStage(parent)
{
    setObjectName(QStringLiteral("KuesaTransparentRenderStage"));

    // TODO these should probably be only enabled if back face culling is on
    // Though we can't have GL_ALWAYS and if we have a transparent object fully
    // hidden by an opaque object, GL_ALWAYS would cause the color of the
    // opaque object to still be blended

    auto states = new Qt3DRender::QRenderStateSet(this);
    auto depthTest = new Qt3DRender::QDepthTest;
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    states->addRenderState(depthTest);

    auto msaa = new Qt3DRender::QMultiSampleAntiAliasing;
    auto noDepthWrite = new Qt3DRender::QNoDepthMask;

    states->addRenderState(msaa);
    states->addRenderState(noDepthWrite);

    m_alphaSortPolicy = new Qt3DRender::QSortPolicy(states);
    m_alphaSortPolicy->setSortTypes(sortTypes(true));

    auto transparentFilter = new Qt3DRender::QRenderPassFilter(m_alphaSortPolicy);
    auto filterKey = new Qt3DRender::QFilterKey(this);
    filterKey->setName(QStringLiteral("KuesaDrawStage"));
    filterKey->setValue(QStringLiteral("Transparent"));

    transparentFilter->addMatch(filterKey);
}

TransparentRenderStage::~TransparentRenderStage()
{
}

bool TransparentRenderStage::backToFrontSorting() const
{
    return m_alphaSortPolicy->sortTypes().contains(Qt3DRender::QSortPolicy::BackToFront);
}

void TransparentRenderStage::setBackToFrontSorting(bool enabled)
{
    if (enabled == backToFrontSorting())
        return;
    m_alphaSortPolicy->setSortTypes(sortTypes(enabled));
}
