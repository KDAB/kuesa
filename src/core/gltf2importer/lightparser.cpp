/*
    lightparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "kuesa_p.h"

#include <QJsonValue>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
using namespace Kuesa::GLTF2Import;

namespace {
const QLatin1String KEY_NAME = QLatin1String("name");
const QLatin1String KEY_TYPE = QLatin1String("type");
const QLatin1String KEY_COLOR = QLatin1String("color");
const QLatin1String KEY_INTENSITY = QLatin1String("intensity");
const QLatin1String KEY_RANGE = QLatin1String("range");
const QLatin1String KEY_SPOT = QLatin1String("spot");
const QLatin1String KEY_INNER_CONE_ANGLE = QLatin1String("innerConeAngle");
const QLatin1String KEY_OUTER_CONE_ANGLE = QLatin1String("outerConeAngle");
const QLatin1String KEY_LIGHT_DIRECTIONAL = QLatin1String("directional");
const QLatin1String KEY_LIGHT_POINT = QLatin1String("point");
const QLatin1String KEY_LIGHT_SPOT = QLatin1String("spot");

} // namespace

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
            qCWarning(kuesa) << "unknown KHR_lights_punctual light type: " << typeValue.toString();
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
                qCWarning(kuesa) << "KHR_lights_punctual SpotLight innerConeAngle must be between 0 and outerConeAngle. Invalid light: " << light.name;
            if (light.outerConeAngleRadians > static_cast<float>(M_PI_2))
                qCWarning(kuesa) << "KHR_lights_punctual SpotLight outerConeAngle must be less than PI/2. Invalid light: " << light.name;
        }

        context->addLight(light);
    }
    return true;
}

QT_END_NAMESPACE
