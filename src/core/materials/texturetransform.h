/*
    texturetransform.h

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_TEXTURETRANSFORM_H
#define KUESA_TEXTURETRANSFORM_H

#include <Kuesa/kuesa_global.h>
#include <Qt3DCore/QNode>
#include <QVector2D>
#include <QMatrix3x3>

QT_BEGIN_NAMESPACE

namespace Kuesa {
class KUESASHARED_EXPORT TextureTransform : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QVector2D offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(QVector2D scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(float rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(QMatrix3x3 matrix READ matrix)

public:
    explicit TextureTransform(Qt3DCore::QNode *parent = nullptr);
    ~TextureTransform() = default;

    const QVector2D &offset() const;
    const QVector2D &scale() const;
    float rotation() const;
    QMatrix3x3 matrix() const;

public Q_SLOTS:
    void setOffset(const QVector2D &offset);
    void setScale(const QVector2D &scale);
    void setRotation(float rotation);

Q_SIGNALS:
    void offsetChanged(const QVector2D &offset);
    void scaleChanged(const QVector2D &scale);
    void rotationChanged(float rotation);
    void matrixChanged(QMatrix3x3 matrix);

private:
    QVector2D m_offset;
    QVector2D m_scale;
    float m_rotation;
};
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_TEXTURETRANSFORM_H
