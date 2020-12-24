/*
    kuesa_utils_p.h

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

#ifndef KUESA_UTILS_P_H
#define KUESA_UTILS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QtGlobal>
#include <QtCore/QVector>
#include <Qt3DCore/QEntity>
#include <vector>
#include <algorithm>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
class Entity;
}
} // namespace Qt3DRender
namespace Kuesa {

class ForwardRenderer;

template<typename ComponentType>
inline ComponentType *componentFromEntity(Qt3DCore::QEntity *e)
{
    const auto cmps = e->componentsOfType<ComponentType>();
    return cmps.size() > 0 ? cmps.first() : nullptr;
}

template<typename ComponentType>
inline QVector<ComponentType *> componentsFromEntity(const Qt3DCore::QEntity *e)
{
    return e->componentsOfType<ComponentType>();
}

namespace Utils {

template<typename T, typename U>
bool contains(const std::vector<T>& destination, const U& element) noexcept
{
    return std::find(destination.begin(), destination.end(), element) != destination.end();
}

template<typename T, typename U>
size_t removeAll(std::vector<T>& destination, const U& element) noexcept
{
    auto it = std::remove(destination.begin(), destination.end(), element);
    if (it == destination.end())
        return 0;
    const size_t elementsRemoved = std::distance(it, destination.end());
    destination.erase(it, destination.end());
    return elementsRemoved;
}

// Find ForwardRenderer. This uses Qt3D's backend, so only works once the scene is
// rendering.
ForwardRenderer *findForwardRenderer(Qt3DCore::QNode *nodeInScene);

// Find the root backend entity, which can be used to find the world bounding
// volume
Qt3DRender::Render::Entity *findBackendRootEntity(Qt3DCore::QNode *nodeInScene);

} // namespace Utils

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_UTILS_P_H
