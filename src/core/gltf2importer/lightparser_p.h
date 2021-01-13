/*
    lightparser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#ifndef KUESA_GLTF2IMPORT_LIGHTPARSER_P_H
#define KUESA_GLTF2IMPORT_LIGHTPARSER_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QJsonArray>
#include <Qt3DRender/QAbstractLight>
#include <qmath.h>
#include <QSize>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QAbstractLight;
}

namespace Kuesa {
namespace GLTF2Import {

class GLTF2Context;

struct Light {
    QString name;
    Qt3DRender::QAbstractLight::Type type;
    QColor color = QColor(255, 255, 255);
    float intensity = 1.0f;
    // range > 0.  Undefined means infinite
    QVariant range;
    float innerConeAngleRadians = 0.0;
    float outerConeAngleRadians = (float)M_PI_4;
    bool castsShadows = false;
    bool softShadows = false;
    float shadowMapBias = .005f;
    float nearPlane = 0.0f;
    QSize shadowMapTextureSize = { 512, 512 };

    Qt3DRender::QAbstractLight *lightComponent = nullptr;
};

class Q_AUTOTEST_EXPORT LightParser
{
public:
    LightParser() = default;

    bool parse(const QJsonArray &lights, GLTF2Context *context);
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_LIGHTPARSER_P_H
