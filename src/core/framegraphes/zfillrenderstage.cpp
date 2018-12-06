/*
    zfillrenderstage.cpp

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

#include "zfillrenderstage_p.h"
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QColorMask>
#include <Qt3DRender/QDepthTest>

QT_USE_NAMESPACE

using namespace Kuesa;

ZFillRenderStage::ZFillRenderStage(Qt3DCore::QNode *parent)
    : AbstractRenderStage(parent)
{
    setObjectName(QStringLiteral("KuesaZFillRenderStage"));
    auto filterKey = new Qt3DRender::QFilterKey(this);
    filterKey->setName(QStringLiteral("KuesaDrawStage"));
    filterKey->setValue(QStringLiteral("ZFill"));
    addMatch(filterKey);

    auto states = new Qt3DRender::QRenderStateSet(this);
    auto colorMask = new Qt3DRender::QColorMask;
    colorMask->setRedMasked(false);
    colorMask->setGreenMasked(false);
    colorMask->setBlueMasked(false);
    colorMask->setAlphaMasked(false);
    states->addRenderState(colorMask);
    auto depthTest = new Qt3DRender::QDepthTest;
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Less);
    states->addRenderState(depthTest);
}

ZFillRenderStage::~ZFillRenderStage()
{
}
