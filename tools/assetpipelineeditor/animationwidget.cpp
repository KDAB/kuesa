/*
    animationwidget.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "animationwidget.h"
#include "ui_animationwidget.h"
#include <Kuesa/AnimationClipCollection>
#include <Kuesa/AnimationPlayer>
#include <Kuesa/SceneEntity>

#include <QAnimationClip>
#include <QChannelMapper>
#include <QPainter>
#include <QTimer>

AnimationWidget::AnimationWidget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::AnimationWidget)
    , m_animationModel(nullptr)
    , m_sceneEntity(nullptr)
{
    m_ui->setupUi(this);

    m_animationModel = new AnimationClipModel(this);

    m_ui->animationListView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->animationListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->animationListView->setModel(m_animationModel);
    m_ui->animationListView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_ui->animationListView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    connect(m_ui->animationListView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AnimationWidget::updateButtonStates);
    connect(m_ui->animationListView, &QAbstractItemView::doubleClicked, this, &AnimationWidget::playSelectedIndex);
    connect(m_ui->playButton, &QAbstractButton::clicked, this, &AnimationWidget::playSelectedIndex);
    connect(m_ui->playAllButton, &QAbstractButton::clicked, this, &AnimationWidget::playAll);
    connect(m_ui->stopButton, &QAbstractButton::clicked, this, &AnimationWidget::stopAll);

    updateButtonStates();
}

AnimationWidget::~AnimationWidget()
{
    stopAll();
    delete m_ui;
}

void AnimationWidget::update(Kuesa::SceneEntity *entity)
{
    stopAll();
    m_animationModel->update(entity);
    m_sceneEntity = entity;
    if (m_animationModel->rowCount() > 0 && !selectedIndex().isValid()) {
        const auto firstIndex = m_animationModel->index(0, 0);
        m_ui->animationListView->selectionModel()->setCurrentIndex(firstIndex, QItemSelectionModel::ClearAndSelect);
    }

    updateButtonStates();
}

void AnimationWidget::playSelectedIndex()
{
    const auto index = selectedIndex();
    // first stop the clip if it's already playing (but let any other clips continue to play)
    stopClip(index.data(Qt::DisplayRole).toString());
    playAnimationAtIndex(index);
}

void AnimationWidget::playAll()
{
    stopAll();
    const int clipCount = m_animationModel->rowCount();
    for (int row = 0; row < clipCount; ++row)
        playAnimationAtIndex(m_animationModel->index(row, 0));
}

void AnimationWidget::stopAll()
{
    const auto names = m_playerHash.keys();
    for (auto clipName : names)
        stopClip(clipName);
    m_playerHash.clear();
    updateButtonStates();
}

void AnimationWidget::stopClip(const QString &clip)
{
    auto player = m_playerHash.value(clip, nullptr);
    if (player) {
        // Disconnect the player first so we don't get a callback when it stops
        player->disconnect(this);
        player->setRunning(false);
        player->deleteLater();
    }
    m_animationModel->setClipPlaying(clip, false);
}

QModelIndex AnimationWidget::selectedIndex() const
{
    const auto selection = m_ui->animationListView->selectionModel()->selectedIndexes();
    return selection.count() > 0 ? selection.first() : QModelIndex();
}

void AnimationWidget::handleClipPlayStateChange(const QString &clip, bool running)
{
    if (!running) {
        Q_ASSERT(m_playerHash.contains(clip));
        auto player = m_playerHash.take(clip);
        player->deleteLater();
    }
    m_animationModel->setClipPlaying(clip, running);
    updateButtonStates();
}

void AnimationWidget::playAnimationAtIndex(const QModelIndex &index)
{
    if (!index.isValid() || !m_animationModel->clipEnabled(index))
        return;

    const auto clipName = index.data().toString();
    const auto clip = m_animationModel->clipForIndex(index);
    if (clip) {
        auto player = new Kuesa::AnimationPlayer(m_sceneEntity);
        player->setSceneEntity(m_sceneEntity);
        player->setClip(clipName);
        if (player->status() == Kuesa::AnimationPlayer::Ready) {
            connect(player, &Kuesa::AnimationPlayer::runningChanged, this, [this, clipName](bool running) { handleClipPlayStateChange(clipName, running); });
            m_playerHash.insert(clipName, player);
            player->start();
        } else {
            delete player;
        }
    }
    updateButtonStates();
}

void AnimationWidget::updateButtonStates()
{
    const int clipCount = m_animationModel->rowCount();
    const int playingClipCount = m_playerHash.count();
    m_ui->playButton->setEnabled(clipCount > 0 && selectedIndex().isValid());
    m_ui->playAllButton->setEnabled(clipCount > 0);
    m_ui->stopButton->setEnabled(playingClipCount > 0 && clipCount > 0);
}

AnimationClipModel::AnimationClipModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_sceneEntity(nullptr)
    , m_playingPixmap(":/icons/playing-circleH.png")
    , m_stoppedPixmap(":/icons/stopped.png")
{
    iconUpdateTimer = new QTimer(this);
    iconUpdateTimer->setInterval(100);
    connect(iconUpdateTimer, &QTimer::timeout, this, &AnimationClipModel::updateDecorationIcon);
}

void AnimationClipModel::update(Kuesa::SceneEntity *entity)
{
    iconUpdateTimer->stop();

    beginResetModel();
    m_sceneEntity = entity;
    if (m_sceneEntity) {
        auto clipNames = m_sceneEntity->animationClips()->names();
        clipNames.sort(Qt::CaseInsensitive);
        m_clips.resize(clipNames.size());

        for (int i = 0; i < clipNames.size(); ++i) {
            const auto clipName = clipNames.at(i);
            m_clips[i].name = clipName;
            Qt3DAnimation::QAnimationClip *clip = qobject_cast<Qt3DAnimation::QAnimationClip *>(m_sceneEntity->animationClip(clipName));
            Qt3DAnimation::QChannelMapper *mapper = m_sceneEntity->animationMapping(clipName);
            m_clips[i].enabled = (clip && mapper);
            m_clips[i].duration = clip->duration();

            connect(clip, &Qt3DAnimation::QAnimationClip::durationChanged, this, [this, clipName](float d) {
                updateDuration(clipName, d);
            });
        }
    }
    endResetModel();
}

int AnimationClipModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 2;
}

int AnimationClipModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_clips.size();
    return 0;
}

QVariant AnimationClipModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    switch (index.column()) {
    case 0:
        if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
            return m_clips.at(index.row()).name;
        else if (role == Qt::DecorationRole) {
            return m_clips.at(index.row()).playing ? m_playingPixmap : m_stoppedPixmap;
        }
        break;
    case 1:
        switch (role) {
        case Qt::DisplayRole:
            return QString("%1s").arg(m_clips.at(index.row()).duration, 0, 'f', 2);
        case Qt::TextAlignmentRole:
            return { Qt::AlignRight | Qt::AlignVCenter };
        }
        break;
    }
    return {};
}

QVariant AnimationClipModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    return section == 0 ? tr("Name") : tr("Duration");
}

Qt::ItemFlags AnimationClipModel::flags(const QModelIndex &index) const
{
    auto f = QAbstractItemModel::flags(index);
    f.setFlag(Qt::ItemIsEnabled, clipEnabled(index));
    return f;
}

Qt3DAnimation::QAbstractAnimationClip *AnimationClipModel::clipForIndex(const QModelIndex &index) const
{
    const auto clipName = data(index, Qt::DisplayRole).toString();
    return m_sceneEntity ? m_sceneEntity->animationClip(clipName) : nullptr;
}

bool AnimationClipModel::clipEnabled(const QModelIndex &index) const
{
    return index.isValid() && m_clips.at(index.row()).enabled;
}

void AnimationClipModel::setClipPlaying(const QString &clipName, bool playing)
{
    auto it = std::find_if(m_clips.begin(), m_clips.end(), [clipName](const AnimationData &data) {
        return data.name == clipName;
    });
    if (it != m_clips.end()) {
        it->playing = playing;

        // if no clips are playing, stop the timer. otherwise start it (if not already running)
        if (!playing) {
            bool hasClipPlaying = std::any_of(m_clips.begin(), m_clips.end(), [](const AnimationData &clip) {
                return clip.playing;
            });
            if (!hasClipPlaying) {
                iconUpdateTimer->stop();
                const QModelIndex idx = index(static_cast<int>(std::distance(m_clips.begin(), it)), 0);
                emit dataChanged(idx, idx, { Qt::DecorationRole });
            }
        } else if (!iconUpdateTimer->isActive()) {
            iconUpdateTimer->start();
        }
    }
}

void AnimationClipModel::updateDecorationIcon()
{
    static int rotation = 0.0;
    const int rotationInterval = 1000; //how long to do a complete rotation
    const int rotationPerUpdate = 360 * iconUpdateTimer->interval() / rotationInterval;
    rotation += rotationPerUpdate;

    static QPixmap rotatingCirclePixmap(":/icons/stopped.png");
    static QPixmap playPixmap(":/icons/playing-circleH.png");
    m_playingPixmap = playPixmap;
    QPainter painter(&m_playingPixmap);
    const QPointF translation(rotatingCirclePixmap.width() / 2, rotatingCirclePixmap.height() / 2);
    painter.translate(translation);
    painter.rotate(rotation % 360);
    painter.translate(-translation);
    painter.drawPixmap(0, 0, rotatingCirclePixmap);
    painter.end();

    for (int i = 0; i < m_clips.count(); ++i) {
        if (m_clips.at(i).playing)
            emit dataChanged(index(i, 0), index(i, 0), { Qt::DecorationRole });
    }
}

void AnimationClipModel::updateDuration(const QString &clipName, float duration)
{
    auto it = std::find_if(m_clips.begin(), m_clips.end(), [clipName](const AnimationData &data) {
        return data.name == clipName;
    });
    if (it != m_clips.end()) {
        (*it).duration = duration;
        int row = std::distance(m_clips.begin(), it);
        emit dataChanged(index(row, 1), index(row, 1), { Qt::DisplayRole });
    }
}
