/*
    camerawidget.cpp

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

#include "camerawidget.h"
#include "ui_camerawidget.h"
#include "ui_vectorwidget.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

namespace {
    const QLatin1String LASTPATHSETTING("mainwindow/lastPath");

    QJsonObject vector3DToJson(const QVector3D &v)
    {
        QJsonObject json;
        json[QLatin1String("x")] = static_cast<double>(v.x());
        json[QLatin1String("y")] = static_cast<double>(v.y());
        json[QLatin1String("z")] = static_cast<double>(v.z());
        return json;
    }

    QVector3D jsonToVector3D(const QJsonObject &json)
    {
        QVector3D v(static_cast<float>(json[QLatin1String("x")].toDouble()),
                    static_cast<float>(json[QLatin1String("y")].toDouble()),
                    static_cast<float>(json[QLatin1String("z")].toDouble()));
        return v;
    }
}

VectorWidget::VectorWidget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::VectorWidget)
{
    m_ui->setupUi(this);
    connect(m_ui->x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorWidget::handleXChanged);
    connect(m_ui->y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorWidget::handleYChanged);
    connect(m_ui->z, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorWidget::handleZChanged);
}

VectorWidget::~VectorWidget()
{
}

QVector3D VectorWidget::vector() const
{
    return m_vector;
}

void VectorWidget::setVector(QVector3D vector)
{
    if (m_vector == vector)
        return;

    m_vector = vector;
    updateWidgets();
    emit vectorChanged(m_vector);
}

void VectorWidget::updateWidgets()
{
    QSignalBlocker sb1(m_ui->x);
    QSignalBlocker sb2(m_ui->y);
    QSignalBlocker sb3(m_ui->z);
    m_ui->x->setValue(m_vector.x());
    m_ui->y->setValue(m_vector.y());
    m_ui->z->setValue(m_vector.z());
}

void VectorWidget::setComponentLabelsVisible(bool visible)
{
    m_ui->xLabel->setVisible(visible);
    m_ui->yLabel->setVisible(visible);
    m_ui->zLabel->setVisible(visible);
}

void VectorWidget::setNameLabel(const QString &text)
{
    m_ui->nameLabel->setText(text);
}

void VectorWidget::handleXChanged(double x)
{
    m_vector.setX(x);
    emit vectorChanged(m_vector);
}

void VectorWidget::handleYChanged(double y)
{
    m_vector.setY(y);
    emit vectorChanged(m_vector);
}

void VectorWidget::handleZChanged(double z)
{
    m_vector.setZ(z);
    emit vectorChanged(m_vector);
}

CameraWidget::CameraWidget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::CameraWidget)
{
    m_ui->setupUi(this);

    m_ui->positionVecEdit->setComponentLabelsVisible(true);
    m_ui->centerVecEdit->setComponentLabelsVisible(false);
    m_ui->viewVecEdit->setComponentLabelsVisible(false);

    m_ui->positionVecEdit->setNameLabel(tr("Pos"));
    m_ui->centerVecEdit->setNameLabel(tr("Ctr"));
    m_ui->viewVecEdit->setNameLabel(tr("Dir"));

    connect(m_ui->positionVecEdit, &VectorWidget::vectorChanged, this, &CameraWidget::setPosition);
    connect(m_ui->centerVecEdit, &VectorWidget::vectorChanged, this, &CameraWidget::setCenter);
    connect(m_ui->viewVecEdit, &VectorWidget::vectorChanged, this, &CameraWidget::setViewVector);
    connect(m_ui->bottom, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CameraWidget::setBottom);
    connect(m_ui->top, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CameraWidget::setTop);
    connect(m_ui->left, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CameraWidget::setLeft);
    connect(m_ui->right, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CameraWidget::setRight);
    connect(m_ui->near, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CameraWidget::setNear);
    connect(m_ui->far, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CameraWidget::setFar);
    connect(m_ui->fov, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CameraWidget::setFoV);
    connect(m_ui->saveButton, &QPushButton::clicked, this, &CameraWidget::save);
    connect(m_ui->loadButton, &QPushButton::clicked, this, &CameraWidget::load);
}

CameraWidget::~CameraWidget()
{
}

Qt3DRender::QCamera *CameraWidget::camera() const
{
    return m_camera;
}

void CameraWidget::setCamera(Qt3DRender::QCamera *camera)
{
    if (m_camera == camera)
        return;
    if (m_camera)
        this->disconnect(m_camera);
    m_camera = camera;
    if (m_camera) {
        connect(m_camera, &Qt3DRender::QCamera::bottomChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::topChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::leftChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::rightChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::nearPlaneChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::farPlaneChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::positionChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::viewCenterChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::viewVectorChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::fieldOfViewChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::aspectRatioChanged, this, &CameraWidget::updateWidgetValues);
        connect(m_camera, &Qt3DRender::QCamera::projectionTypeChanged, this, &CameraWidget::updateWidgetValues);
    }
    updateWidgetValues();
    emit cameraChanged(m_camera);
}

void CameraWidget::setCenter(const QVector3D &vector)
{
    if (m_camera)
        m_camera->setViewCenter(vector);
}

void CameraWidget::setViewVector(const QVector3D &vector)
{
    if (m_camera)
        m_camera->setViewCenter(m_camera->position() + vector);
}

void CameraWidget::setPosition(const QVector3D &vector)
{
    if (m_camera)
        m_camera->setPosition(vector);
}

void CameraWidget::setLength(double length)
{
    if (m_camera) {
        auto newVec = m_camera->viewVector().normalized() * length;
        m_camera->setViewCenter(m_camera->position() + newVec);
    }
}

void CameraWidget::setLeft(double left)
{
    if (m_camera)
        m_camera->setLeft(left);
}

void CameraWidget::setRight(double right)
{
    if (m_camera)
        m_camera->setRight(right);
}

void CameraWidget::setTop(double top)
{
    if (m_camera)
        m_camera->setTop(top);
}

void CameraWidget::setBottom(double bottom)
{
    if (m_camera)
        m_camera->setBottom(bottom);
}

void CameraWidget::setNear(double near)
{
    if (m_camera)
        m_camera->setNearPlane(near);
}

void CameraWidget::setFar(double far)
{
    if (m_camera)
        m_camera->setFarPlane(far);
}

void CameraWidget::setFoV(double fov)
{
    if (m_camera)
        m_camera->setFieldOfView(fov);
}

void CameraWidget::save()
{
    QSettings settings;
    const QString lastPath = settings.value(LASTPATHSETTING, QDir::homePath()).toString();
    const auto filename = QFileDialog::getSaveFileName(
                this,
                QLatin1String("Save Camera Settings"),
                lastPath,
                QLatin1String("*.apecam"));

    if (filename.isEmpty())
        return;

    QFileInfo fi(filename);
    settings.setValue(LASTPATHSETTING, fi.absolutePath());

    QFile saveFile(filename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open camera settings save file.");
        return;
    }

    QJsonObject cameraObject;
    toJson(cameraObject);
    QJsonDocument saveDoc(cameraObject);
    saveFile.write(saveDoc.toJson());
}

void CameraWidget::load()
{
    QSettings settings;
    const QString lastPath = settings.value(LASTPATHSETTING, QDir::homePath()).toString();
    const auto filename = QFileDialog::getOpenFileName(
                this,
                QLatin1String("Load Camera Settings"),
                lastPath,
                QLatin1String("*.apecam"));

    if (filename.isEmpty())
        return;

    QFileInfo fi(filename);
    settings.setValue(LASTPATHSETTING, fi.absolutePath());

    QFile loadFile(filename);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open camera settings load file.");
        return;
    }

    const QByteArray savedData = loadFile.readAll();
    const QJsonDocument loadDoc(QJsonDocument::fromJson(savedData));
    fromJson(loadDoc.object());
}

void CameraWidget::updateWidgetValues()
{
    QSignalBlocker sb1(m_ui->centerVecEdit);
    QSignalBlocker sb2(m_ui->positionVecEdit);
    QSignalBlocker sb22(m_ui->viewVecEdit);
    QSignalBlocker sb3(m_ui->left);
    QSignalBlocker sb4(m_ui->right);
    QSignalBlocker sb5(m_ui->top);
    QSignalBlocker sb6(m_ui->bottom);
    QSignalBlocker sb7(m_ui->near);
    QSignalBlocker sb8(m_ui->far);
    QSignalBlocker sb10(m_ui->fov);
    m_ui->centerVecEdit->setVector(m_camera ? m_camera->viewCenter() : QVector3D());
    m_ui->positionVecEdit->setVector(m_camera ? m_camera->position() : QVector3D());
    m_ui->viewVecEdit->setVector(m_camera ? m_camera->viewVector() : QVector3D());
    m_ui->near->setValue(m_camera ? m_camera->nearPlane() : 0.0f);
    m_ui->far->setValue(m_camera ? m_camera->farPlane() : 0.0f);
    const bool isPerspective = m_camera->projectionType() == Qt3DRender::QCameraLens::PerspectiveProjection;
    m_ui->orthoGroupBox->setVisible(!isPerspective);
    m_ui->perspectiveGroupBox->setVisible(isPerspective);
    if (isPerspective) {
        m_ui->fov->setValue(m_camera ? m_camera->fieldOfView() : 0.0f);
        m_ui->aspectRatio->setValue(m_camera ? m_camera->aspectRatio() : 1.0f);
    } else {
        m_ui->left->setValue(m_camera ? m_camera->left() : 0.0f);
        m_ui->right->setValue(m_camera ? m_camera->right() : 0.0f);
        m_ui->top->setValue(m_camera ? m_camera->top() : 0.0f);
        m_ui->bottom->setValue(m_camera ? m_camera->bottom() : 0.0f);
    }
}

void CameraWidget::fromJson(const QJsonObject &json)
{
    if (!m_camera)
        return;

    m_camera->setPosition(jsonToVector3D(json[QLatin1String("position")].toObject()));
    m_camera->setViewCenter(jsonToVector3D(json[QLatin1String("viewCenter")].toObject()));
    m_camera->setUpVector(jsonToVector3D(json[QLatin1String("upVector")].toObject()));
    m_camera->setNearPlane(static_cast<float>(json[QLatin1String("nearPlane")].toDouble()));
    m_camera->setFarPlane(static_cast<float>(json[QLatin1String("farPlane")].toDouble()));

    const auto projectionType = json[QLatin1String("projectionType")].toString();
    if (projectionType == QLatin1String("Perspective")) {
        m_camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
        m_camera->setFieldOfView(static_cast<float>(json[QLatin1String("fieldOfView")].toDouble()));
        m_camera->setAspectRatio(static_cast<float>(json[QLatin1String("aspectRatio")].toDouble()));
    } else {
        m_camera->setProjectionType(Qt3DRender::QCameraLens::OrthographicProjection);
        m_camera->setLeft(static_cast<float>(json[QLatin1String("left")].toDouble()));
        m_camera->setRight(static_cast<float>(json[QLatin1String("right")].toDouble()));
        m_camera->setTop(static_cast<float>(json[QLatin1String("top")].toDouble()));
        m_camera->setBottom(static_cast<float>(json[QLatin1String("bottom")].toDouble()));
    }

    updateWidgetValues();
}

void CameraWidget::toJson(QJsonObject &json)
{
    if (!m_camera)
        return;

    json[QLatin1String("position")] = vector3DToJson(m_camera->position());
    json[QLatin1String("viewCenter")] = vector3DToJson(m_camera->viewCenter());
    json[QLatin1String("upVector")] = vector3DToJson(m_camera->upVector());
    json[QLatin1String("nearPlane")] = static_cast<double>(m_camera->nearPlane());
    json[QLatin1String("farPlane")] = static_cast<double>(m_camera->farPlane());

    const bool isPerspective = m_camera->projectionType() == Qt3DRender::QCameraLens::PerspectiveProjection;
    if (isPerspective) {
        json[QLatin1String("projectionType")] = QLatin1String("Perspective");
        json[QLatin1String("fieldOfView")] = static_cast<double>(m_camera->fieldOfView());
        json[QLatin1String("aspectRatio")] = static_cast<double>(m_camera->aspectRatio());
    } else {
        json[QLatin1String("projectionType")] = QLatin1String("Orthographic");
        json[QLatin1String("left")] = static_cast<double>(m_camera->left());
        json[QLatin1String("right")] = static_cast<double>(m_camera->right());
        json[QLatin1String("top")] = static_cast<double>(m_camera->top());
        json[QLatin1String("bottom")] = static_cast<double>(m_camera->bottom());
    }
}
