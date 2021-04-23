/*
    animationplayer.h

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

#ifndef KUESA_ANIMATIONPLAYER_H
#define KUESA_ANIMATIONPLAYER_H

#include <Qt3DCore/QNode>
#include <Kuesa/kuesa_global.h>
#include <Kuesa/KuesaNode>
#include <Qt3DAnimation/qclock.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
class QClipAnimator;
} // namespace Qt3DAnimation

namespace Kuesa {

class KUESASHARED_EXPORT AnimationPlayer : public KuesaNode
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString clip READ clip WRITE setClip NOTIFY clipChanged)
    Q_PROPERTY(QString mapper READ mapper WRITE setMapper NOTIFY mapperChanged)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int loops READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)
    Q_PROPERTY(int currentLoop READ currentLoop NOTIFY currentLoopChanged)
    Q_PROPERTY(Qt3DAnimation::QClock *clock READ clock WRITE setClock NOTIFY clockChanged)
    Q_PROPERTY(float normalizedTime READ normalizedTime WRITE setNormalizedTime NOTIFY normalizedTimeChanged)
    Q_PROPERTY(float duration READ duration NOTIFY durationChanged)
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

    Status status() const;
    QString clip() const;
    QString mapper() const;
    bool isRunning() const;
    int loopCount() const;
    int currentLoop() const;
    Qt3DAnimation::QClock *clock() const;
    float normalizedTime() const;
    float duration() const;

    QVector<Qt3DCore::QNode *> targets() const;
    void addTarget(Qt3DCore::QNode *target);
    void removeTarget(Qt3DCore::QNode *target);

public Q_SLOTS:
    void setClip(const QString &clip);
    void setMapper(const QString &mapper);
    void setRunning(bool running);
    void setLoopCount(int loops);
    void setClock(Qt3DAnimation::QClock *clock);
    void setNormalizedTime(float timeFraction);

    void start(int delay = 0);
    void stop();
    void reset();
    void restart(int delay = 0);
    void run(float fromTimeFraction, float toTimeFraction);

Q_SIGNALS:
    void statusChanged(Kuesa::AnimationPlayer::Status status);
    void clipChanged(const QString &clip);
    void mapperChanged(const QString &mapper);
    void runningChanged(bool running);
    void loopCountChanged(int loops);
    void clockChanged(Qt3DAnimation::QClock *clock);
    void normalizedTimeChanged(float index);
    void durationChanged(float duration);
    void currentLoopChanged(int currentLoop);

private:
    void matchClipAndTargets();
    void setStatus(Status status);
    void updateSceneFromParent(Qt3DCore::QNode *parent);
    void updateNormalizedTime(float index);
    void setCurrentLoop(int loop);

    Status m_status;
    QString m_clip;
    QString m_mapper;
    QVector<Qt3DCore::QNode *> m_targets;
    Qt3DAnimation::QClipAnimator *m_animator;
    bool m_running;
    float m_runToTimeFraction = -1;
    QMetaObject::Connection m_loadingDoneConnection;
    QMetaObject::Connection m_clipDestroyedConnection;
    QMetaObject::Connection m_mapperDestroyedConnection;
    int m_currentLoop = 0;
    float m_lastKnownNormalizedTime = 0.0f;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_ANIMATIONPLAYER_H
