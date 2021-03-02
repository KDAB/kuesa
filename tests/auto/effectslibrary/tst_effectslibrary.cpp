/*
    tst_effectslibrary.cpp

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
#include <Kuesa/private/effectslibrary_p.h>
#include <Kuesa/metallicroughnesseffect.h>
#include <Kuesa/unliteffect.h>
#include <Kuesa/irodiffuseeffect.h>

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
        effectProps = EffectProperties::MetallicRoughness | EffectProperties::VertexColor;
        auto effects2 = effectsLib.getOrCreateEffect(effectProps, nullptr);

        // THEN
        QVERIFY(effects2 != nullptr);

        // WHEN
        effectProps = EffectProperties::MetallicRoughness | EffectProperties::Skinning;
        auto effects3 = effectsLib.getOrCreateEffect(effectProps, nullptr);

        // THEN
        QVERIFY(effects3 != nullptr);
        QVERIFY(effects3 != effects2);

        // WHEN
        {
            EffectProperties::Properties props;
            props |= EffectProperties::MetallicRoughness | EffectProperties::VertexColor;
            auto tmpEffect = effectsLib.getOrCreateEffect(props, nullptr);
            // THEN
            QCOMPARE(tmpEffect, effects2);
        }
        // WHEN
        {
            EffectProperties::Properties props;
            props |= EffectProperties::MetallicRoughness | EffectProperties::Skinning;
            auto tmpEffect = effectsLib.getOrCreateEffect(props, nullptr);
            // THEN
            QCOMPARE(tmpEffect, effects3);
        }
    }

    void checkMetallicRoughnessEffectInitialState()
    {
        // GIVEN
        Kuesa::MetallicRoughnessEffect e;

        // THEN
        QCOMPARE(e.isDoubleSided(), false);
        QCOMPARE(e.isUsingSkinning(), false);
        QCOMPARE(e.isOpaque(), true);
        QCOMPARE(e.isAlphaCutoffEnabled(), false);
        QCOMPARE(e.isUsingColorAttribute(), false);
        QCOMPARE(e.isUsingNormalAttribute(), false);
        QCOMPARE(e.isUsingTangentAttribute(), false);
        QCOMPARE(e.isUsingTexCoordAttribute(), false);
        QCOMPARE(e.isUsingTexCoord1Attribute(), false);
        QCOMPARE(e.isUsingMorphTargets(), false);
        QCOMPARE(e.isBaseColorMapEnabled(), false);
        QCOMPARE(e.isMetalRoughMapEnabled(), false);
        QCOMPARE(e.isNormalMapEnabled(), false);
        QCOMPARE(e.isAmbientOcclusionMapEnabled(), false);
        QCOMPARE(e.isEmissiveMapEnabled(), false);
    }

    void checkCreateMetallicRoughnessEffectWithKey_data()
    {
        QTest::addColumn<EffectProperties::Properties>("props");

        QTest::newRow("BaseColorMap") << EffectProperties::Properties(EffectProperties::BaseColorMap);
        QTest::newRow("MetalRoughnessMap") << EffectProperties::Properties(EffectProperties::MetalRoughnessMap);
        QTest::newRow("AmbientOcclusionMap") << EffectProperties::Properties(EffectProperties::AOMap);
        QTest::newRow("NormalMap") << EffectProperties::Properties(EffectProperties::NormalMap);
        QTest::newRow("EmissiveMap") << EffectProperties::Properties(EffectProperties::EmissiveMap);
        QTest::newRow("Blend") << EffectProperties::Properties(EffectProperties::Blend);
        QTest::newRow("Mask") << EffectProperties::Properties(EffectProperties::Mask);
        QTest::newRow("DoubleSided") << EffectProperties::Properties(EffectProperties::DoubleSided);
        QTest::newRow("VertexColor") << EffectProperties::Properties(EffectProperties::VertexColor);
        QTest::newRow("VertexNormal") << EffectProperties::Properties(EffectProperties::VertexNormal);
        QTest::newRow("VertexTangent") << EffectProperties::Properties(EffectProperties::VertexTangent);
        QTest::newRow("VertexTexCoord") << EffectProperties::Properties(EffectProperties::VertexTexCoord);
        QTest::newRow("VertexTexCoord1") << EffectProperties::Properties(EffectProperties::VertexTexCoord1);
        QTest::newRow("Skinning") << EffectProperties::Properties(EffectProperties::Skinning);
        QTest::newRow("MorphTargets") << EffectProperties::Properties(EffectProperties::MorphTargets);
    }

    void checkCreateMetallicRoughnessEffectWithKey()
    {
        // GIVEN
        QFETCH(EffectProperties::Properties, props);

        // WHEN
        MetallicRoughnessEffect *e = EffectsLibrary::createMetallicRoughnessEffectWithKey(props);

        // THEN
        QCOMPARE(e->isBaseColorMapEnabled(), (props & EffectProperties::BaseColorMap) != 0);
        QCOMPARE(e->isMetalRoughMapEnabled(), (props & EffectProperties::MetalRoughnessMap) != 0);
        QCOMPARE(e->isAmbientOcclusionMapEnabled(), (props & EffectProperties::AOMap) != 0);
        QCOMPARE(e->isNormalMapEnabled(), (props & EffectProperties::NormalMap) != 0);
        QCOMPARE(e->isEmissiveMapEnabled(), (props & EffectProperties::EmissiveMap) != 0);
        QCOMPARE(e->isOpaque(), !(props & EffectProperties::Blend));
        QCOMPARE(e->isAlphaCutoffEnabled(), (props & EffectProperties::Mask) != 0);
        QCOMPARE(e->isDoubleSided(), (props & EffectProperties::DoubleSided) != 0);
        QCOMPARE(e->isUsingColorAttribute(), (props & EffectProperties::VertexColor) != 0);
        QCOMPARE(e->isUsingNormalAttribute(), (props & EffectProperties::VertexNormal) != 0);
        QCOMPARE(e->isUsingTangentAttribute(), (props & EffectProperties::VertexTangent) != 0);
        QCOMPARE(e->isUsingTexCoordAttribute(), (props & EffectProperties::VertexTexCoord) != 0);
        QCOMPARE(e->isUsingTexCoord1Attribute(), (props & EffectProperties::VertexTexCoord1) != 0);
        QCOMPARE(e->isUsingSkinning(), (props & EffectProperties::Skinning) != 0);
        QCOMPARE(e->isUsingMorphTargets(), (props & EffectProperties::MorphTargets) != 0);

        delete e;
    }

    void checkUnlitEffectInitialState()
    {
        // GIVEN
        Kuesa::UnlitEffect e;

        // THEN
        QCOMPARE(e.isDoubleSided(), false);
        QCOMPARE(e.isUsingSkinning(), false);
        QCOMPARE(e.isOpaque(), true);
        QCOMPARE(e.isAlphaCutoffEnabled(), false);
        QCOMPARE(e.isUsingColorAttribute(), false);
        QCOMPARE(e.isUsingNormalAttribute(), false);
        QCOMPARE(e.isUsingTangentAttribute(), false);
        QCOMPARE(e.isUsingTexCoordAttribute(), false);
        QCOMPARE(e.isUsingTexCoord1Attribute(), false);
        QCOMPARE(e.isUsingMorphTargets(), false);
        QCOMPARE(e.isBaseColorMapEnabled(), false);
        QCOMPARE(e.isUsingColorAttribute(), false);
    }

    void checkCreateUnlitEffectWithKey_data()
    {
        QTest::addColumn<EffectProperties::Properties>("props");

        QTest::newRow("BaseColorMap") << EffectProperties::Properties(EffectProperties::BaseColorMap);
        QTest::newRow("Blend") << EffectProperties::Properties(EffectProperties::Blend);
        QTest::newRow("Mask") << EffectProperties::Properties(EffectProperties::Mask);
        QTest::newRow("DoubleSided") << EffectProperties::Properties(EffectProperties::DoubleSided);
        QTest::newRow("VertexColor") << EffectProperties::Properties(EffectProperties::VertexColor);
        QTest::newRow("VertexNormal") << EffectProperties::Properties(EffectProperties::VertexNormal);
        QTest::newRow("VertexTangent") << EffectProperties::Properties(EffectProperties::VertexTangent);
        QTest::newRow("VertexTexCoord") << EffectProperties::Properties(EffectProperties::VertexTexCoord);
        QTest::newRow("VertexTexCoord1") << EffectProperties::Properties(EffectProperties::VertexTexCoord1);
        QTest::newRow("Skinning") << EffectProperties::Properties(EffectProperties::Skinning);
        QTest::newRow("MorphTargets") << EffectProperties::Properties(EffectProperties::MorphTargets);
    }

    void checkCreateUnlitEffectWithKey()
    {
        // GIVEN
        QFETCH(EffectProperties::Properties, props);

        // WHEN
        UnlitEffect *e = EffectsLibrary::createUnlitEffectWithKey(props);

        // THEN
        QCOMPARE(e->isBaseColorMapEnabled(), (props & EffectProperties::BaseColorMap) != 0);
        QCOMPARE(e->isOpaque(), !(props & EffectProperties::Blend));
        QCOMPARE(e->isAlphaCutoffEnabled(), (props & EffectProperties::Mask) != 0);
        QCOMPARE(e->isDoubleSided(), (props & EffectProperties::DoubleSided) != 0);
        QCOMPARE(e->isUsingColorAttribute(), (props & EffectProperties::VertexColor) != 0);
        QCOMPARE(e->isUsingNormalAttribute(), (props & EffectProperties::VertexNormal) != 0);
        QCOMPARE(e->isUsingTangentAttribute(), (props & EffectProperties::VertexTangent) != 0);
        QCOMPARE(e->isUsingTexCoordAttribute(), (props & EffectProperties::VertexTexCoord) != 0);
        QCOMPARE(e->isUsingTexCoord1Attribute(), (props & EffectProperties::VertexTexCoord1) != 0);
        QCOMPARE(e->isUsingSkinning(), (props & EffectProperties::Skinning) != 0);
        QCOMPARE(e->isUsingMorphTargets(), (props & EffectProperties::MorphTargets) != 0);

        delete e;
    }

    void checkCreateEffectWithKey()
    {
        // GIVEN
        EffectProperties::Properties props;
        props.setFlag(EffectProperties::MetallicRoughness);

        // WHEN
        GLTF2MaterialEffect *e = EffectsLibrary::createEffectWithKey(props);

        // THEN
        QVERIFY(qobject_cast<MetallicRoughnessEffect *>(e) != nullptr);
        QVERIFY(qobject_cast<UnlitEffect *>(e) == nullptr);
        delete e;

        // WHEN
        props.setFlag(EffectProperties::MetallicRoughness, false);
        props.setFlag(EffectProperties::Unlit);
        e = EffectsLibrary::createEffectWithKey(props);

        // THEN
        QVERIFY(qobject_cast<MetallicRoughnessEffect *>(e) == nullptr);
        QVERIFY(qobject_cast<UnlitEffect *>(e) != nullptr);
        delete e;
    }

    void checkCreateCustomEffectWithKey_data()
    {
        QTest::addColumn<EffectProperties::Properties>("props");

        QTest::newRow("BaseColorMap") << EffectProperties::Properties(EffectProperties::BaseColorMap);
        QTest::newRow("Blend") << EffectProperties::Properties(EffectProperties::Blend);
        QTest::newRow("Mask") << EffectProperties::Properties(EffectProperties::Mask);
        QTest::newRow("DoubleSided") << EffectProperties::Properties(EffectProperties::DoubleSided);
        QTest::newRow("VertexColor") << EffectProperties::Properties(EffectProperties::VertexColor);
        QTest::newRow("VertexNormal") << EffectProperties::Properties(EffectProperties::VertexNormal);
        QTest::newRow("VertexTangent") << EffectProperties::Properties(EffectProperties::VertexTangent);
        QTest::newRow("VertexTexCoord") << EffectProperties::Properties(EffectProperties::VertexTexCoord);
        QTest::newRow("VertexTexCoord1") << EffectProperties::Properties(EffectProperties::VertexTexCoord1);
        QTest::newRow("Skinning") << EffectProperties::Properties(EffectProperties::Skinning);
        QTest::newRow("MorphTargets") << EffectProperties::Properties(EffectProperties::MorphTargets);
    }

    void checkCreateCustomEffectWithKey()
    {
        // GIVEN
        EffectsLibrary library;
        QFETCH(EffectProperties::Properties, props);

        // WHEN

        GLTF2MaterialEffect *e = library.getOrCreateCustomEffect({ &IroDiffuseEffect::staticMetaObject,
                                                                   EffectProperties::Custom | props },
                                                                 nullptr);

        // THEN
        QVERIFY(e);
        QVERIFY(qobject_cast<IroDiffuseEffect *>(e) != nullptr);
        QCOMPARE(e->isOpaque(), !(props & EffectProperties::Blend));
        QCOMPARE(e->isAlphaCutoffEnabled(), (props & EffectProperties::Mask) != 0);
        QCOMPARE(e->isDoubleSided(), (props & EffectProperties::DoubleSided) != 0);
        QCOMPARE(e->isUsingColorAttribute(), (props & EffectProperties::VertexColor) != 0);
        QCOMPARE(e->isUsingNormalAttribute(), (props & EffectProperties::VertexNormal) != 0);
        QCOMPARE(e->isUsingTangentAttribute(), (props & EffectProperties::VertexTangent) != 0);
        QCOMPARE(e->isUsingTexCoordAttribute(), (props & EffectProperties::VertexTexCoord) != 0);
        QCOMPARE(e->isUsingTexCoord1Attribute(), (props & EffectProperties::VertexTexCoord1) != 0);
        QCOMPARE(e->isUsingSkinning(), (props & EffectProperties::Skinning) != 0);
        QCOMPARE(e->isUsingMorphTargets(), (props & EffectProperties::MorphTargets) != 0);

        delete e;
    }
};

QTEST_MAIN(tst_EffectsLibrary)

#include "tst_effectslibrary.moc"
