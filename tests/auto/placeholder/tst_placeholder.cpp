/*
    tst_placeholder.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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
#include <Qt3DCore/QTransform>
#include <Kuesa/Placeholder>
#include <Kuesa/private/kuesa_utils_p.h>
#include <Qt3DCore/private/qtransform_p.h>

class tst_Placeholder : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Qt3DCore::QEntity parent;
        Kuesa::Placeholder *placeholder = new Kuesa::Placeholder(&parent);

        // THEN
        QCOMPARE(placeholder->parentEntity(), &parent);
        QVERIFY(Kuesa::componentFromEntity<Qt3DCore::QTransform>(placeholder) != nullptr);
    }
};

QTEST_MAIN(tst_Placeholder)
#include "tst_placeholder.moc"
