/*
    factory.h

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

#ifndef KUESA_FACTORY_H
#define KUESA_FACTORY_H

#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

template<class AbstractProduct,
         typename IdentifierType,
         typename ProductCreator>
class Factory
{
public:
    void registerType(const IdentifierType &id, ProductCreator creator)
    {
        m_typeMap.insert(id, creator);
    }

    void unregisterType(const IdentifierType &id)
    {
        m_typeMap.remove(id);
    }

    AbstractProduct *create(const IdentifierType &id,
                            const QVariantMap &properties)
    {
        auto it = m_typeMap.find(id);
        if (it == m_typeMap.end())
            return nullptr;

        // Call the product creator
        auto object = it.value()();
        if (!object)
            return nullptr;

        // Set properties on the newly created object
        const auto end = properties.constEnd();
        for (auto jt = properties.constBegin(); jt != end; ++jt)
            object->setProperty(jt.key().toLocal8Bit(), jt.value());

        return object;
    }

private:
    using TypeMap = QHash<IdentifierType, ProductCreator>;
    TypeMap m_typeMap;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_FACTORY_H
