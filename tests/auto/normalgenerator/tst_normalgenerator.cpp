/*
    tst_normalgenerator.cpp

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QDir>
#include <QStandardPaths>
#include <Kuesa/private/gltf2exporter_p.h>
#include <Kuesa/private/gltf2parser_p.h>
#include <Kuesa/private/meshparser_utils_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

namespace {

QDir setupTestFolder()
{
    QDir tmp(QStandardPaths::standardLocations(QStandardPaths::TempLocation)[0]);
    if (tmp.exists("kuesa-normalgenerator-test")) {
        tmp.cd("kuesa-normalgenerator-test");
        tmp.removeRecursively();
        tmp.cdUp();
    }
    tmp.mkdir("kuesa-normalgenerator-test");
    tmp.cd("kuesa-normalgenerator-test");
    tmp.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    return tmp;
}

} // namespace

class tst_NormalGenerator : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testTangentGeneration()
    {
        // GIVEN
        GLTF2Context ctx;
        GLTF2Parser parser;
        parser.setContext(&ctx);
        const bool parsingSuccessful = parser.parse(QString(ASSETS "simple_cube_uv.gltf"));

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        QVERIFY(ctx.meshesCount() == 1);
        auto mesh = ctx.mesh(0);

        QVERIFY(mesh.meshPrimitives.size() == 1);
        auto meshRenderer = mesh.meshPrimitives[0].primitiveRenderer;

        QVERIFY(Kuesa::GLTF2Import::MeshParserUtils::needsTangentAttribute(meshRenderer->geometry(),
                                                                           meshRenderer->primitiveType()));

        // WHEN
        QVERIFY(Kuesa::GLTF2Import::MeshParserUtils::generatePrecomputedTangentAttribute(meshRenderer, &ctx));

        // THEN
        QVERIFY(!Kuesa::GLTF2Import::MeshParserUtils::needsTangentAttribute(meshRenderer->geometry(),
                                                                            meshRenderer->primitiveType()));
    }

    void testTangentGenerationFileOutput()
    {
        // GIVEN
        const QDir testDir = setupTestFolder();
        const QString gltfPath = testDir.absolutePath() + QDir::separator() + "cube_no_normals.gltf";

        // WHEN
        {
            // GIVEN
            GLTF2Context ctx;
            GLTF2Parser parser;
            parser.setContext(&ctx);
            const bool parsingSuccessful = parser.parse(QString(ASSETS "cube_no_normals.gltf"));

            // THEN
            QVERIFY(parsingSuccessful);

            // WHEN
            parser.generateContent();
            QVERIFY(ctx.meshesCount() == 1);
            auto mesh = ctx.mesh(0);

            QVERIFY(mesh.meshPrimitives.size() == 1);
            QGeometryRenderer *meshRenderer = mesh.meshPrimitives[0].primitiveRenderer;

            QVERIFY(Kuesa::GLTF2Import::MeshParserUtils::needsNormalAttribute(meshRenderer->geometry(),
                                                                              meshRenderer->primitiveType()));

            GLTF2Exporter::Export exported;

            // WHEN
            QVERIFY(Kuesa::GLTF2Import::MeshParserUtils::generatePrecomputedNormalAttribute(meshRenderer, &ctx));

            // THEN
            QVERIFY(!Kuesa::GLTF2Import::MeshParserUtils::needsNormalAttribute(meshRenderer->geometry(),
                                                                               meshRenderer->primitiveType()));

            GLTF2ExportConfiguration configuration;
            configuration.setMeshCompressionEnabled(false);
            configuration.setEmbedding(GLTF2ExportConfiguration::Embed::Keep);

            GLTF2Exporter exporter;
            exporter.setContext(&ctx);
            exporter.setConfiguration(configuration);
            exported = exporter.saveInFolder(QDir(ASSETS), testDir);

            QVERIFY(exporter.errors().empty());
            QVERIFY(exported.success());

            QFile file(gltfPath);
            QVERIFY(file.open(QIODevice::WriteOnly));
            QJsonDocument doc(exported.json());
            file.write(doc.toJson());
        }

        // THEN
        GLTF2Parser parser;
        GLTF2Context ctx;
        parser.setContext(&ctx);
        const bool parsingSuccessful = parser.parse(gltfPath);

        // THEN
        QVERIFY(parsingSuccessful);

        // WHEN
        parser.generateContent();
        QVERIFY(ctx.meshesCount() == 1);
        auto mesh = ctx.mesh(0);

        QVERIFY(mesh.meshPrimitives.size() == 1);
        auto meshRenderer = mesh.meshPrimitives[0].primitiveRenderer;

        QVERIFY(!Kuesa::GLTF2Import::MeshParserUtils::needsNormalAttribute(meshRenderer->geometry(),
                                                                           meshRenderer->primitiveType()));
    }
};

QTEST_MAIN(tst_NormalGenerator)

#include "tst_normalgenerator.moc"
