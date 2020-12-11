/*
    tst_effectcollection.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#include <Kuesa/effectcollection.h>
#include <Qt3DRender/qeffect.h>

class tst_EffectCollection : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void shouldSupportFactory()
    {
        // GIVEN
        Kuesa::EffectCollection collection;

        // WHEN
        collection.registerType("effectType", []() { return new Qt3DRender::QEffect(); });
        auto effect = collection.addEffect("effect", "effectType", {});

        // THEN
        QVERIFY(effect);
        QCOMPARE(collection.names().size(), 1);

        // WHEN
        collection.unregisterType("effectType");
        effect = collection.addEffect("effect", "effectType", {});

        // THEN
        QVERIFY(!effect);
        QVERIFY(collection.names().empty());
    }
};

QTEST_GUILESS_MAIN(tst_EffectCollection)
#include "tst_effectcollection.moc"
