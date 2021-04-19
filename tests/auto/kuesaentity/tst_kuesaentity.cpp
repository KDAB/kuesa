/*
    tst_kuesanode.cpp

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

#include <qtkuesa-config.h>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <Kuesa/private/kuesaentity_p.h>

using namespace Kuesa;

class tst_KuesaEntity : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkCanAddDynamicProperties()
    {
        // GIVEN
        KuesaEntity e;

        // WHEN
        e.addExtraProperty(QStringLiteral("titi"), QVariant(883.0f));
        e.finalize();

        // THEN
        const QVariant v = e.property("titi");
        QCOMPARE(v, QVariant(883.0f));

        const QStringList extraNames = e.extraPropertyNames();
        QCOMPARE(extraNames, QStringList() << QStringLiteral("titi"));
    }

    void checkNotifiesOnDynamicPropertyWrite()
    {
        // GIVEN
        KuesaEntity e;
        e.addExtraProperty(QStringLiteral("titi"), QVariant(883.0f));
        e.finalize();

        // THEN
        const QMetaObject *m = e.metaObject();
        const int titiChangedIdx = m->indexOfMethod("titiChanged()");
        QVERIFY(titiChangedIdx >= 0);

        // WHEN
        QSignalSpy spy(&e, m->method(titiChangedIdx));
        QSignalSpy spy2(&e, SIGNAL(titiChanged()));

        // THEN
        QVERIFY(spy.isValid());
        QVERIFY(spy2.isValid());

        // WHEN
        e.setProperty("titi", QVariant(1584.0f));

        // THEN
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy2.count(), 1);
        QCOMPARE(e.property("titi"), QVariant(1584.0f));
    }
};

QTEST_MAIN(tst_KuesaEntity)

#include "tst_kuesaentity.moc"
