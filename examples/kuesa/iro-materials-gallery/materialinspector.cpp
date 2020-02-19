/*
    materialinspector.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QPickEvent>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QComponent>
#include <QDebug>
#include <Kuesa/GLTF2Material>
#include <Kuesa/GLTF2MaterialProperties>
#include "materialinspector.h"

namespace {

template<typename ComponentType>
inline ComponentType *componentFromEntity(Qt3DCore::QEntity *e)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    const auto cmps = e->componentsOfType<ComponentType>();
    return cmps.size() > 0 ? cmps.first() : nullptr;
#else
    ComponentType *typedComponent = nullptr;
    const Qt3DCore::QComponentVector cmps = e->components();

    for (Qt3DCore::QComponent *c : cmps) {
        typedComponent = qobject_cast<ComponentType *>(c);
        if (typedComponent != nullptr)
            break;
    }

    return typedComponent;
#endif
}

template<typename ComponentType>
ComponentType *findFirstComponentInstanceInHierarchy(Qt3DCore::QEntity *root)
{
    ComponentType *cmp = componentFromEntity<ComponentType>(root);

    if (cmp)
        return cmp;

    const Qt3DCore::QNodeVector children = root->childNodes();
    for (Qt3DCore::QNode *child : children) {
        Qt3DCore::QEntity *e = qobject_cast<Qt3DCore::QEntity *>(child);
        if (e) {
            cmp = findFirstComponentInstanceInHierarchy<ComponentType>(e);
            if (cmp)
                return cmp;
        }
    }

    return nullptr;
}

} // anonymous

MaterialInspector::MaterialInspector(QObject *parent)
    : QObject(parent)
    , m_material(nullptr)
{
}

Kuesa::GLTF2Material *MaterialInspector::material() const
{
    return  m_material;
}

void MaterialInspector::inspect(Qt3DRender::QPickEvent *pick)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    qWarning() << "Picking materials only available since 5.14";
    return;
#else
    Qt3DCore::QEntity *entity = pick->entity();

    if (entity == nullptr) {
        setMaterial(nullptr);
        return;
    }

    Kuesa::GLTF2Material *material = findFirstComponentInstanceInHierarchy<Kuesa::GLTF2Material>(entity);
    setMaterial(material);
#endif
}

void MaterialInspector::setMaterial(Kuesa::GLTF2Material *material)
{
    if (m_material == material)
        return;
    m_material = material;
    emit materialChanged();
}

