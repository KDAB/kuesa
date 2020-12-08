/*
    tst_fullscreenquad.cpp

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

#include <QtTest/QTest>
#include <Kuesa/FullScreenQuad>
#include <Qt3DRender/QMaterial>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QBuffer>
#else
#include <Qt3DRender/QBuffer>
#endif

using namespace Kuesa;

namespace {
struct V {
    QVector3D pos;
    QVector2D tCoord;
};
}

class tst_FullScreenQuad : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultState()
    {
        // GIVEN
        Qt3DRender::QMaterial *mat = new Qt3DRender::QMaterial();
        FullScreenQuad effect(mat);

        // THEN
        QCOMPARE(effect.viewportRect(), QRectF(0.0f, 0.0f, 1.0f, 1.0f));
        QVERIFY(effect.layer() != nullptr);
        QVERIFY(effect.buffer() != nullptr);
    }

    void checkGeneratedVertexData()
    {
        // GIVEN
        Qt3DRender::QMaterial *mat = new Qt3DRender::QMaterial();
        FullScreenQuad effect(mat);

        // THEN
        {
            const QByteArray data = effect.buffer()->data();

            QCOMPARE(data.size(), sizeof(V) * 6);

            const V* vertices = reinterpret_cast<const V *>(data.data());

            const V v0 = vertices[0];
            const V v1 = vertices[1];
            const V v2 = vertices[2];
            const V v3 = vertices[3];
            const V v4 = vertices[4];
            const V v5 = vertices[5];

            QCOMPARE(v0.pos, QVector3D(-1.0f, 1.0f, 0.0f));
            QCOMPARE(v1.pos, QVector3D(-1.0f, -1.0f, 0.0f));
            QCOMPARE(v2.pos, QVector3D(1.0f, 1.0f, 0.0f));
            QCOMPARE(v3.pos, v2.pos);
            QCOMPARE(v4.pos, v1.pos);
            QCOMPARE(v5.pos, QVector3D(1.0f, -1.0f, 0.0f));

            QCOMPARE(v0.tCoord, QVector2D(0.0f, 1.0f));
            QCOMPARE(v1.tCoord, QVector2D(0.0f, 0.0f));
            QCOMPARE(v2.tCoord, QVector2D(1.0f, 1.0f));
            QCOMPARE(v3.tCoord, v2.tCoord);
            QCOMPARE(v4.tCoord, v1.tCoord);
            QCOMPARE(v5.tCoord, QVector2D(1.0f, 0.0f));
        }

        // WHEN
        effect.setViewportRect(QRectF(0.0f, 0.0f, 0.5f, 0.5f));

        // THEN
        {
            const QByteArray data = effect.buffer()->data();

            QCOMPARE(data.size(), sizeof(V) * 6);

            const V* vertices = reinterpret_cast<const V *>(data.data());

            const V v0 = vertices[0];
            const V v1 = vertices[1];
            const V v2 = vertices[2];
            const V v3 = vertices[3];
            const V v4 = vertices[4];
            const V v5 = vertices[5];

            QCOMPARE(v0.pos, QVector3D(-1.0f, 1.0f, 0.0f));
            QCOMPARE(v1.pos, QVector3D(-1.0f, -1.0f, 0.0f));
            QCOMPARE(v2.pos, QVector3D(1.0f, 1.0f, 0.0f));
            QCOMPARE(v3.pos, v2.pos);
            QCOMPARE(v4.pos, v1.pos);
            QCOMPARE(v5.pos, QVector3D(1.0f, -1.0f, 0.0f));

            QCOMPARE(v0.tCoord, QVector2D(0.0f, 1.0f));
            QCOMPARE(v1.tCoord, QVector2D(0.0f, 0.5f));
            QCOMPARE(v2.tCoord, QVector2D(0.5f, 1.0f));
            QCOMPARE(v3.tCoord, v2.tCoord);
            QCOMPARE(v4.tCoord, v1.tCoord);
            QCOMPARE(v5.tCoord, QVector2D(0.5f, 0.5f));
        }

        // WHEN
        effect.setViewportRect(QRectF(0.5f, 0.5f, 0.5f, 0.5f));

        // THEN
        {
            const QByteArray data = effect.buffer()->data();

            QCOMPARE(data.size(), sizeof(V) * 6);

            const V* vertices = reinterpret_cast<const V *>(data.data());

            const V v0 = vertices[0];
            const V v1 = vertices[1];
            const V v2 = vertices[2];
            const V v3 = vertices[3];
            const V v4 = vertices[4];
            const V v5 = vertices[5];

            QCOMPARE(v0.pos, QVector3D(-1.0f, 1.0f, 0.0f));
            QCOMPARE(v1.pos, QVector3D(-1.0f, -1.0f, 0.0f));
            QCOMPARE(v2.pos, QVector3D(1.0f, 1.0f, 0.0f));
            QCOMPARE(v3.pos, v2.pos);
            QCOMPARE(v4.pos, v1.pos);
            QCOMPARE(v5.pos, QVector3D(1.0f, -1.0f, 0.0f));

            QCOMPARE(v0.tCoord, QVector2D(0.5f, 0.5f));
            QCOMPARE(v1.tCoord, QVector2D(0.5f, 0.0f));
            QCOMPARE(v2.tCoord, QVector2D(1.0f, 0.5f));
            QCOMPARE(v3.tCoord, v2.tCoord);
            QCOMPARE(v4.tCoord, v1.tCoord);
            QCOMPARE(v5.tCoord, QVector2D(1.0f, 0.0f));
        }
    }
};

QTEST_MAIN(tst_FullScreenQuad)

#include "tst_fullscreenquad.moc"
