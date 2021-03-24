/*
    tst_sceneconfiguration.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Kuesa/view.h>
#include <KuesaUtils/viewconfiguration.h>
#include <Kuesa/transformtracker.h>
#include <Kuesa/placeholdertracker.h>

class tst_ViewConfiguration : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkDefaults()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration viewConfiguration;

        // THEN
        QCOMPARE(viewConfiguration.cameraName(), QString());
        QVERIFY(viewConfiguration.transformTrackers().empty());
        QVERIFY(viewConfiguration.placeholderTrackers().empty());
        QVERIFY(viewConfiguration.layerNames().empty());

        // Defaults should match the view defaults
        Kuesa::View view;
        QCOMPARE(viewConfiguration.viewportRect(), view.viewportRect());
        QCOMPARE(viewConfiguration.frustumCulling(), view.frustumCulling());
        QCOMPARE(viewConfiguration.skinning(), view.skinning());
        QCOMPARE(viewConfiguration.backToFrontSorting(), view.backToFrontSorting());
        QCOMPARE(viewConfiguration.zFilling(), view.zFilling());
        QCOMPARE(viewConfiguration.particlesEnabled(), view.particlesEnabled());
        QCOMPARE(viewConfiguration.toneMappingAlgorithm(), view.toneMappingAlgorithm());
        QCOMPARE(viewConfiguration.usesStencilMask(), view.usesStencilMask());
        QCOMPARE(viewConfiguration.exposure(), view.exposure());
        QCOMPARE(viewConfiguration.gamma(), view.gamma());
        QCOMPARE(viewConfiguration.clearColor(), Qt::black);
    }

    void checkSetViewportRect()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration v;
        QSignalSpy spy(&v, &KuesaUtils::ViewConfiguration::viewportRectChanged);

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        v.setViewportRect(QRectF(0.5f, 0.5f, 0.5f, 0.5f));

        // THEN
        QCOMPARE(v.viewportRect(), QRectF(0.5f, 0.5f, 0.5f, 0.5f));
        QCOMPARE(spy.count(), 1);

        // WHEN
        v.setViewportRect(QRectF(0.5f, 0.5f, 0.5f, 0.5f));

        // THEN
        QCOMPARE(spy.count(), 1);

        // WHEN
        v.setViewportRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f));

        // THEN
        QCOMPARE(v.viewportRect(), QRectF(0.0f, 0.0f, 1.0f, 1.0f));
        QCOMPARE(spy.count(), 2);
    }

    void checkSetFrustumCulling()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration v;
        QSignalSpy spy(&v, &KuesaUtils::ViewConfiguration::frustumCullingChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(v.frustumCulling());

        // WHEN
        v.setFrustumCulling(false);

        // THEN
        QVERIFY(!v.frustumCulling());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        v.setFrustumCulling(false);

        // THEN
        QVERIFY(!v.frustumCulling());
        QCOMPARE(spy.count(), 0);

        // WHEN
        v.setFrustumCulling(true);

        // THEN
        QVERIFY(v.frustumCulling());
        QCOMPARE(spy.count(), 1);
    }

    void testSetSkinning()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration v;
        QSignalSpy spy(&v, &KuesaUtils::ViewConfiguration::skinningChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(!v.skinning());

        // WHEN
        v.setSkinning(true);

        // THEN
        QVERIFY(v.skinning());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        v.setSkinning(true);

        // THEN
        QVERIFY(v.skinning());
        QCOMPARE(spy.count(), 0);

        // WHEN
        v.setSkinning(false);

        // THEN
        QVERIFY(!v.skinning());
        QCOMPARE(spy.count(), 1);
    }

    void testChangingBackToFrontSorting()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration view;
        QSignalSpy spy(&view, &KuesaUtils::ViewConfiguration::backToFrontSortingChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(view.backToFrontSorting());

        // WHEN
        view.setBackToFrontSorting(false);

        // THEN
        QVERIFY(!view.backToFrontSorting());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setBackToFrontSorting(false);

        // THEN
        QVERIFY(!view.backToFrontSorting());
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setBackToFrontSorting(true);

        // THEN
        QVERIFY(view.backToFrontSorting());
        QCOMPARE(spy.count(), 1);
    }

    void testChangingZFilling()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration view;
        QSignalSpy spy(&view, &KuesaUtils::ViewConfiguration::zFillingChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(!view.zFilling());

        // WHEN
        view.setZFilling(true);

        // THEN
        QVERIFY(view.zFilling());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setZFilling(true);

        // THEN
        QVERIFY(view.zFilling());
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setZFilling(false);

        // THEN
        QVERIFY(!view.zFilling());
        QCOMPARE(spy.count(), 1);
    }

    void testChangingParticlesEnabled()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration view;
        QSignalSpy spy(&view, &KuesaUtils::ViewConfiguration::particlesEnabledChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(!view.particlesEnabled());

        // WHEN
        view.setParticlesEnabled(true);

        // THEN
        QVERIFY(view.particlesEnabled());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setParticlesEnabled(true);

        // THEN
        QVERIFY(view.particlesEnabled());
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setParticlesEnabled(false);

        // THEN
        QVERIFY(!view.particlesEnabled());
        QCOMPARE(spy.count(), 1);
    }

    void testSetToneMappingAlgorithm()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration view;
        QSignalSpy spy(&view, &KuesaUtils::ViewConfiguration::toneMappingAlgorithmChanged);

        QVERIFY(spy.isValid());

        // THEN
        QCOMPARE(view.toneMappingAlgorithm(), Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping::None);

        // WHEN
        view.setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping::Filmic);

        // THEN
        QCOMPARE(view.toneMappingAlgorithm(), Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping::Filmic);
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping::Filmic);

        // THEN
        QCOMPARE(view.toneMappingAlgorithm(), Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping::Filmic);
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping::Reinhard);

        // THEN
        QCOMPARE(view.toneMappingAlgorithm(), Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping::Reinhard);
        QCOMPARE(spy.count(), 1);
    }

    void testSetUsesStencilMask()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration view;
        QSignalSpy spy(&view, &KuesaUtils::ViewConfiguration::usesStencilMaskChanged);

        QVERIFY(spy.isValid());

        // THEN
        QVERIFY(!view.usesStencilMask());

        // WHEN
        view.setUsesStencilMask(true);

        // THEN
        QVERIFY(view.usesStencilMask());
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setUsesStencilMask(true);

        // THEN
        QVERIFY(view.usesStencilMask());
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setUsesStencilMask(false);

        // THEN
        QVERIFY(!view.usesStencilMask());
        QCOMPARE(spy.count(), 1);
    }

    void testSetExposure()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration view;
        QSignalSpy spy(&view, &KuesaUtils::ViewConfiguration::exposureChanged);

        QVERIFY(spy.isValid());

        QCOMPARE(view.exposure(), 0.0);

        // WHEN
        view.setExposure(1.0);

        // THEN
        QCOMPARE(view.exposure(), 1.0);
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setExposure(1.0);

        // THEN
        QCOMPARE(view.exposure(), 1.0);
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setExposure(0.0);

        // THEN
        QCOMPARE(view.exposure(), 0.0);
        QCOMPARE(spy.count(), 1);
    }

    void testSetGamma()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration view;
        QSignalSpy spy(&view, &KuesaUtils::ViewConfiguration::gammaChanged);

        QVERIFY(spy.isValid());

        QCOMPARE(view.gamma(), 2.2f);

        // WHEN
        view.setGamma(1.0);

        // THEN
        QCOMPARE(view.gamma(), 1.0f);
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setGamma(1.0f);

        // THEN
        QCOMPARE(view.gamma(), 1.0f);
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setGamma(2.3f);

        // THEN
        QCOMPARE(view.gamma(), 2.3f);
        QCOMPARE(spy.count(), 1);
    }

    void testSetClearColor()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration view;
        QSignalSpy spy(&view, &KuesaUtils::ViewConfiguration::clearColorChanged);

        QVERIFY(spy.isValid());

        QCOMPARE(view.clearColor(), Qt::black);

        // WHEN
        view.setClearColor(Qt::blue);

        // THEN
        QCOMPARE(view.clearColor(), Qt::blue);
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        view.setClearColor(Qt::blue);

        // THEN
        QCOMPARE(view.clearColor(), Qt::blue);
        QCOMPARE(spy.count(), 0);

        // WHEN
        view.setClearColor(Qt::red);

        // THEN
        QCOMPARE(view.clearColor(), Qt::red);
        QCOMPARE(spy.count(), 1);
    }

    void checkSetCameraName()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration viewConfiguration;
        QSignalSpy cameraNameChangedSpy(&viewConfiguration, &KuesaUtils::ViewConfiguration::cameraNameChanged);

        // THEN
        QVERIFY(cameraNameChangedSpy.isValid());

        // WHEN
        const QString newCameraName = QStringLiteral("Camera_Orientation");
        viewConfiguration.setCameraName(newCameraName);

        // THEN
        QCOMPARE(viewConfiguration.cameraName(), newCameraName);
        QCOMPARE(cameraNameChangedSpy.count(), 1);
    }

    void checkTransformTrackers()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration viewConfiguration;
        QSignalSpy trackerAddedSpy(&viewConfiguration, &KuesaUtils::ViewConfiguration::transformTrackerAdded);
        QSignalSpy trackerRemovedSpy(&viewConfiguration, &KuesaUtils::ViewConfiguration::transformTrackerRemoved);

        // THEN
        QVERIFY(trackerAddedSpy.isValid());
        QVERIFY(trackerRemovedSpy.isValid());

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            viewConfiguration.addTransformTracker(&t1);
            viewConfiguration.addTransformTracker(&t2);

            // THEN
            QCOMPARE(viewConfiguration.transformTrackers().size(), size_t(2));
            QCOMPARE(trackerAddedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash and should have remove trackers
        QCOMPARE(viewConfiguration.transformTrackers().size(), size_t(0));
        QCOMPARE(trackerRemovedSpy.count(), 2);

        trackerAddedSpy.clear();
        trackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            viewConfiguration.addTransformTracker(&t1);
            viewConfiguration.addTransformTracker(&t1);

            // THEN
            QCOMPARE(viewConfiguration.transformTrackers().size(), size_t(1));
            QCOMPARE(trackerAddedSpy.count(), 1);

            //WHEN
            viewConfiguration.removeTransformTracker(&t2);

            // THEN
            QCOMPARE(viewConfiguration.transformTrackers().size(), size_t(1));
            QCOMPARE(trackerRemovedSpy.count(), 0);
        }

        // THEN -> Shouldn't crash and should have remove trackers
        QCOMPARE(viewConfiguration.transformTrackers().size(), size_t(0));
        QCOMPARE(trackerRemovedSpy.count(), 1);

        trackerAddedSpy.clear();
        trackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            viewConfiguration.addTransformTracker(&t1);
            viewConfiguration.addTransformTracker(&t2);

            // THEN
            QCOMPARE(viewConfiguration.transformTrackers().size(), size_t(2));
            QCOMPARE(trackerAddedSpy.count(), 2);

            // WHEN
            viewConfiguration.clearTransformTrackers();

            // THEN
            QCOMPARE(viewConfiguration.transformTrackers().size(), size_t(0));
            QCOMPARE(trackerRemovedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash
        trackerAddedSpy.clear();
        trackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::TransformTracker t1;
            Kuesa::TransformTracker t2;

            viewConfiguration.addTransformTracker(&t1);
            viewConfiguration.addTransformTracker(&t2);

            // THEN
            QCOMPARE(viewConfiguration.transformTrackers().size(), size_t(2));
            QCOMPARE(trackerAddedSpy.count(), 2);

            // WHEN
            viewConfiguration.removeTransformTracker(&t1);

            // THEN
            QCOMPARE(viewConfiguration.transformTrackers().size(), size_t(1));
            QCOMPARE(viewConfiguration.transformTrackers().front(), &t2);
            QCOMPARE(trackerRemovedSpy.count(), 1);
        }

        // THEN -> Shouldn't crash
        QCOMPARE(viewConfiguration.transformTrackers().size(), size_t(0));
        QCOMPARE(trackerRemovedSpy.count(), 2);

        trackerAddedSpy.clear();
        trackerRemovedSpy.clear();
    }

    void checkPlaceholderTrackers()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration viewConfiguration;
        QSignalSpy placeholderTrackerAddedSpy(&viewConfiguration, &KuesaUtils::ViewConfiguration::placeholderTrackerAdded);
        QSignalSpy placeholderTrackerRemovedSpy(&viewConfiguration, &KuesaUtils::ViewConfiguration::placeholderTrackerRemoved);

        // THEN
        QVERIFY(placeholderTrackerAddedSpy.isValid());
        QVERIFY(placeholderTrackerRemovedSpy.isValid());

        {
            // WHEN
            Kuesa::PlaceholderTracker p1;
            Kuesa::PlaceholderTracker p2;

            viewConfiguration.addPlaceholderTracker(&p1);
            viewConfiguration.addPlaceholderTracker(&p2);

            // THEN
            QCOMPARE(viewConfiguration.placeholderTrackers().size(), size_t(2));
            QCOMPARE(placeholderTrackerAddedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash and should have removed placeholderTrackers
        QCOMPARE(viewConfiguration.placeholderTrackers().size(), size_t(0));
        QCOMPARE(placeholderTrackerRemovedSpy.count(), 2);

        placeholderTrackerAddedSpy.clear();
        placeholderTrackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::PlaceholderTracker p1;
            Kuesa::PlaceholderTracker p2;

            viewConfiguration.addPlaceholderTracker(&p1);
            viewConfiguration.addPlaceholderTracker(&p1);

            // THEN
            QCOMPARE(viewConfiguration.placeholderTrackers().size(), size_t(1));
            QCOMPARE(placeholderTrackerAddedSpy.count(), 1);

            // WHEN
            viewConfiguration.removePlaceholderTracker(&p2);

            // THEN
            QCOMPARE(viewConfiguration.placeholderTrackers().size(), size_t(1));
            QCOMPARE(placeholderTrackerRemovedSpy.count(), 0);
        }

        // THEN -> Shouldn't crash and should have removed placeholderTrackers
        QCOMPARE(viewConfiguration.placeholderTrackers().size(), size_t(0));
        QCOMPARE(placeholderTrackerRemovedSpy.count(), 1);

        placeholderTrackerAddedSpy.clear();
        placeholderTrackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::PlaceholderTracker p1;
            Kuesa::PlaceholderTracker p2;

            viewConfiguration.addPlaceholderTracker(&p1);
            viewConfiguration.addPlaceholderTracker(&p2);

            // THEN
            QCOMPARE(viewConfiguration.placeholderTrackers().size(), size_t(2));
            QCOMPARE(placeholderTrackerAddedSpy.count(), 2);

            // WHEN
            viewConfiguration.clearPlaceholderTrackers();

            // THEN
            QCOMPARE(viewConfiguration.placeholderTrackers().size(), size_t(0));
            QCOMPARE(placeholderTrackerRemovedSpy.count(), 2);
        }

        // THEN -> Shouldn't crash
        placeholderTrackerAddedSpy.clear();
        placeholderTrackerRemovedSpy.clear();

        {
            // WHEN
            Kuesa::PlaceholderTracker p1;
            Kuesa::PlaceholderTracker p2;

            viewConfiguration.addPlaceholderTracker(&p1);
            viewConfiguration.addPlaceholderTracker(&p2);

            // THEN
            QCOMPARE(viewConfiguration.placeholderTrackers().size(), size_t(2));
            QCOMPARE(placeholderTrackerAddedSpy.count(), 2);

            // WHEN
            viewConfiguration.removePlaceholderTracker(&p1);

            // THEN
            QCOMPARE(viewConfiguration.placeholderTrackers().size(), size_t(1));
            QCOMPARE(viewConfiguration.placeholderTrackers().front(), &p2);
            QCOMPARE(placeholderTrackerRemovedSpy.count(), 1);
        }

        // THEN -> Shouldn't crash
        QCOMPARE(viewConfiguration.placeholderTrackers().size(), size_t(0));
        QCOMPARE(placeholderTrackerRemovedSpy.count(), 2);

        placeholderTrackerAddedSpy.clear();
        placeholderTrackerRemovedSpy.clear();
    }

    void checkLayerNames()
    {
        // GIVEN
        KuesaUtils::ViewConfiguration viewConfiguration;
        QSignalSpy layerNamesChangedSpy(&viewConfiguration, &KuesaUtils::ViewConfiguration::layerNamesChanged);

        // THEN
        QVERIFY(layerNamesChangedSpy.isValid());

        // WHEN
        const QStringList layerNames = { QStringLiteral("layer_1"), QStringLiteral("layer_2") };
        viewConfiguration.setLayerNames(layerNames);

        // THEN
        QCOMPARE(viewConfiguration.layerNames(), layerNames);
        QCOMPARE(layerNamesChangedSpy.count(), 1);

        // WHEN
        viewConfiguration.setLayerNames(layerNames);

        // THEN -> Signal not emitted again
        QCOMPARE(viewConfiguration.layerNames(), layerNames);
        QCOMPARE(layerNamesChangedSpy.count(), 1);
    }
};

QTEST_MAIN(tst_ViewConfiguration)
#include "tst_viewconfiguration.moc"
