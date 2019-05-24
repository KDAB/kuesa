/*
    directionallight.h

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_DIRECTIONALLIGHT_H
#define KUESA_DIRECTIONALLIGHT_H

#include <QAbstractLight>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {
class DirectionalLightPrivate;
class KUESASHARED_EXPORT DirectionalLight : public Qt3DRender::QAbstractLight
{
    Q_OBJECT
    Q_PROPERTY(QVector3D localDirection READ localDirection WRITE setLocalDirection NOTIFY localDirectionChanged)

public:
    explicit DirectionalLight(Qt3DCore::QNode *parent = nullptr);
    ~DirectionalLight();

    QVector3D localDirection() const;

public Q_SLOTS:
    void setLocalDirection(const QVector3D &localDirection);

Q_SIGNALS:
    void localDirectionChanged(const QVector3D &localDirection);

protected:
    explicit DirectionalLight(DirectionalLightPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(DirectionalLight)
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_DIRECTIONALLIGHT_H
