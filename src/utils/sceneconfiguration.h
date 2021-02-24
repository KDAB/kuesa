/*
    sceneconfiguration.h

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

#ifndef KUESA_KUESA_UTILS_SCENECONFIGURATION_H
#define KUESA_KUESA_UTILS_SCENECONFIGURATION_H

#include <KuesaUtils/kuesa_utils_global.h>

#include <Qt3DCore/QNode>
#include <QUrl>

#include <Kuesa/animationplayer.h>
#include <Kuesa/transformtracker.h>
#include <Kuesa/placeholdertracker.h>
#include <Kuesa/kuesanode.h>

QT_BEGIN_NAMESPACE

namespace KuesaUtils {

class KUESAUTILS_SHARED_EXPORT SceneConfiguration : public Kuesa::KuesaNode
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString cameraName READ cameraName WRITE setCameraName NOTIFY cameraNameChanged)
public:
    explicit SceneConfiguration(Qt3DCore::QNode *parent = nullptr);

    QUrl source() const;
    QString cameraName() const;

    const std::vector<Kuesa::AnimationPlayer *> &animationPlayers() const { return m_animations; }
    void addAnimationPlayer(Kuesa::AnimationPlayer *animation);
    void removeAnimationPlayer(Kuesa::AnimationPlayer *animation);
    void clearAnimationPlayers();

    const std::vector<Kuesa::TransformTracker *> &transformTrackers() const { return m_trackers; }
    void addTransformTracker(Kuesa::TransformTracker *tracker);
    void removeTransformTracker(Kuesa::TransformTracker *tracker);
    void clearTransformTrackers();

    const std::vector<Kuesa::PlaceholderTracker *> &placeholderTrackers() const { return m_placeholderTrackers; }
    void addPlaceholderTracker(Kuesa::PlaceholderTracker *placeholder);
    void removePlaceholderTracker(Kuesa::PlaceholderTracker *placeholder);
    void clearPlaceholderTrackers();

public Q_SLOTS:
    void setSource(const QUrl &source);
    void setCameraName(const QString &cameraName);

Q_SIGNALS:
    void sourceChanged(const QUrl &source);
    void cameraNameChanged(const QString &cameraName);

    void animationPlayerAdded(Kuesa::AnimationPlayer *player);
    void animationPlayerRemoved(Kuesa::AnimationPlayer *player);

    void transformTrackerAdded(Kuesa::TransformTracker *tracker);
    void transformTrackerRemoved(Kuesa::TransformTracker *tracker);

    void placeholderTrackerAdded(Kuesa::PlaceholderTracker *placeholder);
    void placeholderTrackerRemoved(Kuesa::PlaceholderTracker *placeholder);

    void loadingDone();
    void unloadingDone();

private:
    QUrl m_source;
    QString m_cameraName;
    std::vector<Kuesa::AnimationPlayer *> m_animations;
    std::vector<Kuesa::TransformTracker *> m_trackers;
    std::vector<Kuesa::PlaceholderTracker *> m_placeholderTrackers;
};

} // namespace KuesaUtils

QT_END_NAMESPACE

#endif // KUESA_KUESA_UTILS_SCENECONFIGURATION_H
