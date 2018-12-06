/*
    tst_materialinspector.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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
#include "materialinspector.h"
#include "assetinspector.h"
#include <Kuesa/private/gltf2parser_p.h>
#include <Kuesa/SceneEntity>
#include <QSignalSpy>
#include <Kuesa/metallicroughnessmaterial.h>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_MaterialInspector : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testMaterialChangeSignal()
    {
        // GIVEN
        SceneEntity scene;
        MaterialInspector inspector;
        QSignalSpy spy(&inspector, SIGNAL(materialParamsChanged()));
        QVERIFY(spy.isValid());

        // WHEN
        auto material = loadMaterial(&scene, QString(ASSETS "Box.gltf"));
        QVERIFY(material);
        inspector.setMaterial(material);

        // THEN
        QCOMPARE(spy.count(), 1);
        QCOMPARE(inspector.assetName(), QStringLiteral("Red"));

        // WHEN
        SceneEntity scene2;
        auto simpleMaterial = loadMaterial(&scene2, QString(ASSETS "simple_cube_with_textures.gltf"));
        QVERIFY(simpleMaterial);
        inspector.setMaterial(simpleMaterial);

        //THEN
        QCOMPARE(spy.count(), 2);
        QCOMPARE(inspector.assetName(), "Material");
    }

    void testMaterialDestruction()
    {
        // GIVEN
        auto scene = new SceneEntity;
        GLTF2Parser parser(scene, scene);
        MaterialInspector inspector;

        // WHEN
        parser.parse(QString(ASSETS "Box.gltf"));
        QVERIFY(scene->materials()->names().count() > 0);
        auto material = scene->material(scene->materials()->names().first());
        material->setParent(scene);
        QVERIFY(material);

        // WHEN
        inspector.setMaterial(qobject_cast<Kuesa::MetallicRoughnessMaterial *>(material));
        QSignalSpy spy(&inspector, SIGNAL(materialParamsChanged()));

        // THEN
        QVERIFY(spy.isValid());
        QCOMPARE(spy.count(), 0);
        QVERIFY(!inspector.assetName().isEmpty());

        // WHEN
        delete scene;

        //THEN
        QCOMPARE(spy.count(), 1);
        QVERIFY(inspector.assetName().isEmpty());
    }

    void testMaterialParams()
    {
        // GIVEN
        SceneEntity scene;
        MaterialInspector inspector;
        QSignalSpy spy(&inspector, SIGNAL(materialParamsChanged()));
        QVERIFY(spy.isValid());

        // WHEN
        auto boxMaterial = loadMaterial(&scene, QString(ASSETS "Box.gltf"));
        QVERIFY(boxMaterial);
        inspector.setMaterial(boxMaterial);

        QCOMPARE(inspector.baseColor().redF(), 0.8);
        QCOMPARE(inspector.baseColor().greenF(), 0.0);
        QCOMPARE(inspector.baseColor().blueF(), 0.0);
        QCOMPARE(inspector.metallicFactor(), 0.0f);
        QCOMPARE(inspector.roughnessFactor(), 1.0f);
        QVERIFY(inspector.metalRoughMap() == nullptr);
        QVERIFY(inspector.baseColorMap() == nullptr);
        QVERIFY(inspector.ambientOcclusionMap() == nullptr);
        QVERIFY(inspector.normalMap() == nullptr);
        QVERIFY(inspector.emissiveMap() == nullptr);
        QCOMPARE(inspector.doubleSided(), false);
        QCOMPARE(inspector.opaque(), true);
        QCOMPARE(inspector.useSkinning(), false);
        QCOMPARE(inspector.usingColorAttributes(), false);

        // WHEN
        SceneEntity scene2;
        auto material = loadMaterial(&scene2, QString(ASSETS "simple_cube_with_textures_material_test.gltf"));
        QVERIFY(material);
        inspector.setMaterial(material);

        // THEN
        QCOMPARE(inspector.baseColor().redF(), 0.2);
        QCOMPARE(inspector.baseColor().greenF(), 0.2);
        QCOMPARE(inspector.baseColor().blueF(), 0.2);
        QCOMPARE(inspector.metallicFactor(), 0.6f);
        QCOMPARE(inspector.roughnessFactor(), 0.8f);
        QCOMPARE(inspector.baseColorMap()->objectName(), QStringLiteral("diffuse"));
        QCOMPARE(inspector.metalRoughMap()->objectName(), QStringLiteral("normal"));

        QVERIFY(inspector.ambientOcclusionMap() != nullptr);
        QCOMPARE(inspector.ambientOcclusionMap()->objectName(), QStringLiteral("normal"));

        QCOMPARE(inspector.normalScale(), 10.0f);
        QVERIFY(inspector.normalMap() != nullptr);
        QCOMPARE(inspector.normalMap()->objectName(), QStringLiteral("normal"));

        QVERIFY(inspector.emissiveMap() != nullptr);
        QCOMPARE(inspector.emissiveMap()->objectName(), QStringLiteral("diffuse"));
        QCOMPARE(inspector.emissiveFactor().redF(), 0.6);
        QCOMPARE(inspector.emissiveFactor().greenF(), 0.6);
        QCOMPARE(inspector.emissiveFactor().blueF(), 0.6);

        QCOMPARE(inspector.doubleSided(), true);
        QCOMPARE(inspector.opaque(), true);
        QCOMPARE(inspector.useSkinning(), false);
        QCOMPARE(inspector.usingColorAttributes(), false);
    }

private:
    Kuesa::MetallicRoughnessMaterial *loadMaterial(SceneEntity *scene, const QString &filename)
    {
        GLTF2Parser parser(scene, scene);
        parser.parse(filename);
        auto material = scene->material(scene->materials()->names().first());
        return qobject_cast<Kuesa::MetallicRoughnessMaterial *>(material);
    }
};

QTEST_APPLESS_MAIN(tst_MaterialInspector)

#include "tst_materialinspector.moc"
