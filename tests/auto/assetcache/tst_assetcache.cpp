/*
    tst_assetcache.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Kuesa/private/assetcache_p.h>
#include <Qt3DCore/QNode>

struct MyGLTFAsset {
    QString key;
};

class tst_AssetCache : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Kuesa::AssetCache<MyGLTFAsset, Qt3DCore::QNode> cache;

        // WHEN
        MyGLTFAsset emptyKey;
        MyGLTFAsset someKey { QStringLiteral("some") };

        // THEN
        QVERIFY(!cache.getResourceFromCache(emptyKey));
        QVERIFY(!cache.getResourceFromCache(someKey));
    }

    void checkAddResourceToCache()
    {
        // GIVEN
        Kuesa::AssetCache<MyGLTFAsset, Qt3DCore::QNode> cache;

        // WHEN
        MyGLTFAsset key { QStringLiteral("key") };
        Qt3DCore::QNode *n = new Qt3DCore::QNode();
        cache.addResourceToCache(key, n);

        // THEN
        QCOMPARE(n, cache.getResourceFromCache(key));

        // WHEN
        MyGLTFAsset badKey { QStringLiteral("key_bad") };

        // THEN
        QVERIFY(!cache.getResourceFromCache(badKey));
    }

    void checkHandleResourceDestruction()
    {
        // GIVEN
        Kuesa::AssetCache<MyGLTFAsset, Qt3DCore::QNode> cache;

        // WHEN
        MyGLTFAsset key { QStringLiteral("key") };
        {
            Qt3DCore::QNode n;
            cache.addResourceToCache(key, &n);
        }

        // THEN
        QVERIFY(!cache.getResourceFromCache(key));
    }

    void checkPreserveResource()
    {
        // GIVEN
        Kuesa::SceneEntity scene;
        Kuesa::AssetCache<MyGLTFAsset, Qt3DCore::QNode> cache;
        MyGLTFAsset key { QStringLiteral("key") };
        Qt3DCore::QNode *n = new Qt3DCore::QNode;

        // WHEN
        {
            Qt3DCore::QNode root;
            n->setParent(&root);
            cache.addResourceToCache(key, n);

            // THEN
            QCOMPARE(n->parent(), &root);

            cache.setSceneEntity(&scene);

            // THEN
            QCOMPARE(n->parent(), &scene);
        }

        // THEN
        QCOMPARE(cache.getResourceFromCache(key), n);
    }
};

QTEST_MAIN(tst_AssetCache)
#include "tst_assetcache.moc"
