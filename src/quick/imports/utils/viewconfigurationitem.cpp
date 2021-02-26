/*
    viewconfigurationitem.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include "viewconfigurationitem.h"

using namespace KuesaUtils;

/*!
 * \class KuesaUtils::ViewConfigurationItem
 * \internal
 */

/*!
    \qmlproperty list<Kuesa::TransformTracker> KuesaUtils::SceneConfigurationItem::transformTrackers

    List of \l [QML] {Kuesa::TransformTracker} instances referenced by the
    SceneConfiguration.
*/

/*!
    \qmlproperty list<Kuesa::PlaceholderTracker> KuesaUtils::SceneConfigurationItem::placeholders

    List of \l [QML] {Kuesa::PlaceholderTracker} instances referenced by the
    SceneConfiguration.
*/

ViewConfigurationItem::ViewConfigurationItem(QObject *parent)
    : QObject(parent)
{
}

ViewConfigurationItem::~ViewConfigurationItem() = default;

ViewConfiguration *KuesaUtils::ViewConfigurationItem::parentViewConfiguration() const
{
    return qobject_cast<ViewConfiguration *>(parent());
}

QQmlListProperty<Kuesa::TransformTracker> ViewConfigurationItem::transformTrackers()
{
    return QQmlListProperty<Kuesa::TransformTracker>(this, nullptr,
                                                     ViewConfigurationItem::qmlAppendTransformTrackers,
                                                     ViewConfigurationItem::qmlTransformTrackersCount,
                                                     ViewConfigurationItem::qmlTransformTrackersAt,
                                                     ViewConfigurationItem::qmlClearTransformTrackers);
}

QQmlListProperty<Kuesa::PlaceholderTracker> ViewConfigurationItem::placeholderTrackers()
{
    return QQmlListProperty<Kuesa::PlaceholderTracker>(this, nullptr,
                                                       ViewConfigurationItem::qmlAppendPlaceholderTrackers,
                                                       ViewConfigurationItem::qmlPlaceholderTrackersCount,
                                                       ViewConfigurationItem::qmlPlaceholderTrackersAt,
                                                       ViewConfigurationItem::qmlClearPlaceholderTrackers);
}

void ViewConfigurationItem::qmlAppendTransformTrackers(QQmlListProperty<Kuesa::TransformTracker> *list, Kuesa::TransformTracker *node)
{
    if (node == nullptr)
        return;
    ViewConfigurationItem *self = static_cast<ViewConfigurationItem *>(list->object);
    self->m_managedTransformTrackers.push_back(node);
    self->parentViewConfiguration()->addTransformTracker(node);
}

Kuesa::TransformTracker *ViewConfigurationItem::qmlTransformTrackersAt(QQmlListProperty<Kuesa::TransformTracker> *list, qt_size_type index)
{
    ViewConfigurationItem *self = static_cast<ViewConfigurationItem *>(list->object);
    return self->m_managedTransformTrackers.at(size_t(index));
}

qt_size_type ViewConfigurationItem::qmlTransformTrackersCount(QQmlListProperty<Kuesa::TransformTracker> *list)
{
    ViewConfigurationItem *self = static_cast<ViewConfigurationItem *>(list->object);
    return int(self->m_managedTransformTrackers.size());
}

void ViewConfigurationItem::qmlClearTransformTrackers(QQmlListProperty<Kuesa::TransformTracker> *list)
{
    ViewConfigurationItem *self = static_cast<ViewConfigurationItem *>(list->object);
    self->parentViewConfiguration()->clearTransformTrackers();
    self->m_managedTransformTrackers.clear();
}

void ViewConfigurationItem::qmlAppendPlaceholderTrackers(QQmlListProperty<Kuesa::PlaceholderTracker> *list, Kuesa::PlaceholderTracker *node)
{
    if (node == nullptr)
        return;
    ViewConfigurationItem *self = static_cast<ViewConfigurationItem *>(list->object);
    self->m_managedPlaceholderTrackers.push_back(node);
    self->parentViewConfiguration()->addPlaceholderTracker(node);
}

Kuesa::PlaceholderTracker *ViewConfigurationItem::qmlPlaceholderTrackersAt(QQmlListProperty<Kuesa::PlaceholderTracker> *list, qt_size_type index)
{
    ViewConfigurationItem *self = static_cast<ViewConfigurationItem *>(list->object);
    return self->m_managedPlaceholderTrackers.at(size_t(index));
}

qt_size_type ViewConfigurationItem::qmlPlaceholderTrackersCount(QQmlListProperty<Kuesa::PlaceholderTracker> *list)
{
    ViewConfigurationItem *self = static_cast<ViewConfigurationItem *>(list->object);
    return int(self->m_managedPlaceholderTrackers.size());
}

void ViewConfigurationItem::qmlClearPlaceholderTrackers(QQmlListProperty<Kuesa::PlaceholderTracker> *list)
{
    ViewConfigurationItem *self = static_cast<ViewConfigurationItem *>(list->object);
    self->parentViewConfiguration()->clearPlaceholderTrackers();
    self->m_managedPlaceholderTrackers.clear();
}
