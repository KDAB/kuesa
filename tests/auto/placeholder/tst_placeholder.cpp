/*
    tst_placeholder.cpp

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <Kuesa/Placeholder>

class tst_Placeholder : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Qt3DCore::QEntity *parent = new Qt3DCore::QEntity;
        Kuesa::Placeholder placeholder(parent);

        // THEN
        QCOMPARE(placeholder.parentEntity(), parent);
        QCOMPARE(placeholder.camera(), nullptr);
        QCOMPARE(placeholder.viewport(), QRect());
        QCOMPARE(placeholder.target(), nullptr);
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
        Kuesa::Placeholder placeholder(nullptr);
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
        Kuesa::Placeholder placeholder(nullptr);
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
};

QTEST_MAIN(tst_Placeholder)
#include "tst_placeholder.moc"
