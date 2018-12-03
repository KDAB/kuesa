/*
    orbitcameracontroller.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef ORBITCAMERACONTROLLER_H
#define ORBITCAMERACONTROLLER_H

#include <Kuesa/kuesa_global.h>
#include <Qt3DCore/QEntity>
#include <QPoint>
#include <QSize>
#include <QVector3D>
#include <Qt3DRender/QCamera>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
class QKeyboardDevice;
class QMouseDevice;
class QMouseHandler;
class QLogicalDevice;
class QAction;
class QActionInput;
class QAxis;
class QAnalogAxisInput;
class QButtonAxisInput;
class QAxisActionHandler;
}

namespace Qt3DLogic {
class QFrameAction;
}

class OrbitCameraControllerPrivate;

class OrbitCameraController : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(float panSpeed READ panSpeed WRITE setPanSpeed NOTIFY panSpeedChanged)
    Q_PROPERTY(float zoomSpeed READ zoomSpeed WRITE setZoomSpeed NOTIFY zoomSpeedChanged)
    Q_PROPERTY(float rotationSpeed READ rotationSpeed WRITE setRotationSpeed NOTIFY rotationSpeedChanged)
    Q_PROPERTY(float zoomCameraLimit READ zoomCameraLimit WRITE setZoomCameraLimit NOTIFY zoomCameraLimitChanged)
    Q_PROPERTY(QSize windowSize READ windowSize WRITE setWindowSize NOTIFY windowSizeChanged)
public:
    explicit OrbitCameraController(Qt3DCore::QNode *parent = nullptr);

    Qt3DRender::QCamera *camera() const;

    void setCamera(Qt3DRender::QCamera *camera);

    float panSpeed() const;
    float zoomSpeed() const;
    float rotationSpeed() const;
    float zoomCameraLimit() const;
    QSize windowSize() const;

    void setPanSpeed(float v);
    void setZoomSpeed(float v);
    void setRotationSpeed(float v);
    void setZoomCameraLimit(float v);
    void setWindowSize(const QSize &v);

Q_SIGNALS:
    void cameraChanged();
    void panSpeedChanged();
    void zoomSpeedChanged();
    void rotationSpeedChanged();
    void zoomCameraLimitChanged();
    void windowSizeChanged();

private:
    void onTriggered(float);
    QVector3D tranformCameraLocalToWorld(const QVector3D &vec);
    void zoomCamera(float amount, Qt3DRender::QCamera::CameraTranslationOption translationOption);
    void rotateCamera(float yAxisRotationAmt, float elevationRotationAmt, const QVector3D &initialViewVector);
    void translateCameraXYByPercentOfScreen(const QVector3D &originalCameraPos, float percentX, float percentY);

    bool isMouseOrbiting() const;
    bool isMouseTranslating() const;
    bool isMouseZooming() const;

    Qt3DRender::QCamera *m_camera;

    Qt3DInput::QAction *m_leftMouseButtonAction;
    Qt3DInput::QAction *m_middleMouseButtonAction;
    Qt3DInput::QAction *m_rightMouseButtonAction;
    Qt3DInput::QAction *m_altKeyAction;
    Qt3DInput::QAction *m_shiftKeyAction;
    Qt3DInput::QAction *m_escapeKeyAction;
    Qt3DInput::QAction *m_changeProjectionAction;

    Qt3DInput::QAxis *m_mouseAxisX;
    Qt3DInput::QAxis *m_mouseAxisY;
    Qt3DInput::QAxis *m_keyboardAxisX;
    Qt3DInput::QAxis *m_keyboardAxisY;
    Qt3DInput::QAxis *m_keyboardAxisZ;
    Qt3DInput::QAxis *m_mouseWheelAxis;

    Qt3DInput::QMouseDevice *m_mouseDevice;
    Qt3DInput::QKeyboardDevice *m_keyboardDevice;
    Qt3DInput::QMouseHandler *m_mouseHandler;
    Qt3DInput::QLogicalDevice *m_logicalDevice;
    Qt3DLogic::QFrameAction *m_frameAction;

    QPoint m_mousePressedPosition;
    QPoint m_mouseCurrentPosition;

    QVector3D m_pressedCameraPosition;

    QSize m_windowSize;

    // Movement speed control
    float m_panSpeed;
    float m_zoomSpeed;
    float m_rotationSpeed;
    float m_zoomCameraLimit;

    // holds whether the mouse buttons are set for panning to be active.
    bool m_isTranslationActive;
};

QT_END_NAMESPACE

#endif // ORBITCAMERACONTROLLER_H
