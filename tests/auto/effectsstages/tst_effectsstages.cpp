/*
    tst_effectsstages.cpp

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

#include <QtTest/QTest>

#include <Qt3DRender/QTechniqueFilter>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QTexture>

#include <Kuesa/forwardrenderer.h>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/private/effectsstages_p.h>

namespace {

class tst_FX : public Kuesa::AbstractPostProcessingEffect
{
    Q_OBJECT

public:
    explicit tst_FX(Qt3DCore::QNode *parent = nullptr)
        : Kuesa::AbstractPostProcessingEffect(parent)
        , m_rootNode(new Qt3DRender::QFrameGraphNode())
        , m_inputTexture(nullptr)
    {
    }

    // AbstractPostProcessingEffect interface
    FrameGraphNodePtr frameGraphSubTree() const override
    {
        return m_rootNode;
    }

    QVector<Qt3DRender::QLayer *> layers() const override
    {
        return QVector<Qt3DRender::QLayer *>();
    }

    void setInputTexture(Qt3DRender::QAbstractTexture *texture) override
    {
        m_inputTexture = texture;
    }

    Qt3DRender::QAbstractTexture *inputTexture() const
    {
        return m_inputTexture;
    }

    void setWindowSize(const QSize &size) override
    {
        emit sceneSizeChanged(size);
        m_windowSize = size;
    }

    QSize windowSize() const
    {
        return m_windowSize;
    }

Q_SIGNALS:
    void sceneSizeChanged(const QSize &size);

private:
    FrameGraphNodePtr m_rootNode;
    Qt3DRender::QAbstractTexture *m_inputTexture;
    QSize m_windowSize;
};

} // namespace

class tst_EffectsStages : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        Kuesa::EffectsStages e;

        // THEN
        std::pair<
                Qt3DRender::QRenderTarget *,
                Qt3DRender::QRenderTarget *> emptyRts = { nullptr, nullptr };
        QVERIFY(e.camera() == nullptr);
        QCOMPARE(e.renderTargets(), emptyRts);
        QVERIFY(e.depthTexture() == nullptr);
        QCOMPARE(e.effects().size(), size_t(0));
        QCOMPARE(e.presentToScreen(), false);
    }

    void testCamera()
    {
        // GIVEN
        Kuesa::EffectsStages e;

        {
            // WHEN
            Qt3DCore::QEntity c;

            e.setCamera(&c);

            // THEN
            QCOMPARE(e.camera(), &c);
        }

        // THEN
        QVERIFY(e.camera() == nullptr);
    }

    void testAddFX()
    {
        // GIVEN
        Kuesa::EffectsStages e;

        {
            tst_FX fx;

            // WHEN
            e.addEffect(&fx);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(e.effects().size(), 1U);
            QCOMPARE(e.effects().front(), &fx);
        }
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(e.effects().size(), 0U);
    }

    void testRemoveFX()
    {
        // GIVEN
        Kuesa::EffectsStages e;
        tst_FX fx;

        // WHEN
        e.addEffect(&fx);
        e.removeEffect(&fx);

        // THEN
        QCOMPARE(e.effects().size(), 0U);
    }

    void checkHandleDestructionOfRenderTargets()
    {
        // GIVEN
        Kuesa::EffectsStages e;

        // WHEN
        {
            Qt3DRender::QRenderTarget a;

            {
                Qt3DRender::QRenderTarget b;

                e.setRenderTargets(&a, &b);

                // THEN
                QVERIFY(std::get<0>(e.renderTargets()) == &a);
                QVERIFY(std::get<1>(e.renderTargets()) == &b);
            }
            // THEN
            QVERIFY(std::get<0>(e.renderTargets()) == &a);
            QVERIFY(std::get<1>(e.renderTargets()) == nullptr);
        }
        // THEN
        QVERIFY(std::get<0>(e.renderTargets()) == nullptr);
        QVERIFY(std::get<1>(e.renderTargets()) == nullptr);
    }

    void checkHandleDestructionOfDepthTexture()
    {
        // GIVEN
        Kuesa::EffectsStages e;

        // WHEN
        {
            Qt3DRender::QTexture2D depthTextures;
            e.setDepthTexture(&depthTextures);

            // THEN
            QCOMPARE(e.depthTexture(), &depthTextures);
        }
        // THEN
        QVERIFY(e.depthTexture() == nullptr);
    }

    void checkFrameGraph()
    {
        // GIVEN
        Kuesa::EffectsStages e;
        Qt3DRender::QRenderTarget rt0(&e);
        Qt3DRender::QRenderTarget rt1(&e);

        Qt3DRender::QRenderTargetOutput rt0Out;
        Qt3DRender::QRenderTargetOutput rt1Out;

        Qt3DRender::QTexture2D tex0;
        Qt3DRender::QTexture2D tex1;

        rt0Out.setTexture(&tex0);
        rt0Out.setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

        rt1Out.setTexture(&tex1);
        rt1Out.setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

        // THEN -> No FX, No RT, No Present to Screen
        {
            QCOMPARE(e.children().size(), 2);
            QCOMPARE(e.children().at(0), &rt0);
            QCOMPARE(e.children().at(1), &rt1);
        }

        // WHEN
        tst_FX fx1;
        e.addEffect(&fx1);

        // THEN -> FX(1), Not RT, No Present to Screen
        {
            QCOMPARE(e.children().size(), 2);
            QCOMPARE(e.children().at(0), &rt0);
            QCOMPARE(e.children().at(1), &rt1);
        }

        // WHEN
        e.setRenderTargets(&rt0, nullptr);

        // THEN -> FX(1), RT(1), No Present to Screen
        {
            QCOMPARE(e.children().size(), 2);
            QCOMPARE(e.children().at(0), &rt0);
            QCOMPARE(e.children().at(1), &rt1);
        }

        // WHEN
        e.setRenderTargets(&rt0, &rt1);

        // THEN -> FX(1), RT(2), No Present to Screen
        {
            QCOMPARE(e.children().size(), 3);
            QCOMPARE(e.children().at(0), &rt0);
            QCOMPARE(e.children().at(1), &rt1);
            Qt3DRender::QRenderTargetSelector *rtS = qobject_cast<Qt3DRender::QRenderTargetSelector *>(e.children().last());
            QVERIFY(rtS != nullptr);
            QCOMPARE(rtS->children().size(), 1);
            QCOMPARE(rtS->children().last(), fx1.frameGraphSubTree().data());
            QCOMPARE(rtS->target(), &rt1);
        }

        // WHEN
        e.setRenderTargets(&rt0, nullptr);
        e.setPresentToScreen(true);

        // THEN -> FX(1), RT(1), Present to Screen
        {
            QCOMPARE(e.children().size(), 3);
            QCOMPARE(e.children().at(0), &rt0);
            QCOMPARE(e.children().at(1), &rt1);
            Qt3DRender::QViewport *vp = qobject_cast<Qt3DRender::QViewport *>(e.children().last());
            QVERIFY(vp != nullptr);
            QCOMPARE(vp->children().size(), 1);
            QCOMPARE(vp->children().first(), fx1.frameGraphSubTree().data());
        }

        // WHEN
        e.setPresentToScreen(false);
        tst_FX fx2;
        e.addEffect(&fx2);

        // THEN -> FX(2), RT(1), No Present to Screen
        {
            QCOMPARE(e.children().size(), 2);
            QCOMPARE(e.children().at(0), &rt0);
            QCOMPARE(e.children().at(1), &rt1);
        }

        // WHEN
        e.setRenderTargets(&rt0, &rt1);

        // THEN -> FX(2), RT(2), No Present to Screen
        {
            QCOMPARE(e.children().size(), 4);
            QCOMPARE(e.children().at(0), &rt0);
            QCOMPARE(e.children().at(1), &rt1);
            Qt3DRender::QRenderTargetSelector *rtS0 = qobject_cast<Qt3DRender::QRenderTargetSelector *>(e.children().at(2));
            Qt3DRender::QRenderTargetSelector *rtS1 = qobject_cast<Qt3DRender::QRenderTargetSelector *>(e.children().at(3));
            QVERIFY(rtS0 != nullptr && rtS1 != nullptr);
            QCOMPARE(rtS0->children().size(), 1);
            QCOMPARE(rtS1->children().size(), 1);
            QCOMPARE(rtS0->children().first(), fx1.frameGraphSubTree().data());
            QCOMPARE(rtS1->children().first(), fx2.frameGraphSubTree().data());
            QCOMPARE(rtS0->target(), &rt1);
            QCOMPARE(rtS1->target(), &rt0);
        }

        // WHEN
        e.setPresentToScreen(true);

        // THEN -> FX(2), RT(2), Present to Screen
        {
            QCOMPARE(e.children().size(), 4);
            QCOMPARE(e.children().at(0), &rt0);
            QCOMPARE(e.children().at(1), &rt1);
            Qt3DRender::QRenderTargetSelector *rtS = qobject_cast<Qt3DRender::QRenderTargetSelector *>(e.children().at(2));
            QVERIFY(rtS != nullptr);
            QCOMPARE(rtS->children().size(), 1);
            QCOMPARE(rtS->children().first(), fx1.frameGraphSubTree().data());
            QCOMPARE(rtS->target(), &rt1);
            Qt3DRender::QViewport *vp = qobject_cast<Qt3DRender::QViewport *>(e.children().last());
            QCOMPARE(vp->children().last(), fx2.frameGraphSubTree().data());
        }

        // WHEN
        e.setWindowSize(QSize(1024, 1024));

        // THEN
        {
            QCOMPARE(fx1.windowSize(), QSize(1024, 1024));
            QCOMPARE(fx2.windowSize(), QSize(1024, 1024));
        }
    }

    void checkUnparentsFGSubtreeOnDestruction()
    {
        // GIVEN
        tst_FX fx;
        Qt3DRender::QFrameGraphNode fgRoot;
        Qt3DRender::QRenderTarget *rtA = new Qt3DRender::QRenderTarget(&fgRoot);
        Qt3DRender::QRenderTarget *rtB = new Qt3DRender::QRenderTarget(&fgRoot);

        {
            Kuesa::EffectsStages e;

            // WHEN
            e.addEffect(&fx);
            e.setRenderTargets(rtA, rtB);

            // THEN
            QVERIFY(fx.frameGraphSubTree()->parent() != nullptr);
        }

        // THEN
        QVERIFY(fx.frameGraphSubTree()->parent() == nullptr);
    }
};

QTEST_MAIN(tst_EffectsStages)
#include "tst_effectsstages.moc"
