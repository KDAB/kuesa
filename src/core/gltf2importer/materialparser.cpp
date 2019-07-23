/*
    materialparser.cpp

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

#include "materialparser_p.h"
#include "kuesa_p.h"
#include "gltf2keys_p.h"
#include <QJsonObject>
#include <QJsonArray>
#include "gltf2context_p.h"

#include "metallicroughnessproperties.h"
#include "unlitproperties.h"

QT_BEGIN_NAMESPACE

using namespace Kuesa::GLTF2Import;

namespace {

const QLatin1String KEY_NAME = QLatin1String("name");
const QLatin1String KEY_ALPHA_MODE = QLatin1String("alphaMode");
const QLatin1String KEY_ALPHA_CUTOFF = QLatin1String("alphaCutoff");
const QLatin1String KEY_DOUBLE_SIDED = QLatin1String("doubleSided");
const QLatin1String KEY_PBR_METALLIC_ROUGHNESS = QLatin1String("pbrMetallicRoughness");
const QLatin1String KEY_BASE_COLOR_FACTOR = QLatin1String("baseColorFactor");
const QLatin1String KEY_BASE_COLOR_TEXTURE = QLatin1String("baseColorTexture");
const QLatin1String KEY_ROUGHNESS_FACTOR = QLatin1String("roughnessFactor");
const QLatin1String KEY_ROUGHNESS_TEXTURE = QLatin1String("metallicRoughnessTexture");
const QLatin1String KEY_METALLIC_FACTOR = QLatin1String("metallicFactor");
const QLatin1String KEY_INDEX = QLatin1String("index");
const QLatin1String KEY_TEXCOORD = QLatin1String("texCoord");
const QLatin1String KEY_NORMAL_TEXTURE = QLatin1String("normalTexture");
const QLatin1String KEY_OCCLUSION_TEXTURE = QLatin1String("occlusionTexture");
const QLatin1String KEY_EMISSIVE_TEXTURE = QLatin1String("emissiveTexture");
const QLatin1String KEY_SCALE = QLatin1String("scale");
const QLatin1String KEY_EMISSIVE_FACTOR = QLatin1String("emissiveFactor");

void parseTextureInfo(TextureInfo &info, const QJsonObject &textureInfoObj)
{
    info.index = textureInfoObj.value(KEY_INDEX).toInt(-1);
    info.texCoord = std::max(textureInfoObj.value(KEY_TEXCOORD).toInt(0), 0);
}

template<typename T>
T clamp(T val, T min, T max)
{
    return std::max(std::min(val, max), min);
}

template<typename Vector>
bool parseFloatArray(Vector &v,
                     const QJsonArray &array,
                     qint32 minItems, qint32 maxItems,
                     float minValue, float maxValue)
{
    if (array.size() < minItems || array.size() > maxItems) {
        qCWarning(kuesa) << "array size doesn't satisfy min/maximum requirements";
        return false;
    }

    const qint32 entryCount = std::min(maxItems, array.size());
    for (qint32 i = 0; i < entryCount; ++i)
        v[i] = clamp(float(array.at(i).toDouble(v[i])), minValue, maxValue);

    return true;
}

} // namespace

bool MaterialParser::parse(const QJsonArray &materials, GLTF2Context *context)
{
    static const QHash<QString, Material::Alpha::Mode> modeEnumMap = {
        { QStringLiteral("OPAQUE"), Material::Alpha::Opaque },
        { QStringLiteral("MASK"), Material::Alpha::Mask },
        { QStringLiteral("BLEND"), Material::Alpha::Blend }
    };

    for (const QJsonValue &materialValue : materials) {

        const QJsonObject &materialObject = materialValue.toObject();
        Material mat;

        if (materialObject.isEmpty()) {
            // Having an empty object is allowed
            // Skip parsing and use default material values if that happens
            mat.materialProperties(*context);
            context->addMaterial(mat);
            continue;
        }

        mat.name = materialObject.value(KEY_NAME).toString();
        mat.doubleSided = materialObject.value(KEY_DOUBLE_SIDED).toBool(false);

        // Parse alpa
        {
            const QString modeStr = materialObject.value(KEY_ALPHA_MODE).toString();
            if (!modeStr.isEmpty() && !modeEnumMap.contains(modeStr)) {
                qCWarning(kuesa) << "Invalid material alpha mode";
                return false;
            }

            mat.alpha.mode = modeEnumMap.value(modeStr, Material::Alpha::Opaque);
            mat.alpha.alphaCutoff = std::max(0.0f, float(materialObject.value(KEY_ALPHA_CUTOFF).toDouble(0.5)));
        }

        // Parse pbrMetallicRoughness
        {
            // An empty object is allowed
            const QJsonObject pbrMetallicRoughnessObject = materialObject.value(KEY_PBR_METALLIC_ROUGHNESS).toObject();
            mat.pbr.metallicFactor = clamp(float(pbrMetallicRoughnessObject.value(KEY_METALLIC_FACTOR).toDouble(1.0)), 0.0f, 1.0f);
            mat.pbr.roughtnessFactor = clamp(float(pbrMetallicRoughnessObject.value(KEY_ROUGHNESS_FACTOR).toDouble(1.0)), 0.0f, 1.0f);

            const QJsonObject baseColorTextureObject = pbrMetallicRoughnessObject.value(KEY_BASE_COLOR_TEXTURE).toObject();
            if (!baseColorTextureObject.isEmpty())
                parseTextureInfo(mat.pbr.baseColorTexture, baseColorTextureObject);

            const QJsonObject metallicRoughnessTextureObject = pbrMetallicRoughnessObject.value(KEY_ROUGHNESS_TEXTURE).toObject();
            if (!metallicRoughnessTextureObject.isEmpty())
                parseTextureInfo(mat.pbr.metallicRoughnessTexture, metallicRoughnessTextureObject);

            const QJsonValue baseColorFactors = pbrMetallicRoughnessObject.value(KEY_BASE_COLOR_FACTOR);
            if (!baseColorFactors.isUndefined()) {
                if (!parseFloatArray(mat.pbr.baseColorFactor,
                                     baseColorFactors.toArray(),
                                     4, 4, 0.0f, 1.0f))
                    return false;
            }
        }

        // Parse normalTexture
        {
            const QJsonObject normalTextureObject = materialObject.value(KEY_NORMAL_TEXTURE).toObject();
            if (!normalTextureObject.isEmpty()) {
                parseTextureInfo(mat.normalTexture, normalTextureObject);
                mat.normalTexture.scale = static_cast<float>(normalTextureObject.value(KEY_SCALE).toDouble(0.25));
            }
        }

        // Parse emissiveTexture
        {
            const QJsonObject emissiveTextureObject = materialObject.value(KEY_EMISSIVE_TEXTURE).toObject();
            if (!emissiveTextureObject.isEmpty())
                parseTextureInfo(mat.emissiveTexture, emissiveTextureObject);

            const QJsonValue emissiveFactors = materialObject.value(KEY_EMISSIVE_FACTOR);
            if (!emissiveFactors.isUndefined()) {
                if (!parseFloatArray(mat.emissiveTexture.emissiveFactor,
                                     emissiveFactors.toArray(),
                                     3, 3, 0.0f, 1.0f))
                    return false;
            }
        }

        // Parse occlusionTexture
        {
            const QJsonObject occlusionTextureObject = materialObject.value(KEY_OCCLUSION_TEXTURE).toObject();

            if (!occlusionTextureObject.isEmpty()) {
                parseTextureInfo(mat.occlusionTexture, occlusionTextureObject);
                mat.occlusionTexture.strength = clamp(float(occlusionTextureObject.value(KEY_SCALE).toDouble(1.0)), 0.0f, 1.0f);
            }
        }

        // Parse extensions
        {
            const QJsonObject extensionsObject = materialObject.value(KEY_EXTENSIONS).toObject();
            mat.extensions.KHR_materials_unlit = extensionsObject.contains(KEY_KHR_MATERIALS_UNLIT);
        }

        mat.materialProperties(*context);
        context->addMaterial(mat);
    }

    return materials.size() > 0;
}

Kuesa::GLTF2MaterialProperties *Material::materialProperties(const GLTF2Context &context)
{
    if (m_materialProperties)
        return m_materialProperties;

    if (extensions.KHR_materials_unlit) {
        auto *materialProperties = new Kuesa::UnlitProperties();
        materialProperties->setBaseColorFactor(QColor::fromRgbF(
                pbr.baseColorFactor[0],
                pbr.baseColorFactor[1],
                pbr.baseColorFactor[2],
                pbr.baseColorFactor[3]));

        const qint32 baseColorTextureIdx = pbr.baseColorTexture.index;
        if (baseColorTextureIdx > -1) {
            materialProperties->setBaseColorMap(context.texture(baseColorTextureIdx).texture);
            materialProperties->setBaseColorUsesTexCoord1(pbr.baseColorTexture.texCoord == 1);
        }

        materialProperties->setAlphaCutoff(alpha.alphaCutoff);

        m_materialProperties = materialProperties;
    } else {

        auto *materialProperties = new Kuesa::MetallicRoughnessProperties;
        materialProperties->setMetallicFactor(pbr.metallicFactor);
        materialProperties->setRoughnessFactor(pbr.roughtnessFactor);
        materialProperties->setNormalScale(normalTexture.scale);
        materialProperties->setBaseColorFactor(QColor::fromRgbF(
                pbr.baseColorFactor[0],
                pbr.baseColorFactor[1],
                pbr.baseColorFactor[2],
                pbr.baseColorFactor[3]));
        materialProperties->setEmissiveFactor(QColor::fromRgbF(
                emissiveTexture.emissiveFactor[0],
                emissiveTexture.emissiveFactor[1],
                emissiveTexture.emissiveFactor[2]));

        const qint32 baseColorTextureIdx = pbr.baseColorTexture.index;
        if (baseColorTextureIdx > -1) {
            materialProperties->setBaseColorMap(context.texture(baseColorTextureIdx).texture);
            materialProperties->setBaseColorUsesTexCoord1(pbr.baseColorTexture.texCoord == 1);
        }

        const qint32 metallicRoughnessTextureIdx = pbr.metallicRoughnessTexture.index;
        if (metallicRoughnessTextureIdx > -1) {
            materialProperties->setMetalRoughMap(context.texture(metallicRoughnessTextureIdx).texture);
            materialProperties->setMetallicRoughnessUsesTexCoord1(pbr.metallicRoughnessTexture.texCoord == 1);
        }

        const qint32 normalMapTextureIdx = normalTexture.index;
        if (normalMapTextureIdx > -1) {
            materialProperties->setNormalMap(context.texture(normalMapTextureIdx).texture);
            materialProperties->setNormalUsesTexCoord1(normalTexture.texCoord == 1);
        }

        const qint32 emissiveMapTextureIdx = emissiveTexture.index;
        if (emissiveMapTextureIdx > -1) {
            materialProperties->setEmissiveMap(context.texture(emissiveMapTextureIdx).texture);
            materialProperties->setEmissiveUsesTexCoord1(emissiveTexture.index == 1);
        }

        const qint32 occulsionMapTextureIdx = occlusionTexture.index;
        if (occulsionMapTextureIdx > -1) {
            materialProperties->setAmbientOcclusionMap(context.texture(occulsionMapTextureIdx).texture);
            materialProperties->setAOUsesTexCoord1(occlusionTexture.texCoord == 1);
        }

        materialProperties->setAlphaCutoff(alpha.alphaCutoff);

        m_materialProperties = materialProperties;
    }

    return m_materialProperties;
}

Kuesa::GLTF2MaterialProperties *Material::materialProperties() const
{
    return m_materialProperties;
}

Kuesa::EffectProperties Material::effectPropertiesFromMaterial(const Material &material)
{
    Kuesa::EffectProperties effectProperties;

    if (material.extensions.KHR_materials_unlit)
        effectProperties |= Kuesa::EffectProperty::Unlit;
    else
        effectProperties |= Kuesa::EffectProperty::MetallicRoughness;

    if (material.doubleSided)
        effectProperties |= Kuesa::EffectProperty::DoubleSided;

    if (material.alpha.mode == Material::Alpha::Blend)
        effectProperties |= Kuesa::EffectProperty::Blend;

    if (material.alpha.mode == Material::Alpha::Mask)
        effectProperties |= Kuesa::EffectProperty::Mask;

    if (material.pbr.baseColorTexture.index != -1)
        effectProperties |= Kuesa::EffectProperty::BaseColorMap;

    // Ignore rest of properties for unlit
    if (!material.extensions.KHR_materials_unlit) {

        if (material.pbr.metallicRoughnessTexture.index != -1)
            effectProperties |= Kuesa::EffectProperty::MetalRoughnessMap;

        if (material.normalTexture.index != -1)
            effectProperties |= Kuesa::EffectProperty::NormalMap;

        if (material.occlusionTexture.index != -1)
            effectProperties |= Kuesa::EffectProperty::AOMap;

        if (material.emissiveTexture.index != -1)
            effectProperties |= Kuesa::EffectProperty::EmissiveMap;
    }

    return effectProperties;
}

QT_END_NAMESPACE
