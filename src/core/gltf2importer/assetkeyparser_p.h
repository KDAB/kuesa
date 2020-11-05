/*
    assetkeyparser_p.h

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

#ifndef KUESA_GLTF2IMPORT_ASSETKEYPARSER_H
#define KUESA_GLTF2IMPORT_ASSETKEYPARSER_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QJsonObject>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Kuesa {
namespace GLTF2Import {

class AssetKeyParser
{
public:
    template<typename Asset>
    static void parse(Asset &asset, const QJsonObject &json)
    {
        const QLatin1String KEY_EXTENSIONS = QLatin1String("extensions");
        const QLatin1String KEY_ASSET_KEY_EXTENSION = QLatin1String("KDAB_asset_key");
        const QLatin1String KEY_KEY = QLatin1String("key");

        const QJsonObject extensionObj = json.value(KEY_EXTENSIONS).toObject();
        const QJsonObject sharedKeyExtensionObj = extensionObj.value(KEY_ASSET_KEY_EXTENSION).toObject();
        asset.key = sharedKeyExtensionObj.value(KEY_KEY).toString();
    }
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_ASSETKEYPARSER_H
