/*
    camerawidget.h

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

#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <Qt3DRender/QCamera>
#include <QScopedPointer>

namespace Ui {
class CameraWidget;
class VectorWidget;
} // namespace Ui

class VectorWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QVector3D vector READ vector WRITE setVector NOTIFY vectorChanged)
public:
    VectorWidget(QWidget *parent = nullptr);
    virtual ~VectorWidget();

    QVector3D vector() const;

public slots:
    void setVector(QVector3D vector);
    void updateWidgets();
    void setComponentLabelsVisible(bool visible);
    void setNameLabel(const QString &text);

private:
    void handleXChanged(double x);
    void handleYChanged(double y);
    void handleZChanged(double z);

signals:
    void vectorChanged(QVector3D vector);

private:
    QScopedPointer<Ui::VectorWidget> m_ui;
    QVector3D m_vector;
};

class CameraWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)

public:
    CameraWidget(QWidget *parent = nullptr);
    virtual ~CameraWidget();

    Qt3DRender::QCamera *camera() const;

public slots:
    void setCamera(Qt3DRender::QCamera *camera);
    void setCenter(const QVector3D &vector);
    void setViewVector(const QVector3D &vector);
    void setPosition(const QVector3D &vector);
    void setLength(double length);
    void setLeft(double left);
    void setRight(double right);
    void setTop(double top);
    void setBottom(double bottom);
    void setNear(double near);
    void setFar(double far);
    void setFoV(double fov);
    void save();
    void load();

    void updateWidgetValues();

signals:
    void cameraChanged(Qt3DRender::QCamera *camera);

private:
    void fromJson(const QJsonObject &json);
    void toJson(QJsonObject &json);

    Qt3DRender::QCamera *m_camera = nullptr;
    QScopedPointer<Ui::CameraWidget> m_ui;
};

#endif // CAMERAWIDGET_H
