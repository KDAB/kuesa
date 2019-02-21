/*
    tst_gltfexporter.cpp

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jean-Michaël Celerier <jean-michael.celerier@kdab.com>

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

#include <qtkuesa-config.h>
#include <QtTest/QtTest>
#include <Kuesa/SceneEntity>
#include <Kuesa/GLTF2Importer>
#include <Kuesa/private/gltf2context_p.h>
#include <Kuesa/private/gltf2exporter_p.h>
#include <Kuesa/private/gltf2parser_p.h>
#include <Kuesa/private/kuesa_utils_p.h>
using namespace Kuesa;
using namespace GLTF2Import;

namespace {
static QDir setupTestFolder()
{
    QDir tmp(QStandardPaths::standardLocations(QStandardPaths::TempLocation)[0]);
    if (tmp.exists("kuesa-gltfexporter-test")) {
        tmp.cd("kuesa-gltfexporter-test");
        tmp.removeRecursively();
        tmp.cdUp();
    }
    tmp.mkdir("kuesa-gltfexporter-test");
    tmp.cd("kuesa-gltfexporter-test");
    tmp.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    return tmp;
}
} // namespace

class tst_GLTFExporter : public QObject
{
    Q_OBJECT

private Q_SLOTS:

#if defined(KUESA_DRACO_COMPRESSION)
    void checkBoxCompression()
    {
        SceneEntity scene;
        GLTF2Context ctx;

        GLTF2Parser parser(&scene);
        parser.setContext(&ctx);

        QString asset(ASSETS "Box.gltf");
        QDir tmp = setupTestFolder();

        GLTF2Exporter::Export exported;
        // WHEN
        {
            auto res = parser.parse(asset);
            QVERIFY(res != nullptr);

            GLTF2ExportConfiguration configuration;
            configuration.setMeshCompressionEnabled(true);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Position, 5);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Normal, 5);

            GLTF2Exporter exporter;
            exporter.setContext(&ctx);
            exporter.setScene(&scene);
            exporter.setConfiguration(configuration);

            exported = exporter.saveInFolder(
                    QDir(ASSETS),
                    tmp);
        }

        // THEN
        {
            QVERIFY(exported.success());

            ctx = GLTF2Context{};
            auto res = parser.parse(QJsonDocument{ exported.json() }.toJson(), tmp.absolutePath());
            QVERIFY(res != nullptr);

            QVERIFY(tmp.exists(exported.compressedBufferFilename()));
            QVERIFY(!tmp.exists("Box0.bin"));
            QCOMPARE(tmp.count(), 1U);
        }
    }

    void checkCarCompression()
    {
        SceneEntity scene;
        GLTF2Context ctx;

        GLTF2Parser parser(&scene);
        parser.setContext(&ctx);

        QDir source_dir(ASSETS "car/");
        QString asset(ASSETS "car/DodgeViper.gltf");
        QDir tmp = setupTestFolder();

        GLTF2Exporter::Export exported;
        // WHEN
        {
            auto res = parser.parse(asset);
            QVERIFY(res != nullptr);

            GLTF2ExportConfiguration configuration;
            configuration.setMeshCompressionEnabled(true);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Position, 4);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Normal, 4);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Color, 4);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::TextureCoordinate, 4);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Generic, 4);

            GLTF2Exporter exporter;
            exporter.setContext(&ctx);
            exporter.setScene(&scene);
            exporter.setConfiguration(configuration);

            exported = exporter.saveInFolder(
                    source_dir,
                    tmp);

            // Original model is 14 megabytes
            QFileInfo dodge_info(source_dir.filePath("DodgeViper.bin"));
            QVERIFY(dodge_info.size() == 14406004);
        }

        // THEN
        {
            QVERIFY(exported.success());

            // Check that a compressed buffer is created
            QVERIFY(tmp.exists(exported.compressedBufferFilename()));

            // Check that all the remaining data is correctly copied
            QVERIFY(tmp.exists("DodgeViper.bin"));
            QVERIFY(tmp.exists("hex-base.png.png"));
            QVERIFY(tmp.exists("hex-normal.png.png"));
            QVERIFY(tmp.exists("KDABnormal.png.png"));
            QVERIFY(tmp.exists("KDAB.png.png"));
            QVERIFY(tmp.exists("shadow-plane.png.png"));
            QCOMPARE(tmp.count(), 7U);

            // Some non-mesh data remains
            QFileInfo dodge_info(tmp.filePath("DodgeViper.bin"));
            QVERIFY(dodge_info.size() == 21898);

            // Compressed mesh is less than a megabyte
            QFileInfo comp_info(tmp.filePath(exported.compressedBufferFilename()));
            QVERIFY(comp_info.exists());
            QVERIFY(comp_info.size() < 1024 * 1024);

            // Check that we can reload the mesh properly
            ctx = GLTF2Context{};
            auto res = parser.parse(QJsonDocument{ exported.json() }.toJson(), tmp.absolutePath());
            QVERIFY(res != nullptr);
        }
    }

    void noChangesIfAlreadyCompressd()
    {
        SceneEntity scene;
        GLTF2Context ctx;

        GLTF2Parser parser(&scene);
        parser.setContext(&ctx);

        QString asset(ASSETS "draco/Box.gltf");
        QFile asset_file(asset);
        asset_file.open(QIODevice::ReadOnly);
        QJsonObject asset_json{ QJsonDocument::fromJson({ asset_file.readAll() }).object() };
        asset_file.close();

        QDir tmp = setupTestFolder();

        GLTF2Exporter::Export exported;
        // WHEN
        {
            auto res = parser.parse(asset);
            QVERIFY(res != nullptr);

            GLTF2ExportConfiguration configuration;
            configuration.setMeshCompressionEnabled(true);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Position, 5);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Normal, 5);

            GLTF2Exporter exporter;
            exporter.setContext(&ctx);
            exporter.setScene(&scene);
            exporter.setConfiguration(configuration);

            exported = exporter.saveInFolder(
                    QDir(ASSETS "draco"),
                    tmp);
        }

        // THEN
        {
            // JSON hasn't changed
            QVERIFY(asset_json == exported.json());

            // Referenced buffer Box0.bin is copied
            QCOMPARE(tmp.count(), 1U);
        }
    }

    void compressedBufferFilenameIncrement()
    {
        SceneEntity scene;
        GLTF2Context ctx;

        GLTF2Parser parser(&scene);
        parser.setContext(&ctx);

        // Copy assets in a tmp dir
        QDir tmp = setupTestFolder();
        tmp.mkdir("sub");

        auto sub = tmp;
        sub.cd("sub");

        for (QString filename : { "Box.gltf", "Box0.bin" }) {
            const QString asset(ASSETS + filename);
            QFile(asset).copy(tmp.absolutePath() + "/" + filename);
        }

        // Create a fake file named compressedBuffer.bin
        {
            QFile existingFile(tmp.absolutePath() + "/compressedBuffer.bin");
            existingFile.open(QIODevice::WriteOnly);
            existingFile.write("\0", 1);
        }

        GLTF2Exporter::Export exported;
        // WHEN
        {
            const QString asset = tmp.absolutePath() + "/Box.gltf";
            auto res = parser.parse(asset);
            QVERIFY(res != nullptr);

            GLTF2ExportConfiguration configuration;
            configuration.setMeshCompressionEnabled(true);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Position, 5);
            configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Normal, 5);

            GLTF2Exporter exporter;
            exporter.setContext(&ctx);
            exporter.setScene(&scene);
            exporter.setConfiguration(configuration);

            exported = exporter.saveInFolder(
                    tmp,
                    sub);
        }

        // THEN
        {
            QVERIFY(exported.success());
            QVERIFY(exported.compressedBufferFilename() == "compressedBuffer-1.bin");
            QVERIFY(sub.exists(exported.compressedBufferFilename()));
            QVERIFY(!sub.exists("Box0.bin"));
            QCOMPARE(sub.count(), 1U);

            ctx = GLTF2Context{};
            auto res = parser.parse(QJsonDocument{ exported.json() }.toJson(), sub.absolutePath());
            QVERIFY(res != nullptr);
        }
    }
#endif
};

QTEST_APPLESS_MAIN(tst_GLTFExporter)

#include "tst_gltfexporter.moc"
