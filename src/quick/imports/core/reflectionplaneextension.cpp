/*
    reflectionplaneextension.cpp

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

#include "reflectionplaneextension.h"
#include <Kuesa/reflectionplane.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

namespace {

// This will later be extended to work with all Kuesa framegraphs
ReflectionPlane *reflectionPlane(QObject *parent)
{
    return qobject_cast<ReflectionPlane *>(parent);
}

} // namespace

/*!
    \class ReflectionPlaneExtension
    \internal
 */

/*!
    \qmlproperty list<Layer>  Kuesa::ReflectionPlaneExtensions::layers

    The list of visible layers that are used to select what is
    reflected. If no layers are specified, uses the same set of layers as the
    view.
*/

ReflectionPlaneExtension::ReflectionPlaneExtension(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3DRender::QLayer> ReflectionPlaneExtension::layers()
{
    return QQmlListProperty<Qt3DRender::QLayer>(this, 0,
                                                ReflectionPlaneExtension::appendLayer,
                                                ReflectionPlaneExtension::layersCount,
                                                ReflectionPlaneExtension::layerAt,
                                                ReflectionPlaneExtension::clearLayers);
}

void ReflectionPlaneExtension::appendLayer(QQmlListProperty<Qt3DRender::QLayer> *list, Qt3DRender::QLayer *layer)
{
    ReflectionPlaneExtension *self = static_cast<ReflectionPlaneExtension *>(list->object);
    ReflectionPlane *plane = reflectionPlane(self->parent());
    if (plane == nullptr)
        return;
    return plane->addLayer(layer);
}

Qt3DRender::QLayer *ReflectionPlaneExtension::layerAt(QQmlListProperty<Qt3DRender::QLayer> *list, qt_size_type index)
{
    ReflectionPlaneExtension *self = static_cast<ReflectionPlaneExtension *>(list->object);
    ReflectionPlane *plane = reflectionPlane(self->parent());
    if (plane == nullptr)
        return nullptr;
    return plane->layers().at(index);
}

qt_size_type ReflectionPlaneExtension::layersCount(QQmlListProperty<Qt3DRender::QLayer> *list)
{
    ReflectionPlaneExtension *self = static_cast<ReflectionPlaneExtension *>(list->object);
    ReflectionPlane *plane = reflectionPlane(self->parent());
    if (plane == nullptr)
        return 0;
    return int(plane->layers().size());
}

void ReflectionPlaneExtension::clearLayers(QQmlListProperty<Qt3DRender::QLayer> *list)
{
    ReflectionPlaneExtension *self = static_cast<ReflectionPlaneExtension *>(list->object);
    ReflectionPlane *plane = reflectionPlane(self->parent());
    if (plane == nullptr)
        return;
    const std::vector<Qt3DRender::QLayer *> effects = plane->layers();
    for (Qt3DRender::QLayer *layer : effects)
        plane->removeLayer(layer);
}


} // namespace Kuesa

QT_END_NAMESPACE
