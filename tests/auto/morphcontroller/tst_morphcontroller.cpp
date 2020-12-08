/*
    tst_morphcontroller.cpp

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QtTest/QSignalSpy>
#include <Kuesa/MorphController>

using namespace Kuesa;

class tst_MorphController : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        MorphController ctrl;

        // THEN
        QCOMPARE(ctrl.count(), 0);
        QCOMPARE(ctrl.weight0(), 0.0f);
        QCOMPARE(ctrl.weight1(), 0.0f);
        QCOMPARE(ctrl.weight2(), 0.0f);
        QCOMPARE(ctrl.weight3(), 0.0f);
        QCOMPARE(ctrl.weight4(), 0.0f);
        QCOMPARE(ctrl.weight5(), 0.0f);
        QCOMPARE(ctrl.weight6(), 0.0f);
        QCOMPARE(ctrl.weight7(), 0.0f);
    }

    void checkSetters()
    {
        // GIVEN
        MorphController ctrl;

        {
            QSignalSpy spy(&ctrl, SIGNAL(countChanged(int)));

            QVERIFY(spy.isValid());

            // WHEN
            ctrl.setCount(8);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(ctrl.count(), 8);
        }
        {
            QSignalSpy spy(&ctrl, SIGNAL(weight0Changed(float)));
            QSignalSpy spy2(&ctrl, SIGNAL(morphWeightsChanged()));

            QVERIFY(spy.isValid());
            QVERIFY(spy2.isValid());

            // WHEN
            ctrl.setWeight0(1584.0f);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(spy2.count(), 1);
            QCOMPARE(ctrl.count(), 8);
            QCOMPARE(ctrl.weight0(), 1584.0f);
            QCOMPARE(ctrl.weight1(), 0.0f);
            QCOMPARE(ctrl.weight2(), 0.0f);
            QCOMPARE(ctrl.weight3(), 0.0f);
            QCOMPARE(ctrl.weight4(), 0.0f);
            QCOMPARE(ctrl.weight5(), 0.0f);
            QCOMPARE(ctrl.weight6(), 0.0f);
            QCOMPARE(ctrl.weight7(), 0.0f);
        }
        {
            QSignalSpy spy(&ctrl, SIGNAL(weight1Changed(float)));
            QSignalSpy spy2(&ctrl, SIGNAL(morphWeightsChanged()));

            QVERIFY(spy.isValid());
            QVERIFY(spy2.isValid());

            // WHEN
            ctrl.setWeight1(1584.1f);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(spy2.count(), 1);
            QCOMPARE(ctrl.count(), 8);
            QCOMPARE(ctrl.weight0(), 1584.0f);
            QCOMPARE(ctrl.weight1(), 1584.1f);
            QCOMPARE(ctrl.weight2(), 0.0f);
            QCOMPARE(ctrl.weight3(), 0.0f);
            QCOMPARE(ctrl.weight4(), 0.0f);
            QCOMPARE(ctrl.weight5(), 0.0f);
            QCOMPARE(ctrl.weight6(), 0.0f);
            QCOMPARE(ctrl.weight7(), 0.0f);
        }
        {
            QSignalSpy spy(&ctrl, SIGNAL(weight2Changed(float)));
            QSignalSpy spy2(&ctrl, SIGNAL(morphWeightsChanged()));

            QVERIFY(spy.isValid());
            QVERIFY(spy2.isValid());

            // WHEN
            ctrl.setWeight2(1584.2f);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(spy2.count(), 1);
            QCOMPARE(ctrl.count(), 8);
            QCOMPARE(ctrl.weight0(), 1584.0f);
            QCOMPARE(ctrl.weight1(), 1584.1f);
            QCOMPARE(ctrl.weight2(), 1584.2f);
            QCOMPARE(ctrl.weight3(), 0.0f);
            QCOMPARE(ctrl.weight4(), 0.0f);
            QCOMPARE(ctrl.weight5(), 0.0f);
            QCOMPARE(ctrl.weight6(), 0.0f);
            QCOMPARE(ctrl.weight7(), 0.0f);
        }
        {
            QSignalSpy spy(&ctrl, SIGNAL(weight3Changed(float)));
            QSignalSpy spy2(&ctrl, SIGNAL(morphWeightsChanged()));

            QVERIFY(spy.isValid());
            QVERIFY(spy2.isValid());

            // WHEN
            ctrl.setWeight3(1584.3f);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(spy2.count(), 1);
            QCOMPARE(ctrl.count(), 8);
            QCOMPARE(ctrl.weight0(), 1584.0f);
            QCOMPARE(ctrl.weight1(), 1584.1f);
            QCOMPARE(ctrl.weight2(), 1584.2f);
            QCOMPARE(ctrl.weight3(), 1584.3f);
            QCOMPARE(ctrl.weight4(), 0.0f);
            QCOMPARE(ctrl.weight5(), 0.0f);
            QCOMPARE(ctrl.weight6(), 0.0f);
            QCOMPARE(ctrl.weight7(), 0.0f);
        }
        {
            QSignalSpy spy(&ctrl, SIGNAL(weight4Changed(float)));
            QSignalSpy spy2(&ctrl, SIGNAL(morphWeightsChanged()));

            QVERIFY(spy.isValid());
            QVERIFY(spy2.isValid());

            // WHEN
            ctrl.setWeight4(1584.4f);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(spy2.count(), 1);
            QCOMPARE(ctrl.count(), 8);
            QCOMPARE(ctrl.weight0(), 1584.0f);
            QCOMPARE(ctrl.weight1(), 1584.1f);
            QCOMPARE(ctrl.weight2(), 1584.2f);
            QCOMPARE(ctrl.weight3(), 1584.3f);
            QCOMPARE(ctrl.weight4(), 1584.4f);
            QCOMPARE(ctrl.weight5(), 0.0f);
            QCOMPARE(ctrl.weight6(), 0.0f);
            QCOMPARE(ctrl.weight7(), 0.0f);
        }
        {
            QSignalSpy spy(&ctrl, SIGNAL(weight5Changed(float)));
            QSignalSpy spy2(&ctrl, SIGNAL(morphWeightsChanged()));

            QVERIFY(spy.isValid());
            QVERIFY(spy2.isValid());

            // WHEN
            ctrl.setWeight5(1584.5f);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(spy2.count(), 1);
            QCOMPARE(ctrl.count(), 8);
            QCOMPARE(ctrl.weight0(), 1584.0f);
            QCOMPARE(ctrl.weight1(), 1584.1f);
            QCOMPARE(ctrl.weight2(), 1584.2f);
            QCOMPARE(ctrl.weight3(), 1584.3f);
            QCOMPARE(ctrl.weight4(), 1584.4f);
            QCOMPARE(ctrl.weight5(), 1584.5f);
            QCOMPARE(ctrl.weight6(), 0.0f);
            QCOMPARE(ctrl.weight7(), 0.0f);
        }
        {
            QSignalSpy spy(&ctrl, SIGNAL(weight6Changed(float)));
            QSignalSpy spy2(&ctrl, SIGNAL(morphWeightsChanged()));

            QVERIFY(spy.isValid());
            QVERIFY(spy2.isValid());

            // WHEN
            ctrl.setWeight6(1584.6f);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(spy2.count(), 1);
            QCOMPARE(ctrl.count(), 8);
            QCOMPARE(ctrl.weight0(), 1584.0f);
            QCOMPARE(ctrl.weight1(), 1584.1f);
            QCOMPARE(ctrl.weight2(), 1584.2f);
            QCOMPARE(ctrl.weight3(), 1584.3f);
            QCOMPARE(ctrl.weight4(), 1584.4f);
            QCOMPARE(ctrl.weight5(), 1584.5f);
            QCOMPARE(ctrl.weight6(), 1584.6f);
            QCOMPARE(ctrl.weight7(), 0.0f);
        }
        {
            QSignalSpy spy(&ctrl, SIGNAL(weight7Changed(float)));
            QSignalSpy spy2(&ctrl, SIGNAL(morphWeightsChanged()));

            QVERIFY(spy.isValid());
            QVERIFY(spy2.isValid());

            // WHEN
            ctrl.setWeight7(1584.7f);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(spy2.count(), 1);
            QCOMPARE(ctrl.count(), 8);
            QCOMPARE(ctrl.weight0(), 1584.0f);
            QCOMPARE(ctrl.weight1(), 1584.1f);
            QCOMPARE(ctrl.weight2(), 1584.2f);
            QCOMPARE(ctrl.weight3(), 1584.3f);
            QCOMPARE(ctrl.weight4(), 1584.4f);
            QCOMPARE(ctrl.weight5(), 1584.5f);
            QCOMPARE(ctrl.weight6(), 1584.6f);
            QCOMPARE(ctrl.weight7(), 1584.7f);
        }
        {
            QSignalSpy spy0(&ctrl, SIGNAL(weight0Changed(float)));
            QSignalSpy spy1(&ctrl, SIGNAL(weight1Changed(float)));
            QSignalSpy spy2(&ctrl, SIGNAL(weight2Changed(float)));
            QSignalSpy spy3(&ctrl, SIGNAL(weight3Changed(float)));
            QSignalSpy spy4(&ctrl, SIGNAL(weight4Changed(float)));
            QSignalSpy spy5(&ctrl, SIGNAL(weight5Changed(float)));
            QSignalSpy spy6(&ctrl, SIGNAL(weight6Changed(float)));
            QSignalSpy spy7(&ctrl, SIGNAL(weight7Changed(float)));
            QSignalSpy spy8(&ctrl, SIGNAL(morphWeightsChanged()));

            QVERIFY(spy0.isValid());
            QVERIFY(spy1.isValid());
            QVERIFY(spy2.isValid());
            QVERIFY(spy3.isValid());
            QVERIFY(spy4.isValid());
            QVERIFY(spy5.isValid());
            QVERIFY(spy6.isValid());
            QVERIFY(spy7.isValid());
            QVERIFY(spy8.isValid());

            // WHEN
            QVariantList l = QVariantList() << 1.0f
                                            << 2.0f
                                            << 3.0f
                                            << 4.0f
                                            << 5.0f
                                            << 6.0f
                                            << 7.0f
                                            << 8.0f;
            ctrl.setMorphWeights(l);

            // THEN
            QCOMPARE(spy0.count(), 1);
            QCOMPARE(spy1.count(), 1);
            QCOMPARE(spy2.count(), 1);
            QCOMPARE(spy3.count(), 1);
            QCOMPARE(spy4.count(), 1);
            QCOMPARE(spy5.count(), 1);
            QCOMPARE(spy6.count(), 1);
            QCOMPARE(spy7.count(), 1);
            QCOMPARE(spy8.count(), 1);
            QCOMPARE(ctrl.count(), 8);
            QCOMPARE(ctrl.weight0(), 1.0f);
            QCOMPARE(ctrl.weight1(), 2.0f);
            QCOMPARE(ctrl.weight2(), 3.0f);
            QCOMPARE(ctrl.weight3(), 4.0f);
            QCOMPARE(ctrl.weight4(), 5.0f);
            QCOMPARE(ctrl.weight5(), 6.0f);
            QCOMPARE(ctrl.weight6(), 7.0f);
            QCOMPARE(ctrl.weight7(), 8.0f);
        }
    }
};

QTEST_APPLESS_MAIN(tst_MorphController)

#include "tst_morphcontroller.moc"
