/*
    tst_texturecollection.cpp

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

#include <QtTest/QtTest>

#include <Kuesa/texturecollection.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DCore/qentity.h>

class tst_TextureCollection : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void shouldCreateAssets()
    {
        // GIVEN
        Kuesa::TextureCollection collection;

        // WHEN
        auto loader = collection.add("loader", QUrl("foo://bar"));

        // THEN
        QVERIFY(loader);
        QCOMPARE(collection.names().size(), 1);
        QCOMPARE(collection.find("loader"), loader);
    }
};

QTEST_GUILESS_MAIN(tst_TextureCollection)
#include "tst_texturecollection.moc"
