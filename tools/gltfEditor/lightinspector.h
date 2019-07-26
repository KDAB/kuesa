/*
    lightinspector.h

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

#ifndef LIGHTINSPECTOR_H
#define LIGHTINSPECTOR_H

#include <QObject>
#include <QColor>

namespace Qt3DRender {
class QAbstractLight;
} // namespace Qt3DRender

class LightInspector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY lightParamsChanged)
    Q_PROPERTY(int type READ type NOTIFY lightParamsChanged)
    Q_PROPERTY(QColor color READ color NOTIFY lightParamsChanged)
    Q_PROPERTY(float intensity READ intensity NOTIFY lightParamsChanged)
    Q_PROPERTY(float range READ range NOTIFY lightParamsChanged)
    Q_PROPERTY(float innerConeAngle READ innerConeAngle NOTIFY lightParamsChanged)
    Q_PROPERTY(float outerConeAngle READ outerConeAngle NOTIFY lightParamsChanged)

public:
    explicit LightInspector(QObject *parent = nullptr);
    virtual ~LightInspector() {}

    void setLight(Qt3DRender::QAbstractLight *light);

    QString name() const;
    int type() const;
    QColor color() const;
    float intensity() const;
    float range() const;
    float innerConeAngle() const;
    float outerConeAngle() const;

Q_SIGNALS:
    void lightParamsChanged();

private:
    Qt3DRender::QAbstractLight *m_light = nullptr;

    QString m_name;
    int m_type;
    QColor m_color;
    float m_intensity = 0.0f;
    float m_range = 0.0f;
    float m_innerConeAngle = 0.0f;
    float m_outerConeAngle = 0.0f;

    QMetaObject::Connection m_lightConnection;
};

#endif // LIGHTINSPECTOR_H
