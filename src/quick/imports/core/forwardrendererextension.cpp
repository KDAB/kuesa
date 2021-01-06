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
    \qmlproperty list<View>  Kuesa::ForwardRenderer::views

    List of sub views of the scene to be rendered.
*/
ForwardRendererExtension::ForwardRendererExtension(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Kuesa::View> ForwardRendererExtension::views()
{
    return QQmlListProperty<Kuesa::View>(this, 0,
                                         ForwardRendererExtension::appendView,
                                         ForwardRendererExtension::viewCount,
                                         ForwardRendererExtension::viewAt,
                                         ForwardRendererExtension::clearViews);
}

void ForwardRendererExtension::appendView(QQmlListProperty<Kuesa::View> *list, Kuesa::View *view)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return;
    return kuesaFG->addView(view);
}

Kuesa::View *ForwardRendererExtension::viewAt(QQmlListProperty<Kuesa::View> *list, qt_size_type index)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return nullptr;
    return kuesaFG->views().at(index);
}

qt_size_type ForwardRendererExtension::viewCount(QQmlListProperty<Kuesa::View> *list)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return 0;
    return int(kuesaFG->views().size());
}

void ForwardRendererExtension::clearViews(QQmlListProperty<Kuesa::View> *list)
{
    ForwardRendererExtension *self = static_cast<ForwardRendererExtension *>(list->object);
    auto kuesaFG = kuesaFrameGraph(self->parent());
    if (kuesaFG == nullptr)
        return;
    const std::vector<Kuesa::View *> views = kuesaFG->views();
    for (Kuesa::View *view : views)
        kuesaFG->removeView(view);
}

} // namespace Kuesa

QT_END_NAMESPACE
