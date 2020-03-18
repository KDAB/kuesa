/*
    animationwidget.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#ifndef ANIMATIONWIDGET_H
#define ANIMATIONWIDGET_H

#include <QWidget>
#include <QAbstractListModel>
#include <QHash>
#include <QPixmap>

class QTimer;

class AnimationClipModel;

namespace Ui {
class AnimationWidget;
}

namespace Qt3DAnimation {
class QAbstractAnimationClip;
}

namespace Kuesa {
class AnimationPlayer;
class SceneEntity;
} // namespace Kuesa

class AnimationWidget : public QWidget
{
    Q_OBJECT
public:
    AnimationWidget(QWidget *parent = nullptr);
    virtual ~AnimationWidget();

    void update(Kuesa::SceneEntity *entity);

    void playSelectedIndex();
    void playAll();
    void stopAll();

private:
    void playAnimationAtIndex(const QModelIndex &index);
    void updateButtonStates();
    void handleClipPlayStateChange(const QString &clip, bool running);
    void stopClip(const QString &clip);
    QModelIndex selectedIndex() const;

    Ui::AnimationWidget *m_ui;
    AnimationClipModel *m_animationModel;
    QHash<QString, Kuesa::AnimationPlayer *> m_playerHash;
    Kuesa::SceneEntity *m_sceneEntity;
};

class AnimationClipModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AnimationClipModel(QObject *parent = nullptr);
    ~AnimationClipModel() override {}

    void update(Kuesa::SceneEntity *entity);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    Qt3DAnimation::QAbstractAnimationClip *clipForIndex(const QModelIndex &index) const;
    bool clipEnabled(const QModelIndex &index) const;
    void setClipPlaying(const QString &clipName, bool playing);

private:
    void updateDecorationIcon();
    void updateDuration(const QString &clipName, float duration);

    struct AnimationData {
        QString name;
        float duration = 0.f;
        bool enabled = true;
        bool playing = false;
    };

    QVector<AnimationData> m_clips;
    Kuesa::SceneEntity *m_sceneEntity;

    QTimer *iconUpdateTimer;
    QPixmap m_playingPixmap;
    QPixmap m_stoppedPixmap;
};

#endif // ANIMATIONWIDGET_H
