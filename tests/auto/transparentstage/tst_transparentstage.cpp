/*
    tst_transparentstage.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QTest>

#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QMultiSampleAntiAliasing>
#include <Qt3DRender/QSortPolicy>
#include <Qt3DRender/QFilterKey>

#include <Kuesa/private/transparentrenderstage_p.h>

class tst_TransparentStage : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void testFrameGraph()
    {
        // GIVEN
        Kuesa::TransparentRenderStage stage;

        // THEN
        QCOMPARE(stage.backToFrontSorting(), true);
        QCOMPARE(stage.objectName(), QStringLiteral("KuesaTransparentRenderStage"));

        Qt3DRender::QRenderStateSet *stateSet = stage.findChild<Qt3DRender::QRenderStateSet *>();
        Qt3DRender::QRenderPassFilter *transparentFilter = stage.findChild<Qt3DRender::QRenderPassFilter *>();
        Qt3DRender::QDepthTest *depthTest = stage.findChild<Qt3DRender::QDepthTest *>();
        Qt3DRender::QNoDepthMask *noDepthMask = stage.findChild<Qt3DRender::QNoDepthMask *>();
        Qt3DRender::QMultiSampleAntiAliasing *msaa = stage.findChild<Qt3DRender::QMultiSampleAntiAliasing *>();
        Qt3DRender::QSortPolicy *sortPolicy = stage.findChild<Qt3DRender::QSortPolicy *>();
        Qt3DRender::QFilterKey *transparentFilterKey = stage.findChild<Qt3DRender::QFilterKey *>();

        QVERIFY(transparentFilter);
        QVERIFY(stateSet);
        QVERIFY(depthTest);
        QVERIFY(noDepthMask);
        QVERIFY(msaa);
        QVERIFY(sortPolicy);
        QVERIFY(transparentFilterKey);

        const auto passFilters = transparentFilter->findChildren<Qt3DRender::QRenderPassFilter *>();
        QVERIFY(passFilters.count() > 0);

        QCOMPARE(stateSet->parentFrameGraphNode(), &stage);
        QVERIFY(stateSet->renderStates().contains(depthTest));
        QVERIFY(stateSet->renderStates().contains(noDepthMask));
        QVERIFY(stateSet->renderStates().contains(msaa));
        QCOMPARE(depthTest->depthFunction(), Qt3DRender::QDepthTest::LessOrEqual);
        QCOMPARE(sortPolicy->parentFrameGraphNode(), stateSet);
        QVERIFY(sortPolicy->sortTypes().contains(Qt3DRender::QSortPolicy::BackToFront));
        QVERIFY(sortPolicy->sortTypes().contains(Qt3DRender::QSortPolicy::Material));
        QVERIFY(sortPolicy->sortTypes().contains(Qt3DRender::QSortPolicy::Texture));
        QCOMPARE(transparentFilter->parentFrameGraphNode(), sortPolicy);
        QVERIFY(transparentFilter->matchAny().contains(transparentFilterKey));
        QCOMPARE(transparentFilterKey->name(), QStringLiteral("KuesaDrawStage"));
        QCOMPARE(transparentFilterKey->value(), QStringLiteral("Transparent"));

        for (int i = 0; i < passFilters.count(); ++i) {
            const auto &passFilter = passFilters.at(i);
            QCOMPARE(passFilter->matchAny().size(), 1);
            QCOMPARE(passFilter->matchAny().at(0)->name(), QStringLiteral("Pass"));
            QCOMPARE(passFilter->matchAny().at(0)->value(), QStringLiteral("pass%1").arg(i));
        }

        // WHEN
        stage.setBackToFrontSorting(false);

        // THEN
        QCOMPARE(stage.backToFrontSorting(), false);

        QCOMPARE(stateSet->parentFrameGraphNode(), &stage);
        QVERIFY(stateSet->renderStates().contains(depthTest));
        QVERIFY(stateSet->renderStates().contains(noDepthMask));
        QVERIFY(stateSet->renderStates().contains(msaa));
        QCOMPARE(depthTest->depthFunction(), Qt3DRender::QDepthTest::LessOrEqual);
        QCOMPARE(sortPolicy->parentFrameGraphNode(), stateSet);
        QVERIFY(sortPolicy->sortTypes().contains(Qt3DRender::QSortPolicy::Material));
        QVERIFY(sortPolicy->sortTypes().contains(Qt3DRender::QSortPolicy::Texture));
        QCOMPARE(transparentFilter->parentFrameGraphNode(), sortPolicy);
        QVERIFY(transparentFilter->matchAny().contains(transparentFilterKey));
        QCOMPARE(transparentFilterKey->name(), QStringLiteral("KuesaDrawStage"));
        QCOMPARE(transparentFilterKey->value(), QStringLiteral("Transparent"));

        for (int i = 0; i < passFilters.count(); ++i) {
            const auto &passFilter = passFilters.at(i);
            QCOMPARE(passFilter->matchAny().size(), 1);
            QCOMPARE(passFilter->matchAny().at(0)->name(), QStringLiteral("Pass"));
            QCOMPARE(passFilter->matchAny().at(0)->value(), QStringLiteral("pass%1").arg(i));
        }
    }
};

QTEST_MAIN(tst_TransparentStage)
#include "tst_transparentstage.moc"
