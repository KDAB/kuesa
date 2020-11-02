/*
    tst_zfillstage.cpp

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

#include <QtTest/QtTest>

#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QMultiSampleAntiAliasing>
#include <Qt3DRender/QSortPolicy>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QColorMask>

#include <Kuesa/private/zfillrenderstage_p.h>

class tst_ZFillStage : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void testFrameGraph()
    {
        // GIVEN
        Kuesa::ZFillRenderStage stage;

        // THEN
        QCOMPARE(stage.objectName(), QStringLiteral("KuesaZFillRenderStage"));

        Qt3DRender::QRenderPassFilter *passFilter = stage.findChild<Qt3DRender::QRenderPassFilter *>();
        Qt3DRender::QRenderStateSet *stateSet = stage.findChild<Qt3DRender::QRenderStateSet *>();
        Qt3DRender::QDepthTest *depthTest = stage.findChild<Qt3DRender::QDepthTest *>();
        Qt3DRender::QColorMask *colorMask = stage.findChild<Qt3DRender::QColorMask *>();
        Qt3DRender::QMultiSampleAntiAliasing *msaa = stage.findChild<Qt3DRender::QMultiSampleAntiAliasing *>();
        Qt3DRender::QFilterKey *filterKey = stage.findChild<Qt3DRender::QFilterKey *>();

        QVERIFY(passFilter);
        QVERIFY(stateSet);
        QVERIFY(depthTest);
        QVERIFY(colorMask);
        QVERIFY(msaa);
        QVERIFY(filterKey);

        QCOMPARE(passFilter->parentFrameGraphNode(), &stage);
        QVERIFY(passFilter->matchAny().contains(filterKey));
        QCOMPARE(filterKey->name(), QStringLiteral("KuesaDrawStage"));
        QCOMPARE(filterKey->value(), QStringLiteral("ZFill"));
        QCOMPARE(stateSet->parentFrameGraphNode(), passFilter);
        QCOMPARE(depthTest->depthFunction(), Qt3DRender::QDepthTest::Less);
        QVERIFY(stateSet->renderStates().contains(depthTest));
        QVERIFY(stateSet->renderStates().contains(msaa));
        QVERIFY(stateSet->renderStates().contains(colorMask));
        QVERIFY(!colorMask->isRedMasked());
        QVERIFY(!colorMask->isGreenMasked());
        QVERIFY(!colorMask->isBlueMasked());
        QVERIFY(!colorMask->isAlphaMasked());
    }
};

QTEST_MAIN(tst_ZFillStage)
#include "tst_zfillstage.moc"