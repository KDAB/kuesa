/*
    tst_unliteffect.cpp

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

#include <Qt3DRender/QTexture>
#include <Kuesa/unliteffect.h>
#include <Qt3DRender/private/shaderbuilder_p.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Kuesa/private/effectslibrary_p.h>
#include "fakerenderer.h"

using namespace Kuesa;

class tst_UnlitEffect : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkEffectDefaultState()
    {
        // GIVEN
        Kuesa::UnlitEffect effect;

        QCOMPARE(effect.isBaseColorMapEnabled(), false);
        QCOMPARE(effect.isUsingColorAttribute(), false);
        QCOMPARE(effect.isUsingNormalAttribute(), false);
        QCOMPARE(effect.isUsingTangentAttribute(), false);
        QCOMPARE(effect.isUsingTexCoordAttribute(), false);
        QCOMPARE(effect.isUsingTexCoord1Attribute(), false);
        QCOMPARE(effect.isDoubleSided(), false);
        QCOMPARE(effect.isUsingSkinning(), false);
        QCOMPARE(effect.isOpaque(), true);
        QCOMPARE(effect.isAlphaCutoffEnabled(), false);
        QCOMPARE(effect.isInstanced(), false);
    }

    void checkEffectProperties()
    {
        // GIVEN
        Kuesa::UnlitEffect effect;

        QSignalSpy baseColorMapSpy(&effect, &Kuesa::UnlitEffect::baseColorMapEnabledChanged);
        QSignalSpy usingColorAttributeSpy(&effect, &Kuesa::UnlitEffect::usingColorAttributeChanged);
        QSignalSpy usingNormalAttributeSpy(&effect, &Kuesa::UnlitEffect::usingNormalAttributeChanged);
        QSignalSpy usingTangentAttributeSpy(&effect, &Kuesa::UnlitEffect::usingTangentAttributeChanged);
        QSignalSpy usingTexCoordAttributeSpy(&effect, &Kuesa::UnlitEffect::usingTexCoordAttributeChanged);
        QSignalSpy usingTexCoord1AttributeSpy(&effect, &Kuesa::UnlitEffect::usingTexCoord1AttributeChanged);
        QSignalSpy doubleSidedSpy(&effect, &Kuesa::UnlitEffect::doubleSidedChanged);
        QSignalSpy useSkinningSpy(&effect, &Kuesa::UnlitEffect::usingSkinningChanged);
        QSignalSpy useMorphTargetsSpy(&effect, &Kuesa::UnlitEffect::usingMorphTargetsChanged);
        QSignalSpy opaqueSpy(&effect, &Kuesa::UnlitEffect::opaqueChanged);
        QSignalSpy alphaCutoffSpy(&effect, &Kuesa::UnlitEffect::alphaCutoffEnabledChanged);
        QSignalSpy instancedSpy(&effect, &Kuesa::UnlitEffect::instancedChanged);

        // THEN
        QVERIFY(usingColorAttributeSpy.isValid());
        QVERIFY(usingNormalAttributeSpy.isValid());
        QVERIFY(usingTangentAttributeSpy.isValid());
        QVERIFY(usingTexCoordAttributeSpy.isValid());
        QVERIFY(usingTexCoord1AttributeSpy.isValid());
        QVERIFY(doubleSidedSpy.isValid());
        QVERIFY(useSkinningSpy.isValid());
        QVERIFY(useMorphTargetsSpy.isValid());
        QVERIFY(opaqueSpy.isValid());
        QVERIFY(alphaCutoffSpy.isValid());
        QVERIFY(instancedSpy.isValid());

        {
            // WHEN
            effect.setBaseColorMapEnabled(false);
            effect.setBaseColorMapEnabled(true);
            effect.setBaseColorMapEnabled(true);

            // THEN
            QCOMPARE(effect.isBaseColorMapEnabled(), true);
            QCOMPARE(baseColorMapSpy.count(), 1);
        }

        {
            // WHEN
            effect.setUsingColorAttribute(false);
            effect.setUsingColorAttribute(true);
            effect.setUsingColorAttribute(true);

            // THEN
            QCOMPARE(effect.isUsingColorAttribute(), true);
            QCOMPARE(usingColorAttributeSpy.count(), 1);
        }

        {
            // WHEN
            effect.setUsingNormalAttribute(false);
            effect.setUsingNormalAttribute(true);
            effect.setUsingNormalAttribute(true);

            // THEN
            QCOMPARE(effect.isUsingNormalAttribute(), true);
            QCOMPARE(usingNormalAttributeSpy.count(), 1);
        }

        {
            // WHEN
            effect.setUsingTangentAttribute(false);
            effect.setUsingTangentAttribute(true);
            effect.setUsingTangentAttribute(true);

            // THEN
            QCOMPARE(effect.isUsingTangentAttribute(), true);
            QCOMPARE(usingTangentAttributeSpy.count(), 1);
        }

        {
            // WHEN
            effect.setUsingTexCoordAttribute(false);
            effect.setUsingTexCoordAttribute(true);
            effect.setUsingTexCoordAttribute(true);

            // THEN
            QCOMPARE(effect.isUsingTexCoordAttribute(), true);
            QCOMPARE(usingTexCoordAttributeSpy.count(), 1);
        }

        {
            // WHEN
            effect.setUsingTexCoord1Attribute(false);
            effect.setUsingTexCoord1Attribute(true);
            effect.setUsingTexCoord1Attribute(true);

            // THEN
            QCOMPARE(effect.isUsingTexCoord1Attribute(), true);
            QCOMPARE(usingTexCoord1AttributeSpy.count(), 1);
        }

        {
            // WHEN
            effect.setDoubleSided(false);
            effect.setDoubleSided(true);
            effect.setDoubleSided(true);

            // THEN
            QCOMPARE(effect.isDoubleSided(), true);
            QCOMPARE(doubleSidedSpy.count(), 1);
        }

        {
            // WHEN
            effect.setUsingSkinning(false);
            effect.setUsingSkinning(true);
            effect.setUsingSkinning(true);

            // THEN
            QCOMPARE(effect.isUsingSkinning(), true);
            QCOMPARE(useSkinningSpy.count(), 1);
        }

        {
            // WHEN
            effect.setUsingMorphTargets(false);
            effect.setUsingMorphTargets(true);
            effect.setUsingMorphTargets(true);

            // THEN
            QCOMPARE(effect.isUsingMorphTargets(), true);
            QCOMPARE(useMorphTargetsSpy.count(), 1);
        }

        {
            // WHEN
            effect.setOpaque(true);
            effect.setOpaque(false);
            effect.setOpaque(false);

            // THEN
            QCOMPARE(effect.isOpaque(), false);
            QCOMPARE(opaqueSpy.count(), 1);
        }

        {
            // WHEN
            effect.setAlphaCutoffEnabled(false);
            effect.setAlphaCutoffEnabled(true);
            effect.setAlphaCutoffEnabled(true);

            // THEN
            QCOMPARE(effect.isAlphaCutoffEnabled(), true);
            QCOMPARE(alphaCutoffSpy.count(), 1);
        }

        {
            // WHEN
            effect.setInstanced(false);
            effect.setInstanced(true);
            effect.setInstanced(true);

            // THEN
            QCOMPARE(effect.isInstanced(), true);
            QCOMPARE(instancedSpy.count(), 1);
        }
    }

    void checkShaderGeneration_data()
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
        QTest::newRow("NoSkinningWithBaseMaps") << EffectProperties::Properties(EffectProperties::BaseColorMap |
                                                                                EffectProperties::VertexNormal |
                                                                                EffectProperties::VertexTexCoord);
    }

    void checkShaderGeneration()
    {
        // GIVEN
        QFETCH(EffectProperties::Properties, props);
        FakeRenderer renderer;
        qputenv("QT3D_REBUILD_SHADER_CACHE", "1");

        auto testEffectGenerator = [&] (Kuesa::UnlitEffect *e) {
            for (Qt3DRender::QTechnique *t : e->techniques()) {
                Kuesa::UnlitTechnique *technique = qobject_cast<decltype(technique)>(t);

                // THEN
                QVERIFY(t != nullptr);

                // WHEN
                Qt3DRender::QShaderProgramBuilder *frontendBuilder = technique->unlitShaderBuilder();
                Qt3DRender::Render::ShaderBuilder builder;
                builder.setEnabled(true);
                builder.setRenderer(&renderer);
                builder.syncFromFrontEnd(frontendBuilder, true);

                // THEN -> Shouldn't crash
                builder.setGraphicsApi(Qt3DRender::QGraphicsApiFilterPrivate::get(t->graphicsApiFilter())->m_data);
                builder.generateCode(Qt3DRender::QShaderProgram::Vertex);
                builder.generateCode(Qt3DRender::QShaderProgram::Fragment);
            }
        };

        // WHEN
        {
            UnlitEffect *e = EffectsLibrary::createUnlitEffectWithKey(props);
            testEffectGenerator(e);
        }

        // WHEN
        {
            UnlitEffect *e = EffectsLibrary::createUnlitEffectWithKey(props);
            e->setInstanced(true);
            testEffectGenerator(e);
        }
    }
};

QTEST_MAIN(tst_UnlitEffect)

#include "tst_unliteffect.moc"
