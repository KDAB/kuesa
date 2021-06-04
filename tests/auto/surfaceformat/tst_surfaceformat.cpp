/*
    tst_surfaceformat.cpp

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
#include <Kuesa/kuesa_global.h>

class tst_SurfaceFormat : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultSurfaceFormat()
    {
        // GIVEN
        Kuesa::setupDefaultSurfaceFormat();

        // THEN
        const QSurfaceFormat f = QSurfaceFormat::defaultFormat();

#ifndef KUESA_OPENGL_ES_2
        QCOMPARE(f.majorVersion(), 4);
        QCOMPARE(f.minorVersion(), 6);
        QCOMPARE(f.profile(), QSurfaceFormat::CoreProfile);
#else
        QCOMPARE(f.majorVersion(), 3);
        QCOMPARE(f.minorVersion(), 0);
        QCOMPARE(f.profile(), QSurfaceFormat::NoProfile);
#endif
    }
};

QTEST_MAIN(tst_SurfaceFormat)
#include "tst_surfaceformat.moc"
