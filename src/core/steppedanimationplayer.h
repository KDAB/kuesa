/*
    steppedanimationplayer.h

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

#ifndef KUESA_STEPPEDANIMATIONPLAYER_H
#define KUESA_STEPPEDANIMATIONPLAYER_H

#include <Kuesa/AnimationPulse>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE
namespace Kuesa {

class KUESASHARED_EXPORT SteppedAnimationPlayer : public Kuesa::AnimationPulse
{
    Q_OBJECT
    Q_PROPERTY(QStringList animationNames READ animationNames WRITE setAnimationNames NOTIFY animationNamesChanged)
public:
    explicit SteppedAnimationPlayer(Qt3DCore::QNode *parent = nullptr);

    QStringList animationNames() const;
    void setAnimationNames(const QStringList &animationNames);

Q_SIGNALS:
    void animationNamesChanged();

private:
    void updateAnimationPlayers();
    QStringList m_animationNames;
    std::vector<Kuesa::AnimationPlayer *> m_animationPlayers;
};
} // namespace Kuesa

QT_END_NAMESPACE
#endif // STEPPEDANIMATIONPLAYER_H
