/*
    viewextension.cpp

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

#include "viewextension.h"
#include <Kuesa/forwardrenderer.h>
#include <Kuesa/abstractpostprocessingeffect.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

namespace {

// This will later be extended to work with all Kuesa framegraphs
View *view(QObject *parent)
{
    return qobject_cast<View *>(parent);
}

} // namespace

/*!
 * \class ViewExtension
 * \internal
 */

/*!
    \qmlproperty list<AbstractPostProcessingEffect>  Kuesa::ViewExtension::postProcessingEffects

    List of postprocessing effects to perform. They are executed in the order
    of declaration
*/

/*!
    \qmlproperty list<Layer>  Kuesa::ViewExtension::layers

    Inclusive of layers that are selected to render the scene. If nothing is
    set, no layer filtering takes place and everything is selected.
*/

/*!
    \qmlproperty list<ReflectionPlane>  Kuesa::ViewExtension::reflectionPlanes

    List of reflection planes to used when rendering the view.
*/

ViewExtension::ViewExtension(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<AbstractPostProcessingEffect> ViewExtension::postProcessingEffects()
{
    return QQmlListProperty<AbstractPostProcessingEffect>(this, 0,
                                                          ViewExtension::appendFX,
                                                          ViewExtension::fxCount,
                                                          ViewExtension::fxAt,
                                                          ViewExtension::clearFx);
}

QQmlListProperty<Qt3DRender::QLayer> ViewExtension::layers()
{
    return QQmlListProperty<Qt3DRender::QLayer>(this, 0,
                                                ViewExtension::appendLayer,
                                                ViewExtension::layersCount,
                                                ViewExtension::layerAt,
                                                ViewExtension::clearLayers);
}

QQmlListProperty<ReflectionPlane> ViewExtension::reflectionPlanes()
{
    return QQmlListProperty<ReflectionPlane>(this, 0,
                                             ViewExtension::appendReflectionPlane,
                                             ViewExtension::reflectionPlaneCount,
                                             ViewExtension::reflectionPlaneAt,
                                             ViewExtension::clearReflectionPlanes);
}

void ViewExtension::appendFX(QQmlListProperty<AbstractPostProcessingEffect> *list, AbstractPostProcessingEffect *fx)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (fx == nullptr || kuesaFG == nullptr)
        return;
    kuesaFG->addPostProcessingEffect(fx);
}

AbstractPostProcessingEffect *ViewExtension::fxAt(QQmlListProperty<AbstractPostProcessingEffect> *list, qt_size_type index)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return nullptr;
    return kuesaFG->postProcessingEffects().at(index);
}

qt_size_type ViewExtension::fxCount(QQmlListProperty<AbstractPostProcessingEffect> *list)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    auto kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return 0;
    return int(kuesaFG->postProcessingEffects().size());
}

void ViewExtension::clearFx(QQmlListProperty<AbstractPostProcessingEffect> *list)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return;
    const std::vector<AbstractPostProcessingEffect *> effects = kuesaFG->postProcessingEffects();
    for (AbstractPostProcessingEffect *fx : effects)
        kuesaFG->removePostProcessingEffect(fx);
}

void ViewExtension::appendLayer(QQmlListProperty<Qt3DRender::QLayer> *list, Qt3DRender::QLayer *layer)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return;
    return kuesaFG->addLayer(layer);
}

Qt3DRender::QLayer *ViewExtension::layerAt(QQmlListProperty<Qt3DRender::QLayer> *list, qt_size_type index)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return nullptr;
    return kuesaFG->layers().at(index);
}

qt_size_type ViewExtension::layersCount(QQmlListProperty<Qt3DRender::QLayer> *list)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return 0;
    return int(kuesaFG->layers().size());
}

void ViewExtension::clearLayers(QQmlListProperty<Qt3DRender::QLayer> *list)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return;
    const std::vector<Qt3DRender::QLayer *> effects = kuesaFG->layers();
    for (Qt3DRender::QLayer *layer : effects)
        kuesaFG->removeLayer(layer);
}

void ViewExtension::appendReflectionPlane(QQmlListProperty<ReflectionPlane> *list, ReflectionPlane *plane)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return;
    return kuesaFG->addReflectionPlane(plane);
}

ReflectionPlane *ViewExtension::reflectionPlaneAt(QQmlListProperty<ReflectionPlane> *list, qt_size_type index)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return nullptr;
    return kuesaFG->reflectionPlanes().at(index);
}

qt_size_type ViewExtension::reflectionPlaneCount(QQmlListProperty<ReflectionPlane> *list)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return 0;
    return int(kuesaFG->reflectionPlanes().size());
}

void ViewExtension::clearReflectionPlanes(QQmlListProperty<ReflectionPlane> *list)
{
    ViewExtension *self = static_cast<ViewExtension *>(list->object);
    View *kuesaFG = view(self->parent());
    if (kuesaFG == nullptr)
        return;
    const std::vector<Kuesa::ReflectionPlane *> planes = kuesaFG->reflectionPlanes();
    for (Kuesa::ReflectionPlane *plane : planes)
        kuesaFG->removeReflectionPlane(plane);
}



} // namespace Kuesa

QT_END_NAMESPACE
