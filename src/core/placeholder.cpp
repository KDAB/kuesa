/*
    placeholder.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include "placeholder.h"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>

#include <kuesa_utils_p.h>
#include <Qt3DCore/private/qmath3d_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/logging_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::Placeholder
    \brief Specifies a placeholder which can be used to anchor QtQuick elements.
    \inmodule Kuesa
    \since Kuesa 1.3
    \inherits Qt3DCore::QEntity

    The Placeholder class allows to specify a placeholder to position QtQuick
    elements. It does so by projecting a plane facing the camera to screen
    space and setting the x, y, width and height properties of a target QObject
    to match the projected rectangle.

    It is meant to be used only from a glTF 2.0 file exported with
    KDAB_placeholder extension. In other words, you are not supposed to
    instantiate this object on your own but rather rely on retrieving it from
    the \l {Kuesa::PlaceholderCollection} upon loading.
*/

/*!
    \qmltype Placeholder
    \brief Specifies a placeholder which can be used to anchor QtQuick elements.
    \inmodule Kuesa
    \since Kuesa 1.3
    \instantiates Kuesa::Placeholder

    The Placeholder element allows to specify a placeholder to position QtQuick
    elements. It does so by projecting a plane facing the camera to screen
    space and setting the x, y, width and height properties of a target QObject
    to match the projected rectangle.

    It is meant to be used only from a glTF 2.0 file exported with
    KDAB_placeholder extension. In other words, you are not supposed to
    instantiate this object on your own but rather rely on retrieving it from
    the \l {Kuesa::PlaceholderCollection} upon loading.
*/
Placeholder::Placeholder(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_transform(new Qt3DCore::QTransform)
{

    // Add a transform so we can pick the world transform of this entity
    addComponent(m_transform);

    QObject::connect(m_transform, &Qt3DCore::QTransform::worldMatrixChanged,
                     this, &Placeholder::updatePlaceholder);
}

void Placeholder::updatePlaceholder()
{
    if (m_camera && m_target && m_cameraTransform && m_cameraLens) {
        QVector3D position;
        QQuaternion orientation;
        QVector3D scale;
        decomposeQMatrix4x4(m_transform->worldMatrix(), position, orientation, scale);
        const auto aLocal = QVector3D{-qAbs(scale.x()), -qAbs(scale.z()), 0};
        const auto bLocal = QVector3D{qAbs(scale.x()), qAbs(scale.z()), 0};

        const auto centerWorldSpace = m_transform->worldMatrix() * QVector3D(0,0,0);

        const auto getWorldSpaceForPoint = [centerWorldSpace, this] (QVector3D p) {
            return centerWorldSpace + m_cameraTransform->worldMatrix() * p - m_cameraTransform->worldMatrix() * QVector3D(0,0,0);
        };

        const auto aWorldSpace = getWorldSpaceForPoint(aLocal);
        const auto bWorldSpace = getWorldSpaceForPoint(bLocal);

        const auto viewMatrix = m_cameraTransform->worldMatrix().inverted();

        const QVector3D aViewportSpace = aWorldSpace.project(viewMatrix,
                                                             m_cameraLens->projectionMatrix(),
                                                             m_viewport);

        const QVector3D bViewportSpace = bWorldSpace.project(viewMatrix,
                                                             m_cameraLens->projectionMatrix(),
                                                             m_viewport);

        const auto a = aViewportSpace.toPoint();
        const auto b = bViewportSpace.toPoint();

        if (m_target) {
            const auto width = (b - a).x();
            const auto height = (b - a).y();

            m_target->setProperty("x", a.x());
            m_target->setProperty("y", m_viewport.height() - a.y() - height);
            m_target->setProperty("width", width);
            m_target->setProperty("height", height);
        }
    }
}

/*!
    \property Kuesa::Placeholder::target

    Holds the QObject which should be anchored to the placeholder.

    Commonly it would be a QtQuick element and should be used as

    \badcode
    QQuickItem *target = view->rootItem();
    Kuesa::Placeholder *placeholder = sceneEntity->placeholder(QStringliteral("placeholder"));
    placeholder->setViewport({0, 0, view.width(), view.height()});
    placeholder->setTarget(target);
    \endcode
*/

/*!
    \qmlproperty QtObject Kuesa::Placeholder::target

    Holds the QObject which should be anchored to the placeholder.

    Commonly it would be a QtQuick element and should be used as

    \badcode
    Rectangle {
        id: rect
    }

    Kuesa.Asset {
        id: placeholder
        collection: scene.placeholders
        name: "placeholder"
        readonly property rect viewport: Qt.rect(0,0,root.width, root.height)
        readonly property QtObject target: rect
    }
    \endcode
*/
QObject *Placeholder::target() const
{
    return m_target;
}

void Placeholder::setTarget(QObject *target)
{
    if (m_target != target) {
            m_target = target;
            QObject::disconnect(m_targetDestroyedConnection);

        if (m_target) {
            updatePlaceholder();
            m_targetDestroyedConnection = QObject::connect(m_target, &QObject::destroyed, [this]() {setTarget(nullptr);});
        }

        emit targetChanged(target);
    }
}

/*!
    \property Kuesa::Placeholder::camera

    Holds the camera to which the plane is facing and which is used to project
    the plane to screen space.

    It is set automatically by the glTF 2.0 importer but can be modified by the
    user to face different cameras.

    \badcode
    QQuickItem *target = view->rootItem();
    Kuesa::Placeholder *placeholder = sceneEntity->placeholder(QStringliteral("placeholder"));
    Qt3DRender::QCamera *camera = sceneEntity->camera(QStringliteral("Camera_Orientation"));
    placeholder->setViewport({0, 0, view.width(), view.height()});
    placeholder->setTarget(target);
    placeholder->setCamera(camera);
    \endcode

    \note using a Qt3DRender::QCamera for the placeholder different than the
    one being used for the render might give unexpected results.
*/

/*!
    \qmlproperty Qt3DCore.Entity Kuesa::Placeholder::camera

    Holds the camera to which the plane is facing and which is used to project
    the plane to screen space.

    It is set automatically by the glTF 2.0 importer but can be modified by the
    user to face different cameras.

    \badcode
    Rectangle {
        id: rect
    }

    Kuesa.Asset {
        id: mainCamera
        collection: scene.cameras
        name: "Camera_Orientation"
    }

    Kuesa.Asset {
        id: placeholder
        collection: scene.placeholders
        name: "placeholder"
        onNodeChanged: {
            node.target = rect
            node.camera = mainCamera.node
        }
        readonly property rect viewport: Qt.rect(0,0,root.width, root.height)
    }
    \endcode

    \note using a camera for the placeholder different than the one being used
    for the render might give unexpected results.
*/
Qt3DCore::QEntity *Placeholder::camera() const
{
    return m_camera;
}

void Placeholder::setCamera(Qt3DCore::QEntity *camera)
{
    if (camera != m_camera) {
        auto d = Qt3DCore::QNodePrivate::get(this);

        if (m_cameraTransform)
            QObject::disconnect(m_cameraTransform, &Qt3DCore::QTransform::worldMatrixChanged,
                                this, &Placeholder::updatePlaceholder);
        if (m_cameraLens)
            QObject::disconnect(m_cameraLens, &Qt3DRender::QCameraLens::projectionMatrixChanged,
                                this, &Placeholder::updatePlaceholder);
        if (m_camera) {
            d->unregisterDestructionHelper(m_camera);
            m_cameraTransform = nullptr;
            m_cameraLens = nullptr;
        }

        m_camera = camera;

        if (m_camera) {
            // Note: it is expected the camera is already parented

            d->registerDestructionHelper(m_camera, &Placeholder::setCamera, m_camera);

            m_cameraTransform = componentFromEntity<Qt3DCore::QTransform>(m_camera);
            m_cameraLens = componentFromEntity<Qt3DRender::QCameraLens>(m_camera);

            if (m_cameraTransform)
                QObject::connect(m_cameraTransform, &Qt3DCore::QTransform::worldMatrixChanged,
                                 this, &Placeholder::updatePlaceholder);
            else
                qCWarning(kuesa) << "Placeholder camera has no transform component";

            if (m_cameraLens)
                QObject::connect(m_cameraLens, &Qt3DRender::QCameraLens::projectionMatrixChanged,
                                 this, &Placeholder::updatePlaceholder);
            else
                qCWarning(kuesa) << "Placeholder camera has no camera lens component";

            updatePlaceholder();
        }

        emit cameraChanged(camera);
    }
}

/*!
    \property Kuesa::Placeholder::viewport

    Holds the QRect representing the viewport to which the plane is projected.
    This should match the viewport where the rendering is being performed.

    \badcode
    Rectangle {
        id: rect
    }

    Kuesa.Asset {
        id: placeholder
        collection: scene.placeholders
        name: "placeholder"
        onNodeChanged: {
            node.target = rect
        }

        property rect viewport: Qt.rect(0,0,root.width, root.height)
    }
    \endcode

    \note using a viewport different than the one being used for rendering might give
    unexpected results.
*/

QRect Placeholder::viewport() const
{
    return m_viewport;
}

void Placeholder::setViewport(QRect viewport)
{
    if (m_viewport != viewport) {
        m_viewport = viewport;
        updatePlaceholder();

        emit viewportChanged(viewport);
    }
}

} // namespace Kuesa

QT_END_NAMESPACE
