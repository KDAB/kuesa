/*
    tst_reflectionstages.cpp

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

#include <Qt3DRender/QTechniqueFilter>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QLayerFilter>

#include <Kuesa/forwardrenderer.h>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/private/opaquerenderstage_p.h>
#include <Kuesa/private/zfillrenderstage_p.h>
#include <Kuesa/private/transparentrenderstage_p.h>
#include <Kuesa/private/scenestages_p.h>
#include <Kuesa/private/reflectionstages_p.h>

class tst_ReflectionStages : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkFrameGraph()
    {
        // GIVEN
        Kuesa::ReflectionStages stages;

        QCOMPARE(stages.skinning(), false);
        QCOMPARE(stages.zFilling(), false);
        QCOMPARE(stages.frustumCulling(), false);
        QCOMPARE(stages.backToFrontSorting(), false);
        QVERIFY(stages.camera() == nullptr);
        QCOMPARE(stages.layers().size(), 0);
        QCOMPARE(stages.viewport(), QRectF(0.0f, 0.0f, 1.0f, 1.0f));

        // THEN
        {
            QCOMPARE(stages.children().size(), 4);
            Qt3DRender::QViewport *vp = qobject_cast<Qt3DRender::QViewport *>(stages.children().first());
            QVERIFY(vp);

            Qt3DRender::QParameter *reflectiveEnabledParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[1]);
            QVERIFY(reflectiveEnabledParameter);
            QCOMPARE(reflectiveEnabledParameter->name(), QStringLiteral("isReflective"));
            QCOMPARE(reflectiveEnabledParameter->value(), true);

            Qt3DRender::QParameter *reflectivePlaneParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[2]);
            QVERIFY(reflectivePlaneParameter);
            QCOMPARE(reflectivePlaneParameter->name(), QStringLiteral("reflectionPlane"));
            QCOMPARE(reflectivePlaneParameter->value(), QVector4D());

            Qt3DRender::QClearBuffers *clearBuffers = qobject_cast<Qt3DRender::QClearBuffers *>(stages.children().last());
            QVERIFY(clearBuffers);

            QCOMPARE(vp->children().size(), 1);
            Qt3DRender::QCameraSelector *cameraSelector = qobject_cast<Qt3DRender::QCameraSelector *>(vp->children().first());
            QVERIFY(cameraSelector);

            QCOMPARE(cameraSelector->children().size(), 2);
            Kuesa::ScenePass *opaquePass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().first());
            QVERIFY(opaquePass);
            QCOMPARE(opaquePass->type(), Kuesa::ScenePass::Opaque);

            Kuesa::ScenePass *transparentPass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().last());
            QVERIFY(transparentPass);
            QCOMPARE(transparentPass->type(), Kuesa::ScenePass::Transparent);
        }

        // WHEN
        stages.setZFilling(true);

        // THEN
        {
            QCOMPARE(stages.children().size(), 4);
            Qt3DRender::QViewport *vp = qobject_cast<Qt3DRender::QViewport *>(stages.children().first());
            QVERIFY(vp);

            Qt3DRender::QParameter *reflectiveEnabledParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[1]);
            QVERIFY(reflectiveEnabledParameter);
            QCOMPARE(reflectiveEnabledParameter->name(), QStringLiteral("isReflective"));
            QCOMPARE(reflectiveEnabledParameter->value(), true);

            Qt3DRender::QParameter *reflectivePlaneParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[2]);
            QVERIFY(reflectivePlaneParameter);
            QCOMPARE(reflectivePlaneParameter->name(), QStringLiteral("reflectionPlane"));
            QCOMPARE(reflectivePlaneParameter->value(), QVector4D());

            Qt3DRender::QClearBuffers *clearBuffers = qobject_cast<Qt3DRender::QClearBuffers *>(stages.children().last());
            QVERIFY(clearBuffers);

            QCOMPARE(vp->children().size(), 1);
            Qt3DRender::QCameraSelector *cameraSelector = qobject_cast<Qt3DRender::QCameraSelector *>(vp->children().first());
            QVERIFY(cameraSelector);

            QCOMPARE(cameraSelector->children().size(), 3);
            Kuesa::ScenePass *zFillPass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().first());
            QVERIFY(zFillPass);
            QCOMPARE(zFillPass->type(), Kuesa::ScenePass::ZFill);
            QCOMPARE(zFillPass->zFilling(), true);

            Kuesa::ScenePass *opaquePass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children()[1]);
            QVERIFY(opaquePass);
            QCOMPARE(opaquePass->type(), Kuesa::ScenePass::Opaque);
            QCOMPARE(opaquePass->zFilling(), true);

            Kuesa::ScenePass *transparentPass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().last());
            QVERIFY(transparentPass);
            QCOMPARE(transparentPass->type(), Kuesa::ScenePass::Transparent);
            QCOMPARE(transparentPass->zFilling(), true);
        }
    }
};

QTEST_MAIN(tst_ReflectionStages)
#include "tst_reflectionstages.moc"
