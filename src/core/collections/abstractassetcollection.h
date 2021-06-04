/*
    abstractassetcollection.h

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

#ifndef KUESA_ABSTRACTASSETCOLLECTION_H
#define KUESA_ABSTRACTASSETCOLLECTION_H

#include <Qt3DCore/qnode.h>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class KUESASHARED_EXPORT AbstractAssetCollection : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QStringList names READ names NOTIFY namesChanged)
    Q_PROPERTY(int size READ size NOTIFY sizeChanged)
public:
    ~AbstractAssetCollection();

    QStringList names();
    int size();
    bool contains(const QString &name) const;
    bool contains(Qt3DCore::QNode *asset) const;
    Qt3DCore::QNode *findAsset(const QString &name);

    void remove(const QString &name);
    void clear();

protected:
    explicit AbstractAssetCollection(Qt3DCore::QNode *parent = nullptr);

    void addAsset(const QString &name, Qt3DCore::QNode *asset);

Q_SIGNALS:
    void namesChanged();
    void sizeChanged();
    void assetAdded(const QString &name);

private:
    void handleAssetDestruction(const QString &name);
    void addDestructionConnection(const QString &name, Qt3DCore::QNode *asset);
    void removeDestructionConnection(const QString &name, Qt3DCore::QNode *asset);
    void clearDestructionConnections();

    QMap<QString, Qt3DCore::QNode *> m_assets;
    QHash<std::pair<QString, QNode *>, QMetaObject::Connection> m_destructionConnections;
};

} // namespace Kuesa

QT_END_NAMESPACE

#define KUESA_ASSET_COLLECTION_IMPLEMENTATION(AssetType)                                   \
public:                                                                                    \
    using ContentType = AssetType;                                                         \
    Q_INVOKABLE void add(const QString &name, AssetType *asset) { addAsset(name, asset); } \
    Q_INVOKABLE AssetType *find(const QString &name) { return static_cast<AssetType *>(findAsset(name)); }

#endif // KUESA_ABSTRACTASSETCOLLECTION_H
