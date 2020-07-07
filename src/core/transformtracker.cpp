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
#include <Kuesa/private/entitytransformwatcher_p.h>

using namespace Kuesa;

namespace {

inline QMatrix4x4 computeWorldMatrix(Qt3DCore::QTransform *transformComponent)
{
    if (!transformComponent || transformComponent->entities().empty())
        return {};

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    return transformComponent->worldMatrix();
#else
    QMatrix4x4 worldMatrix;
    Q_ASSERT(transformComponent->entities().size() == 1);
    auto p = transformComponent->entities().front();
    while (p) {
        const auto &tl = Kuesa::componentsFromEntity<Qt3DCore::QTransform>(p);
        if (tl.size())
            worldMatrix = tl.front()->matrix() * worldMatrix;
        p = p->parentEntity();
    }
    return worldMatrix;
#endif
}

} // namespace

TransformTracker::TransformTracker(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
    , m_sceneEntity(nullptr)
    , m_camera(nullptr)
    , m_cameraWatcher(nullptr)
    , m_cameraTransform(nullptr)
    , m_cameraLens(nullptr)
    , m_node(nullptr)
    , m_nodeWatcher(nullptr)
{
    updateSceneFromParent(parent);
    connect(this, &Qt3DCore::QNode::parentChanged, this, [this](QObject *parent) {
        auto parentNode = qobject_cast<Qt3DCore::QNode *>(parent);
        this->updateSceneFromParent(parentNode);
    });
}

TransformTracker::~TransformTracker() = default;

SceneEntity *TransformTracker::sceneEntity() const
{
    return m_sceneEntity;
}

void TransformTracker::setSceneEntity(SceneEntity *sceneEntity)
{
    if (sceneEntity == m_sceneEntity)
        return;

    if (m_sceneEntity)
        disconnect(m_sceneEntity, &SceneEntity::loadingDone, this, &TransformTracker::matchNode);

    m_sceneEntity = sceneEntity;
    connect(m_sceneEntity, &SceneEntity::loadingDone, this, &TransformTracker::matchNode);
    connect(m_sceneEntity, &Qt3DCore::QNode::nodeDestroyed, this, [this] { setSceneEntity(nullptr); });

    emit sceneEntityChanged(sceneEntity);
    matchNode();
}

Qt3DCore::QEntity *TransformTracker::camera() const
{
    return m_camera;
}

void TransformTracker::setCamera(Qt3DCore::QEntity *camera)
{
    if (camera != m_camera) {
        m_camera = camera;
        emit cameraChanged(m_camera);

        delete m_cameraWatcher;
        m_cameraWatcher = nullptr;

        if (m_camera) {
            m_cameraTransform = componentFromEntity<Qt3DCore::QTransform>(m_camera);
            m_cameraLens = componentFromEntity<Qt3DRender::QCameraLens>(m_camera);

            m_cameraWatcher = new EntityTransformWatcher(this);
            m_cameraWatcher->setTarget(m_camera);
            QObject::connect(m_cameraWatcher, &EntityTransformWatcher::worldMatrixChanged,
                             this, &TransformTracker::updateScreenProjection);

            if (m_cameraLens)
                QObject::connect(m_cameraLens, &Qt3DRender::QCameraLens::projectionMatrixChanged,
                                 this, &TransformTracker::updateScreenProjection);
        }

        updateScreenProjection();
    }
}

QSize TransformTracker::screenSize() const
{
    return m_screenSize;
}

void TransformTracker::setScreenSize(const QSize &screenSize)
{
    if (screenSize != m_screenSize) {
        m_screenSize = screenSize;
        emit screenSizeChanged(m_screenSize);
        updateScreenProjection();
    }
}

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

QMatrix4x4 TransformTracker::matrix() const
{
    return m_node ? m_node->matrix() : QMatrix4x4{};
}

QMatrix4x4 TransformTracker::worldMatrix() const
{
    return computeWorldMatrix(m_node);
}

QQuaternion TransformTracker::rotation() const
{
    return m_node ? m_node->rotation() : QQuaternion{};
}

float TransformTracker::rotationX() const
{
    return m_node ? m_node->rotationX() : 0.f;
}

float TransformTracker::rotationY() const
{
    return m_node ? m_node->rotationY() : 0.f;
}

float TransformTracker::rotationZ() const
{
    return m_node ? m_node->rotationZ() : 0.f;
}

float TransformTracker::scale() const
{
    return m_node ? m_node->scale() : 0.f;
}

QVector3D TransformTracker::scale3D() const
{
    return m_node ? m_node->scale3D() : QVector3D{};
}

QVector3D TransformTracker::translation() const
{
    return m_node ? m_node->translation() : QVector3D{};
}

QPointF TransformTracker::screenPosition() const
{
    return m_screenPosition;
}

void TransformTracker::updateSceneFromParent(Qt3DCore::QNode *parent)
{
    if (m_sceneEntity)
        return;

    while (parent) {
        auto scene = qobject_cast<SceneEntity *>(parent);
        if (scene) {
            setSceneEntity(scene);
            break;
        }
        parent = parent->parentNode();
    }
}

void TransformTracker::matchNode()
{
    if (!m_sceneEntity)
        return;

    auto transform = m_sceneEntity->transform(m_name);
    if (!transform || transform == m_node)
        return;

    if (m_node)
        m_node->disconnect(this);

    m_node = transform;
    connect(m_node, &Qt3DCore::QTransform::scaleChanged, this, &TransformTracker::scaleChanged);
    connect(m_node, &Qt3DCore::QTransform::scale3DChanged, this, &TransformTracker::scale3DChanged);
    connect(m_node, &Qt3DCore::QTransform::translationChanged, this, &TransformTracker::translationChanged);
    connect(m_node, &Qt3DCore::QTransform::rotationChanged, this, &TransformTracker::rotationChanged);
    connect(m_node, &Qt3DCore::QTransform::rotationXChanged, this, &TransformTracker::rotationXChanged);
    connect(m_node, &Qt3DCore::QTransform::rotationYChanged, this, &TransformTracker::rotationYChanged);
    connect(m_node, &Qt3DCore::QTransform::rotationZChanged, this, &TransformTracker::rotationZChanged);
    connect(m_node, &Qt3DCore::QTransform::matrixChanged, this, &TransformTracker::matrixChanged);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    connect(m_node, &Qt3DCore::QTransform::worldMatrixChanged, this, &TransformTracker::worldMatrixChanged);
#endif
    connect(m_node, &Qt3DCore::QNode::nodeDestroyed, this, [this] { matchNode(); });

    delete m_nodeWatcher;
    m_nodeWatcher = nullptr;
    if (m_node->entities().size()) {
        m_nodeWatcher = new EntityTransformWatcher(this);
        m_nodeWatcher->setTarget(m_node->entities().front());
        connect(m_nodeWatcher, &EntityTransformWatcher::worldMatrixChanged, this, &TransformTracker::updateScreenProjection);
    }

    updateScreenProjection();
}

void TransformTracker::updateScreenProjection()
{
    if (m_screenSize.isEmpty() || !m_nodeWatcher || !m_cameraWatcher)
        return;

    QMatrix4x4 worldMatrix = m_nodeWatcher->worldMatrix();
    QMatrix4x4 invertedCameraMatrix = m_cameraWatcher->worldMatrix().inverted();

    const QMatrix4x4 projectionMatrix = (m_cameraLens) ? m_cameraLens->projectionMatrix() : QMatrix4x4{};
    const QMatrix4x4 viewMatrix = (m_cameraTransform) ? invertedCameraMatrix : QMatrix4x4{};
    const QVector3D projectedPoint = QVector3D().project(viewMatrix * worldMatrix,
                                                         projectionMatrix,
                                                         QRect(0, 0, m_screenSize.width(), m_screenSize.height()));

    m_screenPosition = QPointF(qreal(projectedPoint.x()), qreal(m_screenSize.height()) - qreal(projectedPoint.y()));
    emit screenPositionChanged(m_screenPosition);
}
