/*
    tst_effectslibrary.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Kuesa/private/effectslibrary_p.h>

using namespace Kuesa;

class tst_EffectsLibrary : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkGetOrCreateEffect()
    {
        // GIVEN`
        EffectsLibrary effectsLib;

        // WHEN
        EffectProperties::Properties effectProps;
        auto effect = effectsLib.getOrCreateEffect(effectProps, nullptr);

        // THEN
        QVERIFY(effect == nullptr);

        // WHEN
        effectProps = EffectProperties::MetallicRoughness|EffectProperties::VertexColor;
        auto effects2 = effectsLib.getOrCreateEffect(effectProps, nullptr);

        // THEN
        QVERIFY(effects2 != nullptr);

        // WHEN
        effectProps = EffectProperties::MetallicRoughness|EffectProperties::Skinning;
        auto effects3 = effectsLib.getOrCreateEffect(effectProps, nullptr);

        // THEN
        QVERIFY(effects3 != nullptr);
        QVERIFY(effects3 != effects2);

        // WHEN
        {
            EffectProperties::Properties props;
            props |= EffectProperties::MetallicRoughness|EffectProperties::VertexColor;
            auto tmpEffect = effectsLib.getOrCreateEffect(props, nullptr);
            // THEN
            QCOMPARE(tmpEffect, effects2);
        }
        // WHEN
        {
            EffectProperties::Properties props;
            props |= EffectProperties::MetallicRoughness|EffectProperties::Skinning;
            auto tmpEffect = effectsLib.getOrCreateEffect(props, nullptr);
            // THEN
            QCOMPARE(tmpEffect, effects3);
        }
    }
};

QTEST_MAIN(tst_EffectsLibrary)

#include "tst_effectslibrary.moc"
