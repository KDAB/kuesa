/*
    tst_unlitmaterial.cpp

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

#include <Qt3DRender/QTexture>
#include <Qt3DRender/QParameter>
#include <Kuesa/unlitproperties.h>

using namespace Kuesa;

namespace {
template<typename PropSetter,
         typename PropGetter,
         typename PropSignal,
         typename PropType>
void testProperty(Kuesa::UnlitProperties *mat,
                  PropSetter setter,
                  PropGetter getter,
                  PropSignal signal,
                  PropType oldValue,
                  PropType newValue)
{
    QSignalSpy spy(mat, signal);
    (mat->*setter)(oldValue);
    (mat->*setter)(newValue);
    (mat->*setter)(newValue);

    QCOMPARE((mat->*getter)(), newValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().value(0).value<PropType>(), newValue);
}
} // namespace

class tst_UnlitProperties : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QAbstractTexture *>();
    }

    void checkDefaultState()
    {
        // GIVEN
        UnlitProperties mat;

        // THEN
        QCOMPARE(mat.isBaseColorUsingTexCoord1(), false);
        QCOMPARE(mat.baseColorFactor(), QColor("gray"));
        QCOMPARE(mat.baseColorMap(), nullptr);
    }

    void checkBaseColorUsesTexCoord1()
    {
        // GIVEN
        UnlitProperties mat;

        // THEN
        ::testProperty(&mat,
                       &UnlitProperties::setBaseColorUsesTexCoord1,
                       &UnlitProperties::isBaseColorUsingTexCoord1,
                       &UnlitProperties::baseColorUsesTexCoord1Changed,
                       false, true);
    }

    void checkBaseColorFactor()
    {
        // GIVEN
        UnlitProperties mat;

        // THEN
        ::testProperty(&mat,
                       &UnlitProperties::setBaseColorFactor,
                       &UnlitProperties::baseColorFactor,
                       &UnlitProperties::baseColorFactorChanged,
                       QColor("gray"), QColor("red"));
    }

    void checkBaseColorMap()
    {
        // GIVEN
        UnlitProperties mat;

        // THEN
        ::testProperty(&mat,
                       &UnlitProperties::setBaseColorMap,
                       &UnlitProperties::baseColorMap,
                       &UnlitProperties::baseColorMapChanged,
                       static_cast<Qt3DRender::QAbstractTexture *>(nullptr),
                       static_cast<Qt3DRender::QAbstractTexture *>(new Qt3DRender::QTexture2D()));
    }

    void checkTextureBookeeping()
    {
        // Given
        UnlitProperties mat;

        // WHEN
        {
            Qt3DRender::QTexture2D t;
            mat.setBaseColorMap(&t);

            // THEN
            QCOMPARE(mat.baseColorMap(), &t);
        }
        // THEN
        QVERIFY(mat.baseColorMap() == nullptr);
    }
};

QTEST_APPLESS_MAIN(tst_UnlitProperties)

#include "tst_unlitproperties.moc"
