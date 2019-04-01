/*
    transparentrenderstage.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qblendequationarguments.h>

QT_USE_NAMESPACE

using namespace Kuesa;

TransparentRenderStage::TransparentRenderStage(Qt3DCore::QNode *parent)
    : AbstractRenderStage(parent)
{
    setObjectName(QStringLiteral("KuesaTransparentRenderStage"));
    auto filterKey = new Qt3DRender::QFilterKey(this);
    filterKey->setName(QStringLiteral("KuesaDrawStage"));
    filterKey->setValue(QStringLiteral("Transparent"));
    addMatch(filterKey);

    // TODO these should probably be only enabled if back face culling is on
    // Though we can't have GL_ALWAYS and if we have a transparent object fully
    // hidden by an opaque object, GL_ALWAYS would cause the color of the
    // opaque object to still be blended

    auto states = new Qt3DRender::QRenderStateSet(this);
    auto depthTest = new Qt3DRender::QDepthTest;
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    states->addRenderState(depthTest);

    auto blendState = new Qt3DRender::QBlendEquation();
    blendState->setBlendFunction(Qt3DRender::QBlendEquation::Add);
    auto blendArgs = new Qt3DRender::QBlendEquationArguments();
    blendArgs->setSourceRgb(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    blendArgs->setSourceAlpha(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    blendArgs->setDestinationRgb(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    blendArgs->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::One);

    states->addRenderState(blendState);
    states->addRenderState(blendArgs);

    m_alphaSortPolicy = new Qt3DRender::QSortPolicy(states);
    m_alphaSortPolicy->setSortTypes(QVector<Qt3DRender::QSortPolicy::SortType>
                                    { Qt3DRender::QSortPolicy::BackToFront,
                                      Qt3DRender::QSortPolicy::Material
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                                      , Qt3DRender::QSortPolicy::Texture
#endif
                                    });
    connect(m_alphaSortPolicy, &Qt3DRender::QSortPolicy::enabledChanged, this, &TransparentRenderStage::backToFrontSortingChanged);
}

TransparentRenderStage::~TransparentRenderStage()
{
}

bool TransparentRenderStage::backToFrontSorting() const
{
    return m_alphaSortPolicy->isEnabled();
}

void TransparentRenderStage::setBackToFrontSorting(bool backToFrontSorting)
{
    m_alphaSortPolicy->setEnabled(backToFrontSorting);
}
