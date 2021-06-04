/*
    tst_animationpulse.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Jose Casafranca <juan.casafranca@kdab.com>

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
#include <Kuesa/AnimationPulse>
#include <QSignalSpy>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_AnimationPulse : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void checkDefaultValue()
    {
        Kuesa::AnimationPulse animationPulse;
        QCOMPARE(animationPulse.pulse(), 0.0f);
    }

    void checkUpDownSignals()
    {
        // GIVEN
        Kuesa::AnimationPulse animationPulse;

        QSignalSpy spyUp(&animationPulse, &Kuesa::AnimationPulse::up);
        QSignalSpy spyDown(&animationPulse, &Kuesa::AnimationPulse::down);

        // THEN
        Qt3DCore::QTransform *proxyTransform = qobject_cast<Qt3DCore::QTransform *>(animationPulse.targets().last());
        QVERIFY(proxyTransform);

        // WHEN
        proxyTransform->setTranslation({ 0, 0, 0 });

        // THEN
        QCOMPARE(spyUp.count(), 0);
        QCOMPARE(spyDown.count(), 0);

        // WHEN
        proxyTransform->setTranslation({ 0, 0.4, 0 });

        // THEN
        QCOMPARE(spyUp.count(), 0);
        QCOMPARE(spyDown.count(), 0);

        // WHEN
        proxyTransform->setTranslation({ 0, 0.6, 0 });

        // THEN
        QCOMPARE(spyUp.count(), 1);
        QCOMPARE(spyDown.count(), 0);

        // WHEN
        proxyTransform->setTranslation({ 0, 1.0, 0 });

        // THEN
        QCOMPARE(spyUp.count(), 1);
        QCOMPARE(spyDown.count(), 0);

        // WHEN
        proxyTransform->setTranslation({ 0, 1.6, 0 });

        // THEN
        QCOMPARE(spyUp.count(), 2);
        QCOMPARE(spyDown.count(), 0);

        // WHEN
        proxyTransform->setTranslation({ 0, 1.0, 0 });

        // THEN
        QCOMPARE(spyUp.count(), 2);
        QCOMPARE(spyDown.count(), 1);

        // WHEN
        proxyTransform->setTranslation({ 0, 0.6, 0 });

        // THEN
        QCOMPARE(spyUp.count(), 2);
        QCOMPARE(spyDown.count(), 1);

        // WHEN
        proxyTransform->setTranslation({ 0, 0.2, 0 });

        // THEN
        QCOMPARE(spyUp.count(), 2);
        QCOMPARE(spyDown.count(), 2);
    }
};

QTEST_APPLESS_MAIN(tst_AnimationPulse)

#include "tst_animationpulse.moc"
