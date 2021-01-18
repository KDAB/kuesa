/*
    tst_placeholder.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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
#include <QtTest/QSignalSpy>
#include <Qt3DCore/QTransform>
#include <Kuesa/Placeholder>
#include <Kuesa/private/kuesa_utils_p.h>
#include <Qt3DCore/private/qtransform_p.h>

class tst_Placeholder : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Qt3DCore::QEntity parent;
        Kuesa::Placeholder *placeholder = new Kuesa::Placeholder(&parent);

        // THEN
        QCOMPARE(placeholder->parentEntity(), &parent);
        QCOMPARE(placeholder->camera(), nullptr);
        QCOMPARE(placeholder->viewport(), QRect());
        QCOMPARE(placeholder->target(), nullptr);
    }

    void testCamera()
    {
        // GIVEN
        Kuesa::Placeholder placeholder(nullptr);
        QSignalSpy spy(&placeholder, &Kuesa::Placeholder::cameraChanged);
        QVERIFY(spy.isValid());

        // WHEN
        Qt3DRender::QCamera *camera = new Qt3DRender::QCamera;
        placeholder.setCamera(camera);

        // THEN
        QCOMPARE(placeholder.camera(), camera);
        QCOMPARE(spy.count(), 1);

        // WHEN
        placeholder.setCamera(camera);

        // THEN
        QCOMPARE(spy.count(), 1);

        // WHEN -> camera destroyed
        delete camera;

        // THEN
        QCOMPARE(placeholder.camera(), nullptr);
        QCOMPARE(spy.count(), 2);
    }

    void testViewport()
    {
        // GIVEN
        Kuesa::Placeholder placeholder;
        QSignalSpy spy(&placeholder, &Kuesa::Placeholder::viewportChanged);
        QVERIFY(spy.isValid());

        // WHEN
        QRect viewport(0, 0, 101, 10);
        placeholder.setViewport(viewport);

        // THEN
        QCOMPARE(placeholder.viewport(), viewport);
        QCOMPARE(spy.count(), 1);

        // WHEN
        placeholder.setViewport(viewport);

        // THEN
        QCOMPARE(spy.count(), 1);
    }

    void testTarget()
    {
        // GIVEN
        Kuesa::Placeholder placeholder;
        QSignalSpy spy(&placeholder, &Kuesa::Placeholder::targetChanged);
        QVERIFY(spy.isValid());

        // WHEN
        QObject *target = new QObject;
        placeholder.setTarget(target);

        // THEN
        QCOMPARE(placeholder.target(), target);
        QCOMPARE(spy.count(), 1);

        // WHEN
        placeholder.setTarget(target);

        // THEN
        QCOMPARE(spy.count(), 1);

        // WHEN -> camera destroyed
        delete target;

        // THEN
        QCOMPARE(placeholder.target(), nullptr);
        QCOMPARE(spy.count(), 2);
    }

    void checkUpdatesTarget()
    {
        // GIVEN
        Kuesa::Placeholder placeholder;
        Qt3DCore::QTransform *placeHolderEntityTransform = Kuesa::componentFromEntity<Qt3DCore::QTransform>(&placeholder);
        QObject *target = new QObject();
        Qt3DRender::QCamera *camera = new Qt3DRender::QCamera;

        auto simulateBackendWorldMatrixUpdate = [&] (Qt3DCore::QTransform *t) {
            Qt3DCore::QTransformPrivate *d = static_cast<Qt3DCore::QTransformPrivate *>(Qt3DCore::QNodePrivate::get(t));
            d->setWorldMatrix(t->matrix());
        };

        // THEN
        QVERIFY(placeHolderEntityTransform);

        // WHEN
        camera->setPosition(QVector3D(0.0f, 0.0f, -10.0f));
        camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
        camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));

        camera->setProjectionType(Qt3DRender::QCameraLens::OrthographicProjection);
        camera->setLeft(-1.0f);
        camera->setTop(1.0f);
        camera->setBottom(-1.0f);
        camera->setRight(1.0f);

        placeHolderEntityTransform->setScale(2.0f);

        placeholder.setViewport({0, 0, 800, 600});
        placeholder.setCamera(camera);
        placeholder.setTarget(target);

        simulateBackendWorldMatrixUpdate(placeHolderEntityTransform);
        simulateBackendWorldMatrixUpdate(camera->transform());

        // THEN
        QCOMPARE(target->property("x").toInt(), -400);
        QCOMPARE(target->property("y").toInt(), -300);
        QCOMPARE(target->property("width").toInt(), 1600);
        QCOMPARE(target->property("height").toInt(), 1200);

        // WHEN
        placeHolderEntityTransform->setScale(1.0f);
        simulateBackendWorldMatrixUpdate(placeHolderEntityTransform);

        // THEN
        QCOMPARE(target->property("x").toInt(), 0);
        QCOMPARE(target->property("y").toInt(), 0);
        QCOMPARE(target->property("width").toInt(), 800);
        QCOMPARE(target->property("height").toInt(), 600);

        // WHEN
        simulateBackendWorldMatrixUpdate(camera->transform());
        camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);

        // THEN
        QCOMPARE(target->property("x").toInt(), 220);
        QCOMPARE(target->property("y").toInt(), 165);
        QCOMPARE(target->property("width").toInt(),360);
        QCOMPARE(target->property("height").toInt(), 270);
    }

    void checkHandlesInvalidCameraSanely()
    {
        // GIVEN
        Kuesa::Placeholder placeholder;
        QObject target;
        placeholder.setTarget(&target);

        {
            // WHEN
            Qt3DCore::QEntity e;
            placeholder.setCamera(&e);

            // THEN -> shouldnt Crash
        }

        {
            // WHEN
            Qt3DCore::QEntity e;
            Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
            e.addComponent(transform);
            placeholder.setCamera(&e);

            // THEN -> shouldnt Crash
        }

        {
            // WHEN
            Qt3DCore::QEntity e;
            Qt3DRender::QCameraLens *lens = new Qt3DRender::QCameraLens;
            e.addComponent(lens);
            placeholder.setCamera(&e);

            // THEN -> shouldnt Crash
        }
    }
};

QTEST_MAIN(tst_Placeholder)
#include "tst_placeholder.moc"
