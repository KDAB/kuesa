/*
    tst_empty2dtexture.cpp

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
#include <QtTest/QSignalSpy>
#include <Kuesa/private/empty2dtexture_p.h>

class tst_Empty2DTexture : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Kuesa::Empty2DTexture t;

        // THEN
        QCOMPARE(t.target(), Qt3DRender::QAbstractTexture::Target2D);
        QCOMPARE(t.format(), Qt3DRender::QAbstractTexture::RGBA8_UNorm);
        QCOMPARE(t.color(), QColor(Qt::black));
    }

    void checkChangeColor()
    {
        // GIVEN
        Kuesa::Empty2DTexture t;
        QSignalSpy colorChangedSpy(&t, &Kuesa::Empty2DTexture::colorChanged);

        // THEN
        QVERIFY(colorChangedSpy.isValid());

        // WHEN
        t.setColor(QColor(Qt::white));

        // THEN
        QCOMPARE(colorChangedSpy.count(), 1);
        QCOMPARE(t.color(), QColor(Qt::white));

        // WHEN
        t.setColor(QColor(Qt::white));

        // THEN
        QCOMPARE(colorChangedSpy.count(), 1);
        QCOMPARE(t.color(), QColor(Qt::white));
    }
};

QTEST_MAIN(tst_Empty2DTexture)
#include "tst_empty2dtexture.moc"
