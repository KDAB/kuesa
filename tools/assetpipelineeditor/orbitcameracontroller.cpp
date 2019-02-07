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

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // For MSVC
#endif

#include "orbitcameracontroller.h"
#include <Qt3DRender/QCamera>
#include <Qt3DInput/QAction>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DInput/QKeyboardHandler>
#include <Qt3DInput/QButtonAxisInput>
#include <Qt3DLogic/QFrameAction>
#include <Qt3DInput/QLogicalDevice>
#include <QtCore/QtGlobal>
#include <Qt3DCore/QTransform>
#include <QVector3D>
#include <cmath>

QT_BEGIN_NAMESPACE

OrbitCameraController::OrbitCameraController(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_camera(nullptr)
    , m_leftMouseButtonAction(new Qt3DInput::QAction())
    , m_middleMouseButtonAction(new Qt3DInput::QAction())
    , m_rightMouseButtonAction(new Qt3DInput::QAction())
    , m_altKeyAction(new Qt3DInput::QAction())
    , m_shiftKeyAction(new Qt3DInput::QAction())
    , m_escapeKeyAction(new Qt3DInput::QAction())
    , m_changeProjectionAction(new Qt3DInput::QAction())
    , m_mouseAxisX(new Qt3DInput::QAxis())
    , m_mouseAxisY(new Qt3DInput::QAxis())
    , m_keyboardAxisX(new Qt3DInput::QAxis())
    , m_keyboardAxisY(new Qt3DInput::QAxis())
    , m_keyboardAxisZ(new Qt3DInput::QAxis())
    , m_mouseWheelAxis(new Qt3DInput::QAxis())
    , m_mouseDevice(new Qt3DInput::QMouseDevice())
    , m_keyboardDevice(new Qt3DInput::QKeyboardDevice())
    , m_mouseHandler(new Qt3DInput::QMouseHandler())
    , m_logicalDevice(new Qt3DInput::QLogicalDevice())
    , m_frameAction(new Qt3DLogic::QFrameAction())
    , m_windowSize(QSize(1920, 1080))
    , m_panSpeed(1.0)
    , m_zoomSpeed(1.0)
    , m_rotationSpeed(1.0)
    , m_zoomCameraLimit(0.0)
    , m_isTranslationActive(false)
{
    //// Actions

    // Left Mouse Button Action
    auto leftMouseButtonInput = new Qt3DInput::QActionInput();
    leftMouseButtonInput->setButtons(QVector<int>() << Qt::LeftButton);
    leftMouseButtonInput->setSourceDevice(m_mouseDevice);
    m_leftMouseButtonAction->addInput(leftMouseButtonInput);

    // Middle Mouse Button Action
    auto middleMouseButtonInput = new Qt3DInput::QActionInput();
    middleMouseButtonInput->setButtons(QVector<int>() << Qt::MiddleButton);
    middleMouseButtonInput->setSourceDevice(m_mouseDevice);
    m_middleMouseButtonAction->addInput(middleMouseButtonInput);

    // Right Mouse Button Action
    auto rightMouseButtonInput = new Qt3DInput::QActionInput();
    rightMouseButtonInput->setButtons(QVector<int>() << Qt::RightButton);
    rightMouseButtonInput->setSourceDevice(m_mouseDevice);
    m_rightMouseButtonAction->addInput(rightMouseButtonInput);

    // Alt Button Action
    auto altKeyInput = new Qt3DInput::QActionInput();
    altKeyInput->setButtons(QVector<int>() << Qt::Key_Alt);
    altKeyInput->setSourceDevice(m_keyboardDevice);
    m_altKeyAction->addInput(altKeyInput);

    // Shift Button Action
    auto shiftKeyInput = new Qt3DInput::QActionInput();
    shiftKeyInput->setButtons(QVector<int>() << Qt::Key_Shift);
    shiftKeyInput->setSourceDevice(m_keyboardDevice);
    m_shiftKeyAction->addInput(shiftKeyInput);

    // Escape Button Action
    auto escapeKeyInput = new Qt3DInput::QActionInput();
    escapeKeyInput->setButtons(QVector<int>() << Qt::Key_Escape);
    escapeKeyInput->setSourceDevice(m_keyboardDevice);
    m_escapeKeyAction->addInput(escapeKeyInput);

    //Projection control
    auto changeProjectionTypeInput = new Qt3DInput::QActionInput();
    changeProjectionTypeInput->setButtons(QVector<int>() << Qt::Key_5);
    changeProjectionTypeInput->setSourceDevice(m_keyboardDevice);
    m_changeProjectionAction->addInput(changeProjectionTypeInput);

    //// Axes

    // Mouse X
    auto mouseRxInput = new Qt3DInput::QAnalogAxisInput();
    mouseRxInput->setAxis(Qt3DInput::QMouseDevice::X);
    mouseRxInput->setSourceDevice(m_mouseDevice);
    m_mouseAxisX->addInput(mouseRxInput);

    // Mouse Y
    auto mouseRyInput = new Qt3DInput::QAnalogAxisInput();
    mouseRyInput->setAxis(Qt3DInput::QMouseDevice::Y);
    mouseRyInput->setSourceDevice(m_mouseDevice);
    m_mouseAxisY->addInput(mouseRyInput);

    auto mouseWheelXInput = new Qt3DInput::QAnalogAxisInput();
    // Mouse Wheel X
    mouseWheelXInput->setAxis(Qt3DInput::QMouseDevice::WheelX);
    mouseWheelXInput->setSourceDevice(m_mouseDevice);
    m_mouseWheelAxis->addInput(mouseWheelXInput);

    // Mouse Wheel Y
    auto mouseWheelYInput = new Qt3DInput::QAnalogAxisInput();
    mouseWheelYInput->setAxis(Qt3DInput::QMouseDevice::WheelY);
    mouseWheelYInput->setSourceDevice(m_mouseDevice);
    m_mouseWheelAxis->addInput(mouseWheelYInput);

    // Keyboard X
    auto keyboardXPosInput = new Qt3DInput::QButtonAxisInput();
    keyboardXPosInput->setButtons(QVector<int>() << Qt::Key_Right);
    keyboardXPosInput->setScale(1.0f);
    keyboardXPosInput->setSourceDevice(m_keyboardDevice);
    m_keyboardAxisX->addInput(keyboardXPosInput);

    auto keyboardXNegInput = new Qt3DInput::QButtonAxisInput();
    keyboardXNegInput->setButtons(QVector<int>() << Qt::Key_Left);
    keyboardXNegInput->setScale(-1.0f);
    keyboardXNegInput->setSourceDevice(m_keyboardDevice);
    m_keyboardAxisX->addInput(keyboardXNegInput);

    // Keyboard Y
    auto keyboardYPosInput = new Qt3DInput::QButtonAxisInput();
    keyboardYPosInput->setButtons(QVector<int>() << Qt::Key_Up);
    keyboardYPosInput->setScale(1.0f);
    keyboardYPosInput->setSourceDevice(m_keyboardDevice);
    m_keyboardAxisY->addInput(keyboardYPosInput);

    auto keyboardYNegInput = new Qt3DInput::QButtonAxisInput();
    keyboardYNegInput->setButtons(QVector<int>() << Qt::Key_Down);
    keyboardYNegInput->setScale(-1.0f);
    keyboardYNegInput->setSourceDevice(m_keyboardDevice);
    m_keyboardAxisY->addInput(keyboardYNegInput);

    // Keyboard Z
    auto keyboardZPosInput = new Qt3DInput::QButtonAxisInput();
    keyboardZPosInput->setButtons(QVector<int>() << Qt::Key_PageUp);
    keyboardZPosInput->setScale(1.0f);
    keyboardZPosInput->setSourceDevice(m_keyboardDevice);
    m_keyboardAxisZ->addInput(keyboardZPosInput);

    auto keyboardZNegInput = new Qt3DInput::QButtonAxisInput();
    keyboardZNegInput->setButtons(QVector<int>() << Qt::Key_PageDown);
    keyboardZNegInput->setScale(-1.0f);
    keyboardZNegInput->setSourceDevice(m_keyboardDevice);
    m_keyboardAxisZ->addInput(keyboardZNegInput);

    //// Logical Device

    m_logicalDevice->addAction(m_leftMouseButtonAction);
    m_logicalDevice->addAction(m_middleMouseButtonAction);
    m_logicalDevice->addAction(m_rightMouseButtonAction);
    m_logicalDevice->addAction(m_altKeyAction);
    m_logicalDevice->addAction(m_shiftKeyAction);
    m_logicalDevice->addAction(m_escapeKeyAction);
    m_logicalDevice->addAxis(m_mouseAxisX);
    m_logicalDevice->addAxis(m_mouseAxisY);
    m_logicalDevice->addAxis(m_keyboardAxisX);
    m_logicalDevice->addAxis(m_keyboardAxisY);
    m_logicalDevice->addAxis(m_keyboardAxisZ);
    m_logicalDevice->addAxis(m_mouseWheelAxis);

    //// Mouse handler
    auto mouseHandler = new Qt3DInput::QMouseHandler();
    mouseHandler->setSourceDevice(m_mouseDevice);

    QObject::connect(mouseHandler, &Qt3DInput::QMouseHandler::pressed,
                     [this](Qt3DInput::QMouseEvent *pressedEvent) {
                         pressedEvent->setAccepted(true);
                         this->m_mousePressedPosition = QPoint(pressedEvent->x(),
                                                               pressedEvent->y());
                         this->m_mouseCurrentPosition = this->m_mousePressedPosition;
                         this->m_pressedCameraPosition = this->m_camera->position();

                         // translation active if ONLY right button is pressed at the start
                         m_isTranslationActive = pressedEvent->buttons() == Qt3DInput::QMouseEvent::RightButton;
#ifdef Q_OS_MACOS
                         if (pressedEvent->buttons() == Qt3DInput::QMouseEvent::LeftButton &&
                             pressedEvent->modifiers() == Qt3DInput::QMouseEvent::MetaModifier)
                             this->m_isTranslationActive = true;
#endif
                     });

    QObject::connect(mouseHandler, &Qt3DInput::QMouseHandler::released,
                     [this](Qt3DInput::QMouseEvent *released) {
                         // turn off translation when any button is released.
                         this->m_isTranslationActive = false;
                         released->setAccepted(true);
                     });

    QObject::connect(mouseHandler, &Qt3DInput::QMouseHandler::positionChanged,
                     [this](Qt3DInput::QMouseEvent *positionChangedEvent) {
                         positionChangedEvent->setAccepted(true);
                         this->m_mouseCurrentPosition = QPoint(positionChangedEvent->x(),
                                                               positionChangedEvent->y());
                     });

    QObject::connect(m_changeProjectionAction, &Qt3DInput::QAction::activeChanged,
                     [this](bool isActive) {
                         if (isActive) {
                             Qt3DRender::QCamera *camera = this->m_camera;
                             Qt3DRender::QCameraLens::ProjectionType projectionType = camera->projectionType();
                             switch (projectionType) {
                             case Qt3DRender::QCameraLens::PerspectiveProjection:
                                 camera->setProjectionType(Qt3DRender::QCameraLens::OrthographicProjection);
                                 break;
                             case Qt3DRender::QCameraLens::OrthographicProjection:
                                 camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
                                 break;
                             default:
                                 break;
                             }
                         }
                     });

    //// FrameAction

    QObject::connect(m_frameAction, &Qt3DLogic::QFrameAction::triggered, this,
                     [this](float dt) {
                         this->onTriggered(dt);
                     });

    QObject::connect(m_escapeKeyAction, &Qt3DInput::QAction::activeChanged,
                     this, [this](bool isActive) {
                         if (isActive && m_camera)
                             m_camera->viewAll();
                     });

    // Disable the logical device when the entity is disabled
    QObject::connect(this, &Qt3DCore::QEntity::enabledChanged,
                     m_logicalDevice, &Qt3DInput::QLogicalDevice::setEnabled);

    addComponent(m_frameAction);
    addComponent(m_logicalDevice);
    addComponent(mouseHandler);
}

Qt3DRender::QCamera *OrbitCameraController::camera() const
{
    return m_camera;
}

void OrbitCameraController::setCamera(Qt3DRender::QCamera *camera)
{
    if (m_camera != camera) {

        if (m_camera)
            disconnect(m_camera);

        if (camera && !camera->parent())
            camera->setParent(this);

        m_camera = camera;

        if (m_camera)
            connect(m_camera, &Qt3DCore::QNode::nodeDestroyed, this, [this]() {
                setCamera(nullptr);
            });

        emit cameraChanged();
    }
}

float OrbitCameraController::panSpeed() const
{
    return m_panSpeed;
}

float OrbitCameraController::zoomSpeed() const
{
    return m_zoomSpeed;
}

float OrbitCameraController::rotationSpeed() const
{
    return m_rotationSpeed;
}

float OrbitCameraController::zoomCameraLimit() const
{
    return m_zoomCameraLimit;
}

QSize OrbitCameraController::windowSize() const
{
    return m_windowSize;
}

void OrbitCameraController::setPanSpeed(float v)
{
    if (!qFuzzyCompare(m_panSpeed, v)) {
        m_panSpeed = v;
        emit panSpeedChanged();
    }
}

void OrbitCameraController::setZoomSpeed(float v)
{
    if (!qFuzzyCompare(m_zoomSpeed, v)) {
        m_zoomSpeed = v;
        emit zoomSpeedChanged();
    }
}

void OrbitCameraController::setRotationSpeed(float v)
{
    if (qFuzzyCompare(m_rotationSpeed, v)) {
        m_rotationSpeed = v;
        emit rotationSpeedChanged();
    }
}

void OrbitCameraController::setZoomCameraLimit(float v)
{
    if (!qFuzzyCompare(m_zoomCameraLimit, v)) {
        m_zoomCameraLimit = v;
        emit zoomCameraLimitChanged();
    }
}

inline float clampInputs(float input1, float input2)
{
    float axisValue = input1 + input2;
    return (axisValue < -1) ? -1 : (axisValue > 1) ? 1 : axisValue;
}

inline float clamp(float value, float min, float max)
{
    if (value > max)
        value = max;
    if (value < min)
        value = min;
    return value;
}

void OrbitCameraController::setWindowSize(const QSize &v)
{
    if (m_windowSize != v) {
        m_windowSize = v;
        emit windowSizeChanged();
    }
}

bool OrbitCameraController::isMouseOrbiting() const
{
    return m_leftMouseButtonAction->isActive();
}

bool OrbitCameraController::isMouseTranslating() const
{
    return m_isTranslationActive;
}

bool OrbitCameraController::isMouseZooming() const
{
    return (m_middleMouseButtonAction->isActive() || (m_leftMouseButtonAction->isActive() && m_rightMouseButtonAction->isActive()));
}

QVector3D OrbitCameraController::tranformCameraLocalToWorld(const QVector3D &vec)
{
    const auto viewVector = m_camera->viewVector();
    // Calculate the amount to move by in world coordinates
    QVector3D vWorld;
    if (!qFuzzyIsNull(vec.x())) {
        // Calculate the vector for the local x axis
        const QVector3D x = QVector3D::crossProduct(viewVector, m_camera->upVector()).normalized();
        vWorld += vec.x() * x;
    }

    if (!qFuzzyIsNull(vec.y()))
        vWorld += vec.y() * m_camera->upVector();

    if (!qFuzzyIsNull(vec.z()))
        vWorld += vec.z() * viewVector.normalized();

    return vWorld;
}

// takes normal axis amount from -1 to 1.0
void OrbitCameraController::zoomCamera(float amount, Qt3DRender::QCamera::CameraTranslationOption translationOption)
{
    if (amount < -1.0f || amount > 1.0f)
        return;

    // double the default zoom to make it feel about right
    float zoomFactor = 2.f * m_zoomSpeed * amount;

    const float length = m_camera->viewVector().length();
    float deltaLength = -1.f * zoomFactor * length;
    if (length + deltaLength < m_zoomCameraLimit && translationOption == Qt3DRender::QCamera::DontTranslateViewCenter)
        return;
    m_camera->translate(QVector3D(0.f, 0.f, -deltaLength), translationOption);
}

//start with original view vector every time and rotate that to new orientation
void OrbitCameraController::rotateCamera(float yAxisRotationAmt, float elevationRotationAmt, const QVector3D &initialViewVector)
{
    const QVector3D upVector(0.0f, 1.0f, 0.0f);
    const auto yRotationQuat = QQuaternion::fromAxisAndAngle(upVector, yAxisRotationAmt);

    const float initialElevationAngle = asin(-initialViewVector.y() / initialViewVector.length()) * 180.0f / M_PI;
    //fix this...breaks if we go to 90 degrees
    const float newElevationAngle = clamp(initialElevationAngle + elevationRotationAmt, -89, 89);
    const float xRotationAmount = newElevationAngle - initialElevationAngle;

    const auto viewVecAfterYAxisRotation = yRotationQuat * initialViewVector;
    const QVector3D xBasis = QVector3D::crossProduct(upVector, viewVecAfterYAxisRotation.normalized()).normalized();
    auto xRotationQuat = QQuaternion::fromAxisAndAngle(xBasis, xRotationAmount);

    auto cameraToCenter2 = xRotationQuat * yRotationQuat * initialViewVector;
    m_camera->setPosition(m_camera->viewCenter() - cameraToCenter2);
    m_camera->setViewCenter(m_camera->position() + cameraToCenter2);
    m_camera->setUpVector(xRotationQuat * upVector);
}

void OrbitCameraController::translateCameraXYByPercentOfScreen(const QVector3D &originalCameraPos, float percentX, float percentY)
{
    // Calculate size of view frustum at camera view center.  We'll translate perpendicular to this plane.
    const float worldHeightAtViewCenter = m_camera->viewVector().length() * tan(m_camera->fieldOfView() * M_PI / 180.0);
    const float wordWidthAtViewCenter = worldHeightAtViewCenter * m_camera->aspectRatio();

    // determine how far the mouse has traveled in plane perp to camera at view center since mouse down
    // take percent mouse has moved in screen and apply to world dimensions from above
    // This is how much we need to translate the camera
    const QVector3D cameraLocalTranslationVec(wordWidthAtViewCenter * percentX, worldHeightAtViewCenter * percentY, 0.0f);

    // transform this transaltion from camera-local coords to world coordinates
    auto worldTransformVec = tranformCameraLocalToWorld(cameraLocalTranslationVec * m_panSpeed);

    // move the camera relative to it's starting position to reduce accumated transform error
    // so moving mouse back to initial press position returns camera exactly to initial position.
    QVector3D translatedCameraPos = originalCameraPos + worldTransformVec;
    m_camera->translateWorld(translatedCameraPos - m_camera->position(), Qt3DRender::QCamera::TranslateViewCenter);
}

void OrbitCameraController::onTriggered(float dt)
{
    if (m_camera == nullptr)
        return;

    // By default all zooming leaves view center.  Shift key enables moving view center.
    const auto zoomTranslationOption = m_shiftKeyAction->isActive() ? Qt3DRender::QCamera::TranslateViewCenter : Qt3DRender::QCamera::DontTranslateViewCenter;

    // Handle Mouse

    const QPointF mouseDelta = m_mouseCurrentPosition - m_mousePressedPosition;

    if (isMouseZooming()) {
        zoomCamera(m_mouseAxisY->value() * dt, zoomTranslationOption);
        return;
    } else if (isMouseTranslating()) {
        translateCameraXYByPercentOfScreen(m_pressedCameraPosition, -mouseDelta.x() / m_windowSize.width(), mouseDelta.y() / m_windowSize.height());
        return;
    } else if (isMouseOrbiting()) {
        const float yAxisRotationAmt = -mouseDelta.x() / m_windowSize.width() * m_rotationSpeed * 180.0f;
        const float elevationRotationAmt = mouseDelta.y() / m_windowSize.height() * m_rotationSpeed * 180.0f;
        const QVector3D origViewVector = m_camera->viewCenter() - m_pressedCameraPosition;
        rotateCamera(yAxisRotationAmt, elevationRotationAmt, origViewVector);
        return;
    }

    // Handle keyboard

    // scale keyboard inputs down a bit.  Mouse ranges from .1 to 1, keyboard values are always 1
    const float keyboardMotionScale = 0.25f * dt;
    const bool hasKeyboardXYInput = !qFuzzyCompare(m_keyboardAxisX->value(), 0.0f) || !qFuzzyCompare(m_keyboardAxisY->value(), 0.0f);
    if (hasKeyboardXYInput) {
        if (m_altKeyAction->isActive()) {
            rotateCamera(-m_keyboardAxisX->value(), -m_keyboardAxisY->value(), m_camera->viewVector());
        } else {
            translateCameraXYByPercentOfScreen(m_camera->position(), -keyboardMotionScale * m_keyboardAxisX->value(), -keyboardMotionScale * m_keyboardAxisY->value());
        }
    }
    // zoom via keyboard or mouse wheel
    if (!qFuzzyCompare(m_keyboardAxisZ->value() + m_mouseWheelAxis->value(), 0.0f)) {
        zoomCamera(clampInputs(m_keyboardAxisZ->value() * keyboardMotionScale, m_mouseWheelAxis->value() * dt), zoomTranslationOption);
    }
}

QT_END_NAMESPACE
