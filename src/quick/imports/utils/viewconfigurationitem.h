/*
    viewconfigurationitem.h

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

#ifndef VIEWCONFIGURATIONITEM_H
#define VIEWCONFIGURATIONITEM_H

#include <QObject>
#include <QtQml/QQmlListProperty>
#include <KuesaUtils/viewconfiguration.h>
#include <Kuesa/transformtracker.h>
#include <Kuesa/placeholdertracker.h>

#include <vector>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using qt_size_type = qsizetype;
#else
using qt_size_type = int;
#endif

QT_BEGIN_NAMESPACE

namespace KuesaUtils {

class ViewConfigurationItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Kuesa::TransformTracker> transformTrackers READ transformTrackers CONSTANT)
    Q_PROPERTY(QQmlListProperty<Kuesa::PlaceholderTracker> placeholderTrackers READ placeholderTrackers CONSTANT)
public:
    explicit ViewConfigurationItem(QObject *parent = nullptr);
    ~ViewConfigurationItem();

    ViewConfiguration *parentViewConfiguration() const;

    QQmlListProperty<Kuesa::TransformTracker> transformTrackers();
    QQmlListProperty<Kuesa::PlaceholderTracker> placeholderTrackers();

private:
    static void qmlAppendTransformTrackers(QQmlListProperty<Kuesa::TransformTracker> *list, Kuesa::TransformTracker *node);
    static Kuesa::TransformTracker *qmlTransformTrackersAt(QQmlListProperty<Kuesa::TransformTracker> *list, qt_size_type index);
    static qt_size_type qmlTransformTrackersCount(QQmlListProperty<Kuesa::TransformTracker> *list);
    static void qmlClearTransformTrackers(QQmlListProperty<Kuesa::TransformTracker> *list);

    static void qmlAppendPlaceholderTrackers(QQmlListProperty<Kuesa::PlaceholderTracker> *list, Kuesa::PlaceholderTracker *node);
    static Kuesa::PlaceholderTracker *qmlPlaceholderTrackersAt(QQmlListProperty<Kuesa::PlaceholderTracker> *list, qt_size_type index);
    static qt_size_type qmlPlaceholderTrackersCount(QQmlListProperty<Kuesa::PlaceholderTracker> *list);
    static void qmlClearPlaceholderTrackers(QQmlListProperty<Kuesa::PlaceholderTracker> *list);

    std::vector<Kuesa::TransformTracker *> m_managedTransformTrackers;
    std::vector<Kuesa::PlaceholderTracker *> m_managedPlaceholderTrackers;
};

} // namespace KuesaUtils

QT_END_NAMESPACE

#endif // VIEWCONFIGURATIONITEM_H
