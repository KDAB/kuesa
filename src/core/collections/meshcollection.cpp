/*
    meshcollection.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "meshcollection.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;

/*!
 * \class Kuesa::MeshCollection
 * \inheaderfile Kuesa/MeshCollection
 * \brief A collection of assets of type Qt3DRender::QGeometryRenderer
 * \inmodule Kuesa
 * \since 1.0
 * \inherits Kuesa::AbstractAssetCollection
 *
 * Kuesa::MeshCollection manages a set of Qt3DRender::QGeometryRenderer assets.
 */

/*!
 * \fn MeshCollection::add
 *
 * Adds an \a asset to the collection with key \name.
 *
 * If the asset has no parent, it will be reparented to the collection.
 *
 * If an asset is already registered with the same name, it will be removed from the collection
 * and deleted if parented to the collection.
 *
 */

/*!
 * \fn MeshCollection::find
 *
 * Returns the asset previously registered with the collection with \a name, if it exists.
 * Otherwise, returns nullptr.
 *
 */

MeshCollection::MeshCollection(Qt3DCore::QNode *parent)
    : AbstractAssetCollection(parent)
{
}

/*! \internal */
MeshCollection::~MeshCollection()
{
}

/*!
 * Utility method to create an instance of Qt3DRender::QMesh for
 * the \a source and register it in the collection with the \a name.
 *
 * The generated instance is parented with the collection and returned to the caller.
 */
Qt3DRender::QMesh *MeshCollection::add(const QString &name, const QUrl &source)
{
    auto mesh = new Qt3DRender::QMesh(this);
    mesh->setObjectName(name);
    addAsset(name, mesh);
    mesh->setSource(source);
    return mesh;
}

QT_END_NAMESPACE

