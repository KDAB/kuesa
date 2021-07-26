/*
    placeholdertracker.cpp

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

#include "placeholdertracker.h"
#include <Qt3DCore/QTransform>
#include <kuesa_utils_p.h>
#include <Qt3DCore/private/qmath3d_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/logging_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::PlaceholderTracker
    \brief PlaceholderTracker allows watching a placeholder for changes and
    computing a screen position changes given a window size and a camera.

    \inmodule Kuesa
    \since Kuesa 1.3
    \inherits Kuesa::KuesaNode

    PlaceholderTracker allows watching a placeholder for changes and
    computing a screen position changes given a window size and a camera.

    This is especially useful to easily map 2D content based on a 3D
    placeholder. It does so by projecting a plane defined by the \l
    {Kuesa::Placeholder} facing the camera to screen space. setting the x, y,
    width and height properties of a target QObject to match the projected
    rectangle

    \badcode
    Kuesa::SceneEntity *sceneEntity = new Kuesa::SceneEntity();
    Kuesa::PlaceholderTracker *tracker = new Kuesa::PlaceholderTracker();
    Qt3DRender::QCamera *camera = new Qt3DRender::Camera();
    QObject *target = new QObject(); // Our UI target

    tracker->setSceneEntity(sceneEntity);
    tracker->setName(QStringLiteral("MyPlaceholder");
    tracker->setCamera(camera);
    tracker->setScreenSize({512, 512});
    tracker->setTarget(target);
    \endcode

    Read more about \l {Kuesa Placeholders}.
    \sa Kuesa::Placeholder
*/

/*!
    \qmltype PlaceholderTracker
    \inqmlmodule Kuesa
    \since Kuesa 1.3
    \instantiates Kuesa::PlaceholderTracker
    \brief PlaceholderTracker allows watching a placeholder for changes and
    computing a screen position changes given a window size and a camera.

    PlaceholderTracker allows watching a placeholder for changes and
    computing a screen position changes given a window size and a camera.

    This is especially useful to easily map 2D content based on a 3D
    placeholder. It does so by projecting a plane defined by the \l
    {Kuesa::Placeholder} facing the camera to screen space. setting the x, y,
    width and height properties of a target QObject to match the projected
    rectangle

    \badcode
    Item {
        id: uiTarget
    }

    ...

    SceneEntity {
        id: sceneEntity

        Camera {
            id: camera
        }

        PlaceholderTracker {
            name: "MyPlaceholder"
            camera: camera
            screenSize: Qt.size(512, 512)
            target: uiTarget
        }
    }
    \endcode

    Read more about \l {Kuesa Placeholders}.
    \sa Kuesa::Placeholder
*/

PlaceholderTracker::PlaceholderTracker(Qt3DCore::QNode *parent)
    : KuesaNode(parent)
{
    connect(this, &KuesaNode::sceneEntityChanged,
            this, [this] {
                disconnect(m_loadingDoneConnection);
                if (m_sceneEntity)
                    m_loadingDoneConnection = connect(m_sceneEntity, &SceneEntity::loadingDone, this, &PlaceholderTracker::matchNode);
                matchNode();
            });
}

PlaceholderTracker::~PlaceholderTracker()
{
}

/*!
    \property Kuesa::PlaceholderTracker::name

    Holds the name of the Placeholder instance to retrieve from the
    Kuesa::SceneEntity.
*/

/*!
    \qmlproperty string Kuesa::PlaceholderTracker::name

    Holds the name of the Placeholder instance to retrieve from the
    SceneEntity.
*/
QString PlaceholderTracker::name() const
{
    return m_name;
}

void PlaceholderTracker::setName(const QString &name)
{
    if (name == m_name)
        return;
    m_name = name;
    matchNode();
    emit nameChanged(m_name);
}

/*!
    \property Kuesa::PlaceholderTracker::camera

    Holds the camera the placeholder is facing and which is used to project the
    plane to screen space.

    \badcode
    QQuickItem *target = view->rootItem();
    Kuesa::PlaceholderTracker *placeholderTracker = new Kuesa::PlaceholderTracker();
    placeholderTracker->setName(QStringLiteral("placeholder"));
    Qt3DRender::QCamera *camera = sceneEntity->camera(QStringLiteral("Camera_Orientation"));
    placeholderTracker->setSceneSize({view.width(), view.height()});
    placeholderTracker->setTarget(target);
    placeholderTracker->setCamera(camera);
    \endcode

    \note using a Qt3DRender::QCamera for the placeholderTracker different than
    the one being used for the render might give unexpected results.
*/

/*!
    \qmlproperty Qt3DCore.Entity Kuesa::PlaceholderTracker::camera

    Holds the camera the tracker is facing and which is used to project the
    plane to screen space.

    \badcode
    Rectangle {
        id: rect
    }

    Kuesa.Asset {
        id: mainCamera
        collection: scene.cameras
        name: "Camera_Orientation"
    }

    PlaceholderTracker {
        name: "MyPlaceholder"
        camera: camera
        screenSize: Qt.size(512, 512)
        target: uiTarget
    }
    \endcode

    \note using a camera for the tracker different than the one being used for
    the render might give unexpected results.
*/
Qt3DCore::QEntity *PlaceholderTracker::camera() const
{
    return m_camera;
}

void PlaceholderTracker::setCamera(Qt3DCore::QEntity *camera)
{
    if (camera != m_camera) {
        auto d = Qt3DCore::QNodePrivate::get(this);

        if (m_cameraTransform)
            QObject::disconnect(m_cameraTransform, &Qt3DCore::QTransform::worldMatrixChanged,
                                this, &PlaceholderTracker::updatePlaceholderProjection);
        if (m_cameraLens)
            QObject::disconnect(m_cameraLens, &Qt3DRender::QCameraLens::projectionMatrixChanged,
                                this, &PlaceholderTracker::updatePlaceholderProjection);
        if (m_camera) {
            d->unregisterDestructionHelper(m_camera);
            m_cameraTransform = nullptr;
            m_cameraLens = nullptr;
        }

        m_camera = camera;

        if (m_camera) {
            // Note: it is expected the camera is already parented

            d->registerDestructionHelper(m_camera, &PlaceholderTracker::setCamera, m_camera);

            m_cameraTransform = componentFromEntity<Qt3DCore::QTransform>(m_camera);
            m_cameraLens = componentFromEntity<Qt3DRender::QCameraLens>(m_camera);

            if (m_cameraTransform)
                QObject::connect(m_cameraTransform, &Qt3DCore::QTransform::worldMatrixChanged,
                                 this, &PlaceholderTracker::updatePlaceholderProjection);
            else
                qCWarning(kuesa) << "PlaceholderTracker camera has no transform component";

            if (m_cameraLens)
                QObject::connect(m_cameraLens, &Qt3DRender::QCameraLens::projectionMatrixChanged,
                                 this, &PlaceholderTracker::updatePlaceholderProjection);
            else
                qCWarning(kuesa) << "PlaceholderTracker camera has no camera lens component";

            updatePlaceholderProjection();
        }

        emit cameraChanged(camera);
    }
}

/*!
    \property Kuesa::PlaceholderTracker::screenSize

    Holds the size of the area within which we should compute screen positions.
    This would usually be the size of the window or the size of the sub view
    within the window.
*/

/*!
    \qmlproperty size Kuesa::PlaceholderTracker::screenSize

    Holds the size of the area within which we should compute screen positions.
    This would usually be the size of the window or the size of the sub view
    within the window.
*/
QSize PlaceholderTracker::screenSize() const
{
    return m_screenSize;
}

void PlaceholderTracker::setScreenSize(const QSize &screenSize)
{
    if (screenSize == m_screenSize)
        return;
    m_screenSize = screenSize;
    emit screenSizeChanged(m_screenSize);
    updatePlaceholderProjection();
}

/*!
    \property Kuesa::PlaceholderTracker::viewportRect

    Holds the normalized viewport rectangle defining the area into which
    screen positions should be computed.
    If unspecified QRectF(0.0f, 0.0f, 1.0f, 1.0f) will be used.
*/

/*!
    \qmlproperty rect Kuesa::PlaceholderTracker::viewportRect

    Holds the normalized viewport rectangle defining the area into which
    screen positions should be computed.
    If unspecified Qt.rect(0.0, 0.0, 1.0, 1.0) will be used.
*/
QRectF PlaceholderTracker::viewportRect() const
{
    return m_viewportRect;
}

void PlaceholderTracker::setViewportRect(const QRectF &viewportRect)
{
    if (viewportRect == m_viewportRect)
        return;
    m_viewportRect = viewportRect;
    emit viewportRectChanged(m_viewportRect);
    updatePlaceholderProjection();
}

void PlaceholderTracker::setViewportRect(qreal x, qreal y, qreal width, qreal height)
{
    setViewportRect({ x, y, width, height });
}

/*!
    \property Kuesa::PlaceholderTracker::target

    Holds the target object on which to set x, y, width and height properties
    computed from the Placeholder and Camera transformations.

    Commonly it would be a QtQuick element and should be used as

    \badcode
    QQuickItem *target = view->rootItem();
    Kuesa::PlaceholderTracker *placeholderTracker = new Kuesa::PlaceholderTracker();
    placeholder->setName(QStringLiteral("placeholder"));
    placeholder->setScreenSize({view.width(), view.height()});
    placeholder->setTarget(target);
    \endcode
*/

/*!
    \qmlproperty QtObject Kuesa::PlaceholderTracker::target

    Holds the target object on which to set x, y, width and height properties
    computed from the Placeholder and Camera transformations.

    Commonly it would be a QtQuick element and should be used as

    \badcode
    Rectangle {
        id: rect
    }

    PlaceholderTracker {
        name: "MyPlaceholder"
        camera: camera
        screenSize: Qt.size(512, 512)
        target: uiTarget
    }
    \endcode

*/
QObject *PlaceholderTracker::target() const
{
    return m_target;
}

void PlaceholderTracker::setTarget(QObject *target)
{
    if (m_target != target) {
        m_target = target;
        QObject::disconnect(m_targetDestroyedConnection);

        emit targetChanged(target);

        if (m_target) {
            updatePlaceholderProjection();
            m_targetDestroyedConnection = QObject::connect(m_target, &QObject::destroyed, [this]() { setTarget(nullptr); });
        }
    }
}

/*!
    \property Kuesa::PlaceholderTracker::x

    Holds the top left x screen position coordinate computed from the placeholder.
*/

/*!
    \qmlproperty real Kuesa::PlaceholderTracker::x

    Holds the top left x screen position coordinate computed from the placeholder.
*/
int PlaceholderTracker::x() const
{
    return m_x;
}

/*!
    \property Kuesa::PlaceholderTracker::y

    Holds the top left y screen position coordinate computed from the placeholder.
*/

/*!
    \qmlproperty real Kuesa::PlaceholderTracker::y

    Holds the top left y screen position coordinate computed from the placeholder.
*/
int PlaceholderTracker::y() const
{
    return m_y;
}

/*!
    \property Kuesa::PlaceholderTracker::width

    Holds the width in screen coordinates computed from the placeholder.
*/

/*!
    \qmlproperty real Kuesa::PlaceholderTracker::width

    Holds the width in screen coordinates computed from the placeholder.
*/
int PlaceholderTracker::width() const
{
    return m_width;
}

/*!
    \property Kuesa::PlaceholderTracker::height

    Holds the height in screen coordinates computed from the placeholder.
*/

/*!
    \qmlproperty real Kuesa::PlaceholderTracker::height

    Holds the height in screen coordinates computed from the placeholder.
*/
int PlaceholderTracker::height() const
{
    return m_height;
}

/*!
    \property Kuesa::PlaceholderTracker::screenPosition

    Holds the screenPosition computed from the placeholder.
*/

/*!
    \qmlproperty real Kuesa::PlaceholderTracker::screenPosition

    Holds the screenPosition computed from the placeholder.
*/
QPointF PlaceholderTracker::screenPosition() const
{
    return { float(m_x), float(m_y) };
}

/*!
 * \internal
 */
void PlaceholderTracker::matchNode()
{
    Kuesa::Placeholder *placeholder = m_sceneEntity ? m_sceneEntity->placeholder(m_name) : nullptr;
    if (placeholder == m_placeHolder)
        return;

    if (m_placeHolder)
        m_placeHolder->disconnect(this);
    if (m_placeHolderTransform)
        m_placeHolderTransform->disconnect(this);

    m_placeHolder = placeholder;
    m_placeHolderTransform = nullptr;

    if (m_placeHolder) {
        m_placeHolderTransform = componentFromEntity<Qt3DCore::QTransform>(m_placeHolder);

        connect(m_placeHolderTransform, &Qt3DCore::QTransform::worldMatrixChanged, this, &PlaceholderTracker::updatePlaceholderProjection);
        connect(m_placeHolder, &Qt3DCore::QNode::nodeDestroyed, this, [this] { matchNode(); });
    }

    // Emit signal changes for each property of the transform we proxy
    // to ensure any binding gets reevaluated with up to data values
    emit xChanged(x());
    emit yChanged(y());
    emit widthChanged(width());
    emit heightChanged(height());
    emit screenPositionChanged(screenPosition());

    updatePlaceholderProjection();
}

/*!
 * \internal
 */
void PlaceholderTracker::updatePlaceholderProjection()
{
    if (m_camera && m_placeHolderTransform && m_cameraTransform && m_cameraLens) {
        QVector3D position;
        QQuaternion orientation;
        QVector3D scale;
        decomposeQMatrix4x4(m_placeHolderTransform->worldMatrix(), position, orientation, scale);
        const auto aLocal = QVector3D{ -qAbs(scale.x()), -qAbs(scale.z()), 0 };
        const auto bLocal = QVector3D{ qAbs(scale.x()), qAbs(scale.z()), 0 };

        const auto centerWorldSpace = m_placeHolderTransform->worldMatrix() * QVector3D(0, 0, 0);

        const auto getWorldSpaceForPoint = [centerWorldSpace, this](QVector3D p) {
            return centerWorldSpace + m_cameraTransform->worldMatrix() * p - m_cameraTransform->worldMatrix() * QVector3D(0, 0, 0);
        };

        const auto aWorldSpace = getWorldSpaceForPoint(aLocal);
        const auto bWorldSpace = getWorldSpaceForPoint(bLocal);

        const auto viewMatrix = m_cameraTransform->worldMatrix().inverted();

        QRect viewport{ 0, 0, m_screenSize.width(), m_screenSize.height() };
        if (m_viewportRect.isValid()) {
            viewport.setX(int(qreal(m_screenSize.width()) * m_viewportRect.x()));
            viewport.setY(int(qreal(m_screenSize.height()) * m_viewportRect.y()));
            viewport.setWidth(int(qreal(m_screenSize.width()) * m_viewportRect.width()));
            viewport.setHeight(int(qreal(m_screenSize.height()) * m_viewportRect.height()));
        }

        const QVector3D aViewportSpace = aWorldSpace.project(viewMatrix,
                                                             m_cameraLens->projectionMatrix(),
                                                             viewport);

        const QVector3D bViewportSpace = bWorldSpace.project(viewMatrix,
                                                             m_cameraLens->projectionMatrix(),
                                                             viewport);

        const auto a = aViewportSpace.toPoint();
        const auto b = bViewportSpace.toPoint();
        const int width = (b - a).x();
        const int height = (b - a).y();

        setX(a.x());
        setY(m_screenSize.height() - a.y() - height + viewport.y());
        setWidth(width);
        setHeight(height);

        if (m_target) {
            m_target->setProperty("x", m_x);
            m_target->setProperty("y", m_y);
            m_target->setProperty("width", m_width);
            m_target->setProperty("height", m_height);
        }
    }
}

/*!
 * \internal
 */
void PlaceholderTracker::setX(int x)
{
    if (x == m_x)
        return;
    m_x = x;
    emit xChanged(x);
}

/*!
 * \internal
 */
void PlaceholderTracker::setY(int y)
{
    if (y == m_y)
        return;
    m_y = y;
    emit yChanged(y);
}

/*!
 * \internal
 */
void PlaceholderTracker::setWidth(int width)
{
    if (width == m_width)
        return;
    m_width = width;
    emit widthChanged(width);
}

/*!
 * \internal
 */
void PlaceholderTracker::setHeight(int height)
{
    if (height == m_height)
        return;
    m_height = height;
    emit heightChanged(height);
}

} // namespace Kuesa

QT_END_NAMESPACE
