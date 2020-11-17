/*
    asset.cpp

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

#include "asset.h"
#include <Kuesa/SceneEntity>
#include <Kuesa/AbstractAssetCollection>

#include "assetproperty_p.h"

#include <QMetaProperty>
#include <QMetaMethod>

QT_USE_NAMESPACE
using namespace Kuesa;

/*!
    \qmltype Asset
    \inherits KuesaNode
    \inqmlmodule Kuesa

    \brief Asset is to access names nodes loaded from a glTF 2 file in a
    SceneEntity.

    When loading a scene from a glTF 2 file, the resulting nodes are inserted
    into various collections. They can then searched for by name within these
    collections.

    Asset provides a declarative way of retrieving assets by name within a
    specified collection or the entire scene.

    If a collection is provided, the search will be limited to that collection.
    Otherwise, the search will be done in all collections, returning the first
    asset matching the name.

    \sa Kuesa::SceneEntity, Kuesa::AbstractAssetCollection
 */

/*!
    \qmlproperty Kuesa::AbstractAssetCollection Asset::collection
    Defines in which collection the asset name will be searched for.

    \note If undefined, the name will searched for in all the scene's collection.
 */

/*!
    \qmlproperty string Asset::name
    The name of the asset. The asset collections will ensure that the name is
    unique within a collection, but the same name may be used in different
    collections.
 */

/*!
    \qmlproperty Qt3D.Core::Node Asset::node
    The resulting asset. Will be null if no scene is loaded or if the names
    asset is not found.
    \readonly
 */

Asset::Asset(Qt3DCore::QNode *parent)
    : KuesaNode(parent)
    , m_collection(nullptr)
    , m_node(nullptr)
{
    QObject::connect(this, &KuesaNode::sceneEntityChanged,
                     this, [this] {
                         disconnect(m_loadingDoneConnection);
                         if (m_sceneEntity)
                             m_loadingDoneConnection = connect(m_sceneEntity, &SceneEntity::loadingDone, this, &Asset::findAsset);
                         findAsset();
                     });
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

        qDeleteAll(m_assetProperties);
        m_assetProperties.clear();
        QObject::disconnect(m_releaseAssetPropertiesConnection);

        // Get only the Kuesa.Asset user defined properties. We are not interested in default properties!
        // Iterate on the metaObject properties of this and forward node properties to metaObject properties
        if (node != nullptr) {
            const QString &className = metaObject()->className();

            // The Kuesa.Asset has been extended with some properties! This is the case we are interested in!
            if (className.contains(QStringLiteral("_QML_"))) {
                const auto propertyOffset = this->metaObject()->propertyOffset();
                const auto nbProperties = this->metaObject()->propertyCount();
                for (int propertyIdx = propertyOffset; propertyIdx < nbProperties; ++propertyIdx) {
                    const QMetaProperty property = metaObject()->property(propertyIdx);

                    // Check if current node has a property with this name
                    const auto nodePropertyId = m_node->metaObject()->indexOfProperty(property.name());
                    if (nodePropertyId != -1) {
                        // node has a property with this name. Forward signals and slots
                        const QMetaProperty nodeProperty = m_node->metaObject()->property(nodePropertyId);
                        if (nodeProperty.hasNotifySignal()) {
                            AssetProperty *assetProperty = new AssetProperty(this);
                            assetProperty->node = m_node;
                            assetProperty->nodeProperty = nodeProperty;
                            assetProperty->qmlProperty = property;
                            QObject::connect(this,
                                             property.notifySignal(),
                                             assetProperty,
                                             assetProperty->metaObject()->method(assetProperty->metaObject()->indexOfMethod("setNodeProperty()")));
                            QObject::connect(m_node,
                                             nodeProperty.notifySignal(),
                                             assetProperty,
                                             assetProperty->metaObject()->method(assetProperty->metaObject()->indexOfMethod("setQmlProperty()")));
                            assetProperty->setQmlProperty();
                            m_assetProperties.push_back(assetProperty);
                        }
                    }
                }
            }
            m_releaseAssetPropertiesConnection = QObject::connect(m_node, &QNode::nodeDestroyed, this, [this]() {
                setNode(nullptr);
            });
        }
        emit nodeChanged(m_node);
    }
}

Qt3DCore::QNode *Asset::node() const
{
    return m_node;
}

void Asset::findAsset()
{
    if ((nullptr == m_collection && nullptr == m_sceneEntity) || m_name.isEmpty()) {
        setNode(nullptr);
        return;
    }

    QVector<AbstractAssetCollection *> collections;
    if (m_collection) {
        collections << m_collection;
    } else {
        collections << m_sceneEntity->animationClips()
                    << m_sceneEntity->armatures()
                    << m_sceneEntity->effects()
                    << m_sceneEntity->layers()
                    << m_sceneEntity->materials()
                    << m_sceneEntity->meshes()
                    << m_sceneEntity->skeletons()
                    << m_sceneEntity->textures()
                    << m_sceneEntity->cameras()
                    << m_sceneEntity->entities()
                    << m_sceneEntity->textureImages()
                    << m_sceneEntity->animationMappings();
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
