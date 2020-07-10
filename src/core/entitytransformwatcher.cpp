/*
    entitytransformwatcher.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "entitytransformwatcher_p.h"
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Kuesa/private/kuesa_utils_p.h>

using namespace Kuesa;

EntityTransformWatcher::EntityTransformWatcher(QObject *parent)
    : QObject(parent)
{
}

void EntityTransformWatcher::setTarget(Qt3DCore::QEntity *target)
{
    if (target == m_target)
        return;

    m_target = target;

    for (const QMetaObject::Connection &c : m_connections)
        QObject::disconnect(c);
    m_connections.clear();

    Qt3DCore::QEntity *e = m_target;
    // worldMatrix only exists since 5.14
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    Qt3DCore::QTransform *transform = nullptr;
    while (transform == nullptr && e != nullptr) {
        transform = Kuesa::componentFromEntity<Qt3DCore::QTransform>(e);
        e = e->parentEntity();
    }
    if (transform)
        m_connections.push_back(QObject::connect(transform, &Qt3DCore::QTransform::worldMatrixChanged, this, &EntityTransformWatcher::updateWorldMatrix));
#else
    // We need to tranverse back through all the Entity hierarchy of the Camera and build the worldMatrix ourselves

    // Connect to all parent
    while (e != nullptr) {
        Qt3DCore::QTransform *transform = Kuesa::componentFromEntity<Qt3DCore::QTransform>(e);
        if (transform) {
            transform->setPropertyTracking("scale3D", Qt3DCore::QNode::TrackAllValues);
            transform->setPropertyTracking("rotation", Qt3DCore::QNode::TrackAllValues);
            transform->setPropertyTracking("translation", Qt3DCore::QNode::TrackAllValues);
            m_connections.push_back(
                    QObject::connect(transform, &Qt3DCore::QTransform::matrixChanged, this, [this] {
                        if (!m_updateRequested) {
                            m_updateRequested = true;
                            QMetaObject::invokeMethod(this, &EntityTransformWatcher::updateWorldMatrix, Qt::QueuedConnection);
                        }
                    }));
        }
        e = e->parentEntity();
    }
#endif
    updateWorldMatrix();
}

Qt3DCore::QEntity *EntityTransformWatcher::target() const
{
    return m_target;
}

QMatrix4x4 EntityTransformWatcher::worldMatrix() const
{
    return m_worldMatrix;
}

void EntityTransformWatcher::updateWorldMatrix()
{
    m_updateRequested = false;
    QMatrix4x4 worldMatrix;

    if (m_target) {
        Qt3DCore::QEntity *e = m_target;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        Qt3DCore::QTransform *transform = nullptr;
        while (transform == nullptr && e != nullptr) {
            transform = Kuesa::componentFromEntity<Qt3DCore::QTransform>(e);
            e = e->parentEntity();
        }
#else
        // We need to tranverse back through all the Entity hierarchy of the Camera and build the worldMatrix ourselves
        while (e != nullptr) {
            const Qt3DCore::QTransform *transform = Kuesa::componentFromEntity<Qt3DCore::QTransform>(e);
            if (transform != nullptr)
                worldMatrix = transform->matrix() * worldMatrix;
            e = e->parentEntity();
        }
#endif
    }
    m_worldMatrix = worldMatrix;
    emit worldMatrixChanged(worldMatrix);
}
