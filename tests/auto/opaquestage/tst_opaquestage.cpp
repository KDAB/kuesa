/*
    tst_opaquestage.cpp

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

#include <Kuesa/private/opaquerenderstage_p.h>

class tst_OpaqueStage : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void testFrameGraph()
    {
        // GIVEN
        Kuesa::OpaqueRenderStage stage;

        // THEN
        QCOMPARE(stage.zFilling(), false);
        QCOMPARE(stage.objectName(), QStringLiteral("KuesaOpaqueRenderStage"));

        Qt3DRender::QRenderPassFilter *passFilter = stage.findChild<Qt3DRender::QRenderPassFilter *>();
        Qt3DRender::QRenderStateSet *stateSet = stage.findChild<Qt3DRender::QRenderStateSet *>();
        Qt3DRender::QDepthTest *depthTest = stage.findChild<Qt3DRender::QDepthTest *>();
        Qt3DRender::QCullFace *cullFace = stage.findChild<Qt3DRender::QCullFace *>();
        Qt3DRender::QNoDepthMask *noDepthMask = stage.findChild<Qt3DRender::QNoDepthMask *>();
        Qt3DRender::QMultiSampleAntiAliasing *msaa = stage.findChild<Qt3DRender::QMultiSampleAntiAliasing *>();
        Qt3DRender::QSortPolicy *sortPolicy = stage.findChild<Qt3DRender::QSortPolicy *>();
        Qt3DRender::QFilterKey *filterKey = stage.findChild<Qt3DRender::QFilterKey *>();

        QVERIFY(passFilter);
        QVERIFY(stateSet);
        QVERIFY(depthTest);
        QVERIFY(cullFace);
        QVERIFY(noDepthMask);
        QVERIFY(msaa);
        QVERIFY(sortPolicy);
        QVERIFY(filterKey);

        QCOMPARE(passFilter->parentFrameGraphNode(), &stage);
        QVERIFY(passFilter->matchAny().contains(filterKey));
        QCOMPARE(filterKey->name(), QStringLiteral("KuesaDrawStage"));
        QCOMPARE(filterKey->value(), QStringLiteral("Opaque"));
        QCOMPARE(stateSet->parentFrameGraphNode(), passFilter);
        QCOMPARE(depthTest->depthFunction(), Qt3DRender::QDepthTest::Less);
        QCOMPARE(cullFace->mode(), Qt3DRender::QCullFace::Back);
        QVERIFY(stateSet->renderStates().contains(depthTest));
        QVERIFY(stateSet->renderStates().contains(msaa));
        QVERIFY(stateSet->renderStates().contains(cullFace));
        QVERIFY(!stateSet->renderStates().contains(noDepthMask));
        QCOMPARE(sortPolicy->parentFrameGraphNode(), stateSet);
        QVERIFY(sortPolicy->sortTypes().contains(Qt3DRender::QSortPolicy::Material));

        // WHEN
        stage.setZFilling(true);

        // THEN
        QCOMPARE(stage.zFilling(), true);

        QCOMPARE(passFilter->parentFrameGraphNode(), &stage);
        QVERIFY(passFilter->matchAny().contains(filterKey));
        QCOMPARE(filterKey->name(), QStringLiteral("KuesaDrawStage"));
        QCOMPARE(filterKey->value(), QStringLiteral("Opaque"));
        QCOMPARE(stateSet->parentFrameGraphNode(), passFilter);
        QCOMPARE(depthTest->depthFunction(), Qt3DRender::QDepthTest::Equal);
        QCOMPARE(cullFace->mode(), Qt3DRender::QCullFace::Back);
        QVERIFY(stateSet->renderStates().contains(depthTest));
        QVERIFY(stateSet->renderStates().contains(msaa));
        QVERIFY(stateSet->renderStates().contains(cullFace));
        QVERIFY(stateSet->renderStates().contains(noDepthMask));
        QCOMPARE(sortPolicy->parentFrameGraphNode(), stateSet);
        QVERIFY(sortPolicy->sortTypes().contains(Qt3DRender::QSortPolicy::Material));
    }
};

QTEST_MAIN(tst_OpaqueStage)
#include "tst_opaquestage.moc"
