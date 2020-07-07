/*
    view3dscene.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef VIEW3DSCENE_H
#define VIEW3DSCENE_H

#include <QObject>
#include <KuesaUtils/kuesa_utils_global.h>

#include <Kuesa/sceneentity.h>
#include <Kuesa/gltf2importer.h>
#include <Kuesa/forwardrenderer.h>
#include <Kuesa/animationplayer.h>
#include <Kuesa/transformtracker.h>

#include <Qt3DAnimation/qclock.h>

#include <vector>

QT_BEGIN_NAMESPACE

namespace KuesaUtils {

class KUESAUTILS_SHARED_EXPORT View3DScene : public Kuesa::SceneEntity
{
    Q_OBJECT
    Q_PROPERTY(Kuesa::GLTF2Importer *importer READ importer CONSTANT)
    Q_PROPERTY(Kuesa::ForwardRenderer *frameGraph READ frameGraph CONSTANT)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString cameraName READ cameraName WRITE setCameraName NOTIFY cameraNameChanged)
    Q_PROPERTY(bool showDebugOverlay READ showDebugOverlay WRITE setShowDebugOverlay NOTIFY showDebugOverlayChanged)
    Q_PROPERTY(QSize screenSize READ screenSize WRITE setScreenSize NOTIFY screenSizeChanged)
public:
    explicit View3DScene(Qt3DCore::QNode *parent = nullptr);
    ~View3DScene();

    Kuesa::GLTF2Importer *importer() const { return m_importer; }
    Kuesa::ForwardRenderer *frameGraph() const { return m_frameGraph; }
    QUrl source() const;
    QString cameraName() const;
    bool showDebugOverlay() const;
    QSize screenSize() const;

    void addAnimationPlayer(Kuesa::AnimationPlayer *animation);
    void removeAnimationPlayer(Kuesa::AnimationPlayer *animation);
    void clearAnimationPlayers();

    void addTransformTracker(Kuesa::TransformTracker *tracker);
    void removeTransformTracker(Kuesa::TransformTracker *tracker);
    void clearTransformTrackers();

public Q_SLOTS:
    void setSource(const QUrl &source);
    void setCameraName(const QString &cameraName);
    void setShowDebugOverlay(bool showDebugOverlay);
    void setScreenSize(const QSize &screenSize);

    void adoptNode(QObject *object);

    void start();
    void stop();
    void gotoNormalizedTime(float time);
    void gotoStart();
    void gotoEnd();

Q_SIGNALS:
    void sourceChanged(const QUrl &source);
    void cameraNameChanged(const QString &cameraName);
    void showDebugOverlayChanged(bool showDebugOverlay);
    void screenSizeChanged(const QSize &screenSize);

private:
    void onSceneLoaded();
    void updateTrackers();

    Kuesa::GLTF2Importer *m_importer;
    Kuesa::ForwardRenderer *m_frameGraph;
    QString m_cameraName;
    std::vector<Kuesa::AnimationPlayer *> m_animations;
    std::vector<Kuesa::TransformTracker *> m_trackers;
    Qt3DAnimation::QClock *m_clock;
    QSize m_screenSize;
};

} // namespace KuesaUtils

QT_END_NAMESPACE

#endif // VIEW3D_H
