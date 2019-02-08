/*
    main.cpp

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

#include <Kuesa/private/gltf2parser_p.h>
#include <Kuesa/private/gltf2exporter_p.h>
#include <Kuesa/SceneEntity>

#include <QCoreApplication>
#include <QDir>
#include <QCommandLineParser>
#include <QElapsedTimer>

int main(int argc, char *argv[])
{
    using namespace Kuesa;
    using namespace Kuesa::GLTF2Import;

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("Kuesa Asset Compressor"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kdab.com"));
    QCoreApplication::setOrganizationName(QStringLiteral("KDAB"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));

    QCommandLineParser cmdline;
    cmdline.addHelpOption();
    cmdline.addVersionOption();
    cmdline.setApplicationDescription(
            QObject::tr(
                    "\nThis tool re-exports glTF 2.0 files with meshes compressed with the Draco algorithm.\n"
                    "Example : ./assetcompressor -i my_file.gltf -o /path/to/new/folder -q 8"));

    QCommandLineOption inputOption({ "i", "input" }, QObject::tr("glTF2 <file> to compress."), QObject::tr("file"), QString());
    QCommandLineOption outputOption({ "o", "output" }, QObject::tr("Output <directory>."), QObject::tr("directory"), QString());
    QCommandLineOption ratioOption({ "q", "quantization" }, QObject::tr("Compression quantization, between 1 (more compressed) and 16 (less compressed)."), QObject::tr("value"), "7");
    QCommandLineOption forceOption({ "f", "force" }, QObject::tr("Force overwriting existing files."));

    cmdline.addOptions({ inputOption, outputOption, ratioOption, forceOption });

    cmdline.process(app);

    const auto asset = cmdline.value(inputOption);
    const QFileInfo assetFileInfo(asset);
    if (!cmdline.isSet(inputOption) || asset.isEmpty() || !assetFileInfo.exists()) {
        qCritical("Invalid input file.\n%s", cmdline.helpText().toLatin1().constData());
        return 1;
    }

    const auto outputDirArg = cmdline.value(outputOption);
    if (!cmdline.isSet(outputOption) || outputDirArg.isEmpty()) {
        qCritical("Invalid output directory.\n%s", cmdline.helpText().toLatin1().constData());
        return 1;
    }

    const int ratio = cmdline.value(ratioOption).toInt();
    if (ratio < 1 || ratio > 16) {
        qCritical("Invalid compression ratio.\n%s", cmdline.helpText().toLatin1().constData());
        return 1;
    }

    const bool force = cmdline.isSet(forceOption);

    const QDir targetDir = outputDirArg;
    if (!targetDir.exists()) {
        if (!QDir{}.mkpath(outputDirArg)) {
            qCritical("Could not create output directory.");
            return 1;
        }
    }

    // Parsing
    SceneEntity scene;
    GLTF2Context ctx;

    GLTF2Parser parser(&scene);
    parser.setContext(&ctx);

    const auto res = parser.parse(asset);
    if (!res) {
        qCritical("Could not parse glTF2 file.");
        return 1;
    }

    // Compression
    GLTF2ExportConfiguration configuration;
    configuration.setMeshCompressionEnabled(true);
    configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Position, ratio);
    configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Normal, ratio);
    configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Color, ratio);
    configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::TextureCoordinate, ratio);
    configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Generic, ratio);

    GLTF2Exporter exporter;
    exporter.setContext(&ctx);
    exporter.setScene(&scene);
    exporter.setConfiguration(configuration);

    const auto sourceDir = assetFileInfo.dir();

    // Check that we won't accidentally overwrite something
    auto overwriteable = exporter.overwritableFiles(targetDir);
    if (!overwriteable.empty() && !force) {
        qCritical("Use --force if you want to overwrite the following files:\n%s\n", overwriteable.join("\n").toLatin1().constData());
    }

    // Meaasure the actual export step duration
    QElapsedTimer timer;
    timer.start();

    const auto new_asset = exporter.saveInFolder(sourceDir, targetDir);

    const auto time_taken = timer.nsecsElapsed();

    if (!new_asset.success()) {
        QTextStream errors;
        for (const auto &err : exporter.errors())
            errors << err << "\n";
        qCritical("Errors during compression: \n%s", errors.readAll().toLatin1().constData());
        return 1;
    }

    // Write down the new glTF file
    {
        const auto gltfFilename = assetFileInfo.baseName();
        const auto gltfPath = QStringLiteral("%1/%2.gltf")
                                      .arg(targetDir.absolutePath())
                                      .arg(gltfFilename);

        QFile gltfOut(gltfPath);
        if (gltfOut.exists() && !force) {
            qCritical("Use --force if you want to overwrite %s\n", gltfPath.toLatin1().constData());
        }

        if (!gltfOut.open(QIODevice::WriteOnly)) {
            qCritical("Could not open glTF2 file for writing: \n%s", gltfOut.errorString().toLatin1().constData());
            return 1;
        }

        gltfOut.write(QJsonDocument{ new_asset.json() }.toJson());
    }

    // Show some stats
    const auto space_taken = QFileInfo(targetDir.filePath(new_asset.compressedBufferFilename())).size();
    QTextStream out(stdout);
    out << QObject::tr("Kuesa: Compressing mesh took %1 milliseconds\n").arg(time_taken / 1000000.);
    out << QObject::tr("Kuesa: Space taken by compressed meshes: %2 bytes\n").arg(space_taken);

    return 0;
}
