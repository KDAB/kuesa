/*
    tst_tonemappingandgammacorrectioneffect.cpp

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
#include <QtTest/QSignalSpy>
#include <Kuesa/ToneMappingAndGammaCorrectionEffect>

using namespace Kuesa;

class tst_ToneMappingAndGammaCorrectionEffect : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping>("ToneMappingAndGammaCorrectionEffect::ToneMapping");
    }

    void checkEffectDefaultState()
    {
        // GIVEN
        Kuesa::ToneMappingAndGammaCorrectionEffect effect;

        QCOMPARE(effect.exposure(), 0.0f);
        QCOMPARE(effect.gamma(), 2.2f);
        QCOMPARE(effect.toneMappingAlgorithm(), Kuesa::ToneMappingAndGammaCorrectionEffect::None);
    }

    void checkEffectProperties()
    {
        // GIVEN
        Kuesa::ToneMappingAndGammaCorrectionEffect effect;

        {
            QSignalSpy signalSpy(&effect, &Kuesa::ToneMappingAndGammaCorrectionEffect::gammaChanged);

            // WHEN
            effect.setGamma(3.3f);

            // THEN
            QCOMPARE(effect.gamma(), 3.3f);
            QCOMPARE(signalSpy.count(), 1);

            signalSpy.clear();

            // WHEN
            effect.setGamma(3.3f);

            // THEN
            QCOMPARE(signalSpy.count(), 0);
        }
        {
            QSignalSpy signalSpy(&effect, &Kuesa::ToneMappingAndGammaCorrectionEffect::exposureChanged);

            // WHEN
            effect.setExposure(1.0f);

            // THEN
            QCOMPARE(effect.exposure(), 1.0f);
            QCOMPARE(signalSpy.count(), 1);

            signalSpy.clear();

            // WHEN
            effect.setExposure(1.0f);

            // THEN
            QCOMPARE(signalSpy.count(), 0);
        }
        {
            QSignalSpy signalSpy(&effect, &Kuesa::ToneMappingAndGammaCorrectionEffect::toneMappingAlgorithmChanged);

            // WHEN
            effect.setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::Filmic);

            // THEN
            QCOMPARE(effect.toneMappingAlgorithm(), Kuesa::ToneMappingAndGammaCorrectionEffect::Filmic);
            QCOMPARE(signalSpy.count(), 1);

            signalSpy.clear();

            // WHEN
            effect.setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::Filmic);

            // THEN
            QCOMPARE(signalSpy.count(), 0);
        }
    }
};

QTEST_MAIN(tst_ToneMappingAndGammaCorrectionEffect)

#include "tst_tonemappingandgammacorrectioneffect.moc"
