/*
    placeholder.h

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

#ifndef KUESA_PLACEHOLDER_H
#define KUESA_PLACEHOLDER_H

#include <Kuesa/kuesa_global.h>
#include <QObject>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{
class QCamera;
}

namespace Qt3DCore
{
class QTransform;
}

namespace Kuesa
{

class KUESASHARED_EXPORT Placeholder : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(QRect viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
    Q_PROPERTY(Qt3DCore::QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)
public:
    explicit Placeholder(Qt3DCore::QNode *parent = nullptr);

    Qt3DCore::QEntity *camera() const;
    void setCamera(Qt3DCore::QEntity *camera);

    QRect viewport() const;
    void setViewport(QRect viewport);

    QObject *target() const;
    void setTarget(QObject *target);

Q_SIGNALS:
    void viewportChanged(QRect viewport);
    void cameraChanged(Qt3DCore::QEntity *camera);
    void targetChanged(QObject *target);

private:
    void updatePlaceholder();

    Qt3DCore::QTransform *m_transform = nullptr;
    Qt3DCore::QEntity *m_camera = nullptr;
    Qt3DRender::QCameraLens *m_cameraLens = nullptr;
    Qt3DCore::QTransform *m_cameraTransform = nullptr;
    QObject *m_target = nullptr;
    QRect m_viewport;
    QMetaObject::Connection m_targetDestroyedConnection;
};

} // Kuesa

QT_END_NAMESPACE

#endif // KUESA_PLACEHOLDER_H
