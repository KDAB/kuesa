/*
    tst_metallicroughnesseffect.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QtTest>

#include <Qt3DRender/QTexture>
#include <Kuesa/metallicroughnesseffect.h>

using namespace Kuesa;

class tst_MetallicRoughnessEffect : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkEffectDefaultState()
    {
        // GIVEN
        Kuesa::MetallicRoughnessEffect effect;

        QCOMPARE(effect.isBaseColorMapEnabled(), false);
        QCOMPARE(effect.isMetalRoughMapEnabled(), false);
        QCOMPARE(effect.isNormalMapEnabled(), false);
        QCOMPARE(effect.isAmbientOcclusionMapEnabled(), false);
        QCOMPARE(effect.isEmissiveMapEnabled(), false);
        QCOMPARE(effect.isUsingColorAttribute(), false);
        QCOMPARE(effect.isDoubleSided(), false);
        QCOMPARE(effect.useSkinning(), false);
        QCOMPARE(effect.isOpaque(), true);
        QCOMPARE(effect.isAlphaCutoffEnabled(), false);
        QCOMPARE(effect.brdfLUT(), nullptr);
    }

    void checkEffectProperties()
    {
        // GIVEN
        Kuesa::MetallicRoughnessEffect effect;

        QSignalSpy baseColorMapSpy(&effect, &Kuesa::MetallicRoughnessEffect::baseColorMapEnabledChanged);
        QSignalSpy metalRoughMapSpy(&effect, &Kuesa::MetallicRoughnessEffect::metalRoughMapEnabledChanged);
        QSignalSpy normalMapSpy(&effect, &Kuesa::MetallicRoughnessEffect::normalMapEnabledChanged);
        QSignalSpy ambientOcclusionMapSpy(&effect, &Kuesa::MetallicRoughnessEffect::ambientOcclusionMapEnabledChanged);
        QSignalSpy emissiveMapSpy(&effect, &Kuesa::MetallicRoughnessEffect::emissiveMapEnabledChanged);
        QSignalSpy usingColorAttributeSpy(&effect, &Kuesa::MetallicRoughnessEffect::usingColorAttributeChanged);
        QSignalSpy doubleSidedSpy(&effect, &Kuesa::MetallicRoughnessEffect::doubleSidedChanged);
        QSignalSpy useSkinningSpy(&effect, &Kuesa::MetallicRoughnessEffect::useSkinningChanged);
        QSignalSpy opaqueSpy(&effect, &Kuesa::MetallicRoughnessEffect::opaqueChanged);
        QSignalSpy alphaCutoffSpy(&effect, &Kuesa::MetallicRoughnessEffect::alphaCutoffEnabledChanged);
        QSignalSpy brdfLUTSpy(&effect, &Kuesa::MetallicRoughnessEffect::brdfLUTChanged);

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
            effect.setMetalRoughMapEnabled(false);
            effect.setMetalRoughMapEnabled(true);
            effect.setMetalRoughMapEnabled(true);

            // THEN
            QCOMPARE(effect.isMetalRoughMapEnabled(), true);
            QCOMPARE(metalRoughMapSpy.count(), 1);
        }

        {
            // WHEN
            effect.setNormalMapEnabled(false);
            effect.setNormalMapEnabled(true);
            effect.setNormalMapEnabled(true);

            // THEN
            QCOMPARE(effect.isNormalMapEnabled(), true);
            QCOMPARE(normalMapSpy.count(), 1);
        }

        {
            // WHEN
            effect.setAmbientOcclusionMapEnabled(false);
            effect.setAmbientOcclusionMapEnabled(true);
            effect.setAmbientOcclusionMapEnabled(true);

            // THEN
            QCOMPARE(effect.isAmbientOcclusionMapEnabled(), true);
            QCOMPARE(ambientOcclusionMapSpy.count(), 1);
        }

        {
            // WHEN
            effect.setEmissiveMapEnabled(false);
            effect.setEmissiveMapEnabled(true);
            effect.setEmissiveMapEnabled(true);

            // THEN
            QCOMPARE(effect.isEmissiveMapEnabled(), true);
            QCOMPARE(emissiveMapSpy.count(), 1);
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
            effect.setDoubleSided(false);
            effect.setDoubleSided(true);
            effect.setDoubleSided(true);

            // THEN
            QCOMPARE(effect.isDoubleSided(), true);
            QCOMPARE(doubleSidedSpy.count(), 1);
        }

        {
            // WHEN
            effect.setUseSkinning(false);
            effect.setUseSkinning(true);
            effect.setUseSkinning(true);

            // THEN
            QCOMPARE(effect.useSkinning(), true);
            QCOMPARE(useSkinningSpy.count(), 1);
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
            effect.setBrdfLUT(nullptr);

            auto *texture2D = new Qt3DRender::QTexture2D();

            effect.setBrdfLUT(texture2D);
            effect.setBrdfLUT(texture2D);

            QCOMPARE(effect.brdfLUT(), texture2D);
            QCOMPARE(brdfLUTSpy.count(), 1);
        }
    }
};

QTEST_APPLESS_MAIN(tst_MetallicRoughnessEffect)

#include "tst_metallicroughnesseffect.moc"
