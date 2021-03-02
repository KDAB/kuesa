/*
    textureinfoparser_p.h

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

#ifndef KUESA_GLTF2IMPORT_TEXTUREINFOPARSER_H
#define KUESA_GLTF2IMPORT_TEXTUREINFOPARSER_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <Kuesa/kuesa_global.h>
#include <QJsonObject>
#include <QVector2D>

QT_BEGIN_NAMESPACE

namespace Kuesa {
namespace GLTF2Import {
struct Q_AUTOTEST_EXPORT TextureInfo {
    qint32 index = -1;
    qint32 texCoord = 0; // Attribute will be TEXCOORD_<texCoord>

    struct KHR_texture_transform {
        QVector2D offset = { 0.0f, 0.0f };
        float rotation = 0.0f;
        QVector2D scale = { 1.0f, 1.0f };
    };

    KHR_texture_transform khr_texture_transform;

    static TextureInfo parse(const QJsonObject &textureInfoObj);
};
} // namespace GLTF2Import
} // namespace Kuesa

Q_DECLARE_METATYPE(Kuesa::GLTF2Import::TextureInfo)

QT_END_NAMESPACE

#endif //  KUESA_GLTF2IMPORT_TEXTUREINFOPARSER_H
