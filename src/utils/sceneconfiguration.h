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
#include <KuesaUtils/viewconfiguration.h>
#include <Kuesa/kuesanode.h>

QT_BEGIN_NAMESPACE

namespace KuesaUtils {

class KUESAUTILS_SHARED_EXPORT SceneConfiguration : public Kuesa::KuesaNode
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
public:
    explicit SceneConfiguration(Qt3DCore::QNode *parent = nullptr);

    QUrl source() const;

    const std::vector<KuesaUtils::ViewConfiguration *> &viewConfigurations() const;
    void addViewConfiguration(KuesaUtils::ViewConfiguration *viewConfiguration);
    void removeViewConfiguration(KuesaUtils::ViewConfiguration *viewConfiguration);
    void clearViewConfigurations();

    const std::vector<Kuesa::AnimationPlayer *> &animationPlayers() const;
    void addAnimationPlayer(Kuesa::AnimationPlayer *animation);
    void removeAnimationPlayer(Kuesa::AnimationPlayer *animation);
    void clearAnimationPlayers();

public Q_SLOTS:
    void setSource(const QUrl &source);

Q_SIGNALS:
    void sourceChanged(const QUrl &source);

    void viewConfigurationAdded(KuesaUtils::ViewConfiguration *viewConfiguration);
    void viewConfigurationRemoved(KuesaUtils::ViewConfiguration *viewConfiguration);

    void animationPlayerAdded(Kuesa::AnimationPlayer *player);
    void animationPlayerRemoved(Kuesa::AnimationPlayer *player);

    void loadingDone();
    void unloadingDone();

private:
    QUrl m_source;
    std::vector<Kuesa::AnimationPlayer *> m_animations;
    std::vector<KuesaUtils::ViewConfiguration *> m_viewConfigurations;
};

} // namespace KuesaUtils

QT_END_NAMESPACE

#endif // KUESA_KUESA_UTILS_SCENECONFIGURATION_H
