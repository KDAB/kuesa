/*
    tst_materialparser.cpp

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
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <Kuesa/private/materialparser_p.h>
#include <Kuesa/private/gltf2context_p.h>
#include <Kuesa/metallicroughnessmaterial.h>
#include <Kuesa/metallicroughnesseffect.h>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_MaterialParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<int>("materialCount");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "materialparser_valid.gltf")
                               << true
                               << 1;

        QTest::newRow("Empty") << QStringLiteral(ASSETS "materialparser_empty.gltf")
                               << false
                               << 0;

        QTest::newRow("Empty Material") << QStringLiteral(ASSETS "materialparser_empty_material.gltf")
                                        << true
                                        << 1;

        QTest::newRow("PartiallyComplete") << QStringLiteral(ASSETS "materialparser_partially_complete.gltf")
                                           << true
                                           << 1;

        QTest::newRow("Invalid_AlphaMode") << QStringLiteral(ASSETS "materialparser_invalid_alpha_mode.gltf")
                                           << false
                                           << 0;

        QTest::newRow("Invalid_BaseColorArray") << QStringLiteral(ASSETS "materialparser_invalid_basecolorarray.gltf")
                                                << false
                                                << 0;

        QTest::newRow("Invalid_BaseColorArray2") << QStringLiteral(ASSETS "materialparser_invalid_basecolorarray2.gltf")
                                                 << false
                                                 << 0;

        QTest::newRow("Invalid_EmissiveFactor") << QStringLiteral(ASSETS "materialparser_invalid_emissivefactor.gltf")
                                                << false
                                                << 0;

        QTest::newRow("Invalid_EmissiveFactor2") << QStringLiteral(ASSETS "materialparser_invalid_emissivefactor2.gltf")
                                                 << false
                                                 << 0;

        QTest::newRow("KHR_materials_unlit") << QStringLiteral(ASSETS "materialparser_KHR_materials_unlit.gltf")
                                             << true
                                             << 1;
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(int, materialCount);

        // GIVEN
        GLTF2Context context;
        MaterialParser parser;
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        // THEN
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QCOMPARE(success, succeeded);
        QCOMPARE(context.materialsCount(), materialCount);
    }

    void checkMaterialAlpha_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<int>("expectedBlendMode");
        QTest::addColumn<float>("expectedAlphaCutoff");

        QTest::newRow("Default") << QStringLiteral(ASSETS "materialparser_defaults.gltf")
                                 << int(Kuesa::GLTF2Import::Material::Alpha::Opaque)
                                 << 0.5f;
        QTest::newRow("Opaque") << QStringLiteral(ASSETS "materialparser_opaque.gltf")
                                << int(Kuesa::GLTF2Import::Material::Alpha::Opaque)
                                << 0.1f;
        QTest::newRow("Blend") << QStringLiteral(ASSETS "materialparser_blend.gltf")
                               << int(Kuesa::GLTF2Import::Material::Alpha::Blend)
                               << 0.1f;
        QTest::newRow("Mask") << QStringLiteral(ASSETS "materialparser_mask.gltf")
                              << int(Kuesa::GLTF2Import::Material::Alpha::Mask)
                              << 0.1f;
    }

    void checkMaterialAlpha()
    {
        QFETCH(QString, filePath);
        QFETCH(int, expectedBlendMode);
        QFETCH(float, expectedAlphaCutoff);

        // GIVEN
        GLTF2Context context;
        MaterialParser parser;
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        // THEN
        QVERIFY(!json.isNull() && json.isArray());

        // WHEN
        bool success = parser.parse(json.array(), &context);

        // THEN
        QVERIFY(success);
        QCOMPARE(context.materialsCount(), 1);
        QCOMPARE(context.material(0).alpha.alphaCutoff, expectedAlphaCutoff);
        QCOMPARE(int(context.material(0).alpha.mode), expectedBlendMode);
    }

    void checkUnlit()
    {
        // GIVEN
        GLTF2Context context;
        MaterialParser parser;
        QFile file(QStringLiteral(ASSETS "materialparser_KHR_materials_unlit.gltf"));
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        parser.parse(json.array(), &context);

        // THEN
        QVERIFY(context.material(0).extensions.KHR_materials_unlit == true);
    }
};

QTEST_APPLESS_MAIN(tst_MaterialParser)

#include "tst_materialparser.moc"
