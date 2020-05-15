/*
    spotlight.h

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

#ifndef KUESA_SPOTLIGHT_H
#define KUESA_SPOTLIGHT_H

#include <Kuesa/shadowcastinglight.h>
#include <Kuesa/kuesa_global.h>
QT_BEGIN_NAMESPACE

namespace Kuesa {
class SpotLightPrivate;
class KUESASHARED_EXPORT SpotLight : public ShadowCastingLight
{
    Q_OBJECT
    Q_PROPERTY(QVector3D localDirection READ localDirection WRITE setLocalDirection NOTIFY localDirectionChanged)
    Q_PROPERTY(float innerConeAngle READ innerConeAngle WRITE setInnerConeAngle NOTIFY innerConeAngleChanged)
    Q_PROPERTY(float outerConeAngle READ outerConeAngle WRITE setOuterConeAngle NOTIFY outerConeAngleChanged)
    Q_PROPERTY(float range READ range WRITE setRange NOTIFY rangeChanged)

public:
    explicit SpotLight(Qt3DCore::QNode *parent = nullptr);
    ~SpotLight();

    QVector3D localDirection() const;
    float innerConeAngle() const;
    float outerConeAngle() const;
    float range() const;

public Q_SLOTS:
    void setLocalDirection(const QVector3D &localDirection);
    void setInnerConeAngle(float innerConeAngle);
    void setOuterConeAngle(float outerConeAngle);
    void setRange(float range);

Q_SIGNALS:
    void localDirectionChanged(const QVector3D &localDirection);
    void innerConeAngleChanged(float innerConeAngle);
    void outerConeAngleChanged(float outerConeAngle);
    void rangeChanged(float range);

protected:
    explicit SpotLight(SpotLightPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(SpotLight)
    void updateAngularAttenuationUniforms();

    float m_innerConeAngle;
    float m_outerConeAngle;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_SPOTLIGHT_H
