/*
    tst_gltfparser.cpp

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
#include <Kuesa/SceneEntity>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QComponent>
#include <Kuesa/MetallicRoughnessMaterial>
#include <Qt3DCore/QSkeleton>
#include <Qt3DCore/QJoint>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QCamera>
#include <Kuesa/LayerCollection>
#include <Kuesa/private/kuesa_utils_p.h>
#include <Kuesa/GLTF2Options>
#include <Kuesa/GLTF2Importer>
#include <Qt3DRender/QAttribute>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_ParserOptions : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultProperties()
    {
        // GIVEN
        GLTF2Options options;

        // THEN
        QCOMPARE(options.generateTangents(), false);
    }

    void checkSignals()
    {
        // GIVEN
        GLTF2Options options;
        QSignalSpy generateTangentsSpy(&options, &GLTF2Options::generateTangentsChanged);

        // WHEN
        options.setGenerateTangents(true);
        options.setGenerateTangents(true);

        // THEN
        QCOMPARE(generateTangentsSpy.count(), 1);
    }

    void checkGenerateTangents_data()
    {
        QTest::addColumn<bool>("generateTangents");

        QTest::newRow("Generate Tangents") << true;
        QTest::newRow("Don't Generate Tangents") << false;
    }

    void checkGenerateTangents()
    {
        // GIVEN
        QFETCH(bool, generateTangents);

        GLTF2Options options;
        options.setGenerateTangents(generateTangents);

        GLTF2Importer importer;
        Kuesa::SceneEntity sceneEntity;
        importer.setSceneEntity(&sceneEntity);
        importer.setOptions(options);

        // WHEN
        importer.setSource(QUrl::fromLocalFile(ASSETS "simple_cube_uv.gltf"));

        Kuesa::GLTF2Importer::Status status = Kuesa::GLTF2Importer::None;
        QObject::connect(&importer, &GLTF2Importer::statusChanged, [&status](Kuesa::GLTF2Importer::Status newStatus) {
            status = newStatus;
        });
        qApp->processEvents();
        while (status == Kuesa::GLTF2Importer::Loading) {
            qApp->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        // THEN
        auto cube0 = sceneEntity.mesh("Cube_0");
        const auto &attributes = cube0->geometry()->attributes();
        // Look for the tangents attribute
        const auto attrIt = std::find_if(std::begin(attributes), std::end(attributes), [](const Qt3DRender::QAttribute *attr) {
            return attr->name() == Qt3DRender::QAttribute::defaultTangentAttributeName();
        });

        QVERIFY((attrIt != std::end(attributes)) == generateTangents);
    }
};

QTEST_MAIN(tst_ParserOptions)

#include "tst_parseroptions.moc"
