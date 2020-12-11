/*
    kuesanode.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "kuesanode.h"

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
 * \class Kuesa::KuesaNode
 * \inheaderfile Kuesa/KuesaNode
 * \inmodule Kuesa
 * \inherits Qt3DCore::QNode
 * \since Kuesa 1.3
 * \brief Base class for nodes that require a Kuesa::SceneEnity for assets
 * insertion or retrieval.
 *
 * The KuesaNode class is a convenience wrapper that handles setting a
 * SceneEntity property on a node. It also automatically traverse the QObject
 * tree hierarchy to find if a Kuesa::SceneEntity is part of the hierarchy. If
 * so and if no Kuesa::SceneEntity was provided by the user, it will set the
 * sceneEntity property to that.
*/

/*!
    \property Kuesa::KuesaNode::sceneEntity

    Pointer to the SceneEntity with which assets are registered as they are
    loaded from the glTF file.

    \sa Kuesa::SceneEntity
*/

/*!
 * \qmltype KuesaNode
 * \instantiates Kuesa::KuesaNode
 * \inqmlmodule Kuesa
 * \inherits Qt3DCore.Node
 * \since Kuesa 1.3
 * \brief  Base class for nodes that require a Kuesa::SceneEnity for assets
 * insertion or retrieval.
 *
 * The KuesaNode class is a convenience wrapper that handles setting a
 * SceneEntity property on a node. It also automatically traverse the QObject
 * tree hierarchy to find if a Kuesa::SceneEntity is part of the hierarchy. If
 * so and if no Kuesa::SceneEntity was provided by the user, it will set the
 * sceneEntity property to that.
 *
 * \note It cannot be instantiated directly in QML
*/

/*!
    \qmlproperty Kuesa::SceneEntity Kuesa::KuesaNode::sceneEntity

    Pointer to the SceneEntity with which assets are registered as they are
    loaded from the glTF file.

    \sa Kuesa::SceneEntity
 */

KuesaNode::KuesaNode(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
{
    updateSceneFromParent(parent);
    connect(this, &Qt3DCore::QNode::parentChanged, this, [this](QObject *parent) {
        auto parentNode = qobject_cast<Qt3DCore::QNode *>(parent);
        this->updateSceneFromParent(parentNode);
    });
}

void KuesaNode::updateSceneFromParent(Qt3DCore::QNode *parent)
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

SceneEntity *KuesaNode::sceneEntity() const
{
    return m_sceneEntity;
}

void KuesaNode::setSceneEntity(SceneEntity *sceneEntity)
{
    if (m_sceneEntity != sceneEntity) {
        if (m_sceneEntity)
            disconnect(m_sceneEntityDestructionConnection);
        m_sceneEntity = sceneEntity;
        emit sceneEntityChanged(m_sceneEntity);
        if (m_sceneEntity) {
            auto f = [this]() { setSceneEntity(nullptr); };
            m_sceneEntityDestructionConnection = connect(m_sceneEntity, &Qt3DCore::QNode::nodeDestroyed, this, f);
        }
    }
}

} // namespace Kuesa

QT_END_NAMESPACE
