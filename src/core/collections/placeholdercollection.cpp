/*
    placeholdercollection.cpp

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

#include "placeholdercollection.h"

/*!
 * \class Kuesa::PlaceholderCollection
 * \inheaderfile Kuesa/PlaceholderCollection
 * \brief A collection of assets of type Kuesa::Placeholder.
 * \inmodule Kuesa
 * \since Kuesa 1.3
 * \inherits Kuesa::AbstractAssetCollection
 *
 * Kuesa::PlaceholderCollection manages a set of Kuesa::Placeholder assets.
 */

/*!
 * \fn  PlaceholderCollection::add
 *
 * Adds an \a asset to the collection with key \a name.
 *
 * If the asset has no parent, it will be reparented to the collection.
 *
 * If an asset is already registered with the same name, it will be removed
 * from the collection and deleted if parented to the collection.
 *
 */

/*!
 * \fn PlaceholderCollection::find
 *
 * Returns the asset previously registered with the collection with \a name, if
 * it exists. Otherwise, returns nullptr.
 *
 */

QT_BEGIN_NAMESPACE

namespace Kuesa {

PlaceholderCollection::PlaceholderCollection(Qt3DCore::QNode *parent)
    : AbstractAssetCollection(parent)
{
}

PlaceholderCollection::~PlaceholderCollection()
{
}

} // namespace Kuesa

QT_END_NAMESPACE
