/*
    tst_scenestages.cpp

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

#include <Qt3DRender/QTechniqueFilter>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QLayerFilter>

#include <Kuesa/forwardrenderer.h>
#include <Kuesa/abstractpostprocessingeffect.h>
#include <Kuesa/private/opaquerenderstage_p.h>
#include <Kuesa/private/zfillrenderstage_p.h>
#include <Kuesa/private/transparentrenderstage_p.h>
#include <Kuesa/private/scenestages_p.h>
#include <Kuesa/private/reflectionstages_p.h>

class FakeSceneFeaturedRenderStage : public Kuesa::SceneFeaturedRenderStageBase
{
    Q_OBJECT
public:
    int reconfigurations() const { return m_reconfigurations; }

    // SceneFeaturedRenderStageBase interface
private:
    void reconfigure(const Features features) override
    {
        Q_UNUSED(features);
        ++m_reconfigurations;
    }

    int m_reconfigurations = 0;
};

class tst_SceneStages : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkSceneFeaturedRenderStage()
    {
        // GIVEN
        FakeSceneFeaturedRenderStage s;

        // THEN
        QCOMPARE(s.skinning(), false);
        QCOMPARE(s.zFilling(), false);
        QCOMPARE(s.frustumCulling(), false);
        QCOMPARE(s.backToFrontSorting(), false);
        QCOMPARE(s.reconfigurations(), 0);

        // WHEN
        s.setSkinning(true);

        // THEN
        QCOMPARE(s.skinning(), true);
        QCOMPARE(s.zFilling(), false);
        QCOMPARE(s.frustumCulling(), false);
        QCOMPARE(s.backToFrontSorting(), false);
        QCOMPARE(s.reconfigurations(), 1);

        // WHEN
        s.setSkinning(false);

        // THEN
        QCOMPARE(s.skinning(), false);
        QCOMPARE(s.zFilling(), false);
        QCOMPARE(s.frustumCulling(), false);
        QCOMPARE(s.backToFrontSorting(), false);
        QCOMPARE(s.reconfigurations(), 2);

        // WHEN
        s.setZFilling(true);

        // THEN
        QCOMPARE(s.skinning(), false);
        QCOMPARE(s.zFilling(), true);
        QCOMPARE(s.frustumCulling(), false);
        QCOMPARE(s.backToFrontSorting(), false);
        QCOMPARE(s.reconfigurations(), 3);

        // WHEN
        s.setZFilling(false);

        // THEN
        QCOMPARE(s.skinning(), false);
        QCOMPARE(s.zFilling(), false);
        QCOMPARE(s.frustumCulling(), false);
        QCOMPARE(s.backToFrontSorting(), false);
        QCOMPARE(s.reconfigurations(), 4);

        // WHEN
        s.setFrustumCulling(true);

        // THEN
        QCOMPARE(s.skinning(), false);
        QCOMPARE(s.zFilling(), false);
        QCOMPARE(s.frustumCulling(), true);
        QCOMPARE(s.backToFrontSorting(), false);
        QCOMPARE(s.reconfigurations(), 5);

        // WHEN
        s.setFrustumCulling(false);

        // THEN
        QCOMPARE(s.skinning(), false);
        QCOMPARE(s.zFilling(), false);
        QCOMPARE(s.frustumCulling(), false);
        QCOMPARE(s.backToFrontSorting(), false);
        QCOMPARE(s.reconfigurations(), 6);

        // WHEN
        s.setBackToFrontSorting(true);

        // THEN
        QCOMPARE(s.skinning(), false);
        QCOMPARE(s.zFilling(), false);
        QCOMPARE(s.frustumCulling(), false);
        QCOMPARE(s.backToFrontSorting(), true);
        QCOMPARE(s.reconfigurations(), 7);

        // WHEN
        s.setBackToFrontSorting(false);

        // THEN
        QCOMPARE(s.skinning(), false);
        QCOMPARE(s.zFilling(), false);
        QCOMPARE(s.frustumCulling(), false);
        QCOMPARE(s.backToFrontSorting(), false);
        QCOMPARE(s.reconfigurations(), 8);
    }

    void checkZFillScenePass()
    {
        // GIVEN
        Kuesa::ScenePass p(Kuesa::ScenePass::ZFill);

        // THEN
        QCOMPARE(p.type(), Kuesa::ScenePass::ZFill);
        QCOMPARE(p.skinning(), false);
        QCOMPARE(p.zFilling(), false);
        QCOMPARE(p.frustumCulling(), false);
        QCOMPARE(p.backToFrontSorting(), false);
        QCOMPARE(p.cullingMode(), Qt3DRender::QCullFace::NoCulling);

        // THEN -> No skinning, no frustum culling
        {
            QCOMPARE(p.children().size(), 4);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(nonSkinnedTechniqueFilter);
            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);
            Kuesa::ZFillRenderStage *zFillStage = qobject_cast<Kuesa::ZFillRenderStage *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(zFillStage);
            QCOMPARE(zFillStage->cullingMode(), Qt3DRender::QCullFace::NoCulling);
        }

        // WHEN
        p.setSkinning(true);

        // THEN
        {
            QCOMPARE(p.children().size(), 5);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.children()[3]);
            QVERIFY(nonSkinnedTechniqueFilter);

            Qt3DRender::QTechniqueFilter *skinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(skinnedTechniqueFilter);

            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);
            Kuesa::ZFillRenderStage *zFillStageNonSkinned = qobject_cast<Kuesa::ZFillRenderStage *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(zFillStageNonSkinned);
            QCOMPARE(zFillStageNonSkinned->cullingMode(), Qt3DRender::QCullFace::NoCulling);

            QCOMPARE(skinnedTechniqueFilter->children().size(), 1);
            Kuesa::ZFillRenderStage *zFillStageSkinned = qobject_cast<Kuesa::ZFillRenderStage *>(skinnedTechniqueFilter->childNodes().first());
            QVERIFY(zFillStageSkinned);
            QCOMPARE(zFillStageSkinned->cullingMode(), Qt3DRender::QCullFace::NoCulling);
        }

        // WHEN
        p.setFrustumCulling(true);

        // THEN
        {
            QCOMPARE(p.children().size(), 5);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.children()[3]);
            QVERIFY(nonSkinnedTechniqueFilter);

            Qt3DRender::QTechniqueFilter *skinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(skinnedTechniqueFilter);

            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);
            Qt3DRender::QFrustumCulling *nonSkinnedFrustumCulling = qobject_cast<Qt3DRender::QFrustumCulling *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(nonSkinnedFrustumCulling);
            Kuesa::ZFillRenderStage *zFillStageNonSkinned = qobject_cast<Kuesa::ZFillRenderStage *>(nonSkinnedFrustumCulling->childNodes().first());
            QVERIFY(zFillStageNonSkinned);
            QCOMPARE(zFillStageNonSkinned->cullingMode(), Qt3DRender::QCullFace::NoCulling);

            QCOMPARE(skinnedTechniqueFilter->children().size(), 1);
            Kuesa::ZFillRenderStage *zFillStageSkinned = qobject_cast<Kuesa::ZFillRenderStage *>(skinnedTechniqueFilter->childNodes().first());
            QVERIFY(zFillStageSkinned);
            QCOMPARE(zFillStageSkinned->cullingMode(), Qt3DRender::QCullFace::NoCulling);
        }

        // WHEN
        p.setCullingMode(Qt3DRender::QCullFace::Back);

        // THEN
        {
            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.children()[3]);
            QVERIFY(nonSkinnedTechniqueFilter);

            Qt3DRender::QTechniqueFilter *skinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(skinnedTechniqueFilter);

            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);
            Qt3DRender::QFrustumCulling *nonSkinnedFrustumCulling = qobject_cast<Qt3DRender::QFrustumCulling *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(nonSkinnedFrustumCulling);
            Kuesa::ZFillRenderStage *zFillStageNonSkinned = qobject_cast<Kuesa::ZFillRenderStage *>(nonSkinnedFrustumCulling->childNodes().first());
            QVERIFY(zFillStageNonSkinned);
            QCOMPARE(zFillStageNonSkinned->cullingMode(), Qt3DRender::QCullFace::Back);

            QCOMPARE(skinnedTechniqueFilter->children().size(), 1);
            Kuesa::ZFillRenderStage *zFillStageSkinned = qobject_cast<Kuesa::ZFillRenderStage *>(skinnedTechniqueFilter->childNodes().first());
            QVERIFY(zFillStageSkinned);
            QCOMPARE(zFillStageSkinned->cullingMode(), Qt3DRender::QCullFace::Back);
        }
    }

    void checkOpaqueScenePass()
    {
        // GIVEN
        Kuesa::ScenePass p(Kuesa::ScenePass::Opaque);

        // THEN
        QCOMPARE(p.type(), Kuesa::ScenePass::Opaque);
        QCOMPARE(p.skinning(), false);
        QCOMPARE(p.zFilling(), false);
        QCOMPARE(p.frustumCulling(), false);
        QCOMPARE(p.backToFrontSorting(), false);
        QCOMPARE(p.cullingMode(), Qt3DRender::QCullFace::NoCulling);

        // THEN -> No ZFilling, No skinning, no frustum culling
        {
            QCOMPARE(p.children().size(), 4);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.children().last());
            QVERIFY(nonSkinnedTechniqueFilter);
            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);

            Kuesa::OpaqueRenderStage *opaqueStage = qobject_cast<Kuesa::OpaqueRenderStage *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(opaqueStage);

            QCOMPARE(opaqueStage->zFilling(), false);
            QCOMPARE(opaqueStage->cullingMode(), Qt3DRender::QCullFace::NoCulling);
        }

        // WHEN
        p.setZFilling(true);

        // THEN -> No skinning, no frustum culling
        {
            QCOMPARE(p.children().size(), 4);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.children().last());
            QVERIFY(nonSkinnedTechniqueFilter);
            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);

            Kuesa::OpaqueRenderStage *opaqueStage = qobject_cast<Kuesa::OpaqueRenderStage *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(opaqueStage);
            QCOMPARE(opaqueStage->zFilling(), true);
            QCOMPARE(opaqueStage->cullingMode(), Qt3DRender::QCullFace::NoCulling);
        }

        // WHEN
        p.setSkinning(true);

        // THEN
        {
            QCOMPARE(p.children().size(), 5);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.children()[3]);
            QVERIFY(nonSkinnedTechniqueFilter);
            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);

            Qt3DRender::QTechniqueFilter *skinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(skinnedTechniqueFilter);
            QCOMPARE(skinnedTechniqueFilter->children().size(), 1);

            Kuesa::OpaqueRenderStage *nonSkinnedOpaqueStage = qobject_cast<Kuesa::OpaqueRenderStage *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(nonSkinnedOpaqueStage);
            QCOMPARE(nonSkinnedOpaqueStage->zFilling(), true);
            QCOMPARE(nonSkinnedOpaqueStage->cullingMode(), Qt3DRender::QCullFace::NoCulling);

            Kuesa::OpaqueRenderStage *skinnedOpaqueStage = qobject_cast<Kuesa::OpaqueRenderStage *>(skinnedTechniqueFilter->childNodes().first());
            QVERIFY(skinnedOpaqueStage);
            QCOMPARE(skinnedOpaqueStage->zFilling(), true);
            QCOMPARE(skinnedOpaqueStage->cullingMode(), Qt3DRender::QCullFace::NoCulling);
        }

        // WHEN
        p.setFrustumCulling(true);

        // THEN
        {
            QCOMPARE(p.children().size(), 5);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.children()[3]);
            QVERIFY(nonSkinnedTechniqueFilter);
            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);

            Qt3DRender::QTechniqueFilter *skinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(skinnedTechniqueFilter);
            QCOMPARE(skinnedTechniqueFilter->children().size(), 1);

            Qt3DRender::QFrustumCulling *nonSkinnedFrustumCulling = qobject_cast<Qt3DRender::QFrustumCulling *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(nonSkinnedFrustumCulling);
            QCOMPARE(skinnedTechniqueFilter->children().size(), 1);

            Kuesa::OpaqueRenderStage *nonSkinnedOpaqueStage = qobject_cast<Kuesa::OpaqueRenderStage *>(nonSkinnedFrustumCulling->childNodes().first());
            QVERIFY(nonSkinnedOpaqueStage);
            QCOMPARE(nonSkinnedOpaqueStage->zFilling(), true);
            QCOMPARE(nonSkinnedOpaqueStage->cullingMode(), Qt3DRender::QCullFace::NoCulling);

            Kuesa::OpaqueRenderStage *skinnedOpaqueStage = qobject_cast<Kuesa::OpaqueRenderStage *>(skinnedTechniqueFilter->childNodes().first());
            QVERIFY(skinnedOpaqueStage);
            QCOMPARE(skinnedOpaqueStage->zFilling(), true);
            QCOMPARE(skinnedOpaqueStage->cullingMode(), Qt3DRender::QCullFace::NoCulling);
        }

        // WHEN
        p.setCullingMode(Qt3DRender::QCullFace::Back);

        // THEN
        {
            QCOMPARE(p.children().size(), 5);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.children()[3]);
            QVERIFY(nonSkinnedTechniqueFilter);
            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);

            Qt3DRender::QTechniqueFilter *skinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(skinnedTechniqueFilter);
            QCOMPARE(skinnedTechniqueFilter->children().size(), 1);

            Qt3DRender::QFrustumCulling *nonSkinnedFrustumCulling = qobject_cast<Qt3DRender::QFrustumCulling *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(nonSkinnedFrustumCulling);
            QCOMPARE(skinnedTechniqueFilter->children().size(), 1);

            Kuesa::OpaqueRenderStage *nonSkinnedOpaqueStage = qobject_cast<Kuesa::OpaqueRenderStage *>(nonSkinnedFrustumCulling->childNodes().first());
            QVERIFY(nonSkinnedOpaqueStage);
            QCOMPARE(nonSkinnedOpaqueStage->zFilling(), true);
            QCOMPARE(nonSkinnedOpaqueStage->cullingMode(), Qt3DRender::QCullFace::Back);

            Kuesa::OpaqueRenderStage *skinnedOpaqueStage = qobject_cast<Kuesa::OpaqueRenderStage *>(skinnedTechniqueFilter->childNodes().first());
            QVERIFY(skinnedOpaqueStage);
            QCOMPARE(skinnedOpaqueStage->zFilling(), true);
            QCOMPARE(skinnedOpaqueStage->cullingMode(), Qt3DRender::QCullFace::Back);
        }
    }

    void checkTransparentScenePass()
    {
        // GIVEN
        Kuesa::ScenePass p(Kuesa::ScenePass::Transparent);

        // THEN
        QCOMPARE(p.type(), Kuesa::ScenePass::Transparent);
        QCOMPARE(p.skinning(), false);
        QCOMPARE(p.zFilling(), false);
        QCOMPARE(p.frustumCulling(), false);
        QCOMPARE(p.backToFrontSorting(), false);

        // THEN -> No skinning, no frustum culling
        {
            QCOMPARE(p.children().size(), 4);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(nonSkinnedTechniqueFilter);
            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);
            Kuesa::TransparentRenderStage *transparentStage = qobject_cast<Kuesa::TransparentRenderStage *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(transparentStage);
            QCOMPARE(transparentStage->backToFrontSorting(), false);
        }

        // WHEN
        p.setBackToFrontSorting(true);

        // THEN
        {
            QCOMPARE(p.children().size(), 4);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(nonSkinnedTechniqueFilter);
            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);
            Kuesa::TransparentRenderStage *transparentStage = qobject_cast<Kuesa::TransparentRenderStage *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(transparentStage);
            QCOMPARE(transparentStage->backToFrontSorting(), true);
        }

        // WHEN
        p.setSkinning(true);

        // THEN
        {
            QCOMPARE(p.children().size(), 5);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.children()[3]);
            QVERIFY(nonSkinnedTechniqueFilter);

            Qt3DRender::QTechniqueFilter *skinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(skinnedTechniqueFilter);

            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);
            Kuesa::TransparentRenderStage *transparentNonSkinnedStage = qobject_cast<Kuesa::TransparentRenderStage *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(transparentNonSkinnedStage);
            QCOMPARE(transparentNonSkinnedStage->backToFrontSorting(), true);

            QCOMPARE(skinnedTechniqueFilter->children().size(), 1);
            Kuesa::TransparentRenderStage *transparentSkinnedStage = qobject_cast<Kuesa::TransparentRenderStage *>(skinnedTechniqueFilter->childNodes().first());
            QVERIFY(transparentSkinnedStage);
            QCOMPARE(transparentSkinnedStage->backToFrontSorting(), true);
        }

        // WHEN
        p.setFrustumCulling(true);

        // THEN
        {
            QCOMPARE(p.children().size(), 5);

            Qt3DRender::QFilterKey *forwardKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[0]);
            QVERIFY(forwardKey);
            QCOMPARE(forwardKey->name(), QStringLiteral("renderingStyle"));
            QCOMPARE(forwardKey->value(), QStringLiteral("forward"));

            Qt3DRender::QFilterKey *cullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[1]);
            QVERIFY(cullingKey);
            QCOMPARE(cullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(cullingKey->value(), true);

            Qt3DRender::QFilterKey *noCullingKey = qobject_cast<Qt3DRender::QFilterKey *>(p.children()[2]);
            QVERIFY(noCullingKey);
            QCOMPARE(noCullingKey->name(), QStringLiteral("allowCulling"));
            QCOMPARE(noCullingKey->value(), false);

            Qt3DRender::QTechniqueFilter *nonSkinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.children()[3]);
            QVERIFY(nonSkinnedTechniqueFilter);

            Qt3DRender::QTechniqueFilter *skinnedTechniqueFilter = qobject_cast<Qt3DRender::QTechniqueFilter *>(p.childNodes().last());
            QVERIFY(skinnedTechniqueFilter);

            QCOMPARE(nonSkinnedTechniqueFilter->children().size(), 1);
            Qt3DRender::QFrustumCulling *nonSkinnedFrustumCulling = qobject_cast<Qt3DRender::QFrustumCulling *>(nonSkinnedTechniqueFilter->childNodes().first());
            QVERIFY(nonSkinnedFrustumCulling);
            QCOMPARE(nonSkinnedFrustumCulling->children().size(), 1);
            Kuesa::TransparentRenderStage *transparentNonSkinnedStage = qobject_cast<Kuesa::TransparentRenderStage *>(nonSkinnedFrustumCulling->childNodes().first());
            QVERIFY(transparentNonSkinnedStage);
            QCOMPARE(transparentNonSkinnedStage->backToFrontSorting(), true);

            QCOMPARE(skinnedTechniqueFilter->children().size(), 1);
            Kuesa::TransparentRenderStage *transparentSkinnedStage = qobject_cast<Kuesa::TransparentRenderStage *>(skinnedTechniqueFilter->childNodes().first());
            QVERIFY(transparentSkinnedStage);
            QCOMPARE(transparentSkinnedStage->backToFrontSorting(), true);
        }
    }

    void checkFrameGraph()
    {
        // GIVEN
        Kuesa::SceneStages stages;

        QCOMPARE(stages.skinning(), false);
        QCOMPARE(stages.zFilling(), false);
        QCOMPARE(stages.frustumCulling(), false);
        QCOMPARE(stages.backToFrontSorting(), false);
        QVERIFY(stages.camera() == nullptr);
        QCOMPARE(stages.layers().size(), 0);
        QCOMPARE(stages.viewport(), QRectF(0.0f, 0.0f, 1.0f, 1.0f));

        // THEN -> No Layers, No ZFilling
        {
            QCOMPARE(stages.children().size(), 3);
            Qt3DRender::QViewport *vp = qobject_cast<Qt3DRender::QViewport *>(stages.children().first());
            QVERIFY(vp);

            Qt3DRender::QParameter *reflectiveEnabledParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[1]);
            QVERIFY(reflectiveEnabledParameter);
            QCOMPARE(reflectiveEnabledParameter->name(), QStringLiteral("isReflective"));
            QCOMPARE(reflectiveEnabledParameter->value(), false);

            Qt3DRender::QParameter *reflectivePlaneParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[2]);
            QVERIFY(reflectivePlaneParameter);
            QCOMPARE(reflectivePlaneParameter->name(), QStringLiteral("reflectionPlane"));
            QCOMPARE(reflectivePlaneParameter->value(), QVector4D());

            QCOMPARE(vp->children().size(), 1);
            Qt3DRender::QCameraSelector *cameraSelector = qobject_cast<Qt3DRender::QCameraSelector *>(vp->children().first());
            QVERIFY(cameraSelector);

            QCOMPARE(cameraSelector->children().size(), 2);
            Kuesa::ScenePass *opaquePass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().first());
            QVERIFY(opaquePass);
            QCOMPARE(opaquePass->type(), Kuesa::ScenePass::Opaque);

            Kuesa::ScenePass *transparentPass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().last());
            QVERIFY(transparentPass);
            QCOMPARE(transparentPass->type(), Kuesa::ScenePass::Transparent);
        }

        // WHEN
        stages.setZFilling(true);

        // THEN
        {
            QCOMPARE(stages.children().size(), 3);
            Qt3DRender::QViewport *vp = qobject_cast<Qt3DRender::QViewport *>(stages.children().first());
            QVERIFY(vp);

            Qt3DRender::QParameter *reflectiveEnabledParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[1]);
            QVERIFY(reflectiveEnabledParameter);
            QCOMPARE(reflectiveEnabledParameter->name(), QStringLiteral("isReflective"));
            QCOMPARE(reflectiveEnabledParameter->value(), false);

            Qt3DRender::QParameter *reflectivePlaneParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[2]);
            QVERIFY(reflectivePlaneParameter);
            QCOMPARE(reflectivePlaneParameter->name(), QStringLiteral("reflectionPlane"));
            QCOMPARE(reflectivePlaneParameter->value(), QVector4D());

            QCOMPARE(vp->children().size(), 1);
            Qt3DRender::QCameraSelector *cameraSelector = qobject_cast<Qt3DRender::QCameraSelector *>(vp->children().first());
            QVERIFY(cameraSelector);

            QCOMPARE(cameraSelector->children().size(), 3);
            Kuesa::ScenePass *zFillPass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().first());
            QVERIFY(zFillPass);
            QCOMPARE(zFillPass->type(), Kuesa::ScenePass::ZFill);
            QCOMPARE(zFillPass->zFilling(), true);
            QCOMPARE(zFillPass->cullingMode(), Qt3DRender::QCullFace::NoCulling);

            Kuesa::ScenePass *opaquePass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children()[1]);
            QVERIFY(opaquePass);
            QCOMPARE(opaquePass->type(), Kuesa::ScenePass::Opaque);
            QCOMPARE(opaquePass->zFilling(), true);
            QCOMPARE(opaquePass->cullingMode(), Qt3DRender::QCullFace::NoCulling);

            Kuesa::ScenePass *transparentPass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().last());
            QVERIFY(transparentPass);
            QCOMPARE(transparentPass->type(), Kuesa::ScenePass::Transparent);
            QCOMPARE(transparentPass->zFilling(), true);
            QCOMPARE(transparentPass->cullingMode(), Qt3DRender::QCullFace::NoCulling);
        }

        // WHEN
        {
            {
                Qt3DRender::QLayer layer;
                stages.addLayer(&layer);

                // THEN
                QCOMPARE(stages.children().size(), 3);
                Qt3DRender::QViewport *vp = qobject_cast<Qt3DRender::QViewport *>(stages.children().first());
                QVERIFY(vp);

                Qt3DRender::QParameter *reflectiveEnabledParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[1]);
                QVERIFY(reflectiveEnabledParameter);
                QCOMPARE(reflectiveEnabledParameter->name(), QStringLiteral("isReflective"));
                QCOMPARE(reflectiveEnabledParameter->value(), false);

                Qt3DRender::QParameter *reflectivePlaneParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[2]);
                QVERIFY(reflectivePlaneParameter);
                QCOMPARE(reflectivePlaneParameter->name(), QStringLiteral("reflectionPlane"));
                QCOMPARE(reflectivePlaneParameter->value(), QVector4D());

                QCOMPARE(vp->children().size(), 1);
                Qt3DRender::QCameraSelector *cameraSelector = qobject_cast<Qt3DRender::QCameraSelector *>(vp->children().first());
                QVERIFY(cameraSelector);

                Qt3DRender::QLayerFilter *layerFilter = qobject_cast<Qt3DRender::QLayerFilter *>(cameraSelector->children().first());
                QVERIFY(layerFilter);
                QCOMPARE(layerFilter->children().size(), 4);

                Qt3DRender::QLayer *l = qobject_cast<Qt3DRender::QLayer *>(layerFilter->children()[0]);
                QCOMPARE(l, &layer);

                Kuesa::ScenePass *zFillPass = qobject_cast<Kuesa::ScenePass *>(layerFilter->children()[1]);
                QVERIFY(zFillPass);
                QCOMPARE(zFillPass->type(), Kuesa::ScenePass::ZFill);
                QCOMPARE(zFillPass->zFilling(), true);
                QCOMPARE(zFillPass->cullingMode(), Qt3DRender::QCullFace::NoCulling);

                Kuesa::ScenePass *opaquePass = qobject_cast<Kuesa::ScenePass *>(layerFilter->children()[2]);
                QVERIFY(opaquePass);
                QCOMPARE(opaquePass->type(), Kuesa::ScenePass::Opaque);
                QCOMPARE(opaquePass->zFilling(), true);
                QCOMPARE(opaquePass->cullingMode(), Qt3DRender::QCullFace::NoCulling);

                Kuesa::ScenePass *transparentPass = qobject_cast<Kuesa::ScenePass *>(layerFilter->children().last());
                QVERIFY(transparentPass);
                QCOMPARE(transparentPass->type(), Kuesa::ScenePass::Transparent);
                QCOMPARE(transparentPass->zFilling(), true);
                QCOMPARE(transparentPass->cullingMode(), Qt3DRender::QCullFace::NoCulling);
            }

            // THEN -> layer destroyed
            QCOMPARE(stages.children().size(), 3);
            Qt3DRender::QViewport *vp = qobject_cast<Qt3DRender::QViewport *>(stages.children().first());
            QVERIFY(vp);

            Qt3DRender::QParameter *reflectiveEnabledParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[1]);
            QVERIFY(reflectiveEnabledParameter);
            QCOMPARE(reflectiveEnabledParameter->name(), QStringLiteral("isReflective"));
            QCOMPARE(reflectiveEnabledParameter->value(), false);

            Qt3DRender::QParameter *reflectivePlaneParameter = qobject_cast<Qt3DRender::QParameter *>(stages.children()[2]);
            QVERIFY(reflectivePlaneParameter);
            QCOMPARE(reflectivePlaneParameter->name(), QStringLiteral("reflectionPlane"));
            QCOMPARE(reflectivePlaneParameter->value(), QVector4D());

            QCOMPARE(vp->children().size(), 1);
            Qt3DRender::QCameraSelector *cameraSelector = qobject_cast<Qt3DRender::QCameraSelector *>(vp->children().first());
            QVERIFY(cameraSelector);

            QCOMPARE(cameraSelector->children().size(), 3);
            Kuesa::ScenePass *zFillPass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().first());
            QVERIFY(zFillPass);
            QCOMPARE(zFillPass->type(), Kuesa::ScenePass::ZFill);
            QCOMPARE(zFillPass->zFilling(), true);

            Kuesa::ScenePass *opaquePass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children()[1]);
            QVERIFY(opaquePass);
            QCOMPARE(opaquePass->type(), Kuesa::ScenePass::Opaque);
            QCOMPARE(opaquePass->zFilling(), true);

            Kuesa::ScenePass *transparentPass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().last());
            QVERIFY(transparentPass);
            QCOMPARE(transparentPass->type(), Kuesa::ScenePass::Transparent);
            QCOMPARE(transparentPass->zFilling(), true);
        }

        // WHEN
        stages.setCullingMode(Qt3DRender::QCullFace::Front);

        // THEN
        {
            QCOMPARE(stages.children().size(), 3);
            Qt3DRender::QViewport *vp = qobject_cast<Qt3DRender::QViewport *>(stages.children().first());
            QVERIFY(vp);

            QCOMPARE(vp->children().size(), 1);
            Qt3DRender::QCameraSelector *cameraSelector = qobject_cast<Qt3DRender::QCameraSelector *>(vp->children().first());
            QVERIFY(cameraSelector);

            QCOMPARE(cameraSelector->children().size(), 3);
            Kuesa::ScenePass *zFillPass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().first());
            QVERIFY(zFillPass);
            QCOMPARE(zFillPass->type(), Kuesa::ScenePass::ZFill);
            QCOMPARE(zFillPass->zFilling(), true);
            QCOMPARE(zFillPass->cullingMode(), Qt3DRender::QCullFace::Front);

            Kuesa::ScenePass *opaquePass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children()[1]);
            QVERIFY(opaquePass);
            QCOMPARE(opaquePass->type(), Kuesa::ScenePass::Opaque);
            QCOMPARE(opaquePass->zFilling(), true);
            QCOMPARE(opaquePass->cullingMode(), Qt3DRender::QCullFace::Front);

            Kuesa::ScenePass *transparentPass = qobject_cast<Kuesa::ScenePass *>(cameraSelector->children().last());
            QVERIFY(transparentPass);
            QCOMPARE(transparentPass->type(), Kuesa::ScenePass::Transparent);
            QCOMPARE(transparentPass->zFilling(), true);
            QCOMPARE(transparentPass->cullingMode(), Qt3DRender::QCullFace::NoCulling);
        }
    }
};

QTEST_MAIN(tst_SceneStages)
#include "tst_scenestages.moc"
