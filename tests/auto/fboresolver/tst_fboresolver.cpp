/*
    tst_fboresolver.cpp

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
#include <Kuesa/private/fboresolver_p.h>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTexture>

using namespace Kuesa;

class tst_FBOResolver : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultState()
    {
        // GIVEN
        FBOResolver resolver;

        // THEN
        QVERIFY(resolver.source() == nullptr);
        QVERIFY(resolver.destination() == nullptr);
        QCOMPARE(resolver.yFlip(), false);
        QCOMPARE(resolver.targetHasSamples(), false);
    }

    void checkUpdatesShaderOnSampleChanges()
    {
        // GIVEN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        FBOResolver resolver;

        Qt3DRender::QShaderProgram *shader = resolver.findChild<Qt3DRender::QShaderProgram *>();

        // THEN
        QVERIFY(shader);
        QVERIFY(!resolver.targetHasSamples());

        const QByteArray fboResolverShaderCode = shader->shaderCode(Qt3DRender::QShaderProgram::Fragment);

        // WHEN
        Qt3DRender::QTexture2D tex2d;
        resolver.setSource(&tex2d);

        // THEN
        QVERIFY(!resolver.targetHasSamples());
        QCOMPARE(shader->shaderCode(Qt3DRender::QShaderProgram::Fragment), fboResolverShaderCode);

        // WHEN
        Qt3DRender::QTexture2DMultisample tex2dMSAA;
        resolver.setSource(&tex2dMSAA);

        // THEN
        QVERIFY(resolver.targetHasSamples());
        QVERIFY(shader->shaderCode(Qt3DRender::QShaderProgram::Fragment) != fboResolverShaderCode);
#endif
    }
};

QTEST_MAIN(tst_FBOResolver)

#include "tst_fboresolver.moc"
