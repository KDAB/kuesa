/*
    tst_meshinstantiator.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QTest>
#include <QSignalSpy>

#include <Kuesa/meshinstantiator.h>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QGeometryRenderer>
#include <Kuesa/MetallicRoughnessMaterial>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#else
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#endif

class tst_MeshInstantiator : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void init()
    {
        qRegisterMetaType<std::vector<QMatrix4x4>>();
    }

    void checkDefaults()
    {
        // GIVEN
        Kuesa::MeshInstantiator instantiator;

        // THEN
        QVERIFY(instantiator.entityName().isEmpty());
        QVERIFY(instantiator.transformationMatrices().empty());
        QCOMPARE(instantiator.count(), 1);
        QVERIFY(instantiator.sceneEntity() == nullptr);
    }

    void checkSetEntityName()
    {
        // GIVEN
        Kuesa::MeshInstantiator instantiator;
        QSignalSpy spy(&instantiator, &Kuesa::MeshInstantiator::entityNameChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        instantiator.setEntityName(QStringLiteral("TestName"));

        // THEN
        QCOMPARE(instantiator.entityName(), QStringLiteral("TestName"));
        QCOMPARE(spy.count(), 1);

        // WHEN
        instantiator.setEntityName(QStringLiteral("TestName"));

        // THEN
        QCOMPARE(spy.count(), 1);
    }

    void checkSetTransformationMatricesAndCount()
    {
        // GIVEN
        Kuesa::MeshInstantiator instantiator;
        QSignalSpy transformsSpy(&instantiator, &Kuesa::MeshInstantiator::transformationMatricesChanged);
        QSignalSpy countSpy(&instantiator, &Kuesa::MeshInstantiator::countChanged);

        // THEN
        QVERIFY(transformsSpy.isValid());
        QVERIFY(countSpy.isValid());

        // WHEN
        QMatrix4x4 m1;
        QMatrix4x4 m2;

        m1.translate(QVector3D(1, 1, 1));
        m2.translate(QVector3D(0, 1, 0));
        instantiator.setTransformationMatrices({m1, m2});

        // THEN
        QCOMPARE(instantiator.transformationMatrices().size(), 2);
        QCOMPARE(instantiator.transformationMatrices().front(), m1);
        QCOMPARE(instantiator.transformationMatrices().back(), m2);
        QCOMPARE(instantiator.count(), 2);
        QCOMPARE(transformsSpy.count(), 1);
        QCOMPARE(countSpy.count(), 1);

        // WHEN
        instantiator.setTransformationMatrices({m1, m2});

        // THEN
        QCOMPARE(transformsSpy.count(), 1);
        QCOMPARE(countSpy.count(), 1);
    }

    void checkHandlesSceneEntityDestruction()
    {
        // GIVEN
        Kuesa::MeshInstantiator instantiator;

        // WHEN
        {
            Kuesa::SceneEntity scene;

            instantiator.setSceneEntity(&scene);

            QCOMPARE(instantiator.sceneEntity(), &scene);
        }

        // THEN
        QVERIFY(instantiator.sceneEntity() == nullptr);
    }

    void checkUpdatesRenderersWithInstances()
    {
        // GIVEN
        Kuesa::MeshInstantiator instantiator;
        Kuesa::SceneEntity scene;

        Qt3DCore::QEntity root;
        root.setObjectName("MyEntity");

        Qt3DCore::QEntity *e = new Qt3DCore::QEntity;
        Qt3DRender::QGeometryRenderer *g = new Qt3DRender::QGeometryRenderer;
        Qt3DGeometry::QGeometry *ge = new Qt3DGeometry::QGeometry();
        g->setGeometry(ge);
        Kuesa::MetallicRoughnessMaterial *m = new Kuesa::MetallicRoughnessMaterial;
        Kuesa::MetallicRoughnessEffect *f = new Kuesa::MetallicRoughnessEffect;
        m->setEffect(f);

        e->addComponent(g);
        e->addComponent(m);
        e->setParent(&root);

        // WHEN
        instantiator.setEntityName(QStringLiteral("MyEntity"));
        instantiator.setTransformationMatrices({QMatrix4x4(), QMatrix4x4()});

        // THEN
        QCOMPARE(g->instanceCount(), 1);
        QCOMPARE(ge->attributes().size(), 0);

        // WHEN
        scene.entities()->add(QStringLiteral("MyEntity"), &root);
        instantiator.setSceneEntity(&scene);

        // THEN
        QCOMPARE(g->instanceCount(), 2);
        QCOMPARE(ge->attributes().size(), 1);

        auto *attr = ge->attributes().first();
        QCOMPARE(attr->name(), QStringLiteral("perInstanceTransform"));
        QCOMPARE(attr->vertexSize(), 16);
        QCOMPARE(attr->divisor(), 1);
        QCOMPARE(attr->buffer()->data().size(), 2 * 16 * sizeof(float));
    }
};

QTEST_MAIN(tst_MeshInstantiator)
#include "tst_meshinstantiator.moc"
