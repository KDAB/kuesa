/*
    tst_assetcollection.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#include <Kuesa/abstractassetcollection.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DCore/qentity.h>

class DummyAssetCollection : public Kuesa::AbstractAssetCollection
{
public:
    KUESA_ASSET_COLLECTION_IMPLEMENTATION(Qt3DRender::QMaterial)
};

class tst_AssetCollection : public QObject
{
    Q_OBJECT
public:
    tst_AssetCollection() { }
    ~tst_AssetCollection() { }

private Q_SLOTS:
    void shouldHaveDefaultState()
    {
        // GIVEN
        DummyAssetCollection dummyCollection;

        // THEN
        QVERIFY(dummyCollection.names().empty());
        QCOMPARE(dummyCollection.find("test"), nullptr);
    }

    void shouldSupportAddingAssets()
    {
        // GIVEN
        DummyAssetCollection dummyCollection;
        auto asset = new Qt3DRender::QMaterial;
        QSignalSpy nameChangeSpy(&dummyCollection, SIGNAL(namesChanged()));

        // WHEN
        dummyCollection.add("testAsset", asset);

        // THEN
        QCOMPARE(dummyCollection.names().size(), 1);
        QCOMPARE(dummyCollection.find("testAsset"), asset);
        QCOMPARE(dummyCollection.find("test"), nullptr);
        QCOMPARE(nameChangeSpy.count(), 1);

        // WHEN (adding new object with same name)
        QSignalSpy destroyedSpy(asset, SIGNAL(destroyed(QObject *)));
        dummyCollection.add("testAsset", new Qt3DRender::QMaterial);

        // THEN
        QCOMPARE(dummyCollection.names().size(), 1);
        QVERIFY(dummyCollection.find("testAsset") != asset);
        QCOMPARE(destroyedSpy.count(), 1);
        QCOMPARE(nameChangeSpy.count(), 1);
    }

    void shouldSupportRemovingAssets()
    {
        // GIVEN
        DummyAssetCollection dummyCollection;
        auto asset = new Qt3DRender::QMaterial;
        QSignalSpy destroyedSpy(asset, SIGNAL(destroyed(QObject *)));

        // WHEN
        dummyCollection.add("testAsset", asset);
        QSignalSpy nameChangeSpy(&dummyCollection, SIGNAL(namesChanged()));
        dummyCollection.remove("testAsset");

        // THEN
        QVERIFY(dummyCollection.names().empty());
        QCOMPARE(dummyCollection.find("testAsset"), nullptr);
        QCOMPARE(destroyedSpy.count(), 1);
        QCOMPARE(nameChangeSpy.count(), 1);
    }

    void shouldSupportClearAll()
    {
        // GIVEN
        DummyAssetCollection dummyCollection;

        // WHEN
        dummyCollection.add("testAsset1", new Qt3DRender::QMaterial);
        dummyCollection.add("testAsset2", new Qt3DRender::QMaterial);
        QSignalSpy nameChangeSpy(&dummyCollection, SIGNAL(namesChanged()));
        dummyCollection.clear();

        // THEN
        QVERIFY(dummyCollection.names().empty());
        QCOMPARE(nameChangeSpy.count(), 1);
    }

    void shouldReplaceExistingAsset()
    {
        // GIVEN
        DummyAssetCollection collection;
        Qt3DCore::QEntity entity;
        auto asset = new Qt3DRender::QMaterial(&entity); // this has a parent, it should be not deleted when replaced in the collection
        QSignalSpy spy(asset, SIGNAL(destroyed(QObject *)));

        // WHEN
        collection.add("asset", asset);

        // THEN
        QCOMPARE(collection.names().size(), 1);
        QCOMPARE(collection.find("asset"), asset);
        QCOMPARE(asset->parent(), &entity);

        // WHEN
        auto newAsset = new Qt3DRender::QMaterial;
        collection.add("asset", newAsset);

        // THEN
        QVERIFY(newAsset);
        QCOMPARE(collection.names().size(), 1);
        QCOMPARE(collection.find("asset"), newAsset);
        QVERIFY(spy.empty());
    }

    void shouldReplaceAndDeleteExistingAsset()
    {
        // GIVEN
        DummyAssetCollection collection;
        auto asset = new Qt3DRender::QMaterial;
        QSignalSpy spy(asset, SIGNAL(destroyed(QObject *)));

        // WHEN
        collection.add("asset", asset);

        // THEN
        QCOMPARE(collection.names().size(), 1);
        QCOMPARE(collection.find("asset"), asset);
        QCOMPARE(asset->parent(), &collection);

        // WHEN
        auto newAsset = new Qt3DRender::QMaterial;
        collection.add("asset", newAsset);

        // THEN
        QVERIFY(newAsset);
        QCOMPARE(collection.names().size(), 1);
        QCOMPARE(collection.find("asset"), newAsset);
        QCOMPARE(spy.size(), 1);
    }

    void shouldRemoveDeletedAsset()
    {
        // GIVEN
        DummyAssetCollection collection;
        Qt3DCore::QEntity entity;
        auto asset = new Qt3DRender::QMaterial(&entity); // has a parent, will not be deleted when replaced in the collection
        QSignalSpy nameChangeSpy(&collection, SIGNAL(namesChanged()));

        // WHEN
        collection.add("asset", asset);

        // THEN
        QCOMPARE(collection.names().size(), 1);
        QCOMPARE(collection.find("asset"), asset);
        QCOMPARE(nameChangeSpy.count(), 1);

        // WHEN (replacing asset with same name and deleting the original)
        auto newAsset = new Qt3DRender::QMaterial;
        collection.add("asset", newAsset);
        delete asset;

        // THEN (the new asset is not removed and nameChanged not fired)
        QCOMPARE(collection.names().size(), 1);
        QCOMPARE(collection.find("asset"), newAsset);
        QCOMPARE(nameChangeSpy.count(), 1);

        // WHEN (deleting new asset)
        delete newAsset;

        // THEN (new asset is removed from collection and nameChanged is fired)
        QCOMPARE(collection.names().size(), 0);
        QCOMPARE(collection.find("asset"), nullptr);
        QCOMPARE(nameChangeSpy.count(), 2);
    }

    void handleSameAssetRegisteredTwice()
    {
        // GIVEN
        DummyAssetCollection collection;
        QSignalSpy nameChangeSpy(&collection, SIGNAL(namesChanged()));

        // THEN
        QVERIFY(nameChangeSpy.isValid());

        // WHEN
        {
            Qt3DCore::QEntity entity;
            auto asset = new Qt3DRender::QMaterial(&entity); // has a parent, will not be deleted when replaced in the collection

            // WHEN
            collection.add("asset_name1", asset);
            collection.add("asset_name2", asset);

            // THEN
            QCOMPARE(collection.names().size(), 2);
            QCOMPARE(collection.find("asset_name1"), asset);
            QCOMPARE(collection.find("asset_name2"), asset);
            QCOMPARE(nameChangeSpy.count(), 2);
        }

        // THEN
        QCOMPARE(nameChangeSpy.count(), 4);
        QVERIFY(collection.find("asset_name1") == nullptr);
        QVERIFY(collection.find("asset_name2") == nullptr);
        QCOMPARE(collection.names().size(), 0);
    }

    void ensureAssetsAreSortedByName()
    {
        // GIVEN
        DummyAssetCollection collection;
        auto asset1 = new Qt3DRender::QMaterial();
        auto asset2 = new Qt3DRender::QMaterial();
        auto asset3 = new Qt3DRender::QMaterial();
        auto asset4 = new Qt3DRender::QMaterial();

        // WHEN
        collection.add(QStringLiteral("Asset1"), asset1);
        collection.add(QStringLiteral("asset2"), asset2);
        collection.add(QStringLiteral("Asset3"), asset3);
        collection.add(QStringLiteral("asset4"), asset4);

        // THEN
        QCOMPARE(collection.names(),
                 (QStringList()
                  << QStringLiteral("Asset1")
                  << QStringLiteral("Asset3")
                  << QStringLiteral("asset2")
                  << QStringLiteral("asset4")));
    }
};

QTEST_GUILESS_MAIN(tst_AssetCollection)
#include "tst_assetcollection.moc"
