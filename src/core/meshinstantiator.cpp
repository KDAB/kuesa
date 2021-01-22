/*
    kuesanode.h

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

#include <Kuesa/private/kuesa_global_p.h>
#include <Kuesa/private/kuesa_utils_p.h>
#include <Qt3DRender/QGeometryRenderer>
#include <Kuesa/GLTF2Material>
#include <Kuesa/GLTF2MaterialEffect>
#include "meshinstantiator.h"
#include <stdio.h>
#include <string.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#else
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#endif

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::MeshInstantiator
    \inheaderfile Kuesa/MeshInstantiator
    \inmodule Kuesa
    \inherits Kuesa::KuesaNode
    \since Kuesa 1.3

    \brief Kuesa::MeshInstantiator allows to render several instances of a same
    mesh with different transformations.

    Instanced drawing is a drawing technique that relies on a single draw call
    being made to the graphics API and letting the GPU draw several instances
    of the same base mesh.

    Each mesh instance can be controlled by per instance properties provided
    under the form of per instance attributes.

    Kuesa::MeshInstantiator simplifies that process by expecting users provide
    an array of QMatrix4x4 transformations. The number of transformations
    dictates the number of instances to draw and each instance will be
    transformed by one of the transformations.

    There is no strict limitations on how many instances can be drawn as it is
    GPU dependent. For simple meshes, this can easily be thousands of
    instances.

    \note For the instances to be visible, you should ensure that either
    frustum culling is disabled or that the initial instances (the mesh with no
    transformation applies) fit within the view frustum. Furthermore care needs
    to be taken to not share the material you intend to use with the instanced
    meshes with the material used for non instanced meshes.
*/

/*!
    \property Kuesa::KuesaNode::entityName

    The name of the entity to be retrieved from the \l {Kuesa::EntityCollection}
    and instantiated.
*/

/*!
    \property Kuesa::KuesaNode::count
    \readonly

    The number of instances being drawn. The number of instances being drawn.
    It will be at least 1 even if no transformation matrices were provided.
*/

/*!
    \qmltype MeshInstantiator
    \instantiates Kuesa::MeshInstantiator
    \inqmlmodule Kuesa
    \inherits KuesaNode
    \since Kuesa 1.3

    \brief Kuesa::MeshInstantiator allows to render several instances of a same
    mesh with different transformations.

    Instanced drawing is a drawing technique that relies on a single draw call
    being made to the graphics API and letting the GPU draw several instances
    of the same base mesh.

    Each mesh instance can be controlled by per instance properties provided
    under the form of per instance attributes.

    Kuesa::MeshInstantiator simplifies that process by expecting users provide
    an array of QMatrix4x4 transformations. The number of transformations
    dictates the number of instances to draw and each instance will be
    transformed by one of the transformations.

    There is no strict limitations on how many instances can be drawn as it is
    GPU dependent. For simple meshes, this can easily be thousands of
    instances.

    \note For the instances to be visible, you should ensure that either
    frustum culling is disabled or that the initial instances (the mesh with no
    transformation applies) fit within the view frustum. Furthermore care needs
    to be taken to not share the material you intend to use with the instanced
    meshes with the material used for non instanced meshes.
*/

/*!
    \qmlproperty string Kuesa::KuesaNode::entityName

    The name of the entity to be retrieved from the \l {Kuesa::EntityCollection}
    and instantiated.
 */

/*!
    \qmlproperty int Kuesa::KuesaNode::count
    \readonly

    The number of instances being drawn. It will be at least 1 even if no
    transformation matrices were provided.
*/

MeshInstantiator::MeshInstantiator(Qt3DCore::QNode *parent)
    : KuesaNode(parent)
    , m_transformationsBuffer(new Qt3DGeometry::QBuffer(this))
    , m_perInstanceTransformationAttribute(new Qt3DGeometry::QAttribute(this))
{
    QObject::connect(this, &KuesaNode::sceneEntityChanged,
                     this, [this] {
        disconnect(m_loadingDoneConnection);
        if (m_sceneEntity)
            m_loadingDoneConnection = connect(m_sceneEntity, &SceneEntity::loadingDone,
                                              this, &MeshInstantiator::update);
        update();
    });

    m_perInstanceTransformationAttribute->setAttributeType(Qt3DGeometry::QAttribute::VertexAttribute);
    m_perInstanceTransformationAttribute->setDivisor(1);
    m_perInstanceTransformationAttribute->setName(QStringLiteral("perInstanceTransform"));
    m_perInstanceTransformationAttribute->setVertexSize(16);
    m_perInstanceTransformationAttribute->setVertexBaseType(Qt3DGeometry::QAttribute::Float);
    m_perInstanceTransformationAttribute->setBuffer(m_transformationsBuffer);

    updateTransformBuffer();
}

int MeshInstantiator::count() const
{
    return int(std::max(m_transformations.size(), size_t(1)));
}

void MeshInstantiator::setEntityName(const QString &entityName)
{
    if (entityName == m_entityName)
        return;
    m_entityName = entityName;
    emit entityNameChanged(m_entityName);
    update();
}

QString MeshInstantiator::entityName() const
{
    return m_entityName;
}

/*!
    Set the transformation matrices \a transformationMatrices to be associated
    and applied to the instances.
 */
void MeshInstantiator::setTransformationMatrices(const std::vector<QMatrix4x4> &transformationMatrices)
{
    if (transformationMatrices == m_transformations)
        return;
    m_transformations = transformationMatrices;
    emit transformationMatricesChanged(m_transformations);
    updateTransformBuffer();
}

/*!
    Returns the transformation matrices.
 */
const std::vector<QMatrix4x4> &MeshInstantiator::transformationMatrices() const
{
    return m_transformations;
}

void MeshInstantiator::updateTransformBuffer()
{
    const std::vector<QMatrix4x4> &matrices = m_transformations.empty() ? std::vector<QMatrix4x4>({QMatrix4x4()}) : m_transformations;

    QByteArray rawData;
    rawData.resize(16 * sizeof(float) * matrices.size());

    // Note sizeof(QMatrix4x4) != 16 * sizeof(float)
    size_t offset = 0;
    for (const QMatrix4x4 &m : matrices) {
        // QMatrix4x4::constData is in column major order which is what we want
        memcpy(rawData.data() + offset, m.constData(), 16 * sizeof(float));
        offset += 16 * sizeof(float);
    }

    m_perInstanceTransformationAttribute->setCount(matrices.size());
    m_transformationsBuffer->setData(rawData);

    update();
}

void MeshInstantiator::update()
{
    Qt3DCore::QEntity *meshEntity = m_sceneEntity ? m_sceneEntity->entity(m_entityName) : nullptr;
    const bool entityChanged = meshEntity != m_entity;
    const size_t instanceCount = std::max(m_transformations.size(), size_t(1));

    // Handle change of Entity
    if (entityChanged) {

        if (m_entity)
            disconnect(m_entity);

        m_entity = meshEntity;

        if (m_entity != nullptr) {
            QObject::connect(m_entity, &Qt3DCore::QNode::nodeDestroyed, this, &MeshInstantiator::update);
            // Add instance attribute on all the child meshes that contain a QGeometryRenderer
            // and update material of each QGeometryRenderer with instancing layer
            for (QObject *c : m_entity->children()) {
                Qt3DCore::QEntity *e = qobject_cast<Qt3DCore::QEntity *>(c);

                if (e == nullptr)
                    continue;

                Qt3DRender::QGeometryRenderer *r = componentFromEntity<Qt3DRender::QGeometryRenderer>(e);
                Kuesa::GLTF2Material *m = componentFromEntity<Kuesa::GLTF2Material>(e);

                if (r == nullptr || m == nullptr)
                    continue;

                GLTF2MaterialEffect *effect = qobject_cast<GLTF2MaterialEffect *>(m->effect());
                if (effect == nullptr)
                    continue;

                // Create Effect with instancing if not set
                if (!effect->isInstanced())
                    effect->setInstanced(true);

                if (r->geometry()) {
                    const auto &attrs = r->geometry()->attributes();
                    auto it = std::find_if(attrs.cbegin(),
                                           attrs.cend(), [] (const Qt3DGeometry::QAttribute *attr) {
                        return attr->name() == QStringLiteral("perInstanceTransform");
                    });
                    if (it == attrs.end())
                        r->geometry()->addAttribute(m_perInstanceTransformationAttribute);
                }
                r->setInstanceCount(instanceCount);
            }
        }
    }

    // Handle instance count change
    const bool instanceCountHasChanged = m_instanceCount != instanceCount;

    if (m_entity && instanceCountHasChanged) {
        for (QObject *c : m_entity->children()) {
            Qt3DCore::QEntity *e = qobject_cast<Qt3DCore::QEntity *>(c);
            if (e == nullptr)
                continue;

            Qt3DRender::QGeometryRenderer *r = componentFromEntity<Qt3DRender::QGeometryRenderer>(e);
            if (r == nullptr)
                continue;

            r->setInstanceCount(instanceCount);
        }
    }

    if (instanceCountHasChanged)
        emit countChanged(instanceCount);
}

} // namespace Kuesa

QT_END_NAMESPACE
