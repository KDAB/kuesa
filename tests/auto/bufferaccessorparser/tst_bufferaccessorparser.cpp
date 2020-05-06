/*
    tst_bufferaccessorparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QtTest>

#include <QVector>
#include <QByteArray>
#include <QJsonObject>
#include <QOpenGLContext>

#include <Kuesa/private/bufferaccessorparser_p.h>
#include <Kuesa/private/gltf2context_p.h>

using namespace Qt3DGeometry;

class tst_BufferAccessorParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void shouldHaveDefaultStateForAccessors()
    {
        // GIVEN
        QJsonObject json;
        json["componentType"] = GL_FLOAT;
        json["type"] = QStringLiteral("SCALAR");
        json["count"] = 0;
        json["bufferView"] = 0;
        QJsonArray accessorsArray;
        accessorsArray.push_back(json);

        Kuesa::GLTF2Import::GLTF2Context context;
        Kuesa::GLTF2Import::BufferAccessorParser parser;

        // WHEN
        bool result = parser.parse(accessorsArray, &context);

        // THEN
        QCOMPARE(result, true);
        QCOMPARE(context.accessorCount(), 1);

        const Kuesa::GLTF2Import::Accessor accessor = context.accessor(0);
        QCOMPARE(accessor.bufferViewIndex, 0);
        QCOMPARE(accessor.type, QAttribute::Float);
        QCOMPARE(accessor.count, 0U);
        QCOMPARE(accessor.dataSize, 1);
        QCOMPARE(accessor.offset, 0U);
        QCOMPARE(accessor.normalized, false);
        QCOMPARE(accessor.max.size(), 0);
        QCOMPARE(accessor.min.size(), 0);
    }

    void shouldFailWhenDoesntHaveRequiredProperties_data()
    {
        QTest::addColumn<QJsonObject>("json");

        {
            QTest::newRow("empty") << QJsonObject();
        }

        {
            // WHEN
            QJsonObject json;
            json["componentType"] = GL_FLOAT;
            QTest::newRow("componentType_only") << json;
        }

        {
            // WHEN
            QJsonObject json;
            json["type"] = "VEC2";
            QTest::newRow("type_only") << json;
        }

        {
            // WHEN
            QJsonObject json;
            json["count"] = 128;
            QTest::newRow("count_only") << json;
        }

        {
            // WHEN
            QJsonObject json;
            json["count"] = 128;
            json["type"] = "VEC2";
            QTest::newRow("count_and_type_only") << json;
        }

        {
            // WHEN
            QJsonObject json;
            json["count"] = 128;
            json["componentType"] = GL_FLOAT;
            QTest::newRow("count_and_componentType_only") << json;
        }

        {
            // WHEN
            QJsonObject json;
            json["type"] = 128;
            json["componentType"] = GL_FLOAT;
            QTest::newRow("type_and_componentType_only") << json;
        }

        {
            // WHEN
            QJsonObject json;
            json["count"] = 128;
            json["type"] = 128;
            json["componentType"] = GL_FLOAT;
            QTest::newRow("count_and_type_and_componentType_only") << json;
        }
    }

    void shouldFailWhenDoesntHaveRequiredProperties()
    {
        // GIVEN
        QFETCH(QJsonObject, json);
        Kuesa::GLTF2Import::GLTF2Context context;
        Kuesa::GLTF2Import::BufferAccessorParser parser;

        // WHEN
        QJsonArray accessorsArray;
        accessorsArray.push_back(json);
        bool result = parser.parse(accessorsArray, &context);

        // THEN
        QCOMPARE(result, false);
        QCOMPARE(context.accessorCount(), 0);
    }

    void shouldHandleComponentTypes_data()
    {
        QTest::addColumn<int>("componentType");
        QTest::addColumn<QAttribute::VertexBaseType>("expectedType");

        QTest::addRow("byte") << GL_BYTE << QAttribute::Byte;
        QTest::addRow("unsignedByte") << GL_UNSIGNED_BYTE << QAttribute::UnsignedByte;
        QTest::addRow("short") << GL_SHORT << QAttribute::Short;
        QTest::addRow("unsignedShort") << GL_UNSIGNED_SHORT << QAttribute::UnsignedShort;
        QTest::addRow("unsiginedInt") << GL_UNSIGNED_INT << QAttribute::UnsignedInt;
        QTest::addRow("float") << GL_FLOAT << QAttribute::Float;
        QTest::addRow("invalid") << 0 << QAttribute::Float;
    }

    void shouldHandleComponentTypes()
    {
        // GIVEN
        QFETCH(int, componentType);
        QFETCH(QAttribute::VertexBaseType, expectedType);

        QJsonObject json;
        json["componentType"] = componentType;
        json["type"] = QStringLiteral("SCALAR");
        json["count"] = 0;
        json["bufferView"] = 0;
        QJsonArray accessorArray;
        accessorArray.push_back(json);

        Kuesa::GLTF2Import::GLTF2Context context;
        Kuesa::GLTF2Import::BufferAccessorParser parser;

        // WHEN
        bool result = parser.parse(accessorArray, &context);

        // THEN
        QCOMPARE(result, true);
        QCOMPARE(context.accessorCount(), 1);
        QCOMPARE(context.accessor(0).type, expectedType);
    }

    void shouldProvideDataSize_data()
    {
        QTest::addColumn<QString>("dataType");
        QTest::addColumn<int>("expectedDataSize");

        QTest::addRow("SCALAR") << "SCALAR" << 1;
        QTest::addRow("VEC2") << "VEC2" << 2;
        QTest::addRow("VEC3") << "VEC3" << 3;
        QTest::addRow("VEC4") << "VEC4" << 4;
        QTest::addRow("MAT2") << "MAT2" << 4;
        QTest::addRow("MAT3") << "MAT3" << 9;
        QTest::addRow("MAT4") << "MAT4" << 16;
        QTest::addRow("Invalid") << "INVALID" << 0;
    }

    void shouldProvideDataSize()
    {
        // GIVEN
        QFETCH(QString, dataType);
        QFETCH(int, expectedDataSize);

        QJsonObject json;
        json["componentType"] = GL_FLOAT;
        json["type"] = dataType;
        json["count"] = 0;
        json["bufferView"] = 0;
        QJsonArray accessorArray;
        accessorArray.push_back(json);

        Kuesa::GLTF2Import::GLTF2Context context;
        Kuesa::GLTF2Import::BufferAccessorParser parser;

        // WHEN
        bool result = parser.parse(accessorArray, &context);

        // THEN
        QCOMPARE(result, true);
        QCOMPARE(context.accessorCount(), 1);
        QCOMPARE(context.accessor(0).dataSize, expectedDataSize);
    }

    void shouldCreateAccessorsWithProperties()
    {
        // GIVEN
        QJsonObject firstAccessorJson;
        QJsonObject secondAccessorJson;
        firstAccessorJson["componentType"] = GL_FLOAT;
        firstAccessorJson["type"] = "VEC2";
        firstAccessorJson["normalized"] = true;
        firstAccessorJson["bufferView"] = 1;
        firstAccessorJson["count"] = 128;
        firstAccessorJson["byteOffset"] = 32;
        QJsonArray max;
        max.push_back(1);
        max.push_back(1);
        QJsonArray min;
        min.push_back(0);
        min.push_back(0);
        firstAccessorJson["max"] = max;
        firstAccessorJson["min"] = min;

        secondAccessorJson["componentType"] = GL_FLOAT;
        secondAccessorJson["type"] = "VEC2";
        secondAccessorJson["normalized"] = true;
        secondAccessorJson["bufferView"] = 1;
        secondAccessorJson["count"] = 128;
        secondAccessorJson["byteOffset"] = 32;
        secondAccessorJson["max"] = max;
        secondAccessorJson["min"] = min;
        QJsonArray accessorArray;
        accessorArray.push_back(firstAccessorJson);
        accessorArray.push_back(secondAccessorJson);

        Kuesa::GLTF2Import::GLTF2Context context;
        Kuesa::GLTF2Import::BufferAccessorParser parser;

        // WHEN
        bool result = parser.parse(accessorArray, &context);

        // THEN
        QCOMPARE(result, true);
        QCOMPARE(context.accessorCount(), 2);
        const Kuesa::GLTF2Import::Accessor firstAccessor = context.accessor(0);
        const Kuesa::GLTF2Import::Accessor secondAccessor = context.accessor(1);
        QCOMPARE(firstAccessor.dataSize, 2);
        QCOMPARE(firstAccessor.bufferViewIndex, 1);
        QCOMPARE(firstAccessor.count, 128U);
        QCOMPARE(firstAccessor.type, QAttribute::Float);
        QCOMPARE(firstAccessor.offset, 32U);
        QCOMPARE(firstAccessor.max.size(), 2);
        QCOMPARE(firstAccessor.min.size(), 2);
        QCOMPARE(firstAccessor.max.at(0), 1);
        QCOMPARE(firstAccessor.max.at(1), 1);
        QCOMPARE(firstAccessor.min.at(0), 0);
        QCOMPARE(firstAccessor.min.at(1), 0);
        QCOMPARE(firstAccessor.bufferViewIndex, secondAccessor.bufferViewIndex);
        QCOMPARE(firstAccessor.count, secondAccessor.count);
        QCOMPARE(firstAccessor.dataSize, secondAccessor.dataSize);
        QCOMPARE(firstAccessor.normalized, secondAccessor.normalized);
        QCOMPARE(firstAccessor.offset, secondAccessor.offset);
        QCOMPARE(firstAccessor.type, secondAccessor.type);
        QCOMPARE(firstAccessor.max, secondAccessor.max);
        QCOMPARE(firstAccessor.min, secondAccessor.min);
    }

    void shouldCreateAccessorsWithCorrectDefaultValues()
    {
        // GIVEN
        QJsonObject json;
        // Mandatory
        json["componentType"] = GL_FLOAT;
        json["type"] = "VEC2";
        json["count"] = 128;
        json["bufferView"] = 0;

        Kuesa::GLTF2Import::GLTF2Context context;
        Kuesa::GLTF2Import::BufferAccessorParser parser;

        // WHEN
        QJsonArray accessorArray;
        accessorArray.push_back(json);
        bool result = parser.parse(accessorArray, &context);

        // THEN
        QCOMPARE(result, true);
        QCOMPARE(context.accessorCount(), 1);

        const Kuesa::GLTF2Import::Accessor firstAccessor = context.accessor(0);
        QCOMPARE(firstAccessor.type, QAttribute::Float);
        QCOMPARE(firstAccessor.dataSize, 2);
        QCOMPARE(firstAccessor.count, 128U);

        QCOMPARE(firstAccessor.normalized, false);
        QCOMPARE(firstAccessor.offset, 0U);
        QCOMPARE(firstAccessor.max.size(), 0);
        QCOMPARE(firstAccessor.min.size(), 0);
    }
};

QTEST_GUILESS_MAIN(tst_BufferAccessorParser)
#include "tst_bufferaccessorparser.moc"
