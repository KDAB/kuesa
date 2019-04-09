/*
    tst_metallicroughnessmaterial.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Qt3DRender/QParameter>
#include <Kuesa/metallicroughnessmaterial.h>

using namespace Kuesa;

namespace {
template<typename PropSetter,
         typename PropGetter,
         typename PropSignal,
         typename PropType>
void testProperty(Kuesa::MetallicRoughnessMaterial *mat,
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

bool testActiveParametersAreValid(const QVector<Qt3DRender::QParameter *> &activeParameters)
{
    std::vector<QString> validParameters = { QStringLiteral("baseColorUsesTexCoord1"),
                                             QStringLiteral("metallicRoughnessUsesTexCoord1"),
                                             QStringLiteral("normalUsesTexCoord1"),
                                             QStringLiteral("aoUsesTexCoord1"),
                                             QStringLiteral("emissiveUsesTexCoord1"),
                                             QStringLiteral("baseColorFactor"),
                                             QStringLiteral("baseColorMap"),
                                             QStringLiteral("metallicFactor"),
                                             QStringLiteral("roughnessFactor"),
                                             QStringLiteral("metalRoughMap"),
                                             QStringLiteral("normalScale"),
                                             QStringLiteral("normalMap"),
                                             QStringLiteral("ambientOcclusionMap"),
                                             QStringLiteral("emissiveFactor"),
                                             QStringLiteral("emissiveMap"),
                                             QStringLiteral("alphaCutoff"),
                                             QStringLiteral("texCoordTransform") };

    for (const auto &parameter : activeParameters) {
        auto it = std::find_if(std::begin(validParameters),
                               std::end(validParameters),
                               [parameter](const auto &parameterName) {
                                   return parameter->name() == parameterName;
                               });
        if (it == std::end(validParameters))
            return false;
    }
    return true;
}
} // namespace

class tst_MetallicRoughnessMaterial : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Kuesa::MetallicRoughnessEffect::ToneMapping>("MetallicRoughnessEffect::ToneMapping");
    }

    void checkDefaultState()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        QCOMPARE(mat.isBaseColorUsingTexCoord1(), false);
        QCOMPARE(mat.baseColorFactor(), QColor("gray"));
        QCOMPARE(mat.baseColorMap(), nullptr);

        QCOMPARE(mat.isMetallicRoughnessUsingTexCoord1(), false);
        QCOMPARE(mat.metallicFactor(), 0.0f);
        QCOMPARE(mat.roughnessFactor(), 0.0f);
        QCOMPARE(mat.metalRoughMap(), nullptr);

        QCOMPARE(mat.isNormalUsingTexCoord1(), false);
        QCOMPARE(mat.normalScale(), 0.25f);
        QCOMPARE(mat.normalMap(), nullptr);

        QCOMPARE(mat.isAOUsingTexCoord1(), false);
        QCOMPARE(mat.ambientOcclusionMap(), nullptr);

        QCOMPARE(mat.isEmissiveUsingTexCoord1(), false);
        QCOMPARE(mat.emissiveFactor(), QColor("black"));
        QCOMPARE(mat.emissiveMap(), nullptr);

        QCOMPARE(mat.textureTransform(), QMatrix3x3{});
        QCOMPARE(mat.isUsingColorAttribute(), false);
        QCOMPARE(mat.isDoubleSided(), false);
        QCOMPARE(mat.useSkinning(), false);
        QCOMPARE(mat.isOpaque(), true);
        QCOMPARE(mat.isAlphaCutoffEnabled(), false);
        QCOMPARE(mat.alphaCutoff(), 0.0f);
        QCOMPARE(mat.toneMappingAlgorithm(), MetallicRoughnessEffect::Reinhard);
    }

    void checkParametersAreAdded()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        QVERIFY(::testActiveParametersAreValid(mat.parameters()));

        mat.setBaseColorMap(new Qt3DRender::QTexture2D{});
        QVERIFY(::testActiveParametersAreValid(mat.parameters()));

        mat.setMetalRoughMap(new Qt3DRender::QTexture2D{});
        QVERIFY(::testActiveParametersAreValid(mat.parameters()));

        mat.setNormalMap(new Qt3DRender::QTexture2D{});
        QVERIFY(::testActiveParametersAreValid(mat.parameters()));

        mat.setAmbientOcclusionMap(new Qt3DRender::QTexture2D{});
        QVERIFY(::testActiveParametersAreValid(mat.parameters()));

        mat.setEmissiveMap(new Qt3DRender::QTexture2D{});
        QVERIFY(::testActiveParametersAreValid(mat.parameters()));
    }

    void checkBaseColorUsesTexCoord1()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setBaseColorUsesTexCoord1,
                       &MetallicRoughnessMaterial::isBaseColorUsingTexCoord1,
                       &MetallicRoughnessMaterial::baseColorUsesTexCoord1Changed,
                       false, true);
    }

    void checkMetallicRoughnessUsesTexCoord1()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setMetallicRoughnessUsesTexCoord1,
                       &MetallicRoughnessMaterial::isMetallicRoughnessUsingTexCoord1,
                       &MetallicRoughnessMaterial::metallicRoughnessUsesTexCoord1Changed,
                       false, true);
    }

    void checkNormalUsesTexCoord1()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setNormalUsesTexCoord1,
                       &MetallicRoughnessMaterial::isNormalUsingTexCoord1,
                       &MetallicRoughnessMaterial::normalUsesTexCoord1Changed,
                       false, true);
    }

    void checkAOUsesTexCoord1()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setAOUsesTexCoord1,
                       &MetallicRoughnessMaterial::isAOUsingTexCoord1,
                       &MetallicRoughnessMaterial::aoUsesTexCoord1Changed,
                       false, true);
    }

    void checkEmissiveUsesTexCoord1()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setEmissiveUsesTexCoord1,
                       &MetallicRoughnessMaterial::isEmissiveUsingTexCoord1,
                       &MetallicRoughnessMaterial::emissiveUsesTexCoord1Changed,
                       false, true);
    }

    void checkBaseColorFactor()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setBaseColorFactor,
                       &MetallicRoughnessMaterial::baseColorFactor,
                       &MetallicRoughnessMaterial::baseColorFactorChanged,
                       QColor("gray"), QColor("red"));
    }

    void checkBaseColorMap()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setBaseColorMap,
                       &MetallicRoughnessMaterial::baseColorMap,
                       &MetallicRoughnessMaterial::baseColorMapChanged,
                       static_cast<Qt3DRender::QTexture2D *>(nullptr),
                       new Qt3DRender::QTexture2D());
    }

    void checkMetallicFactor()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setMetallicFactor,
                       &MetallicRoughnessMaterial::metallicFactor,
                       &MetallicRoughnessMaterial::metallicFactorChanged,
                       0.0f, 1.0f);
    }

    void checkRoughnessFactor()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setRoughnessFactor,
                       &MetallicRoughnessMaterial::roughnessFactor,
                       &MetallicRoughnessMaterial::roughnessFactorChanged,
                       0.0f, 1.0f);
    }

    void checkMetalRoughMap()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setMetalRoughMap,
                       &MetallicRoughnessMaterial::metalRoughMap,
                       &MetallicRoughnessMaterial::metalRoughMapChanged,
                       static_cast<Qt3DRender::QTexture2D *>(nullptr),
                       new Qt3DRender::QTexture2D());
    }

    void checkNormalScale()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setNormalScale,
                       &MetallicRoughnessMaterial::normalScale,
                       &MetallicRoughnessMaterial::normalScaleChanged,
                       0.25f, 0.5f);
    }

    void checkNormalMap()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setNormalMap,
                       &MetallicRoughnessMaterial::normalMap,
                       &MetallicRoughnessMaterial::normalMapChanged,
                       static_cast<Qt3DRender::QTexture2D *>(nullptr),
                       new Qt3DRender::QTexture2D());
    }

    void checkAmbientOcclusionMap()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setAmbientOcclusionMap,
                       &MetallicRoughnessMaterial::ambientOcclusionMap,
                       &MetallicRoughnessMaterial::ambientOcclusionMapChanged,
                       static_cast<Qt3DRender::QTexture2D *>(nullptr),
                       new Qt3DRender::QTexture2D());
    }

    void checkEmissiveFactor()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setEmissiveFactor,
                       &MetallicRoughnessMaterial::emissiveFactor,
                       &MetallicRoughnessMaterial::emissiveFactorChanged,
                       QColor("black"), QColor("red"));
    }

    void checkEmissiveMap()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setEmissiveMap,
                       &MetallicRoughnessMaterial::emissiveMap,
                       &MetallicRoughnessMaterial::emissiveMapChanged,
                       static_cast<Qt3DRender::QTexture2D *>(nullptr),
                       new Qt3DRender::QTexture2D());
    }

    void checkTextureTransform()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        QMatrix3x3 matrix;
        matrix.fill(5.0);

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setTextureTransform,
                       &MetallicRoughnessMaterial::textureTransform,
                       &MetallicRoughnessMaterial::textureTransformChanged,
                       QMatrix3x3{}, matrix);
    }

    void checkUsingColorAttribute()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setUsingColorAttribute,
                       &MetallicRoughnessMaterial::isUsingColorAttribute,
                       &MetallicRoughnessMaterial::usingColorAttributeChanged,
                       false, true);
    }

    void checkDoubleSided()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setDoubleSided,
                       &MetallicRoughnessMaterial::isDoubleSided,
                       &MetallicRoughnessMaterial::doubleSidedChanged,
                       false, true);
    }

    void checkUseSkinning()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setUseSkinning,
                       &MetallicRoughnessMaterial::useSkinning,
                       &MetallicRoughnessMaterial::useSkinningChanged,
                       false, true);
    }

    void checkOpaque()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setOpaque,
                       &MetallicRoughnessMaterial::isOpaque,
                       &MetallicRoughnessMaterial::opaqueChanged,
                       true, false);
    }

    void checkAlphaCutoff()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setAlphaCutoff,
                       &MetallicRoughnessMaterial::alphaCutoff,
                       &MetallicRoughnessMaterial::alphaCutoffChanged,
                       0.0f, 0.5f);
    }

    void checkAlphaCutoffEnabled()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setAlphaCutoffEnabled,
                       &MetallicRoughnessMaterial::isAlphaCutoffEnabled,
                       &MetallicRoughnessMaterial::alphaCutoffEnabledChanged,
                       false, true);
    }

    void checkToneMappingAlgorithm()
    {
        // GIVEN
        Kuesa::MetallicRoughnessMaterial mat;

        // THEN
        ::testProperty(&mat,
                       &MetallicRoughnessMaterial::setToneMappingAlgorithm,
                       &MetallicRoughnessMaterial::toneMappingAlgorithm,
                       &MetallicRoughnessMaterial::toneMappingAlgorithmChanged,
                       MetallicRoughnessEffect::Reinhard, MetallicRoughnessEffect::Filmic);
    }
};

QTEST_APPLESS_MAIN(tst_MetallicRoughnessMaterial)

#include "tst_metallicroughnessmaterial.moc"
