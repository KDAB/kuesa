/*
    placeholdertracker.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef PLACEHOLDERTRACKER_H
#define PLACEHOLDERTRACKER_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/sceneentity.h>
#include <Kuesa/kuesanode.h>
#include <Qt3DCore/QEntity>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class KUESASHARED_EXPORT PlaceholderTracker : public KuesaNode
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Qt3DCore::QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QSize screenSize READ screenSize WRITE setScreenSize NOTIFY screenSizeChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(int x READ x NOTIFY xChanged)
    Q_PROPERTY(int y READ y NOTIFY yChanged)
    Q_PROPERTY(int width READ width NOTIFY widthChanged)
    Q_PROPERTY(int height READ height NOTIFY heightChanged)
    Q_PROPERTY(QPointF screenPosition READ screenPosition NOTIFY screenPositionChanged)

public:
    explicit PlaceholderTracker(Qt3DCore::QNode *parent = nullptr);
    ~PlaceholderTracker();

    QString name() const;
    Qt3DCore::QEntity *camera() const;
    QSize screenSize() const;
    QRectF viewportRect() const;
    QObject *target() const;

    int x() const;
    int y() const;
    int width() const;
    int height() const;
    QPointF screenPosition() const;

public Q_SLOTS:
    void setName(const QString &name);
    void setCamera(Qt3DCore::QEntity *camera);
    void setScreenSize(const QSize &screenSize);
    void setViewportRect(const QRectF &viewportRect);
    void setViewportRect(qreal x, qreal y, qreal width, qreal height);
    void setTarget(QObject *target);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void cameraChanged(Qt3DCore::QEntity *camera);
    void screenSizeChanged(const QSize &screenSize);
    void viewportRectChanged(QRectF viewportRect);
    void targetChanged(QObject *target);
    void xChanged(int x);
    void yChanged(int y);
    void widthChanged(int width);
    void heightChanged(int height);
    void screenPositionChanged(const QPointF &screenPosition);

private:
    void matchNode();
    void updatePlaceholderProjection();

    void setX(int x);
    void setY(int y);
    void setWidth(int width);
    void setHeight(int height);

    Qt3DCore::QEntity *m_camera = nullptr;
    Qt3DRender::QCameraLens *m_cameraLens = nullptr;
    Qt3DCore::QTransform *m_cameraTransform = nullptr;

    Kuesa::Placeholder *m_placeHolder = nullptr;
    Qt3DCore::QTransform *m_placeHolderTransform = nullptr;

    QObject *m_target = nullptr;
    QSize m_screenSize;
    QRectF m_viewportRect;
    QString m_name;
    QMetaObject::Connection m_loadingDoneConnection;
    QMetaObject::Connection m_targetDestroyedConnection;
    int m_x = 0;
    int m_y = 0;
    int m_width = 0;
    int m_height = 0;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // PLACEHOLDERTRACKER_H
