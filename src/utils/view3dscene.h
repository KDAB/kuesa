/*
    view3dscene.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#ifndef KUESA_KUESA_UTILS_VIEW3DSCENE_H
#define KUESA_KUESA_UTILS_VIEW3DSCENE_H

#include <QObject>
#include <QPointer>
#include <KuesaUtils/kuesa_utils_global.h>
#include <KuesaUtils/sceneconfiguration.h>

#include <Kuesa/sceneentity.h>
#include <Kuesa/gltf2importer.h>
#include <Kuesa/forwardrenderer.h>
#include <Kuesa/animationplayer.h>
#include <Kuesa/transformtracker.h>

#include <Qt3DAnimation/qclock.h>
#include <Qt3DLogic/qframeaction.h>
#include <Qt3DRender/qrendersettings.h>

#include <vector>

QT_BEGIN_NAMESPACE

namespace KuesaUtils {

class KUESAUTILS_SHARED_EXPORT View3DScene : public Kuesa::SceneEntity
{
    Q_OBJECT
    Q_PROPERTY(Kuesa::GLTF2Importer *importer READ importer CONSTANT)
    Q_PROPERTY(Kuesa::ForwardRenderer *frameGraph READ frameGraph CONSTANT)
    Q_PROPERTY(Qt3DRender::QRenderSettings *renderSettings READ renderSettings CONSTANT)
    Q_PROPERTY(QUrl source READ source NOTIFY sourceChanged)
    Q_PROPERTY(bool showDebugOverlay READ showDebugOverlay WRITE setShowDebugOverlay NOTIFY showDebugOverlayChanged)
    Q_PROPERTY(QSize screenSize READ screenSize WRITE setScreenSize NOTIFY screenSizeChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
    Q_PROPERTY(KuesaUtils::SceneConfiguration *activeScene READ activeScene WRITE setActiveScene NOTIFY activeSceneChanged)
    Q_PROPERTY(QString reflectionPlaneName READ reflectionPlaneName WRITE setReflectionPlaneName NOTIFY reflectionPlaneNameChanged)

public:
    explicit View3DScene(Qt3DCore::QNode *parent = nullptr);
    ~View3DScene();

    Kuesa::GLTF2Importer *importer() const { return m_importer; }
    Kuesa::ForwardRenderer *frameGraph() const { return m_frameGraph; }
    Qt3DRender::QRenderSettings *renderSettings() const { return m_renderSettings; }
    QUrl source() const;
    bool showDebugOverlay() const;
    QSize screenSize() const;
    bool asynchronous() const;
    QString reflectionPlaneName() const;

    const std::vector<Kuesa::AnimationPlayer *> &animationPlayers() const;

    SceneConfiguration *activeScene() const;

    bool isReady() const;
    bool isLoaded() const;

public Q_SLOTS:
    void setShowDebugOverlay(bool showDebugOverlay);
    void setScreenSize(const QSize &screenSize);
    void setAsynchronous(bool asynchronous);
    void setReflectionPlaneName(const QString &reflectionPlaneName);

    void setActiveScene(SceneConfiguration *scene);

    void adoptNode(QObject *object);

    // Animation Controls
    void start();
    void restart();
    void stop();
    void gotoNormalizedTime(float time);
    void gotoStart();
    void gotoEnd();

Q_SIGNALS:
    void sourceChanged(const QUrl &source);
    void cameraNameChanged(const QString &cameraName);
    void showDebugOverlayChanged(bool showDebugOverlay);
    void screenSizeChanged(const QSize &screenSize);
    void readyChanged(bool ready);
    void loadedChanged(bool loaded);
    void asynchronousChanged(bool asynchronous);
    void activeSceneChanged(SceneConfiguration *activeScene);
    void reflectionPlaneNameChanged(const QString &reflectionPlaneName);

private:
    struct ViewConfigurationResources {
        KuesaUtils::ViewConfiguration *viewConfiguration;
        Kuesa::View *view;
        std::vector<QMetaObject::Connection> connections;
    };

    void setSource(const QUrl &source);
    void retrieveAndSetCamera(QString cameraName, Kuesa::View *view);
    void retrieveAndSetLayers(QStringList layers, Kuesa::View *view);

    void addAnimationPlayer(Kuesa::AnimationPlayer *animation);
    void removeAnimationPlayer(Kuesa::AnimationPlayer *animation);
    void clearAnimationPlayers();

    void addTransformTracker(Kuesa::TransformTracker *tracker);
    void removeTransformTracker(Kuesa::TransformTracker *tracker);
    void clearTransformTrackers();

    void addPlaceholderTracker(Kuesa::PlaceholderTracker *placeholder);
    void removePlaceholderTracker(Kuesa::PlaceholderTracker *placeholder);
    void clearPlaceholderTrackers();

    void addViewConfiguration(KuesaUtils::ViewConfiguration *viewConfiguration);
    void removeViewConfiguration(KuesaUtils::ViewConfiguration *viewConfiguration);
    void clearViewConfigurations();

    void onSceneLoaded();
    void updateTransformTrackers(const std::vector<Kuesa::TransformTracker *> &transformTrackers, Kuesa::View *view);
    void updatePlaceholderTrackers(const std::vector<Kuesa::PlaceholderTracker *> &placeholderTrackers, Kuesa::View *view);
    void updateFrame(float dt);
    void loadReflections();

    Kuesa::GLTF2Importer *m_importer;
    Kuesa::ForwardRenderer *m_frameGraph;
    Qt3DRender::QRenderSettings *m_renderSettings;
    std::vector<Kuesa::AnimationPlayer *> m_animations;
    std::vector<ViewConfigurationResources> m_viewConfigurationsResources;
    QSize m_screenSize;
    SceneConfiguration *m_activeScene;
    QPointer<QObject> m_activeSceneOwner;

    bool m_ready;
    QString m_reflectionPlaneName;
    int m_frameCount;
    Qt3DLogic::QFrameAction *m_frameAction;
};

} // namespace KuesaUtils

QT_END_NAMESPACE

#endif // KUESA_KUESA_UTILS_VIEW3DSCENE_H
