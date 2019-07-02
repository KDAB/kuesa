/*
    abstractassetcollection.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "abstractassetcollection.h"
#include "kuesa_p.h"

Q_LOGGING_CATEGORY(kuesa, "Kuesa")

QT_BEGIN_NAMESPACE
using namespace Kuesa;

/*!
 * \class Kuesa::AbstractAssetCollection
 * \inheaderfile Kuesa/AbstractAssetCollection
 * \brief An abstract base class for managing collections of assets in 3d
 * models.
 * \inmodule Kuesa
 * \since Kuesa 1.0
 * \inherits Qt3DCore::Node
 *
 * Kuesa::AbstractAssetCollection is an abstract class used to implement
 * collections of shared assets in 3d models.
 *
 * It manages a set of QNode derived classes, each associated with a unique
 * name.
 *
 * Assets can be added and removed. Assets which do not have a valid parent
 * will be reparented to the asset collection.
 *
 * If no asset can be found for a given name, nullptr will be returned.
 *
 * If a new asset is added with a name matching that of an already existing
 * asset in the collection, the existing asset will be replaced with the new
 * one, the old one will be removed.
 *
 * When an asset referenced by the collection is destroyed, it will
 * automatically be removed from the collection.
 *
 * Removing an asset from the collection also results in the asset being
 * destroyed if the asset's parent is the collection itself.
 */

/*!
    \property Kuesa::AbstractAssetCollection::names

    Holds the list of names for the currently managed assets.
*/

/*!
 * \internal
 */
AbstractAssetCollection::AbstractAssetCollection(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
{
    connect(this, &AbstractAssetCollection::namesChanged, this, &AbstractAssetCollection::sizeChanged);
}

/*!
 * \internal
 */
AbstractAssetCollection::~AbstractAssetCollection()
{
    clearDestructionConnections();
}

QStringList AbstractAssetCollection::names()
{
    return m_assets.keys();
}

int AbstractAssetCollection::size()
{
    return m_assets.size();
}

/*!
 * Returns true if the name exists in the collection
 */
bool AbstractAssetCollection::contains(const QString &name) const
{
    return m_assets.find(name) != m_assets.end();
}

/*!
 * Removes the asset corresponding to \a name, if it exists
 */
void AbstractAssetCollection::remove(const QString &name)
{
    auto asset = m_assets.take(name);
    if (asset) {
        //remove connection before deleting so handleAssetDestruction() is not called
        removeDestructionConnection(asset);
        if (asset->parent() == this)
            delete asset;
        emit namesChanged();
    }
}

/*!
 * Removes all assets from the collection
 */
void AbstractAssetCollection::clear()
{
    clearDestructionConnections();
    for (auto a : qAsConst(m_assets)) {
        if (a->parent() == this)
            delete a;
    }
    m_assets.clear();
    emit namesChanged();
}

/*!
 * \internal
 */
void AbstractAssetCollection::addAsset(const QString &name, Qt3DCore::QNode *asset)
{
    Q_ASSERT(asset);
    auto it = m_assets.find(name);
    const bool nameExists = it != m_assets.end();
    if (nameExists) {
        auto oldAsset = it.value();
        //remove connection before deleting so handleAssetDestruction() is not called
        removeDestructionConnection(oldAsset);
        if (oldAsset->parent() == this)
            delete oldAsset;
    }

    // Take ownership if asset has no parent
    if (asset->parent() == nullptr)
        asset->setParent(this);
    m_assets.insert(name, asset);

    if (!nameExists)
        emit namesChanged();

    addDestructionConnection(name, asset);
}

void AbstractAssetCollection::handleAssetDestruction(const QString &name)
{
    auto asset = m_assets.take(name);
    Q_ASSERT(asset);
    if (asset) {
        removeDestructionConnection(asset);
        emit namesChanged();
    }
}

void AbstractAssetCollection::addDestructionConnection(const QString &name, Qt3DCore::QNode *asset)
{
    // Remove destroyed nodes from our collection so we don't keep dangling pointer
    auto f = [this, name]() { handleAssetDestruction(name); };
    m_destructionConnections.insert(asset, connect(asset, &Qt3DCore::QNode::nodeDestroyed, this, f));
}

void AbstractAssetCollection::removeDestructionConnection(Qt3DCore::QNode *asset)
{
    QObject::disconnect(m_destructionConnections.take(asset));
}

void AbstractAssetCollection::clearDestructionConnections()
{
    // Disconnect each connection that was stored
    for (const auto &connection : qAsConst(m_destructionConnections))
        QObject::disconnect(connection);
    m_destructionConnections.clear();
}

/*!
 * \internal
 */
Qt3DCore::QNode *AbstractAssetCollection::findAsset(const QString &name)
{
    return m_assets.value(name, nullptr);
}

QT_END_NAMESPACE
