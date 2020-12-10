/*
    reflectionplane.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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

#ifndef KUESA_REFLECTIONPLANE_H
#define KUESA_REFLECTIONPLANE_H

#include <Kuesa/kuesa_global.h>
#include <Qt3DCore/QNode>
#include <QVector4D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QLayer;
}

namespace Kuesa {

class KUESASHARED_EXPORT ReflectionPlane : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QVector4D equation READ equation WRITE setEquation NOTIFY equationChanged)
public:
    explicit ReflectionPlane(Qt3DCore::QNode *parent = nullptr);

    void setEquation(const QVector4D &equation);
    QVector4D equation() const;

    void addLayer(Qt3DRender::QLayer *layer);
    void removeLayer(Qt3DRender::QLayer *layer);
    const std::vector<Qt3DRender::QLayer *> &layers() const;

Q_SIGNALS:
    void equationChanged(const QVector4D &equation);
    void layersChanged();

private:
    std::vector<Qt3DRender::QLayer *> m_layers;
    QVector4D m_equation;
};

} // Kuesa

QT_END_NAMESPACE


#endif // KUESA_REFLECTIONPLANE_H
