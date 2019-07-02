/*
    textureimagecollection.cpp

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

#include "textureimagecollection.h"

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
 * \class Kuesa::TextureImageCollection
 * \inheaderfile Kuesa/TextureImageCollection
 * \brief A collection of assets of type Qt3DRender::QAbstractTextureImage.
 * \inmodule Kuesa
 * \since Kuesa 1.0
 * \inherits Kuesa::AbstractAssetCollection
 *
 * Kuesa::TextureImageCollection manages a set of
 * Qt3DRender::QAbstractTextureImage assets.
 */

/*!
 * \fn TextureImageCollection::add
 *
 * Adds an \a asset to the collection with key \a name.
 *
 * If the asset has no parent, it will be reparented to the collection.
 *
 * If an asset is already registered with the same name, it will be removed
 * from the collection and deleted if parented to the collection.
 *
 * As of Qt 5.11, Qt3D doesn't support sharing of QAbstractTextureImage
 * instances among several QTexture instances. It should therefore be noted
 * that the collection should only be used as a way to reference images used by
 * a single texture at a time.
 *
 */

/*!
 * \fn TextureImageCollection::find
 *
 * Returns the asset previously registered with the collection with \a name, if
 * it exists. Otherwise, returns nullptr.
 *
 */

TextureImageCollection::TextureImageCollection(Qt3DCore::QNode *parent)
    : AbstractAssetCollection(parent)
{
}

TextureImageCollection::~TextureImageCollection()
{
}

/*!
 * Utility method to create an instance of Qt3DRender::QTextureImage for the
 * \a source and register it in the collection with the \a name.
 *
 * The generated instance is parented with the collection and returned to the
 * caller.
 */
Qt3DRender::QTextureImage *TextureImageCollection::add(const QString &name, const QUrl &source)
{
    auto textureImage = new Qt3DRender::QTextureImage(this);
    textureImage->setObjectName(name);
    textureImage->setSource(source);
    addAsset(name, textureImage);
    return textureImage;
}

} // namespace Kuesa

QT_END_NAMESPACE
