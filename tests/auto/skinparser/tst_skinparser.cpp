/*
    tst_skinparser.cpp

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

#include <QtTest/QtTest>
#include <QJsonDocument>
#include <QFile>
#include <QLatin1String>
#include <QString>
#include <Qt3DRender/QTextureImage>
#include <Kuesa/private/skinparser_p.h>
#include <Kuesa/private/bufferviewsparser_p.h>
#include <Kuesa/private/bufferparser_p.h>
#include <Kuesa/private/bufferaccessorparser_p.h>
#include <Kuesa/private/nodeparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

using namespace Kuesa;
using namespace GLTF2Import;

Q_DECLARE_METATYPE(Kuesa::GLTF2Import::Skin)

namespace {
const QLatin1String KEY_BUFFERS = QLatin1String("buffers");
const QLatin1String KEY_BUFFERVIEWS = QLatin1String("bufferViews");
const QLatin1String KEY_ACCESSORS = QLatin1String("accessors");
const QLatin1String KEY_NODES = QLatin1String("nodes");
const QLatin1String KEY_SKINS = QLatin1String("skins");

bool compareSkins(const Kuesa::GLTF2Import::Skin a, const Kuesa::GLTF2Import::Skin b)
{
    return a.name == b.name &&
            a.jointsIndices == b.jointsIndices &&
            a.skeletonIdx == b.skeletonIdx &&
            a.inverseBindMatricesAccessorIdx == b.inverseBindMatricesAccessorIdx;
}

} // namespace

class tst_SkinParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void init()
    {
        qRegisterMetaType<Kuesa::GLTF2Import::Skin>();
    }

    void checkParse_data()
    {
        QTest::addColumn<QString>("filePath");
        QTest::addColumn<bool>("succeeded");
        QTest::addColumn<QVector<Kuesa::GLTF2Import::Skin>>("skins");

        Kuesa::GLTF2Import::Skin skin1;
        skin1.inverseBindMatricesAccessorIdx = 13;
        skin1.skeletonIdx = 2;
        skin1.jointsIndices = { 2, 3 };
        skin1.name = QStringLiteral("Armature");

        Kuesa::GLTF2Import::Skin skin2;
        skin2.inverseBindMatricesAccessorIdx = 13;
        skin2.skeletonIdx = -1;
        skin2.jointsIndices = { 2, 3 };
        skin2.name = QStringLiteral("Armature");

        Kuesa::GLTF2Import::Skin skin3;
        skin3.inverseBindMatricesAccessorIdx = -1;
        skin3.skeletonIdx = 2;
        skin3.jointsIndices = { 2, 3 };
        skin3.name = QStringLiteral("Armature");

        QTest::newRow("Valid") << QStringLiteral(ASSETS "skins_valid.gltf")
                               << true
                               << (QVector<Kuesa::GLTF2Import::Skin>() << skin1);

        QTest::newRow("Empty") << QStringLiteral(ASSETS "skins_empty.gltf")
                               << false
                               << (QVector<Kuesa::GLTF2Import::Skin>());

        QTest::newRow("Invalid-Joints-Empty") << QStringLiteral(ASSETS "skins_joints_empty.gltf")
                                              << false
                                              << (QVector<Kuesa::GLTF2Import::Skin>());

        QTest::newRow("Invalid-Joints-Missing") << QStringLiteral(ASSETS "skins_joints_missing.gltf")
                                                << false
                                                << (QVector<Kuesa::GLTF2Import::Skin>());

        QTest::newRow("Invalid-Skeleton") << QStringLiteral(ASSETS "skins_skeleton_invalid.gltf")
                                          << false
                                          << (QVector<Kuesa::GLTF2Import::Skin>());

        QTest::newRow("Missing-Skeleton") << QStringLiteral(ASSETS "skins_skeleton_missing.gltf")
                                          << true
                                          << (QVector<Kuesa::GLTF2Import::Skin>() << skin2);

        QTest::newRow("Invalid-InverseBindingMatrices") << QStringLiteral(ASSETS "skins_inverse_bind_matrices_invalid.gltf")
                                                        << false
                                                        << (QVector<Kuesa::GLTF2Import::Skin>());

        QTest::newRow("Missing-InverseBindingMatrices") << QStringLiteral(ASSETS "skins_inverse_bind_matrices_missing.gltf")
                                                        << true
                                                        << (QVector<Kuesa::GLTF2Import::Skin>() << skin3);
    }

    void checkParse()
    {
        QFETCH(QString, filePath);
        QFETCH(bool, succeeded);
        QFETCH(QVector<Skin>, skins);

        // GIVEN
        GLTF2Context context;
        BufferParser bufferParser(QDir(ASSETS));
        BufferViewsParser bufferViewParser;
        BufferAccessorParser accessorParser;
        NodeParser nodeParser;

        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QVERIFY(file.isOpen());

        // WHEN
        const QJsonDocument json = QJsonDocument::fromJson(file.readAll());
        const QJsonObject rootObj = json.object();
        // THEN
        QVERIFY(!json.isNull() && json.isObject() && !rootObj.isEmpty());

        // WHEN
        const bool initSuccess = bufferParser.parse(rootObj.value(KEY_BUFFERS).toArray(), &context) &&
                bufferViewParser.parse(rootObj.value(KEY_BUFFERVIEWS).toArray(), &context) &&
                accessorParser.parse(rootObj.value(KEY_ACCESSORS).toArray(), &context) &&
                nodeParser.parse(rootObj.value(KEY_NODES).toArray(), &context);
        // THEN
        QVERIFY(initSuccess);

        // WHEN
        SkinParser parser;
        bool success = parser.parse(rootObj.value(KEY_SKINS).toArray(), &context);

        // THEN
        QCOMPARE(success, succeeded);

        if (succeeded) {
            QCOMPARE(context.skinsCount(), skins.size());

            for (int i = 0; i < skins.size(); ++i) {
                const Kuesa::GLTF2Import::Skin skin = context.skin(i);
                QVERIFY(compareSkins(skin, skins.at(i)));

                if (skin.inverseBindMatricesAccessorIdx > 0) {
                    QCOMPARE(skin.jointsIndices.size(), skin.inverseBindMatrices.size());
                }
            }
        }
    }
};

QTEST_APPLESS_MAIN(tst_SkinParser)

#include "tst_skinparser.moc"
