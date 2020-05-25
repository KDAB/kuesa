/*
    tst_postfxlistextension.cpp

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
#include <Kuesa/forwardrenderer.h>
#include <Kuesa/opacitymask.h>
#include <Kuesa/thresholdeffect.h>
#include <Kuesa/bloomeffect.h>
#include <QQmlComponent>
#include <QQmlEngine>

class tst_PostFXListExtension : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkExtension()
    {
        // GIVEN
        QQmlEngine engine;
        QQmlComponent cmp(&engine);

        cmp.setData(QByteArrayLiteral("import Kuesa 1.0\n\
                                       import Kuesa.Effects 1.0\n\
                                       import Qt3D.Core 2.10\n\
                                       ForwardRenderer {\n\
                                           function getEffectsCount() { return postProcessingEffects.length }\n\
                                           function getEffectAt(idx) { return postProcessingEffects[idx] }\n\
                                           function clearAndSetEffects() { postProcessingEffects = [opacityMask, bloomEffect, threshold] }\n\
                                           function clearEffects() { postProcessingEffects = [] }\n\
                                           OpacityMask { id: opacityMask }\n\
                                           BloomEffect { id: bloomEffect }\n\
                                           ThresholdEffect { id: threshold }\n\
                                           postProcessingEffects: [threshold, bloomEffect, opacityMask]\n\
                                       }"),
                    QUrl());

        // WHEN
        QObject *rootObj = cmp.create();
        Kuesa::ForwardRenderer *fg = qobject_cast<Kuesa::ForwardRenderer *>(rootObj);

        // THEN
        qDebug() << cmp.errorString();
        QVERIFY(cmp.isReady());
        QVERIFY(rootObj != nullptr);
        QVERIFY(fg != nullptr);
        QCOMPARE(fg->postProcessingEffects().size(), 3);
        QVERIFY(qobject_cast<Kuesa::ThresholdEffect *>(fg->postProcessingEffects().at(0)) != nullptr);
        QVERIFY(qobject_cast<Kuesa::BloomEffect *>(fg->postProcessingEffects().at(1)) != nullptr);
        QVERIFY(qobject_cast<Kuesa::OpacityMask *>(fg->postProcessingEffects().at(2)) != nullptr);

        {
            // WHEN
            QVariant ret;
            const bool invoke = QMetaObject::invokeMethod(rootObj, "getEffectsCount",
                                                          Q_RETURN_ARG(QVariant, ret));

            // THEN
            QVERIFY(invoke);
            QCOMPARE(ret.toInt(), 3);
        }

        {
            // WHEN
            QVariant ret;
            const bool invoke = QMetaObject::invokeMethod(rootObj, "getEffectAt",
                                                          Q_RETURN_ARG(QVariant, ret),
                                                          Q_ARG(QVariant, QVariant(0)));

            // THEN
            QVERIFY(invoke);
            QVERIFY(ret.value<Kuesa::ThresholdEffect *>() != nullptr);
        }

        {
            // WHEN
            QVariant ret;
            const bool invoke = QMetaObject::invokeMethod(rootObj, "getEffectAt",
                                                          Q_RETURN_ARG(QVariant, ret),
                                                          Q_ARG(QVariant, QVariant(1)));

            // THEN
            QVERIFY(invoke);
            QVERIFY(ret.value<Kuesa::BloomEffect *>() != nullptr);
        }

        {
            // WHEN
            QVariant ret;
            const bool invoke = QMetaObject::invokeMethod(rootObj, "getEffectAt",
                                                          Q_RETURN_ARG(QVariant, ret),
                                                          Q_ARG(QVariant, QVariant(2)));

            // THEN
            QVERIFY(invoke);
            QVERIFY(ret.value<Kuesa::OpacityMask *>() != nullptr);
        }

        {
            // WHEN
            const bool invoke = QMetaObject::invokeMethod(rootObj, "clearAndSetEffects");

            // THEN
            QVERIFY(invoke);
            QCOMPARE(fg->postProcessingEffects().size(), 3);
            QVERIFY(qobject_cast<Kuesa::OpacityMask *>(fg->postProcessingEffects().at(0)) != nullptr);
            QVERIFY(qobject_cast<Kuesa::BloomEffect *>(fg->postProcessingEffects().at(1)) != nullptr);
            QVERIFY(qobject_cast<Kuesa::ThresholdEffect *>(fg->postProcessingEffects().at(2)) != nullptr);
        }

        {
            // WHEN
            QVariant ret;
            const bool invoke = QMetaObject::invokeMethod(rootObj, "getEffectAt",
                                                          Q_RETURN_ARG(QVariant, ret),
                                                          Q_ARG(QVariant, QVariant(2)));

            // THEN
            QVERIFY(invoke);
            QVERIFY(ret.value<Kuesa::ThresholdEffect *>() != nullptr);
        }

        {
            // WHEN
            QVariant ret;
            const bool invoke = QMetaObject::invokeMethod(rootObj, "getEffectAt",
                                                          Q_RETURN_ARG(QVariant, ret),
                                                          Q_ARG(QVariant, QVariant(1)));

            // THEN
            QVERIFY(invoke);
            QVERIFY(ret.value<Kuesa::BloomEffect *>() != nullptr);
        }

        {
            // WHEN
            QVariant ret;
            const bool invoke = QMetaObject::invokeMethod(rootObj, "getEffectAt",
                                                          Q_RETURN_ARG(QVariant, ret),
                                                          Q_ARG(QVariant, QVariant(0)));

            // THEN
            QVERIFY(invoke);
            QVERIFY(ret.value<Kuesa::OpacityMask *>() != nullptr);
        }

        {
            // WHEN
            const bool invoke = QMetaObject::invokeMethod(rootObj, "clearEffects");

            // THEN
            QVERIFY(invoke);
            QCOMPARE(fg->postProcessingEffects().size(), 0);
        }
    }
};

QTEST_MAIN(tst_PostFXListExtension)

#include "tst_postfxlistextension.moc"
