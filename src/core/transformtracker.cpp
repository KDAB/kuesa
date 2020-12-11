/*
    transformtracker.cpp

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

#include "transformtracker.h"
#include <Kuesa/private/kuesa_utils_p.h>
#include <Qt3DCore/private/qnode_p.h>

using namespace Kuesa;

namespace {

inline QMatrix4x4 computeWorldMatrix(Qt3DCore::QTransform *transformComponent)
{
    if (!transformComponent || transformComponent->entities().empty())
        return {};

    return transformComponent->worldMatrix();
}

} // namespace

/*!
    \class Kuesa::TransformTracker
    \brief TransformTracker allows watching a transform for change and computing
    a projected screen position given a window size and a camera.

    \inmodule Kuesa
    \since Kuesa 1.3
    \inherits Kuesa::KuesaNode

    TransformTracker allows watching a transform for change and computing
    a projected screen position given a window size and a camera.

    This is especially useful to place 2D content based on a 3D position.

    \badcode
    Kuesa::SceneEntity *sceneEntity = new Kuesa::SceneEntity();
    Kuesa::TransformTracker *tracker = new Kuesa::TransformTracker();
    Qt3DRender::QCamera *camera = new Qt3DRender::Camera();


    tracker->setSceneEntity(sceneEntity);
    tracker->setName(QStringLiteral("MyTransform");
    tracker->setCamera(camera);
    tracker->setScreenSize({512, 512});

    QObject::connect(tracker, &Kuesa::TransformTracker::screenPositionChanged,
                     this, [this] (const QPointF &screenPosition) {
                        // Reacts to new position
                        ....
                     });
    \endcode
*/

/*!
    \qmltype TransformTracker
    \brief TransformTracker allows watching a transform for change and computing
    a projected screen position given a window size and a camera.
    \inqmlmodule Kuesa
    \since Kuesa 1.3
    \instantiates Kuesa::TransformTracker

    TransformTracker allows watching a transform for change and computing
    a projected screen position given a window size and a camera.

    This is especially useful to place 2D content based on a 3D position.

    \badcode
    SceneEntity {
        id: sceneEntity

        Camera {
            id: camera
        }

        TransformTracker {
            name: "MyTransform"
            camera: camera
            screenSize: Qt.size(512, 512)
            onScreenPositionChanged: {
                // Reacts to new position
                ...
            }
        }
    }
    \endcode
*/
TransformTracker::TransformTracker(Qt3DCore::QNode *parent)
    : KuesaNode(parent)
    , m_camera(nullptr)
    , m_cameraTransform(nullptr)
    , m_cameraLens(nullptr)
    , m_node(nullptr)
{
    connect(this, &KuesaNode::sceneEntityChanged,
            this, [this] {
                disconnect(m_loadingDoneConnection);
                if (m_sceneEntity)
                    m_loadingDoneConnection = connect(m_sceneEntity, &SceneEntity::loadingDone, this, &TransformTracker::matchNode);
                matchNode();
            });
}

TransformTracker::~TransformTracker() = default;

/*!
    \property Kuesa::TransformTracker::camera

    Holds the camera entity from which we should compute the screen position.
*/

/*!
    \qmlproperty Entity Kuesa::TransformTracker::camera

    Holds the camera entity from which we should compute the screen position.
*/
Qt3DCore::QEntity *TransformTracker::camera() const
{
    return m_camera;
}

void TransformTracker::setCamera(Qt3DCore::QEntity *camera)
{
    if (camera != m_camera) {
        auto d = Qt3DCore::QNodePrivate::get(this);

        if (m_camera) {
            if (m_cameraTransform)
                QObject::disconnect(m_cameraTransform, &Qt3DCore::QTransform::worldMatrixChanged,
                                    this, &TransformTracker::updateScreenProjection);
            if (m_cameraLens)
                QObject::disconnect(m_cameraLens, &Qt3DRender::QCameraLens::projectionMatrixChanged,
                                    this, &TransformTracker::updateScreenProjection);

            d->unregisterDestructionHelper(m_camera);
            m_cameraLens = nullptr;
            m_cameraTransform = nullptr;
        }

        m_camera = camera;
        emit cameraChanged(m_camera);

        if (m_camera) {
            if (!m_camera->parent())
                m_camera->setParent(this);
            d->registerDestructionHelper(m_camera, &TransformTracker::setCamera, m_camera);

            m_cameraTransform = componentFromEntity<Qt3DCore::QTransform>(m_camera);
            m_cameraLens = componentFromEntity<Qt3DRender::QCameraLens>(m_camera);

            if (m_cameraTransform)
                QObject::connect(m_cameraTransform, &Qt3DCore::QTransform::worldMatrixChanged,
                                 this, &TransformTracker::updateScreenProjection);

            if (m_cameraLens)
                QObject::connect(m_cameraLens, &Qt3DRender::QCameraLens::projectionMatrixChanged,
                                 this, &TransformTracker::updateScreenProjection);
        }

        updateScreenProjection();
    }
}

/*!
    \property Kuesa::TransformTracker::screenSize

    Holds the size of the area within which we should compute screen positions.
    This would usually be the size of the window of the size of the sub view
    within the window.
*/

/*!
    \qmlproperty size Kuesa::TransformTracker::screenSize

    Holds the size of the area within which we should compute screen positions.
    This would usually be the size of the window of the size of the sub view
    within the window.
*/
QSize TransformTracker::screenSize() const
{
    return m_screenSize;
}

/*!
    \property Kuesa::TransformTracker::viewportRect

    Holds the normalized viewport rectangle defining the area into which
    screen positions should be computed.
    If unspecified QRectF(0.0f, 0.0f, 1.0f, 1.0f) will be used.
*/

/*!
    \qmlproperty rect Kuesa::TransformTracker::viewportRect

    Holds the normalized viewport rectangle defining the area into which
    screen positions should be computed.
    If unspecified Qt.rect(0.0, 0.0, 1.0, 1.0) will be used.
*/
QRectF TransformTracker::viewportRect() const
{
    return m_viewportRect;
}

void TransformTracker::setScreenSize(const QSize &screenSize)
{
    if (screenSize != m_screenSize) {
        m_screenSize = screenSize;
        emit screenSizeChanged(m_screenSize);
        updateScreenProjection();
    }
}

void TransformTracker::setViewportRect(QRectF viewportRect)
{
    if (m_viewportRect == viewportRect)
        return;

    m_viewportRect = viewportRect;
    emit viewportRectChanged(m_viewportRect);
}

void TransformTracker::setViewportRect(qreal x, qreal y, qreal width, qreal height)
{
    setViewportRect({ x, y, width, height });
}

void TransformTracker::setMatrix(const QMatrix4x4 &matrix)
{
    if (m_node)
        m_node->setMatrix(matrix);
}

void TransformTracker::setRotation(const QQuaternion &rotation)
{
    if (m_node)
        m_node->setRotation(rotation);
}

void TransformTracker::setRotationX(float rotationX)
{
    if (m_node)
        m_node->setRotationX(rotationX);
}

void TransformTracker::setRotationY(float rotationY)
{
    if (m_node)
        m_node->setRotationY(rotationY);
}

void TransformTracker::setRotationZ(float rotationZ)
{
    if (m_node)
        m_node->setRotationZ(rotationZ);
}

void TransformTracker::setScale(float scale)
{
    if (m_node)
        m_node->setScale(scale);
}

void TransformTracker::setScale3D(const QVector3D &scale3D)
{
    if (m_node)
        m_node->setScale3D(scale3D);
}

void TransformTracker::setTranslation(const QVector3D &translation)
{
    if (m_node)
        m_node->setTranslation(translation);
}

/*!
    \property Kuesa::TransformTracker::name

    Holds the name of the Qt3DCore::QTransform instance to retrieve from the
    Kuesa::SceneEntity.
*/

/*!
    \qmlproperty string Kuesa::TransformTracker::name

    Holds the name of theTransform instance to retrieve from the SceneEntity.
*/
QString TransformTracker::name() const
{
    return m_name;
}

void TransformTracker::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        matchNode();
        emit nameChanged(m_name);
    }
}

/*!
    \property Kuesa::TransformTracker::matrix

    Holds the local transformation matrix associated to the tracker's
    referenced QTransform instance.
*/

/*!
    \qmlproperty matrix4x4 Kuesa::TransformTracker::matrix

    Holds the local transformation matrix associated to the tracker's
    referenced Transform instance.
*/
QMatrix4x4 TransformTracker::matrix() const
{
    return m_node ? m_node->matrix() : QMatrix4x4{};
}

/*!
    \property Kuesa::TransformTracker::worldMatrix

    Holds the world transformation matrix associated to the tracker's
    referenced QTransform instance.

    This property is readonly.
*/

/*!
    \qmlproperty matrix4x4 Kuesa::TransformTracker::worldMatrix

    Holds the world transformation matrix associated to the tracker's
    referenced Transform instance.

    \readonly
*/
QMatrix4x4 TransformTracker::worldMatrix() const
{
    return computeWorldMatrix(m_node);
}

/*!
    \property Kuesa::TransformTracker::rotation

    Holds the local rotation associated to the tracker's referenced QTransform
    instance.
*/

/*!
    \qmlproperty quaternion Kuesa::TransformTracker::rotation

    Holds the local rotation associated to the tracker's referenced Transform
    instance.
*/
QQuaternion TransformTracker::rotation() const
{
    return m_node ? m_node->rotation() : QQuaternion{};
}

/*!
    \property Kuesa::TransformTracker::rotationX

    Holds the local x rotation (euler angle) associated to the tracker's
    referenced QTransform instance.
*/

/*!
    \qmlproperty float Kuesa::TransformTracker::rotationX

    Holds the local x rotation (euler angle) associated to the tracker's referenced Transform
    instance.
*/
float TransformTracker::rotationX() const
{
    return m_node ? m_node->rotationX() : 0.f;
}

/*!
    \property Kuesa::TransformTracker::rotationY

    Holds the local y rotation (euler angle) associated to the tracker's
    referenced QTransform instance.
*/

/*!
    \qmlproperty float Kuesa::TransformTracker::rotationY

    Holds the local y rotation (euler angle) associated to the tracker's referenced Transform
    instance.
*/
float TransformTracker::rotationY() const
{
    return m_node ? m_node->rotationY() : 0.f;
}

/*!
    \property Kuesa::TransformTracker::rotationZ

    Holds the local z rotation (euler angle) associated to the tracker's
    referenced QTransform instance.
*/

/*!
    \qmlproperty float Kuesa::TransformTracker::rotationZ

    Holds the local z rotation (euler angle) associated to the tracker's referenced Transform
    instance.
*/
float TransformTracker::rotationZ() const
{
    return m_node ? m_node->rotationZ() : 0.f;
}

/*!
    \property Kuesa::TransformTracker::scale

    Holds the local uniform scale associated to the tracker's referenced
    QTransform instance.
*/

/*!
    \qmlproperty float Kuesa::TransformTracker::scale

    Holds the local uniform scale associated to the tracker's referenced
    Transform instance.
*/
float TransformTracker::scale() const
{
    return m_node ? m_node->scale() : 0.f;
}

/*!
    \property Kuesa::TransformTracker::scale3D

    Holds the local 3D scale associated to the tracker's referenced
    QTransform instance.
*/

/*!
    \qmlproperty vector3d Kuesa::TransformTracker::scale3D

    Holds the local 3D scale associated to the tracker's referenced
    Transform instance.
*/
QVector3D TransformTracker::scale3D() const
{
    return m_node ? m_node->scale3D() : QVector3D{};
}

/*!
    \property Kuesa::TransformTracker::translation

    Holds the local translation associated to the tracker's referenced
    QTransform instance.
*/

/*!
    \qmlproperty vector3d Kuesa::TransformTracker::translation

    Holds the local translation associated to the tracker's referenced
    Transform instance.
*/
QVector3D TransformTracker::translation() const
{
    return m_node ? m_node->translation() : QVector3D{};
}

/*!
    \property Kuesa::TransformTracker::screenPosition

    Holds the 2D screen position computed from the transformation in world
    space obtained from the tracker's referenced Transform, the camera, the
    sceneSize and viewportRect.
*/

/*!
    \qmlproperty point Kuesa::TransformTracker::screenPosition

    Holds the 2D screen position computed from the transformation in world
    space obtained from the tracker's referenced Transform, the camera, the
    sceneSize and viewportRect.
*/
QPointF TransformTracker::screenPosition() const
{
    return m_screenPosition;
}

/*!
 * \internal
 */
void TransformTracker::matchNode()
{
    auto transform = m_sceneEntity ? m_sceneEntity->transform(m_name) : nullptr;
    if (transform == m_node)
        return;

    if (m_node)
        m_node->disconnect(this);

    m_node = transform;

    if (m_node) {
        connect(m_node, &Qt3DCore::QTransform::scaleChanged, this, &TransformTracker::scaleChanged);
        connect(m_node, &Qt3DCore::QTransform::scale3DChanged, this, &TransformTracker::scale3DChanged);
        connect(m_node, &Qt3DCore::QTransform::translationChanged, this, &TransformTracker::translationChanged);
        connect(m_node, &Qt3DCore::QTransform::rotationChanged, this, &TransformTracker::rotationChanged);
        connect(m_node, &Qt3DCore::QTransform::rotationXChanged, this, &TransformTracker::rotationXChanged);
        connect(m_node, &Qt3DCore::QTransform::rotationYChanged, this, &TransformTracker::rotationYChanged);
        connect(m_node, &Qt3DCore::QTransform::rotationZChanged, this, &TransformTracker::rotationZChanged);
        connect(m_node, &Qt3DCore::QTransform::matrixChanged, this, [this] { emit matrixChanged(matrix()); });
        connect(m_node, &Qt3DCore::QTransform::worldMatrixChanged, this, &TransformTracker::worldMatrixChanged);
        connect(m_node, &Qt3DCore::QTransform::worldMatrixChanged, this, &TransformTracker::updateScreenProjection);
        connect(m_node, &Qt3DCore::QNode::nodeDestroyed, this, [this] {
            matchNode();
        });
    }

    // Emit signal changes for each property of the transform we proxy
    // to ensure any binding gets reevaluated with up to data values
    emit scaleChanged(scale());
    emit scale3DChanged(scale3D());
    emit translationChanged(translation());
    emit rotationChanged(rotation());
    emit rotationXChanged(rotationX());
    emit rotationYChanged(rotationY());
    emit rotationZChanged(rotationZ());
    emit matrixChanged(matrix());
    emit worldMatrixChanged(worldMatrix());

    updateScreenProjection();
}

void TransformTracker::updateScreenProjection()
{
    QRect viewport{ 0, 0, m_screenSize.width(), m_screenSize.height() };
    if (m_viewportRect.isValid()) {
        viewport.setX(int(qreal(m_screenSize.width()) * m_viewportRect.x()));
        viewport.setY(int(qreal(m_screenSize.height()) * m_viewportRect.y()));
        viewport.setWidth(int(qreal(m_screenSize.width()) * m_viewportRect.width()));
        viewport.setHeight(int(qreal(m_screenSize.height()) * m_viewportRect.height()));
    }

    const QMatrix4x4 worldMatrix = this->worldMatrix();
    const QMatrix4x4 projectionMatrix = (m_cameraLens) ? m_cameraLens->projectionMatrix() : QMatrix4x4{};
    const QMatrix4x4 viewMatrix = (m_cameraTransform) ? m_cameraTransform->worldMatrix().inverted() : QMatrix4x4{};
    const QVector3D projectedPoint = QVector3D().project(viewMatrix * worldMatrix,
                                                         projectionMatrix,
                                                         viewport);

    const QPointF orientationCorrectScreenPos = QPointF(qreal(projectedPoint.x()),
                                                        m_screenSize.height() - qreal(projectedPoint.y()));
    auto clamp = [](float n, float low, float high) {
        return std::max(low, std::min(n, high));
    };
    m_screenPosition = QPointF(clamp(orientationCorrectScreenPos.x(), viewport.x(), viewport.x() + viewport.width()),
                               clamp(orientationCorrectScreenPos.y(), viewport.y(), viewport.y() + viewport.height()));
    emit screenPositionChanged(m_screenPosition);
}
