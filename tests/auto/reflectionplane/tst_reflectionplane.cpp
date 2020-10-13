/*
    tst_reflectionplane.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#include <Kuesa/reflectionplane.h>
#include <Qt3DRender/QLayer>
#include <QVector4D>

class tst_ReflectionPlane : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Kuesa::ReflectionPlane plane;

        // THEN
        QCOMPARE(plane.equation(), QVector4D());
        QCOMPARE(plane.layers().size(), size_t(0));
    }

    void testEquation()
    {
        // GIVEN
        Kuesa::ReflectionPlane plane;
        QSignalSpy spy(&plane, &Kuesa::ReflectionPlane::equationChanged);
        QVERIFY(spy.isValid());

        // WHEN
        QVector4D newEquation(0.0f, 0.0f, 1.0f, 0.5f);
        plane.setEquation(newEquation);

        // THEN
        QCOMPARE(plane.equation(), newEquation);
        QCOMPARE(spy.count(), 1);

        // WHEN
        plane.setEquation(newEquation);

        // THEN
        QCOMPARE(spy.count(), 1);
    }

    void testLayers()
    {
        // GIVEN
        Kuesa::ReflectionPlane plane;
        QSignalSpy spy(&plane, &Kuesa::ReflectionPlane::layersChanged);
        QVERIFY(spy.isValid());

        {
            // WHEN
            Qt3DRender::QLayer layer;
            plane.addLayer(&layer);

            // THEN
            QCOMPARE(plane.layers().size(), size_t(1));
            QCOMPARE(plane.layers().front(), &layer);
            QCOMPARE(spy.count(), 1);
        }

        // WHEN -> layer destroyed

        // THEN
        QCOMPARE(spy.count(), 2);
        QCOMPARE(plane.layers().size(), size_t(0));
    }
};

QTEST_MAIN(tst_ReflectionPlane)
#include "tst_reflectionplane.moc"
