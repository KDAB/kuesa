/*
    tst_gltf2material.cpp

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

#include <Qt3DRender/QTexture>
#include <Kuesa/gltf2materialproperties.h>
#include <Kuesa/morphcontroller.h>

using namespace Kuesa;

namespace {
template<typename PropSetter,
         typename PropGetter,
         typename PropSignal,
         typename PropType>
void testProperty(Kuesa::GLTF2MaterialProperties *mat,
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

class MyGLTF2MaterialProperties : public GLTF2MaterialProperties
{
    Q_OBJECT
public:
    explicit MyGLTF2MaterialProperties(Qt3DCore::QNode *parent = nullptr)
        : GLTF2MaterialProperties(parent)
    {}

    // GLTF2MaterialProperties interface
public:
    Qt3DRender::QShaderData *shaderData() const override { return nullptr; }
};

class tst_GLTF2MaterialProperties : public QObject
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
        MyGLTF2MaterialProperties mat;

        // THEN
        QCOMPARE(mat.textureTransform(), QMatrix3x3{});
        QCOMPARE(mat.isBaseColorUsingTexCoord1(), false);
        QCOMPARE(mat.alphaCutoff(), 0.0f);
        QCOMPARE(mat.baseColorFactor(), QColor("gray"));
        QVERIFY(mat.baseColorMap() == nullptr);
    }

    void checkTextureTransform()
    {
        // GIVEN
        MyGLTF2MaterialProperties mat;

        QMatrix3x3 matrix;
        matrix.fill(5.0);

        // THEN
        ::testProperty(&mat,
                       &MyGLTF2MaterialProperties::setTextureTransform,
                       &MyGLTF2MaterialProperties::textureTransform,
                       &MyGLTF2MaterialProperties::textureTransformChanged,
                       QMatrix3x3{}, matrix);
    }

    void checkAlphaCutoff()
    {
        // GIVEN
        MyGLTF2MaterialProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MyGLTF2MaterialProperties::setAlphaCutoff,
                       &MyGLTF2MaterialProperties::alphaCutoff,
                       &MyGLTF2MaterialProperties::alphaCutoffChanged,
                       0.0f, 0.5f);
    }

    void checkBaseColorFactor()
    {
        // GIVEN
        MyGLTF2MaterialProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MyGLTF2MaterialProperties::setBaseColorFactor,
                       &MyGLTF2MaterialProperties::baseColorFactor,
                       &MyGLTF2MaterialProperties::baseColorFactorChanged,
                       QColor("gray"), QColor(Qt::red));
    }

    void checbBaseColorMap()
    {
        // GIVEN
        MyGLTF2MaterialProperties mat;

        // THEN
        Qt3DRender::QAbstractTexture *t = new Qt3DRender::QTexture2D();
        ::testProperty(&mat,
                       &MyGLTF2MaterialProperties::setBaseColorMap,
                       &MyGLTF2MaterialProperties::baseColorMap,
                       &MyGLTF2MaterialProperties::baseColorMapChanged,
                       static_cast<Qt3DRender::QAbstractTexture*>(nullptr),
                       t);
    }

    void checbBaseColorMapUsesTexCoord1()
    {
        // GIVEN
        MyGLTF2MaterialProperties mat;

        // THEN
        ::testProperty(&mat,
                       &MyGLTF2MaterialProperties::setBaseColorUsesTexCoord1,
                       &MyGLTF2MaterialProperties::isBaseColorUsingTexCoord1,
                       &MyGLTF2MaterialProperties::baseColorUsesTexCoord1Changed,
                       false, true);
    }

    void checkTextureBookeeping()
    {
        // Given
        MyGLTF2MaterialProperties mat;

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

QTEST_APPLESS_MAIN(tst_GLTF2MaterialProperties)

#include "tst_gltf2materialproperties.moc"
