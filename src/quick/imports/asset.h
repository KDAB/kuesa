/*
    asset.h

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

#ifndef KUESA_ASSET_H
#define KUESA_ASSET_H

#include <Qt3DCore/QNode>

QT_BEGIN_NAMESPACE

namespace Kuesa {
class SceneEntity;
class AbstractAssetCollection;

class AssetProperty;

class Asset : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Kuesa::SceneEntity *sceneEntity READ sceneEntity WRITE setSceneEntity NOTIFY sceneEntityChanged)
    Q_PROPERTY(Kuesa::AbstractAssetCollection *collection READ collection WRITE setCollection NOTIFY collectionChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Qt3DCore::QNode *node READ node NOTIFY nodeChanged)

public:
    Asset(Qt3DCore::QNode *parent = nullptr);

    SceneEntity *sceneEntity() const;
    AbstractAssetCollection *collection() const;
    QString name() const;
    Qt3DCore::QNode *node() const;

public Q_SLOTS:
    void setSceneEntity(Kuesa::SceneEntity *sceneEntity);
    void setCollection(Kuesa::AbstractAssetCollection *collection);
    void setName(const QString &name);

Q_SIGNALS:
    void sceneEntityChanged(Kuesa::SceneEntity *sceneEntity);
    void collectionChanged(Kuesa::AbstractAssetCollection *collection);
    void nameChanged(QString name);
    void nodeChanged(Qt3DCore::QNode *node);

private:
    SceneEntity *m_scene;
    AbstractAssetCollection *m_collection;
    QString m_name;
    Qt3DCore::QNode *m_node;
    QHash<QNode *, QMetaObject::Connection> m_destructionConnections;
    QMetaObject::Connection m_releaseAssetPropertiesConnection;

    void setNode(Qt3DCore::QNode *node);
    void findAsset();
    void updateSceneFromParent(Qt3DCore::QNode *parent);

    std::vector<AssetProperty *> m_assetProperties;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_ASSET_H
