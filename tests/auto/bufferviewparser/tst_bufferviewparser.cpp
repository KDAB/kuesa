/*
    tst_bufferviewparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Jose Casafranca <juan.casafranca@kdab.com>

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

#include <QVector>
#include <QByteArray>
#include <QJsonObject>
#include <QOpenGLContext>

#include <Kuesa/private/bufferviewsparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

class tst_BufferViewParser : public QObject
{
    Q_OBJECT
public:
    tst_BufferViewParser() {}
    ~tst_BufferViewParser() {}

private Q_SLOTS:
    void bufferViewParse_data()
    {
        QTest::addColumn<int>("bufferIdx");
        QTest::addColumn<int>("byteOffset");
        QTest::addColumn<int>("byteLength");
        QTest::addColumn<int>("byteStride");
        QTest::addColumn<QByteArray>("buffer");
        QTest::addColumn<bool>("expectedParseResult");
        QTest::addColumn<QByteArray>("expectedBuffer");

        QByteArray fullyDefinedBuffer;
        fullyDefinedBuffer.resize(128);
        fullyDefinedBuffer.fill('a');

        QByteArray fullyDefined_offset = fullyDefinedBuffer.mid(64);
        QByteArray fullyDefined_offset_short = fullyDefinedBuffer.mid(64, 32);

        QTest::newRow("fullyDefined") << 0 << 0 << 128 << 0 << fullyDefinedBuffer << true << fullyDefinedBuffer;
        QTest::newRow("fullyDefined_offset") << 0 << 64 << 64 << 0 << fullyDefinedBuffer << true << fullyDefined_offset;
        QTest::newRow("fullyDefined_offset_short") << 0 << 64 << 32 << 0 << fullyDefinedBuffer << true << fullyDefined_offset_short;
        QTest::newRow("fulleDefinedWithStride") << 0 << 0 << 128 << 4 << fullyDefinedBuffer << true << fullyDefinedBuffer;
        QTest::newRow("nonExistintBufferIdx") << 1 << 0 << 128 << 4 << fullyDefinedBuffer << false << QByteArray();
    }

    void bufferViewParse()
    {
        // GIVEN
        QFETCH(int, bufferIdx);
        QFETCH(int, byteOffset);
        QFETCH(int, byteLength);
        QFETCH(int, byteStride);
        QFETCH(QByteArray, buffer);
        QFETCH(bool, expectedParseResult);
        QFETCH(QByteArray, expectedBuffer);

        QJsonObject json;
        json["buffer"] = bufferIdx;
        json["byteOffset"] = byteOffset;
        json["byteLength"] = byteLength;
        json["byteStride"] = byteStride;

        Kuesa::GLTF2Import::GLTF2Context context;
        context.addBuffer(buffer);

        QJsonArray bufferViewsArray;
        bufferViewsArray.push_back(json);

        // WHEN
        Kuesa::GLTF2Import::BufferViewsParser parser;
        bool parseResult = parser.parse(bufferViewsArray, &context);

        // THEN
        QCOMPARE(parseResult, expectedParseResult);
        if (parseResult) {
            QCOMPARE(context.bufferViewCount(), 1);
            QCOMPARE(context.bufferView(0).bufferIdx, bufferIdx);
            QCOMPARE(context.bufferView(0).byteLength, byteLength);
            QCOMPARE(context.bufferView(0).byteOffset, byteOffset);
            QCOMPARE(context.bufferView(0).byteStride, byteStride);
        }
    }
};

QTEST_GUILESS_MAIN(tst_BufferViewParser)
#include "tst_bufferviewparser.moc"
