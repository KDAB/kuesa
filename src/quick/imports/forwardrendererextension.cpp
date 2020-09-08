/*
    forwardrenderextension.cpp

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

#include "forwardrendererextension.h"
#include <Kuesa/forwardrenderer.h>
#include <Kuesa/abstractpostprocessingeffect.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

namespace {

// This will later be extended to work with all Kuesa framegraphs
ForwardRenderer *kuesaFrameGraph(QObject *parent)
{
    return qobject_cast<ForwardRenderer *>(parent);
}

} // namespace

/*!
 * \class ForwardRendererExtension
 * \internal
 */

/*!
    \qmlproperty list<AbstractPostProcessingEffect>  Kuesa::Forward::childNodes

    List of postprocessing effects to perform. They are executed in the order
    of declaration
*/
ForwardRendererExtension::ForwardRendererExtension(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<AbstractPostProcessingEffect> ForwardRendererExtension::postProcessingEffects()
{
    return QQmlListProperty<AbstractPostProcessingEffect>(this, 0,
                                                          ForwardRendererExtension::appendFX,
                                                          ForwardRendererExtension::fxCount,
                                                          ForwardRendererExtension::fxAt,
                                                          ForwardRendererExtension::clearFx);
}

QQmlListProperty<Qt3DRender::QLayer> ForwardRendererExtension::layers()
{
    return QQmlListProperty<Qt3DRender::QLayer>(this, 0,
                                                ForwardRendererExtension::appendLayer,
                                                ForwardRendererExtension::layersCount,
                                                ForwardRendererExtension::layerAt,
                                                ForwardRendererExtension::clearLayers);
}

void ForwardRendererExtension::appendFX(QQmlListProperty<AbstractPostProcessingEffect> *list, AbstractPostProcessingEffect *fx)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (fx == nullptr || kuesaFG == nullptr)
        return;
    kuesaFG->addPostProcessingEffect(fx);
}

AbstractPostProcessingEffect *ForwardRendererExtension::fxAt(QQmlListProperty<AbstractPostProcessingEffect> *list, int index)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return nullptr;
    return kuesaFG->postProcessingEffects().at(index);
}

int ForwardRendererExtension::fxCount(QQmlListProperty<AbstractPostProcessingEffect> *list)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return 0;
    return kuesaFG->postProcessingEffects().size();
}

void ForwardRendererExtension::clearFx(QQmlListProperty<AbstractPostProcessingEffect> *list)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return;
    const QVector<AbstractPostProcessingEffect *> effects = kuesaFG->postProcessingEffects();
    for (AbstractPostProcessingEffect *fx : effects)
        kuesaFG->removePostProcessingEffect(fx);
}

void ForwardRendererExtension::appendLayer(QQmlListProperty<Qt3DRender::QLayer> *list, Qt3DRender::QLayer *layer)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return;
    return kuesaFG->addLayer(layer);
}

Qt3DRender::QLayer *ForwardRendererExtension::layerAt(QQmlListProperty<Qt3DRender::QLayer> *list, int index)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return nullptr;
    return kuesaFG->layers().at(index);
}

int ForwardRendererExtension::layersCount(QQmlListProperty<Qt3DRender::QLayer> *list)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return 0;
    return kuesaFG->layers().count();
}

void ForwardRendererExtension::clearLayers(QQmlListProperty<Qt3DRender::QLayer> *list)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return;
    const QVector<Qt3DRender::QLayer *> effects = kuesaFG->layers();
    for (Qt3DRender::QLayer *layer : effects)
        kuesaFG->removeLayer(layer);
}

} // namespace Kuesa

QT_END_NAMESPACE
