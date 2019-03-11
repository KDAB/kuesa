/*
    animationplayer.h

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_ANIMATIONPLAYER_H
#define KUESA_ANIMATIONPLAYER_H

#include <Qt3DCore/QNode>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
class QClipAnimator;
class QClock;
} // namespace Qt3DAnimation

namespace Kuesa {
class SceneEntity;

class KUESASHARED_EXPORT AnimationPlayer : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Kuesa::SceneEntity *sceneEntity READ sceneEntity WRITE setSceneEntity NOTIFY sceneEntityChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged REVISION 11)
    Q_PROPERTY(QString clip READ clip WRITE setClip NOTIFY clipChanged)
    Q_PROPERTY(QString mapper READ mapper WRITE setMapper NOTIFY mapperChanged)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int loops READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)
    Q_PROPERTY(Qt3DAnimation::QClock *clock READ clock WRITE setClock NOTIFY clockChanged)
    Q_PROPERTY(float normalizedTime READ normalizedTime WRITE setNormalizedTime NOTIFY normalizedTimeChanged)
public:
    enum Loops { Infinite = -1 };
    Q_ENUM(Loops) // LCOV_EXCL_LINE

    enum Status {
        None = 0,
        Ready,
        Error
    };
    Q_ENUM(Status) // LCOV_EXCL_LINE

    explicit AnimationPlayer(Qt3DCore::QNode *parent = nullptr);
    ~AnimationPlayer();

    SceneEntity *sceneEntity() const;
    Status status() const;
    QString clip() const;
    QString mapper() const;
    bool isRunning() const;
    int loopCount() const;
    Qt3DAnimation::QClock *clock() const;
    float normalizedTime() const;

    QVector<Qt3DCore::QNode *> targets() const;
    void addTarget(Qt3DCore::QNode *target);
    void removeTarget(Qt3DCore::QNode *target);

public Q_SLOTS:
    void setSceneEntity(SceneEntity *sceneEntity);
    void setClip(const QString &clip);
    void setMapper(const QString &mapper);
    void setRunning(bool running);
    void setLoopCount(int loops);
    void setClock(Qt3DAnimation::QClock *clock);
    void setNormalizedTime(float timeFraction);

    void start();
    void stop();

Q_SIGNALS:
    void sceneEntityChanged(const SceneEntity *sceneEntity);
    void statusChanged(Status status);
    void clipChanged(const QString &clip);
    void mapperChanged(const QString &mapper);
    void runningChanged(bool running);
    void loopCountChanged(int loops);
    void clockChanged(Qt3DAnimation::QClock *clock);
    void normalizedTimeChanged(float index);

private:
    void matchClipAndTargets();
    void setStatus(Status status);
    void updateSceneFromParent(Qt3DCore::QNode *parent);

    SceneEntity *m_sceneEntity;
    Status m_status;
    QString m_clip;
    QString m_mapper;
    QVector<Qt3DCore::QNode *> m_targets;
    Qt3DAnimation::QClipAnimator *m_animator;
    bool m_running;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_ANIMATIONPLAYER_H
