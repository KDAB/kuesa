/*
    tst_gaussianblureffect.cpp

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

#include <Kuesa/gaussianblureffect.h>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QTexture>

class tst_GaussianBlurEffect : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Kuesa::GaussianBlurEffect fx;

        // THEN
        QCOMPARE(fx.blurPassCount(), 8);
        QCOMPARE(fx.layers().size(), 1);
        QVERIFY(fx.frameGraphSubTree() != nullptr);
    }

    void checkSetBlurPassCount()
    {
        // GIVEN
        Kuesa::GaussianBlurEffect fx;
        QSignalSpy blurPassCountSpy(&fx, &Kuesa::GaussianBlurEffect::blurPassCountChanged);

        // THEN
        QVERIFY(blurPassCountSpy.isValid());

        // WHEN
        fx.setBlurPassCount(0);

        // THEN -> Nothing
        QCOMPARE(fx.blurPassCount(), 8);
        QCOMPARE(blurPassCountSpy.count(), 0);

        // WHEN
        fx.setBlurPassCount(2);

        // THEN
        QCOMPARE(fx.blurPassCount(), 2);
        QCOMPARE(blurPassCountSpy.count(), 1);

        // WHEN
        fx.setBlurPassCount(2);

        // THEN
        QCOMPARE(fx.blurPassCount(), 2);
        QCOMPARE(blurPassCountSpy.count(), 1);
    }

    void checkSetInputTexture()
    {
        // GIVEN
        Kuesa::GaussianBlurEffect fx;

        // THEN
        Kuesa::AbstractPostProcessingEffect::FrameGraphNodePtr fg = fx.frameGraphSubTree();
        Qt3DRender::QRenderTarget *rt1 = fg->findChildren<Qt3DRender::QRenderTarget *>()[0];

        QVERIFY(rt1 != nullptr);
        QCOMPARE(rt1->outputs().size(), 1);
        QCOMPARE(rt1->outputs().first()->attachmentPoint(), Qt3DRender::QRenderTargetOutput::Color0);
        QVERIFY(rt1->outputs().first()->texture() == nullptr);

        // WHEN
        Qt3DRender::QTexture2D *tex = new Qt3DRender::QTexture2D();
        fx.setInputTexture(tex);

        // THEN
        QCOMPARE(rt1->outputs().size(), 1);
        QCOMPARE(rt1->outputs().first()->attachmentPoint(), Qt3DRender::QRenderTargetOutput::Color0);
        QCOMPARE(rt1->outputs().first()->texture(), tex);
    }

    void checkSetWindowSize()
    {
        // GIVEN
        Kuesa::GaussianBlurEffect fx;

        // THEN
        Kuesa::AbstractPostProcessingEffect::FrameGraphNodePtr fg = fx.frameGraphSubTree();
        Qt3DRender::QRenderTarget *rt2 = fg->findChildren<Qt3DRender::QRenderTarget *>()[1];
        Qt3DRender::QMaterial *blurMaterial = fg->findChild<Qt3DRender::QMaterial *>();

        QVERIFY(rt2 != nullptr);
        QVERIFY(blurMaterial != nullptr);
        QCOMPARE(rt2->outputs().size(), 1);
        QCOMPARE(rt2->outputs().first()->attachmentPoint(), Qt3DRender::QRenderTargetOutput::Color0);

        Qt3DRender::QAbstractTexture *t = rt2->outputs().first()->texture();
        QVERIFY(t != nullptr);

        auto findParameter = [&](const QString &name) -> Qt3DRender::QParameter * {
            for (Qt3DRender::QParameter *p : blurMaterial->parameters())
                if (p->name() == name)
                    return p;
            return nullptr;
        };

        Qt3DRender::QParameter *wParam = findParameter(QStringLiteral("width"));
        Qt3DRender::QParameter *hParam = findParameter(QStringLiteral("height"));

        QCOMPARE(t->width(), 512);
        QCOMPARE(t->height(), 512);
        QCOMPARE(wParam->value(), QVariant(512.0f));
        QCOMPARE(hParam->value(), QVariant(512.0f));

        // WHEN
        fx.setWindowSize(QSize(1024, 768));

        // THEN
        QCOMPARE(t->width(), 1024);
        QCOMPARE(t->height(), 768);
        QCOMPARE(wParam->value(), QVariant(1024.0f));
        QCOMPARE(hParam->value(), QVariant(768.0f));
    }

    void checkFrameGraphSubTree()
    {
        // GIVEN
        Kuesa::GaussianBlurEffect fx;

        // THEN
        Kuesa::AbstractPostProcessingEffect::FrameGraphNodePtr fg = fx.frameGraphSubTree();

        Qt3DRender::QRenderTarget *rt1 = fg->findChildren<Qt3DRender::QRenderTarget *>()[0];
        Qt3DRender::QRenderTarget *rt2 = fg->findChildren<Qt3DRender::QRenderTarget *>()[1];
        Qt3DRender::QLayerFilter *layerFilter = fg->findChild<Qt3DRender::QLayerFilter *>();

        QVERIFY(rt1 != nullptr);
        QVERIFY(rt2 != nullptr);
        QVERIFY(layerFilter != nullptr);
        QCOMPARE(layerFilter->children().size(), 1);

        Qt3DRender::QFrameGraphNode *blurFGRoot = qobject_cast<Qt3DRender::QFrameGraphNode *>(layerFilter->children().first());
        QVERIFY(blurFGRoot != nullptr);

        auto checkBlurPasses = [&](int passCount) {
            QCOMPARE(blurFGRoot->children().size(), passCount * 2);

            const QObjectList &cs = blurFGRoot->children();

            for (int i = 0; i < passCount; ++i) {
                const int idx = i * 2;
                if (i == passCount - 1) { // Last Pass
                    Qt3DRender::QRenderTargetSelector *rtSA = qobject_cast<Qt3DRender::QRenderTargetSelector *>(cs[idx]);
                    QVERIFY(rtSA != nullptr);
                    QCOMPARE(rtSA->target(), rt2);

                    Qt3DRender::QRenderPassFilter *finalBlurPassToScreen = qobject_cast<Qt3DRender::QRenderPassFilter *>(cs[idx + 1]);
                    QVERIFY(finalBlurPassToScreen != nullptr);
                } else { // Intermediate pass
                    Qt3DRender::QRenderTargetSelector *rtSA = qobject_cast<Qt3DRender::QRenderTargetSelector *>(cs[idx]);
                    Qt3DRender::QRenderTargetSelector *rtSB = qobject_cast<Qt3DRender::QRenderTargetSelector *>(cs[idx + 1]);

                    QVERIFY(rtSA != nullptr);
                    QVERIFY(rtSB != nullptr);
                    QCOMPARE(rtSA->target(), rt2);
                    QCOMPARE(rtSB->target(), rt1);
                }
            }
        };

        checkBlurPasses(fx.blurPassCount());

        // WHEN
        fx.setBlurPassCount(2);

        // THEN
        checkBlurPasses(2);
    }
};

QTEST_MAIN(tst_GaussianBlurEffect)
#include "tst_gaussianblureffect.moc"
