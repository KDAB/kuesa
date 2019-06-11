/*
    effectcollection.cpp

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

#include "effectcollection.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;

/*!
 * \class Kuesa::EffectCollection
 * \inheaderfile Kuesa/EffectCollection
 * \brief A collection of assets of type Qt3DRender::QEffect.
 * \inmodule Kuesa
 * \since 1.0
 * \inherits Kuesa::AbstractAssetCollection
 *
 * Kuesa::EffectCollection manages a set of Qt3DRender::QEffect assets.
 *
 * It also works as a factory and provides a way of creating instances of
 * effects based on a unique key.
 *
 * Factory functions are created by calling EffectCollection::registerType.
 *
 * These functions are used to create specific instances of QEffect derived
 * class when calling EffectCollection::addEffect.
 *
 */

/*!
 * \fn EffectCollection::add
 *
 * Adds an \a asset to the collection with key \a name.
 *
 * If the asset has no parent, it will be reparented to the collection.
 *
 * If an asset is already registered with the same name, it will be removed from the collection
 * and deleted if parented to the collection.
 *
 */

/*!
 * \fn EffectCollection::find
 *
 * Returns the asset previously registered with the collection with \a name, if it exists.
 * Otherwise, returns nullptr.
 *
 */

EffectCollection::EffectCollection(Qt3DCore::QNode *parent)
    : AbstractAssetCollection(parent)
{
}

/*! \internal */
EffectCollection::~EffectCollection()
{
}

/*!
 * Creates a new instance of an asset using a previously registered factory method.
 *
 * \a id is used to find the appropriate factory. Once the effect is created, the
 * content of the \a properties map will be assigned on the new effect. The effect
 * is then added to the collection and returned to the caller.
 *
 * Any pre-existing effect with the same \a name will be deleted.
 */
Qt3DRender::QEffect *EffectCollection::addEffect(const QString &name,
                                                 const QString &id,
                                                 const QVariantMap &properties)
{
    // Delete any old effect under this name
    remove(name);

    // Create a new effect
    auto effect = m_factory.create(id, properties);
    if (effect)
        addAsset(name, effect);
    return effect;
}

/*!
 * Register a new functor with the factory.
 *
 * The functor \a f can be any callable function that returns an new QEffect (or
 * derived) instance.
 *
 * The registered \a id should be passed to EffectCollection::addEffect for the
 * functor to be invoked.
 */
void EffectCollection::registerType(const QString &id, EffectCreator f)
{
    m_factory.registerType(id, f);
}

/*!
 * Removes a previous registered functor from the factory.
 */
void EffectCollection::unregisterType(const QString &id)
{
    m_factory.unregisterType(id);
}

QT_END_NAMESPACE
