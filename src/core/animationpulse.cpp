/*
    animationpulse.cpp

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

#include "animationpulse.h"
#include <Qt3DCore/QTransform>

/*!
 * \class Kuesa::AnimationPulse
 * \inheaderfile Kuesa/AnimationPulse
 * \inherits Kuesa::AnimationPlayer
 * \inmodule Kuesa
 * \since Kuesa 1.4
 * \brief Given a pulse animation, emits signals when the pulse goes up or down
 *
 * AnimationPulse is an utility class designed to let the designer encode certain
 * events directly on the 3D design tool. Given an animation, it emits an up signal
 * when qRound(animatedValue) increases and a down signal when qRound(animatedValue)
 * decreases.
 *
 * In combination with a STEP pulse animation, a designer can encode the moment in which
 * certain actions should happen, for example, pausing an animation, directly from
 * the 3D design tool.
 *
 * \note To support AnimationPulse without any glTF extension, the animation should
 * animate the translation property of a glTF.Node. In particular, the pulse fcurve
 * should animate the "y" property of the translation.
 */

/*!
    \qmltype AnimationPulse
    \inherits Kuesa::AnimationPlayer
    \inqmlmodule Kuesa
    \since Kuesa 1.4
    \instantiates Kuesa::AnimationPulse
    \brief Given a pulse animation, emits signals when the pulse goes up or down

    AnimationPulse is an utility class designed to let the designer encode certain
    events directly on the 3D design tool. Given an animation, it emits an up signal
    when qRound(animatedValue) increases and a down signal when qRound(animatedValue)
    decreases.

    In combination with a STEP pulse animation, a designer can encode the moment in which
    certain actions should happen, for example, pausing an animation, directly from
    the 3D design tool.

    \note To support AnimationPulse without any glTF extension, the animation should
    animate the translation property of a glTF.Node. In particular, the pulse fcurve
    should animate the "y" property of the translation.
*/

QT_BEGIN_NAMESPACE

void Kuesa::AnimationPulse::onPulseChanged()
{
    if (m_pulse > m_previousValue) {
        m_previousValue = m_pulse;
        emit up();
        return;
    }

    if (m_pulse < m_previousValue) {
        m_previousValue = m_pulse;
        emit down();
        return;
    }
}

Kuesa::AnimationPulse::AnimationPulse(Qt3DCore::QNode *parent)
    : Kuesa::AnimationPlayer(parent)
    , m_pulse(0)
    , m_previousValue(0)
    , m_proxyTransform(new Qt3DCore::QTransform(this))
{
    addTarget(m_proxyTransform);
    QObject::connect(m_proxyTransform, &Qt3DCore::QTransform::translationChanged,
                     [this]() {
                         if (m_pulse == qRound(m_proxyTransform->translation().y()))
                             return;
                         m_pulse = qRound(m_proxyTransform->translation().y());
                         emit pulseChanged(m_pulse);
                     });

    QObject::connect(this, &AnimationPulse::pulseChanged,
                     this, &AnimationPulse::onPulseChanged);
}

float Kuesa::AnimationPulse::pulse() const
{
    return m_pulse;
}

QT_END_NAMESPACE
