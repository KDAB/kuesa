/*
    skinparser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_GLTF2IMPORT_SKINPARSER_P_H
#define KUESA_GLTF2IMPORT_SKINPARSER_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QtCore/qglobal.h>
#include <QtCore/qvector.h>
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_NAMESPACE

class QJsonArray;

namespace Qt3DCore {
class QSkeleton;
}

namespace Kuesa {
namespace GLTF2Import {

class GLTF2Context;

struct Skin {
    qint32 inverseBindMatricesAccessorIdx = -1;
    qint32 skeletonIdx = -1;
    QVector<qint32> jointsIndices;
    QString name;
    QVector<QMatrix4x4> inverseBindMatrices;
    Qt3DCore::QSkeleton *skeleton = nullptr;
};

class Q_AUTOTEST_EXPORT SkinParser
{
public:
    SkinParser() = default;

    bool parse(const QJsonArray &skinsArray, GLTF2Context *context) const;
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_SKINPARSER_P_H
