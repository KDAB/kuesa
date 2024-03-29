/*
    meshinstantiatorextension.h

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

#ifndef KUESA_MESHINSTANTIATOREXTENSION_H
#define KUESA_MESHINSTANTIATOREXTENSION_H

#include <QObject>
#include <QtQml/QQmlListProperty>
#include <vector>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using qt_size_type = qsizetype;
#else
using qt_size_type = int;
#endif

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QTransform;
}

namespace Kuesa {

class MeshInstantiatorExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DCore::QTransform> transforms READ transforms CONSTANT)

public:
    explicit MeshInstantiatorExtension(QObject *parent = nullptr);

    QQmlListProperty<Qt3DCore::QTransform> transforms();

private:
    void updateTransforms();

    static void appendTransform(QQmlListProperty<Qt3DCore::QTransform> *list, Qt3DCore::QTransform *t);
    static Qt3DCore::QTransform *transformAt(QQmlListProperty<Qt3DCore::QTransform> *list, qt_size_type index);
    static qt_size_type transformCount(QQmlListProperty<Qt3DCore::QTransform> *list);
    static void clearTransforms(QQmlListProperty<Qt3DCore::QTransform> *list);

    std::vector<Qt3DCore::QTransform *> m_transforms;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_MESHINSTANTIATOREXTENSION_H
