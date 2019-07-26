/*
    main.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QGuiApplication>
#include <QDir>
#include <QCommandLineParser>
#include <QElapsedTimer>

/*!
    \page assetprocessor.html
    \title Kuesa Asset Processor (assetprocessor)
    \keyword assetprocessor

    The \c assetprocessor tool is used to perform transformations of glTF files.

    Usage:
    \c{./assetcompressor -i my_file.gltf -o /path/to/new/folder -q 8}

    assetprocessor accepts the following command line options:

    \table
    \header \li Option         \li Argument \li Description

    \row \li \c{-i, --input} \li \c{file} \li glTF \c{file} to process.
    \row \li \c{-o, --output} \li \c{folder} \li Output folder.
    \row \li \c{-q, --quantization} \li \c{level} \li Mesh compression quantization. Uses the Draco mesh compression method.
    \row \li \c{-e, --embed} \li \li Embed assets as base64 in the glTF file.
    \row \li \c{-s, --separate} \li \li Separate embedded assets.
    \row \li \c{-f, --force} \li \li Force overwriting of existing files.
    \row \li \c{--stats} \li \li Compute and show size statistics.
    \row \li \c{--version} \li \li Display version information.
    \row \li \c{--help} \li \li Display usage information.

    \endtable

    If neither of \c{--embed} or \c{--separate} is specified, the assets will be kept
    as they are.
*/

int main(int argc, char *argv[])
{
    using namespace Kuesa;
    using namespace Kuesa::GLTF2Import;

    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("Kuesa Asset Compressor"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kdab.com"));
    QCoreApplication::setOrganizationName(QStringLiteral("KDAB"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));

    QCommandLineParser cmdline;
    cmdline.addHelpOption();
    cmdline.addVersionOption();
    cmdline.setApplicationDescription(
            QObject::tr(
                    "\nThis tool processes and transforms glTF 2.0 files.\n"
                    "Example : ./assetcompressor -i my_file.gltf -o /path/to/new/folder -q 8"));

    QCommandLineOption inputOption({ "i", "input" }, QObject::tr("glTF2 <file> to process."), QObject::tr("file"), QString());
    QCommandLineOption outputOption({ "o", "output" }, QObject::tr("Output <directory>."), QObject::tr("directory"), QString());
    QCommandLineOption ratioOption({ "q", "quantization" }, QObject::tr("Mesh compression quantization, between 1 (more compressed) and 16 (less compressed)."), QObject::tr("value"));
    QCommandLineOption embedOption({ "e", "embed" }, QObject::tr("Embed assets in base64."));
    QCommandLineOption extractOption({ "s", "separate" }, QObject::tr("Separate embedded assets."));
    QCommandLineOption forceOption({ "f", "force" }, QObject::tr("Force overwriting existing files."));
    QCommandLineOption statsOption("stats", QObject::tr("Compute and show size statistics."));

    cmdline.addOptions({ inputOption, outputOption, ratioOption, embedOption, extractOption, forceOption, statsOption });

    cmdline.process(app);

    const auto asset = cmdline.value(inputOption);
    const QFileInfo assetFileInfo(asset);
    const auto sourceDir = assetFileInfo.dir();
    if (!cmdline.isSet(inputOption) || asset.isEmpty() || !assetFileInfo.exists()) {
        qCritical("Invalid input file.\n%s", cmdline.helpText().toLatin1().constData());
        return 1;
    }

    const auto outputDirArg = cmdline.value(outputOption);
    if (!cmdline.isSet(outputOption) || outputDirArg.isEmpty()) {
        qCritical("Invalid output directory.\n%s", cmdline.helpText().toLatin1().constData());
        return 1;
    }

    const bool hasRatio = cmdline.isSet(ratioOption);
    const int ratio = cmdline.value(ratioOption).toInt();
    if (hasRatio && (ratio < 1 || ratio > 16)) {
        qCritical("Invalid compression ratio.\n%s", cmdline.helpText().toLatin1().constData());
        return 1;
    }

    const bool force = cmdline.isSet(forceOption);
    const bool embed = cmdline.isSet(embedOption);
    const bool extract = cmdline.isSet(extractOption);
    if (embed && extract) {
        qCritical("Cannot specify both --embed and --extract.\n%s", cmdline.helpText().toLatin1().constData());
        return 1;
    }

    const bool stats = cmdline.isSet(statsOption);

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

    QElapsedTimer timer;
    qint64 sourceParseTime = 0;
    qint64 processedParseTime = 0;

    timer.start();
    const auto res = parser.parse(asset);
    sourceParseTime = timer.elapsed();

    if (!res) {
        qCritical("Could not parse glTF2 file.");
        return 1;
    }

    // Compute total input size
    qint64 sourceSize = 0;
    qint64 processedSize = 0;
    if (stats) {
        for (const auto &file : ctx.localFiles()) {
            sourceSize += QFileInfo(sourceDir.filePath(file)).size();
        }
        sourceSize += assetFileInfo.size();
    }

    // Compression
    GLTF2ExportConfiguration configuration;
    if (hasRatio) {
        configuration.setMeshCompressionEnabled(true);
        configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Position, ratio);
        configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Normal, ratio);
        configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Color, ratio);
        configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::TextureCoordinate, ratio);
        configuration.setAttributeQuantizationLevel(GLTF2ExportConfiguration::Generic, ratio);
    }

    if (embed)
        configuration.setEmbedding(GLTF2ExportConfiguration::Embed::All);
    else if (extract)
        configuration.setEmbedding(GLTF2ExportConfiguration::Embed::None);
    else
        configuration.setEmbedding(GLTF2ExportConfiguration::Embed::Keep);

    GLTF2Exporter exporter;
    exporter.setContext(&ctx);
    exporter.setScene(&scene);
    exporter.setConfiguration(configuration);

    // Check that we won't accidentally overwrite something
    auto overwriteable = exporter.overwritableFiles(targetDir);
    if (!overwriteable.empty() && !force) {
        qCritical("Use --force if you want to overwrite the following files:\n%s\n", overwriteable.join("\n").toLatin1().constData());
    }

    // Measure the actual export step duration
    timer.restart();
    const auto newAsset = exporter.saveInFolder(sourceDir, targetDir);
    const auto exportTime = timer.elapsed();

    if (!newAsset.success()) {
        QTextStream errors;
        const auto &reporterErrors = exporter.errors();
        for (const auto &err : reporterErrors)
            errors << err << "\n";
        qCritical("Errors during compression: \n%s", errors.readAll().toLatin1().constData());
        return 1;
    }

    // Write down the new glTF file
    const auto newAssetJson = QJsonDocument{ newAsset.json() }.toJson();

    const auto gltfFilename = assetFileInfo.baseName();
    const auto gltfPath = QStringLiteral("%1/%2.gltf").arg(targetDir.absolutePath(), gltfFilename);

    QFile gltfOut(gltfPath);
    if (gltfOut.exists() && !force) {
        qCritical("Use --force if you want to overwrite %s\n", gltfPath.toLatin1().constData());
    }

    if (!gltfOut.open(QIODevice::WriteOnly)) {
        qCritical("Could not open glTF2 file for writing: \n%s", gltfOut.errorString().toLatin1().constData());
        return 1;
    }

    processedSize += gltfOut.write(newAssetJson);

    // Show some stats
    if (stats) {
        SceneEntity scene;
        GLTF2Context ctx;
        GLTF2Parser parser(&scene);
        parser.setContext(&ctx);

        timer.restart();
        parser.parse(gltfPath);
        processedParseTime = timer.elapsed();

        for (const auto &file : ctx.localFiles()) {
            processedSize += QFileInfo(targetDir.filePath(file)).size();
        }

        QTextStream out(stdout);
        out << QObject::tr("Kuesa: Space taken by original asset: %1 kilobytes\n").arg(sourceSize / 1000.);
        out << QObject::tr("Kuesa: Space taken by processed asset: %1 kilobytes\n").arg(processedSize / 1000.);
        out << QObject::tr("Kuesa: Size delta: %1 kilobytes (%2 % of the original)\n")
                        .arg((processedSize - sourceSize) / 1000.)
                        .arg(100. * double(processedSize) / double(sourceSize));

        out << QObject::tr("Kuesa: Initial parsing took %1 milliseconds\n").arg(sourceParseTime);
        out << QObject::tr("Kuesa: Asset processing took %1 milliseconds\n").arg(exportTime);
        out << QObject::tr("Kuesa: Parsing the processed asset took %1 milliseconds\n").arg(processedParseTime);
        out << QObject::tr("Kuesa: Time delta: %1 milliseconds (%2 % of the original)\n")
                        .arg((processedParseTime - sourceParseTime))
                        .arg(100. * processedParseTime / sourceParseTime);
    }

    return 0;
}
