/*
    tst_gltf2material.cpp

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
#include <Kuesa/gltf2material.h>
#include <Kuesa/morphcontroller.h>

using namespace Kuesa;

namespace {
template<typename PropSetter,
         typename PropGetter,
         typename PropSignal,
         typename PropType>
void testProperty(Kuesa::GLTF2Material *mat,
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

class MyGLTF2Material : public GLTF2Material
{
    Q_OBJECT
public:
    explicit MyGLTF2Material(Qt3DCore::QNode *parent = nullptr)
        : GLTF2Material(parent)
    {
    }
};

class tst_GLTF2Material : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Kuesa::MorphController *>("MorphController*");
    }

    void checkDefaultState()
    {
        // GIVEN
        MyGLTF2Material mat;

        // THEN -> Default MorphController
        QVERIFY(mat.morphController() != nullptr);
    }

    void checkMorphWeights()
    {
        // GIVEN
        MyGLTF2Material mat;
        MorphController ctrl;

        // THEN
        ::testProperty(&mat,
                       &MyGLTF2Material::setMorphController,
                       &MyGLTF2Material::morphController,
                       &MyGLTF2Material::morphControllerChanged,
                       mat.morphController(), &ctrl);
    }
};

QTEST_APPLESS_MAIN(tst_GLTF2Material)

#include "tst_gltf2material.moc"
