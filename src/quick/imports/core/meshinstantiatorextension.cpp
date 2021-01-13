/*
    meshinstantiatorextension.cpp

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

#include "meshinstantiatorextension.h"
#include <Kuesa/MeshInstantiator>
#include <Qt3DCore/QTransform>

QT_BEGIN_NAMESPACE

namespace Kuesa {

namespace {

MeshInstantiator *meshInstantiator(QObject *parent)
{
    return qobject_cast<MeshInstantiator *>(parent);
}

} // namespace

MeshInstantiatorExtension::MeshInstantiatorExtension(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3DCore::QTransform> MeshInstantiatorExtension::transforms()
{
    return QQmlListProperty<Qt3DCore::QTransform>(this, 0,
                                             MeshInstantiatorExtension::appendTransform,
                                             MeshInstantiatorExtension::transformCount,
                                             MeshInstantiatorExtension::transformAt,
                                                  MeshInstantiatorExtension::clearTransforms);
}

void MeshInstantiatorExtension::updateTransforms()
{
    std::vector<QMatrix4x4> matrices;
    matrices.reserve(m_transforms.size());
    for (const Qt3DCore::QTransform *t : m_transforms)
        matrices.push_back(t->matrix());
    meshInstantiator(parent())->setTransformationMatrices(matrices);
}

void MeshInstantiatorExtension::appendTransform(QQmlListProperty<Qt3DCore::QTransform> *list,
                                                Qt3DCore::QTransform *t)
{
    MeshInstantiatorExtension *self = static_cast<MeshInstantiatorExtension *>(list->object);
    self->m_transforms.push_back(t);
    QObject::connect(t, &Qt3DCore::QTransform::matrixChanged,
                     self, &MeshInstantiatorExtension::updateTransforms);
    self->updateTransforms();
}

Qt3DCore::QTransform *MeshInstantiatorExtension::transformAt(QQmlListProperty<Qt3DCore::QTransform> *list,
                                                             qt_size_type index)
{
    MeshInstantiatorExtension *self = static_cast<MeshInstantiatorExtension *>(list->object);
    return self->m_transforms.at(index);
}

qt_size_type MeshInstantiatorExtension::transformCount(QQmlListProperty<Qt3DCore::QTransform> *list)
{
    MeshInstantiatorExtension *self = static_cast<MeshInstantiatorExtension *>(list->object);
    return self->m_transforms.size();
}

void MeshInstantiatorExtension::clearTransforms(QQmlListProperty<Qt3DCore::QTransform> *list)
{
    MeshInstantiatorExtension *self = static_cast<MeshInstantiatorExtension *>(list->object);
    for (Qt3DCore::QTransform *t : self->m_transforms)
        t->disconnect(self);
    self->m_transforms.clear();
    self->updateTransforms();
}

} // namespace Kuesa

QT_END_NAMESPACE
