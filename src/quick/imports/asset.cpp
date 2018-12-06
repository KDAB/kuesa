/*
    asset.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "asset.h"
#include <Kuesa/SceneEntity>
#include <Kuesa/AbstractAssetCollection>

QT_USE_NAMESPACE
using namespace Kuesa;

/*!
    \qmltype Asset
    \inherits QtObject
    \inqmlmodule Kuesa

    \brief Asset is to access names nodes loaded from a glTF 2 file in a SceneEntity

    When loading a scene from a glTF 2 file, the resulting nodes are inserted into
    various collections. They can then searched for by name within these collections.

    Asset provides a declarative way of retrieving assets by name within a specified
    collection or the entire scene.

    If a collection is provided, the search will be limited to that collection. Otherwise,
    the search will be done in all collections, returning the first asset matching the name.

    \sa Kuesa::SceneEntity, Kuesa::AbstractAssetCollection
 */

/*!
    \qmlproperty Kuesa::SceneEntity Asset::sceneEntity
    The scene in which assets are loaded.
 */

/*!
    \qmlproperty Kuesa::AbstractAssetCollection Asset::collection
    Defines in which collection the asset name will be searched for.

    \note If undefined, the name will searched for in all the scene's collection.
 */

/*!
    \qmlproperty string Asset::name
    The name of the asset. The asset collections will ensure that the name
    is unique within a collection, but the same name may be used in different
    collections.
 */

/*!
    \qmlproperty Qt3D.Core::Node Asset::node
    The resulting asset. Will be null if no scene is loaded or if the
    names asset is not found.
    \readonly
 */

Asset::Asset(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
    , m_scene(nullptr)
    , m_collection(nullptr)
    , m_node(nullptr)
{
    updateSceneFromParent(parent);
    connect(this, &Qt3DCore::QNode::parentChanged, this, [this](QObject *parent) {
        auto parentNode = qobject_cast<Qt3DCore::QNode *>(parent);
        this->updateSceneFromParent(parentNode);
    });
}

void Asset::updateSceneFromParent(Qt3DCore::QNode *parent)
{
    if (m_scene)
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

SceneEntity *Asset::sceneEntity() const
{
    return m_scene;
}

void Asset::setSceneEntity(SceneEntity *sceneEntity)
{
    if (m_scene != sceneEntity) {
        if (m_scene) {
            QObject::disconnect(m_destructionConnections.take(m_scene));
            disconnect(m_scene, &SceneEntity::loadingDone, this, &Asset::findAsset);
        }
        m_scene = sceneEntity;
        emit sceneEntityChanged(m_scene);
        if (m_scene) {
            auto f = [this]() { setSceneEntity(nullptr); };
            m_destructionConnections.insert(m_scene, connect(m_scene, &Qt3DCore::QNode::nodeDestroyed, this, f));
            connect(m_scene, &SceneEntity::loadingDone, this, &Asset::findAsset);
        }
        findAsset();
    }
}

AbstractAssetCollection *Asset::collection() const
{
    return m_collection;
}

void Asset::setCollection(AbstractAssetCollection *collection)
{
    if (m_collection != collection) {
        if (m_collection) {
            QObject::disconnect(m_destructionConnections.take(m_collection));
            disconnect(m_collection, &AbstractAssetCollection::namesChanged, this, &Asset::findAsset);
        }
        m_collection = collection;
        emit collectionChanged(m_collection);
        if (m_collection) {
            auto f = [this]() { setCollection(nullptr); };
            m_destructionConnections.insert(m_collection, connect(m_collection, &Qt3DCore::QNode::nodeDestroyed, this, f));
            connect(m_collection, &AbstractAssetCollection::namesChanged, this, &Asset::findAsset);
        }
        findAsset();
    }
}

QString Asset::name() const
{
    return m_name;
}

void Asset::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(m_name);
        findAsset();
    }
}

void Asset::setNode(Qt3DCore::QNode *node)
{
    if (m_node != node) {
        m_node = node;
        emit nodeChanged(m_node);
    }
}

Qt3DCore::QNode *Asset::node() const
{
    return m_node;
}

void Asset::findAsset()
{
    if ((nullptr == m_collection && nullptr == m_scene) || m_name.isEmpty()) {
        setNode(nullptr);
        return;
    }

    QVector<AbstractAssetCollection *> collections;
    if (m_collection) {
        collections << m_collection;
    } else {
        collections << m_scene->animationClips()
                    << m_scene->armatures()
                    << m_scene->effects()
                    << m_scene->layers()
                    << m_scene->materials()
                    << m_scene->meshes()
                    << m_scene->skeletons()
                    << m_scene->textures()
                    << m_scene->cameras()
                    << m_scene->entities()
                    << m_scene->textureImages()
                    << m_scene->animationMappings();
    }

    for (AbstractAssetCollection *c : qAsConst(collections)) {
        auto n = c->findAsset(m_name);
        if (n) {
            setNode(n);
            return;
        }
    }

    setNode(nullptr);
}
