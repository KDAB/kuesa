/*
    transformtracker.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef TRANSFORMTRACKER_H
#define TRANSFORMTRACKER_H

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qtransform.h>
#include <Kuesa/kuesa_global.h>
#include <Kuesa/sceneentity.h>
#include <Kuesa/kuesanode.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class EntityTransformWatcher;

class KUESASHARED_EXPORT TransformTracker : public KuesaNode
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Qt3DCore::QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QSize screenSize READ screenSize WRITE setScreenSize NOTIFY screenSizeChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(QMatrix4x4 matrix READ matrix WRITE setMatrix NOTIFY matrixChanged)
    Q_PROPERTY(QMatrix4x4 worldMatrix READ worldMatrix NOTIFY worldMatrixChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(float rotationX READ rotationX WRITE setRotationX NOTIFY rotationXChanged)
    Q_PROPERTY(float rotationY READ rotationY WRITE setRotationY NOTIFY rotationYChanged)
    Q_PROPERTY(float rotationZ READ rotationZ WRITE setRotationZ NOTIFY rotationZChanged)
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QVector3D scale3D READ scale3D WRITE setScale3D NOTIFY scale3DChanged)
    Q_PROPERTY(QVector3D translation READ translation WRITE setTranslation NOTIFY translationChanged)
    Q_PROPERTY(QPointF screenPosition READ screenPosition NOTIFY screenPositionChanged)
public:
    explicit TransformTracker(Qt3DCore::QNode *parent = nullptr);
    ~TransformTracker();

    Qt3DCore::QEntity *camera() const;
    QSize screenSize() const;
    QRectF viewportRect() const;
    QString name() const;
    QMatrix4x4 matrix() const;
    QMatrix4x4 worldMatrix() const;
    QQuaternion rotation() const;
    float rotationX() const;
    float rotationY() const;
    float rotationZ() const;
    float scale() const;
    QVector3D scale3D() const;
    QVector3D translation() const;
    QPointF screenPosition() const;

public Q_SLOTS:
    void setName(const QString &name);
    void setCamera(Qt3DCore::QEntity *camera);
    void setScreenSize(const QSize &screenSize);
    void setViewportRect(QRectF viewportRect);
    void setViewportRect(qreal x, qreal y, qreal width, qreal height);
    void setMatrix(const QMatrix4x4 &matrix);
    void setRotation(const QQuaternion &rotation);
    void setRotationX(float rotationX);
    void setRotationY(float rotationY);
    void setRotationZ(float rotationZ);
    void setScale(float scale);
    void setScale3D(const QVector3D &scale3D);
    void setTranslation(const QVector3D &translation);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void cameraChanged(Qt3DCore::QEntity *camera);
    void screenSizeChanged(const QSize &screenSize);
    void scaleChanged(float scale);
    void scale3DChanged(const QVector3D &scale);
    void rotationChanged(const QQuaternion &rotation);
    void translationChanged(const QVector3D &translation);
    void matrixChanged(const QMatrix4x4 &matrix);
    void rotationXChanged(float rotationX);
    void rotationYChanged(float rotationY);
    void rotationZChanged(float rotationZ);
    void worldMatrixChanged(const QMatrix4x4 &worldMatrix);
    void screenPositionChanged(const QPointF &screenPosition);

    void viewportRectChanged(QRectF viewportRect);

private:
    void matchNode();
    void updateScreenProjection();

    Qt3DCore::QEntity *m_camera;
    Qt3DCore::QTransform *m_cameraTransform;
    Qt3DRender::QCameraLens *m_cameraLens;
    QSize m_screenSize;
    QRectF m_viewportRect;
    QString m_name;
    Qt3DCore::QTransform *m_node;
    QPointF m_screenPosition;
    QMetaObject::Connection m_loadingDoneConnection;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // TRANSFORMTRACKER_H
