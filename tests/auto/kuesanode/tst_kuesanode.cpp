/*
    tst_kuesanode.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <qtkuesa-config.h>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <Kuesa/KuesaNode>
#include <QUrl>

using namespace Kuesa;
using namespace GLTF2Import;

class tst_KuesaNode : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        KuesaNode node;

        // THEN
        QVERIFY(node.sceneEntity() == nullptr);
    }

    void checkSetSceneEntity()
    {
        // GIVEN
        KuesaNode node;
        SceneEntity scene;
        QSignalSpy sceneEntityChangedSpy(&node, &KuesaNode::sceneEntityChanged);

        // THEN
        QVERIFY(sceneEntityChangedSpy.isValid());
        QVERIFY(node.sceneEntity() == nullptr);

        // WHEN
        node.setSceneEntity(&scene);

        // THEN
        QCOMPARE(node.sceneEntity(), &scene);
        QCOMPARE(sceneEntityChangedSpy.count(), 1);

        // WHEN
        node.setSceneEntity(&scene);

        // THEN
        QCOMPARE(node.sceneEntity(), &scene);
        QCOMPARE(sceneEntityChangedSpy.count(), 1);
    }

    void checkHandleSceneEntityDestruction()
    {
        // GIVEN
        KuesaNode node;

        // WHEN
        {
            SceneEntity scene;
            node.setSceneEntity(&scene);

            // THEN
            QCOMPARE(node.sceneEntity(), &scene);
        }

        // THEN
        QVERIFY(node.sceneEntity() == nullptr);
    }

    void checkAutoSetSceneEntityFromParent()
    {
        {
            // GIVEN
            SceneEntity s;
            KuesaNode node(&s);

            // THEN
            QVERIFY(node.sceneEntity() == &s);
        }
        {
            // GIVEN
            Qt3DCore::QEntity e;
            KuesaNode node(&e);

            // THEN
            QVERIFY(node.sceneEntity() == nullptr);
        }
        {
            // GIVEN
            SceneEntity s;
            KuesaNode node;

            // THEN
            QVERIFY(node.sceneEntity() == nullptr);

            // WHEN
            node.setParent(&s);

            // THEN
            QVERIFY(node.sceneEntity() == &s);
        }
        {
            // GIVEN
            Qt3DCore::QEntity e;
            Qt3DCore::QEntity *e2 = new Qt3DCore::QEntity;
            Qt3DCore::QEntity *e3 = new Qt3DCore::QEntity;
            SceneEntity *s = new SceneEntity;

            e2->setParent(&e);
            s->setParent(e2);
            e3->setParent(s);

            KuesaNode *node = new KuesaNode(e3);

            // THEN
            QVERIFY(node->sceneEntity() == s);
        }
        {
            // GIVEN
            SceneEntity s1;
            SceneEntity s2;
            KuesaNode node;

            // WHEN
            node.setSceneEntity(&s2);
            node.setParent(&s1);

            // THEN
            QVERIFY(node.sceneEntity() == &s2);
        }
        {
            // GIVEN
            SceneEntity s1;
            SceneEntity s2;
            KuesaNode node;

            // WHEN
            node.setParent(&s1);
            node.setSceneEntity(&s2);

            // THEN
            QVERIFY(node.sceneEntity() == &s2);
        }
    }
};

QTEST_MAIN(tst_KuesaNode)

#include "tst_kuesanode.moc"
