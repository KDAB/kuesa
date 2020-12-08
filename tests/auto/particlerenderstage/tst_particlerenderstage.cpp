/*
    tst_particlerenderstage.cpp

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

#include <QtTest/QTest>

#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QDispatchCompute>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QMemoryBarrier>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QMultiSampleAntiAliasing>
#include <Qt3DRender/QNoDepthMask>
#include <Kuesa/private/particlerenderstage_p.h>

class tst_ParticleRenderStage : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkFrameGraph()
    {
        // GIVEN
        Kuesa::ParticleRenderStage stages;

        // THEN
        {
            QCOMPARE(stages.children().size(), 4);
            Qt3DRender::QRenderPassFilter *emitPassFilter = qobject_cast<Qt3DRender::QRenderPassFilter *>(stages.children()[0]);
            Qt3DRender::QRenderPassFilter *simulatePassFilter = qobject_cast<Qt3DRender::QRenderPassFilter *>(stages.children()[1]);
            Qt3DRender::QRenderPassFilter *sortPassFilter = qobject_cast<Qt3DRender::QRenderPassFilter *>(stages.children()[2]);
            Qt3DRender::QRenderPassFilter *renderPassFilter = qobject_cast<Qt3DRender::QRenderPassFilter *>(stages.children()[3]);
            QVERIFY(emitPassFilter);
            QVERIFY(simulatePassFilter);
            QVERIFY(sortPassFilter);
            QVERIFY(renderPassFilter);

            {
                QCOMPARE(emitPassFilter->children().size(), 2);
                Qt3DRender::QFilterKey *filter = qobject_cast<Qt3DRender::QFilterKey *>(emitPassFilter->children().first());
                Qt3DRender::QDispatchCompute *compute = qobject_cast<Qt3DRender::QDispatchCompute *>(emitPassFilter->children().last());
                QVERIFY(filter);
                QVERIFY(compute);

                QCOMPARE(emitPassFilter->matchAny().size(), 1);
                QCOMPARE(emitPassFilter->matchAny().first(), filter);
                QCOMPARE(filter->name(), QStringLiteral("KuesaComputeStage"));
                QCOMPARE(filter->value(), QStringLiteral("ParticleEmit"));
            }
            {
                QCOMPARE(simulatePassFilter->children().size(), 2);
                Qt3DRender::QFilterKey *filter = qobject_cast<Qt3DRender::QFilterKey *>(simulatePassFilter->children().first());
                Qt3DRender::QMemoryBarrier *barrier = qobject_cast<Qt3DRender::QMemoryBarrier *>(simulatePassFilter->children()[1]);
                QVERIFY(filter);
                QVERIFY(barrier);

                QCOMPARE(barrier->children().size(), 1);
                Qt3DRender::QDispatchCompute *compute = qobject_cast<Qt3DRender::QDispatchCompute *>(barrier->children().last());
                QVERIFY(compute);
                QCOMPARE(barrier->waitOperations(), Qt3DRender::QMemoryBarrier::ShaderStorage);

                QCOMPARE(simulatePassFilter->matchAny().size(), 1);
                QCOMPARE(simulatePassFilter->matchAny().first(), filter);
                QCOMPARE(filter->name(), QStringLiteral("KuesaComputeStage"));
                QCOMPARE(filter->value(), QStringLiteral("ParticleSimulate"));
            }
            {
                QCOMPARE(sortPassFilter->children().size(), 2);
                Qt3DRender::QFilterKey *filter = qobject_cast<Qt3DRender::QFilterKey *>(sortPassFilter->children().first());
                Qt3DRender::QMemoryBarrier *barrier = qobject_cast<Qt3DRender::QMemoryBarrier *>(sortPassFilter->children()[1]);
                QVERIFY(filter);
                QVERIFY(barrier);

                QCOMPARE(barrier->children().size(), 1);
                Qt3DRender::QDispatchCompute *compute = qobject_cast<Qt3DRender::QDispatchCompute *>(barrier->children().last());
                QVERIFY(compute);
                QCOMPARE(barrier->waitOperations(), Qt3DRender::QMemoryBarrier::ShaderStorage);

                QCOMPARE(sortPassFilter->matchAny().size(), 1);
                QCOMPARE(sortPassFilter->matchAny().first(), filter);
                QCOMPARE(filter->name(), QStringLiteral("KuesaComputeStage"));
                QCOMPARE(filter->value(), QStringLiteral("ParticleSort"));
            }
            {
                QCOMPARE(renderPassFilter->children().size(), 2);
                Qt3DRender::QFilterKey *filter = qobject_cast<Qt3DRender::QFilterKey *>(renderPassFilter->children().first());
                Qt3DRender::QMemoryBarrier *barrier = qobject_cast<Qt3DRender::QMemoryBarrier *>(renderPassFilter->children()[1]);
                QVERIFY(filter);
                QVERIFY(barrier);

                QCOMPARE(barrier->children().size(), 1);
                Qt3DRender::QRenderStateSet *stateSet = qobject_cast<Qt3DRender::QRenderStateSet *>(barrier->children().last());
                QVERIFY(stateSet);
                QCOMPARE(barrier->waitOperations(), Qt3DRender::QMemoryBarrier::VertexAttributeArray|Qt3DRender::QMemoryBarrier::ShaderStorage);

                QCOMPARE(renderPassFilter->matchAny().size(), 1);
                QCOMPARE(renderPassFilter->matchAny().first(), filter);
                QCOMPARE(filter->name(), QStringLiteral("KuesaDrawStage"));
                QCOMPARE(filter->value(), QStringLiteral("ParticleRender"));

                QCOMPARE(stateSet->children().size(), 3);
                QCOMPARE(stateSet->renderStates().size(), 3);

                Qt3DRender::QDepthTest *depthTest = qobject_cast<Qt3DRender::QDepthTest *>(stateSet->renderStates().first());
                QVERIFY(depthTest);
                QCOMPARE(depthTest->depthFunction(), Qt3DRender::QDepthTest::LessOrEqual);
                QCOMPARE(stateSet->children().first(), depthTest);

                Qt3DRender::QMultiSampleAntiAliasing *msaa = qobject_cast<Qt3DRender::QMultiSampleAntiAliasing *>(stateSet->renderStates()[1]);
                QVERIFY(msaa);
                QCOMPARE(stateSet->children()[1], msaa);

                Qt3DRender::QNoDepthMask *noDepthWrite = qobject_cast<Qt3DRender::QNoDepthMask *>(stateSet->renderStates().last());
                QVERIFY(noDepthWrite);
                QCOMPARE(stateSet->children()[2], noDepthWrite);
            }
        }
    }
};

QTEST_MAIN(tst_ParticleRenderStage)
#include "tst_particlerenderstage.moc"
