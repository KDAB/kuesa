/*
    assetproperty_p.h

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef ASSETPROPERTY_P_H
#define ASSETPROPERTY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>
#include <QMetaProperty>
#include <Qt3DCore/QNode>
#include "asset.h"

namespace Kuesa {
class Asset;
class AssetProperty : public QObject
{
    Q_OBJECT
public:
    explicit AssetProperty(Kuesa::Asset *asset = nullptr);

    Q_INVOKABLE void setQmlProperty();
    Q_INVOKABLE void setNodeProperty();

    Kuesa::Asset *asset;
    Qt3DCore::QNode *node;
    QMetaProperty nodeProperty;
    QMetaProperty qmlProperty;
};
} // namespace Kuesa

#endif // ASSETPROPERTY_P_H
