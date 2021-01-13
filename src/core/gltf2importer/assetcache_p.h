/*
    assetcache_p.h

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

#ifndef KUESA_GLTF2IMPORT_ASSETCACHE_P_H
#define KUESA_GLTF2IMPORT_ASSETCACHE_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QHash>
#include <Qt3DCore/QNode>
#include <Kuesa/SceneEntity>

QT_BEGIN_NAMESPACE

namespace Kuesa {

template<typename GLTFAsset, typename Qt3DResource>
class AssetCache
{
public:
    ~AssetCache()
    {
        for (auto connection : m_assetDestructionConnections)
            QObject::disconnect(connection);
    }

    Qt3DResource *getResourceFromCache(const GLTFAsset &asset) const
    {
        if (asset.key.isEmpty())
            return nullptr;
        return m_assets.value(asset.key, nullptr);
    }

    void addResourceToCache(const GLTFAsset &asset, Qt3DResource *resource)
    {
        if (asset.key.isEmpty() || !resource)
            return;

        Q_ASSERT(!m_assets.contains(asset.key));
        m_assets.insert(asset.key, resource);

        if (m_sceneEntity)
           resource->setParent(m_sceneEntity);

        m_assetDestructionConnections.insert(resource,
                                             QObject::connect(resource, &Qt3DCore::QNode::destroyed,
                                                              [=] { removeResourceFromCache(resource); }));
    }

    void removeResourceFromCache(Qt3DResource *resource)
    {
        const auto it = std::find(m_assets.cbegin(), m_assets.cend(), resource);
        if (it != m_assets.cend()) {
            m_assets.erase(it);
            QObject::disconnect(m_assetDestructionConnections.take(resource));
        }
    }

    void setSceneEntity(SceneEntity *sceneEntity)
    {
        if (m_sceneEntity == sceneEntity)
            return;

        m_sceneEntity = sceneEntity;

        for (auto resource : m_assets)
            resource->setParent(sceneEntity);
    }

private:
    QHash<QString, Qt3DResource *> m_assets;
    QHash<Qt3DResource *, QMetaObject::Connection> m_assetDestructionConnections;
    SceneEntity *m_sceneEntity = nullptr;
};

} // Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_ASSETCACHE_P_H
