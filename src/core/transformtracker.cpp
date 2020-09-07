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
    : KuesaNode(parent)
    , m_camera(nullptr)
    , m_cameraWatcher(nullptr)
    , m_cameraTransform(nullptr)
    , m_cameraLens(nullptr)
    , m_node(nullptr)
    , m_nodeWatcher(nullptr)
{
    connect(this, &KuesaNode::sceneEntityChanged,
            this, [this] {
                disconnect(m_loadingDoneConnection);
                if (m_sceneEntity)
                    m_loadingDoneConnection = connect(m_sceneEntity, &SceneEntity::loadingDone, this, &TransformTracker::matchNode);
            });
}

TransformTracker::~TransformTracker() = default;

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

    QRect viewport{ 0, 0, m_screenSize.width(), m_screenSize.height() };
    if (m_viewportRect.isValid()) {
        viewport.setX(int(qreal(m_screenSize.width()) * m_viewportRect.x()));
        viewport.setY(int(qreal(m_screenSize.height()) * m_viewportRect.y()));
        viewport.setWidth(int(qreal(m_screenSize.width()) * m_viewportRect.width()));
        viewport.setHeight(int(qreal(m_screenSize.height()) * m_viewportRect.height()));
    }

    const QMatrix4x4 projectionMatrix = (m_cameraLens) ? m_cameraLens->projectionMatrix() : QMatrix4x4{};
    const QMatrix4x4 viewMatrix = (m_cameraTransform) ? invertedCameraMatrix : QMatrix4x4{};
    const QVector3D projectedPoint = QVector3D().project(viewMatrix * worldMatrix,
                                                         projectionMatrix,
                                                         viewport);

    m_screenPosition = QPointF(qreal(projectedPoint.x()), qreal(viewport.height()) - qreal(projectedPoint.y()));
    emit screenPositionChanged(m_screenPosition);
}
