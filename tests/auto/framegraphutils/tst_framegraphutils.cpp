/*
    tst_framegraphutils.cpp

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

#include <Kuesa/private/framegraphutils_p.h>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QTexture>

class tst_FrameGraphUtils : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkCreateRenderTarget_data()
    {
        QTest::addColumn<Kuesa::FrameGraphUtils::RenderTargetFlags>("flags");
        QTest::addColumn<int>("samples");

        QTest::newRow("Color No MS") << (Kuesa::FrameGraphUtils::RenderTargetFlags()) << 0;
        QTest::newRow("Color No MS float") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::HalfFloatAttachments) << 0;
        QTest::newRow("Color No MS with samples") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::RenderTargetFlags()) << 4;
        QTest::newRow("Color MS no samples") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::RenderTargetFlags()) << 0;
        QTest::newRow("Color MS with samples") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::Multisampled) << 4;
        QTest::newRow("Color MS with samples float") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::Multisampled|Kuesa::FrameGraphUtils::HalfFloatAttachments) << 4;
        QTest::newRow("Color Depth no MS") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::IncludeDepth) << 0;
        QTest::newRow("Color Stencil no MS") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::IncludeStencil) << 0;
        QTest::newRow("Color Depth Stencil no MS") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::IncludeStencil|Kuesa::FrameGraphUtils::IncludeDepth) << 0;
        QTest::newRow("Color Depth MS") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::IncludeDepth|Kuesa::FrameGraphUtils::Multisampled) << 4;
        QTest::newRow("Color Stencil MS") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::IncludeStencil|Kuesa::FrameGraphUtils::Multisampled) << 4;
        QTest::newRow("Color Depth Stencil MS") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::IncludeStencil|Kuesa::FrameGraphUtils::IncludeDepth|Kuesa::FrameGraphUtils::Multisampled) << 4;
        QTest::newRow("Color Depth MS float") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::IncludeDepth|Kuesa::FrameGraphUtils::Multisampled|Kuesa::FrameGraphUtils::HalfFloatAttachments) << 4;
        QTest::newRow("Color Stencil MS float") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::IncludeStencil|Kuesa::FrameGraphUtils::Multisampled|Kuesa::FrameGraphUtils::HalfFloatAttachments) << 4;
        QTest::newRow("Color Depth Stencil MS float") << Kuesa::FrameGraphUtils::RenderTargetFlags(Kuesa::FrameGraphUtils::IncludeStencil|Kuesa::FrameGraphUtils::IncludeDepth|Kuesa::FrameGraphUtils::Multisampled|Kuesa::FrameGraphUtils::HalfFloatAttachments) << 4;
    }

    void checkCreateRenderTarget()
    {
        // GIVEN
        QFETCH(Kuesa::FrameGraphUtils::RenderTargetFlags, flags);
        QFETCH(int, samples);

        Qt3DCore::QNode owner;

        Qt3DRender::QRenderTarget *rt = Kuesa::FrameGraphUtils::createRenderTarget(flags,
                                                                                   &owner,
                                                                                   QSize(1024, 512),
                                                                                   samples);

        // THEN
        if (flags & Kuesa::FrameGraphUtils::IncludeDepth ||
                flags & Kuesa::FrameGraphUtils::IncludeStencil) {
            QCOMPARE(rt->outputs().size(), 2);
        } else {
            QCOMPARE(rt->outputs().size(), 1);
        }

        Qt3DRender::QRenderTargetOutput *colorOutput = rt->outputs().first();
        QCOMPARE(colorOutput->attachmentPoint(), Qt3DRender::QRenderTargetOutput::Color0);
        QVERIFY(colorOutput != nullptr && colorOutput->texture() != nullptr);
        if (flags & Kuesa::FrameGraphUtils::Multisampled) {
            Qt3DRender::QTexture2DMultisample *t = qobject_cast<Qt3DRender::QTexture2DMultisample *>(colorOutput->texture());
            QCOMPARE(t->samples(), samples);
            QCOMPARE(t->generateMipMaps(), false);
            QCOMPARE(t->width(), 1024);
            QCOMPARE(t->height(), 512);
            QVERIFY(t->format() == Qt3DRender::QAbstractTexture::RGBA16F ||
                    t->format() == Qt3DRender::QAbstractTexture::RGBA8_UNorm);
        } else {
            Qt3DRender::QTexture2D *t = qobject_cast<Qt3DRender::QTexture2D *>(colorOutput->texture());
            QCOMPARE(t->generateMipMaps(), false);
            QCOMPARE(t->width(), 1024);
            QCOMPARE(t->height(), 512);
            if (flags & Kuesa::FrameGraphUtils::HalfFloatAttachments)
                QCOMPARE(t->format(), Qt3DRender::QAbstractTexture::RGBA16F);
            else
                QCOMPARE(t->format(), Qt3DRender::QAbstractTexture::RGBA8_UNorm);
        }

        const bool hasDepth = bool(flags & Kuesa::FrameGraphUtils::IncludeDepth);
        const bool hasStencil = bool(flags & Kuesa::FrameGraphUtils::IncludeStencil);
        if (hasStencil || hasDepth) {
            Qt3DRender::QRenderTargetOutput *depthStencilOutput = rt->outputs().last();
            QVERIFY(depthStencilOutput != nullptr);
            Qt3DRender::QRenderTargetOutput::AttachmentPoint expectedAttachmentPoint =
                    (hasStencil && hasDepth) ?
                        Qt3DRender::QRenderTargetOutput::DepthStencil :
                        (hasDepth) ?
                            Qt3DRender::QRenderTargetOutput::Depth :
                            Qt3DRender::QRenderTargetOutput::Stencil;

            QCOMPARE(depthStencilOutput->attachmentPoint(), expectedAttachmentPoint);

            Qt3DRender::QAbstractTexture::TextureFormat expectedFormat =
                    (hasStencil) ?
                        Qt3DRender::QAbstractTexture::D24S8 :
                        Qt3DRender::QAbstractTexture::D24;

            if (flags & Kuesa::FrameGraphUtils::Multisampled) {
                Qt3DRender::QTexture2DMultisample *t = qobject_cast<Qt3DRender::QTexture2DMultisample *>(depthStencilOutput->texture());
                QCOMPARE(t->samples(), samples);
                QCOMPARE(t->generateMipMaps(), false);
                QCOMPARE(t->width(), 1024);
                QCOMPARE(t->height(), 512);
                QCOMPARE(t->format(), expectedFormat);
            } else {
                Qt3DRender::QTexture2D *t = qobject_cast<Qt3DRender::QTexture2D *>(depthStencilOutput->texture());
                QCOMPARE(t->generateMipMaps(), false);
                QCOMPARE(t->width(), 1024);
                QCOMPARE(t->height(), 512);
                QCOMPARE(t->format(), expectedFormat);
            }
        }
    }

    void checkHasRenderTargetAttachmentOfType()
    {
        // GIVEN
        Qt3DRender::QRenderTarget *rt = Kuesa::FrameGraphUtils::createRenderTarget(Kuesa::FrameGraphUtils::IncludeDepth,
                                                                                   nullptr,
                                                                                   QSize(512, 512));

        // THEN
        QVERIFY(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(rt, Qt3DRender::QRenderTargetOutput::Color0));
        QVERIFY(Kuesa::FrameGraphUtils::renderTargetHasAttachmentOfType(rt, Qt3DRender::QRenderTargetOutput::Depth));
    }

    void checkFindRenderTargetTexture()
    {
        // GIVEN
        Qt3DRender::QRenderTarget *rt = Kuesa::FrameGraphUtils::createRenderTarget(Kuesa::FrameGraphUtils::IncludeDepth,
                                                                                   nullptr,
                                                                                   QSize(512, 512));

        // WHEN
        Qt3DRender::QAbstractTexture *colorTexture = Kuesa::FrameGraphUtils::findRenderTargetTexture(
                    rt,
                    Qt3DRender::QRenderTargetOutput::Color0);
        Qt3DRender::QAbstractTexture *depthTexture = Kuesa::FrameGraphUtils::findRenderTargetTexture(
                    rt,
                    Qt3DRender::QRenderTargetOutput::Depth);

        // THEN
        QVERIFY(colorTexture != nullptr);
        QVERIFY(depthTexture != nullptr);
        QCOMPARE(colorTexture->format(), Qt3DRender::QAbstractTexture::RGBA8_UNorm);
        QCOMPARE(depthTexture->format(), Qt3DRender::QAbstractTexture::D24);
    }

};

QTEST_MAIN(tst_FrameGraphUtils)
#include "tst_framegraphutils.moc"
