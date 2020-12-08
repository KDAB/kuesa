/*
    tst_texturetransform.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <cmath>

#include <Kuesa/TextureTransform>

using namespace Kuesa;

namespace {

QMatrix3x3 createMatrix(QVector2D offset, float rotation, QVector2D scale)
{
    QMatrix3x3 mat;
    QMatrix3x3 translationMat;
    translationMat(0, 2) = offset[0];
    translationMat(1, 2) = offset[1];

    QMatrix3x3 scaleMat;
    scaleMat(0, 0) = scale[0];
    scaleMat(1, 1) = scale[1];

    QMatrix3x3 rotationMat;
    rotationMat(0, 0) = std::cos(rotation);
    rotationMat(1, 0) = -std::sin(rotation);
    rotationMat(1, 1) = std::cos(rotation);
    rotationMat(0, 1) = std::sin(rotation);

    return translationMat * rotationMat * scaleMat;
}

template<typename PropSetter,
         typename PropGetter,
         typename PropSignal,
         typename PropType>
void testProperty(Kuesa::TextureTransform *textureTransform,
                  PropSetter setter,
                  PropGetter getter,
                  PropSignal signal,
                  PropType oldValue,
                  PropType newValue)
{
    QSignalSpy spy(textureTransform, signal);
    QSignalSpy spyMatrix(textureTransform, &TextureTransform::matrixChanged);
    (textureTransform->*setter)(oldValue);
    (textureTransform->*setter)(newValue);
    (textureTransform->*setter)(newValue);

    QCOMPARE((textureTransform->*getter)(), newValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().value(0).value<PropType>(), newValue);

    // Check matrix
    QMatrix3x3 expectedMatrix = createMatrix(textureTransform->offset(),
                                             textureTransform->rotation(),
                                             textureTransform->scale());
    QCOMPARE(textureTransform->matrix(), expectedMatrix);
    QCOMPARE(spyMatrix.count(), 1);
    QCOMPARE(spyMatrix.takeFirst().value(0).value<QMatrix3x3>(), expectedMatrix);
}

} // namespace

class tst_TextureTransform : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultState()
    {
        // GIVEN
        Kuesa::TextureTransform texTransform;

        // THEN
        QCOMPARE(texTransform.scale(), QVector2D(1.0, 1.0));
        QCOMPARE(texTransform.rotation(), 0.0);
        QCOMPARE(texTransform.offset(), QVector2D(0.0, 0.0));
        QCOMPARE(texTransform.matrix(), QMatrix3x3{});
    }

    void checkOffset()
    {
        // GIVEN
        Kuesa::TextureTransform texTransform;

        // THEN
        ::testProperty(&texTransform,
                       &TextureTransform::setOffset,
                       &TextureTransform::offset,
                       &TextureTransform::offsetChanged,
                       QVector2D(0.0, 0.0), QVector2D(0.2, 0.5));
    }

    void checkRotation()
    {
        // GIVEN
        Kuesa::TextureTransform texTransform;

        // THEN
        ::testProperty(&texTransform,
                       &TextureTransform::setRotation,
                       &TextureTransform::rotation,
                       &TextureTransform::rotationChanged,
                       0.0f, 1.32f);
    }

    void checkScale()
    {
        // GIVEN
        Kuesa::TextureTransform texTransform;

        // THEN
        ::testProperty(&texTransform,
                       &TextureTransform::setScale,
                       &TextureTransform::scale,
                       &TextureTransform::scaleChanged,
                       QVector2D(1.0f, 1.0f), QVector2D(0.2, 0.5));
    }
};

QTEST_APPLESS_MAIN(tst_TextureTransform)

#include "tst_texturetransform.moc"
