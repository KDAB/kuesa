/*
    lightparser.cpp

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

#include "lightparser_p.h"
#include "gltf2context_p.h"
#include "gltf2keys_p.h"
#include "gltf2utils_p.h"
#include "kuesa_p.h"

#include <QJsonValue>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
using namespace Kuesa::GLTF2Import;

bool Kuesa::GLTF2Import::LightParser::parse(const QJsonArray &lights, Kuesa::GLTF2Import::GLTF2Context *context)
{
    for (const auto &lightValue : lights) {
        const QJsonObject &lightObject = lightValue.toObject();

        Light light;
        const QJsonValue typeValue = lightObject.value(KEY_TYPE);
        if (typeValue == KEY_LIGHT_DIRECTIONAL)
            light.type = Qt3DRender::QAbstractLight::DirectionalLight;
        else if (typeValue == KEY_LIGHT_POINT)
            light.type = Qt3DRender::QAbstractLight::PointLight;
        else if (typeValue == KEY_LIGHT_SPOT)
            light.type = Qt3DRender::QAbstractLight::SpotLight;
        else {
            qCWarning(Kuesa::kuesa) << "unknown KHR_lights_punctual light type: " << typeValue.toString();
            continue;
        }

        light.name = lightObject.value(KEY_NAME).toString();

        const QJsonValue lightColor = lightObject.value(KEY_COLOR);
        if (!lightColor.isUndefined()) {
            const auto colorArray = lightColor.toArray();
            if (colorArray.count() == 3) {
                const float redF = colorArray.at(0).toDouble(1.0);
                const float greenF = colorArray.at(1).toDouble(1.0);
                const float blueF = colorArray.at(2).toDouble(1.0);
                light.color = QColor::fromRgbF(redF, greenF, blueF);
            }
        }

        light.intensity = lightObject.value(KEY_INTENSITY).toDouble(light.intensity);

        // range must be > 0 according to spec.  Undefined means range is infinite
        const auto range = lightObject.value(KEY_RANGE).toDouble(0.0);
        if (range > 0.0)
            light.range = range;

        if (light.type == Qt3DRender::QAbstractLight::SpotLight) {
            const auto spotObject = lightObject.value(KEY_SPOT).toObject();
            light.innerConeAngleRadians = spotObject.value(KEY_INNER_CONE_ANGLE).toDouble(light.innerConeAngleRadians);
            light.outerConeAngleRadians = spotObject.value(KEY_OUTER_CONE_ANGLE).toDouble(light.outerConeAngleRadians);
            if (light.innerConeAngleRadians >= light.outerConeAngleRadians || light.innerConeAngleRadians < 0)
                qCWarning(Kuesa::kuesa) << "KHR_lights_punctual SpotLight innerConeAngle must be between 0 and outerConeAngle. Invalid light: " << light.name;
            if (light.outerConeAngleRadians > static_cast<float>(M_PI_2))
                qCWarning(Kuesa::kuesa) << "KHR_lights_punctual SpotLight outerConeAngle must be less than PI/2. Invalid light: " << light.name;
        }

        // Shadows extension
        const QJsonObject extensions = lightObject.value(KEY_EXTENSIONS).toObject();
        const QString shadowExtensionKey = getNewOrDeprecatedExtensionKey(KEY_KDAB_SHADOWS_EXTENSION,
                                                                          KEY_KDAB_KUESA_SHADOWS_EXTENSION,
                                                                          extensions);
        if (!shadowExtensionKey.isEmpty()) {
            const QJsonObject kdabShadowsExtension = extensions.value(shadowExtensionKey).toObject();
            light.castsShadows = kdabShadowsExtension.value(KEY_CASTS_SHADOWS).toBool(light.castsShadows);
            light.softShadows = kdabShadowsExtension.value(KEY_SOFT_SHADOWS).toBool(light.softShadows);
            light.shadowMapBias = kdabShadowsExtension.value(KEY_SHADOWMAP_BIAS).toDouble(light.shadowMapBias);
            light.nearPlane = kdabShadowsExtension.value(KEY_SHADOWMAP_NEAR_PLANE).toDouble(light.nearPlane);
            QSize textureSize = light.shadowMapTextureSize;
            textureSize.setWidth(kdabShadowsExtension.value(KEY_SHADOWMAP_TEXTURE_WIDTH).toInt(textureSize.width()));
            textureSize.setHeight(kdabShadowsExtension.value(KEY_SHADOWMAP_TEXTURE_HEIGHT).toInt(textureSize.height()));
            light.shadowMapTextureSize = textureSize;
        }
        context->addLight(light);
    }
    return true;
}

QT_END_NAMESPACE
