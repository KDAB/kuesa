/*
    tst_metallicroughnessmaterial.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <Qt3DRender/QTexture>
#include <Qt3DRender/QParameter>
#include <Kuesa/metallicroughnessproperties.h>
#include <Kuesa/morphcontroller.h>

using namespace Kuesa;

namespace {
template<typename PropSetter,
         typename PropGetter,
         typename PropSignal,
         typename PropType>
void testProperty(Kuesa::MetallicRoughnessProperties *mat,
                  PropSetter setter,
                  PropGetter getter,
                  PropSignal signal,
                  PropType oldValue,
                  PropType newValue)
{
    QSignalSpy spy(mat, signal);
    (mat->*setter)(oldValue);
    (mat->*setter)(newValue);
    (mat->*setter)(newValue);

    QCOMPARE((mat->*getter)(), newValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().value(0).value<PropType>(), newValue);
}

} // namespace

bool operator==(const TextureTransform &a, const TextureTransform &b)
{
    return a.scale() == b.scale() &&
            qFuzzyCompare(a.rotation(), b.rotation()) &&
            a.offset() == b.offset() &&
            a.matrix() == b.matrix();
}

class tst_MetallicRoughnessProperties : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QAbstractTexture *>();
    }

    void checkDefaultState()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        QCOMPARE(mat.isBaseColorUsingTexCoord1(), false);
        QCOMPARE(mat.baseColorFactor(), QColor("gray"));
        QCOMPARE(mat.baseColorMap(), nullptr);
        QCOMPARE(*mat.baseColorMapTextureTransform(), TextureTransform{});

        QCOMPARE(mat.isMetallicRoughnessUsingTexCoord1(), false);
        QCOMPARE(mat.metallicFactor(), 0.0f);
        QCOMPARE(mat.roughnessFactor(), 0.0f);
        QCOMPARE(mat.metalRoughMap(), nullptr);
        QCOMPARE(*mat.metalRoughMapTextureTransform(), TextureTransform{});

        QCOMPARE(mat.isNormalUsingTexCoord1(), false);
        QCOMPARE(mat.normalScale(), 1.0f);
        QCOMPARE(mat.normalMap(), nullptr);
        QCOMPARE(*mat.normalMapTextureTransform(), TextureTransform{});

        QCOMPARE(mat.isAOUsingTexCoord1(), false);
        QCOMPARE(mat.ambientOcclusionMap(), nullptr);
        QCOMPARE(*mat.ambientOcclusionMapTextureTransform(), TextureTransform{});

        QCOMPARE(mat.isEmissiveUsingTexCoord1(), false);
        QCOMPARE(mat.emissiveFactor(), QColor("black"));
        QCOMPARE(mat.emissiveMap(), nullptr);
        QCOMPARE(*mat.emissiveMapTextureTransform(), TextureTransform{});

        QCOMPARE(mat.alphaCutoff(), 0.0f);
    }

    void checkBaseColorUsesTexCoord1()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setBaseColorUsesTexCoord1,
                       &MetallicRoughnessProperties::isBaseColorUsingTexCoord1,
                       &MetallicRoughnessProperties::baseColorUsesTexCoord1Changed,
                       false, true);
    }

    void checkMetallicRoughnessUsesTexCoord1()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setMetallicRoughnessUsesTexCoord1,
                       &MetallicRoughnessProperties::isMetallicRoughnessUsingTexCoord1,
                       &MetallicRoughnessProperties::metallicRoughnessUsesTexCoord1Changed,
                       false, true);
    }

    void checkNormalUsesTexCoord1()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setNormalUsesTexCoord1,
                       &MetallicRoughnessProperties::isNormalUsingTexCoord1,
                       &MetallicRoughnessProperties::normalUsesTexCoord1Changed,
                       false, true);
    }

    void checkAOUsesTexCoord1()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setAOUsesTexCoord1,
                       &MetallicRoughnessProperties::isAOUsingTexCoord1,
                       &MetallicRoughnessProperties::aoUsesTexCoord1Changed,
                       false, true);
    }

    void checkEmissiveUsesTexCoord1()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setEmissiveUsesTexCoord1,
                       &MetallicRoughnessProperties::isEmissiveUsingTexCoord1,
                       &MetallicRoughnessProperties::emissiveUsesTexCoord1Changed,
                       false, true);
    }

    void checkBaseColorFactor()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setBaseColorFactor,
                       &MetallicRoughnessProperties::baseColorFactor,
                       &MetallicRoughnessProperties::baseColorFactorChanged,
                       QColor("gray"), QColor("red"));
    }

    void checkBaseColorMap()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setBaseColorMap,
                       &MetallicRoughnessProperties::baseColorMap,
                       &MetallicRoughnessProperties::baseColorMapChanged,
                       static_cast<Qt3DRender::QTexture2D *>(nullptr),
                       new Qt3DRender::QTexture2D());
    }

    void checkMetallicFactor()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setMetallicFactor,
                       &MetallicRoughnessProperties::metallicFactor,
                       &MetallicRoughnessProperties::metallicFactorChanged,
                       0.0f, 1.0f);
    }

    void checkRoughnessFactor()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setRoughnessFactor,
                       &MetallicRoughnessProperties::roughnessFactor,
                       &MetallicRoughnessProperties::roughnessFactorChanged,
                       0.0f, 1.0f);
    }

    void checkMetalRoughMap()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setMetalRoughMap,
                       &MetallicRoughnessProperties::metalRoughMap,
                       &MetallicRoughnessProperties::metalRoughMapChanged,
                       static_cast<Qt3DRender::QTexture2D *>(nullptr),
                       new Qt3DRender::QTexture2D());
    }

    void checkNormalScale()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setNormalScale,
                       &MetallicRoughnessProperties::normalScale,
                       &MetallicRoughnessProperties::normalScaleChanged,
                       1.0f, 0.5f);
    }

    void checkNormalMap()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setNormalMap,
                       &MetallicRoughnessProperties::normalMap,
                       &MetallicRoughnessProperties::normalMapChanged,
                       static_cast<Qt3DRender::QTexture2D *>(nullptr),
                       new Qt3DRender::QTexture2D());
    }

    void checkAmbientOcclusionMap()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setAmbientOcclusionMap,
                       &MetallicRoughnessProperties::ambientOcclusionMap,
                       &MetallicRoughnessProperties::ambientOcclusionMapChanged,
                       static_cast<Qt3DRender::QTexture2D *>(nullptr),
                       new Qt3DRender::QTexture2D());
    }

    void checkEmissiveFactor()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setEmissiveFactor,
                       &MetallicRoughnessProperties::emissiveFactor,
                       &MetallicRoughnessProperties::emissiveFactorChanged,
                       QColor("black"), QColor("red"));
    }

    void checkEmissiveMap()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setEmissiveMap,
                       &MetallicRoughnessProperties::emissiveMap,
                       &MetallicRoughnessProperties::emissiveMapChanged,
                       static_cast<Qt3DRender::QTexture2D *>(nullptr),
                       new Qt3DRender::QTexture2D());
    }

    void checkAlphaCutoff()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setAlphaCutoff,
                       &MetallicRoughnessProperties::alphaCutoff,
                       &MetallicRoughnessProperties::alphaCutoffChanged,
                       0.0f, 0.5f);
    }

    void checkReceivesShadows()
    {
        // GIVEN
        Kuesa::MetallicRoughnessProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessProperties::setReceivesShadows,
                       &MetallicRoughnessProperties::receivesShadows,
                       &MetallicRoughnessProperties::receivesShadowsChanged,
                       true, false);
    }

    void checkTextureBookeeping()
    {
        // Given
        Kuesa::MetallicRoughnessProperties mat;

        // WHEN
        {
            Qt3DRender::QTexture2D t;
            mat.setAmbientOcclusionMap(&t);

            // THEN
            QCOMPARE(mat.ambientOcclusionMap(), &t);
        }
        // THEN
        QVERIFY(mat.ambientOcclusionMap() == nullptr);

        // WHEN
        {
            Qt3DRender::QTexture2D t;
            mat.setBaseColorMap(&t);

            // THEN
            QCOMPARE(mat.baseColorMap(), &t);
        }
        // THEN
        QVERIFY(mat.baseColorMap() == nullptr);

        // WHEN
        {
            Qt3DRender::QTexture2D t;
            mat.setEmissiveMap(&t);

            // THEN
            QCOMPARE(mat.emissiveMap(), &t);
        }
        // THEN
        QVERIFY(mat.emissiveMap() == nullptr);

        // WHEN
        {
            Qt3DRender::QTexture2D t;
            mat.setMetalRoughMap(&t);

            // THEN
            QCOMPARE(mat.metalRoughMap(), &t);
        }
        // THEN
        QVERIFY(mat.metalRoughMap() == nullptr);

        // WHEN
        {
            Qt3DRender::QTexture2D t;
            mat.setNormalMap(&t);

            // THEN
            QCOMPARE(mat.normalMap(), &t);
        }
        // THEN
        QVERIFY(mat.normalMap() == nullptr);
    }
};

QTEST_MAIN(tst_MetallicRoughnessProperties)

#include "tst_metallicroughnessproperties.moc"
