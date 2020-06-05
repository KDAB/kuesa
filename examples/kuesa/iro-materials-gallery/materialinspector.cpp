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

#include "materialinspector.h"

#include <Kuesa/GLTF2Material>
#include <Kuesa/GLTF2MaterialProperties>

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QPickEvent>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QComponent>

#include <QDebug>
#include <QMetaProperty>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

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

QString valueIntrospector(const QVariant &value)
{
    switch (value.userType()) {
    case QVariant::Bool:
        return value.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    case QVariant::Double:
    case QMetaType::Float:
        return QString::number(value.toDouble());
    case QVariant::Int:
    case QVariant::UInt:
        return QString::number(value.toInt());
    case QVariant::Color: {
        const QColor color = value.value<QColor>();
        return QString("[%1, %2, %3, %4]")
                .arg(color.redF())
                .arg(color.greenF())
                .arg(color.blueF())
                .arg(color.alphaF());
    }
    case QVariant::Vector2D: {
        const QVector4D v2 = value.value<QVector2D>();
        return QString("[%1, %2]")
                .arg(v2.x())
                .arg(v2.y());
    }
    case QVariant::Vector3D: {
        const QVector4D v3 = value.value<QVector3D>();
        return QString("[%1, %2, %3]")
                .arg(v3.x())
                .arg(v3.y())
                .arg(v3.z());
    }
    case QVariant::Vector4D: {
        const QVector4D v4 = value.value<QVector4D>();
        return QString("[%1, %2, %3, %4]")
                .arg(v4.x())
                .arg(v4.y())
                .arg(v4.z())
                .arg(v4.w());
    }
    case QVariant::Matrix4x4:
        return QString("Matrix");
    }

    if (value.userType() == qMetaTypeId<Qt3DRender::QAbstractTexture *>()) {
        return QStringLiteral("Texture");
    }

    return QStringLiteral("Unknown");
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

QString MaterialInspector::description() const
{
    return m_description;
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
    introspect();
}

void MaterialInspector::introspect()
{
    m_description.clear();

    if (m_material) {
        Kuesa::GLTF2MaterialProperties *materialProperties = nullptr;
        const QMetaObject *materialMetaObj = m_material->metaObject();
        QString descriptionString = QString("%1:\n").arg(materialMetaObj->className());
        QString propertyDescription = QString("%1: %2\n");
        const int materialPropertiesIdx = materialMetaObj->indexOfProperty("materialProperties");
        if (materialPropertiesIdx != -1) {
            materialProperties = m_material->property("materialProperties").value<Kuesa::GLTF2MaterialProperties *>();

            // Introspect properties
            const QMetaObject *propertiesMetaObj = materialProperties->metaObject();
            const int defaultPropertiesOffset = 5; // Offset of properties on the meta object we don't care about (e.g objectName ...)
            for (int i = defaultPropertiesOffset, m = propertiesMetaObj->propertyCount(); i < m; ++i) {
                const QMetaProperty metaProp = propertiesMetaObj->property(i);
                descriptionString += propertyDescription.arg(metaProp.name(), valueIntrospector(metaProp.read(materialProperties)));
            }
        }
        m_description = descriptionString;
    }
    emit descriptionChanged();
}

