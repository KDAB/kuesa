/*
    materialparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QJsonObject>
#include <QJsonArray>
#include "gltf2context_p.h"
#include <Kuesa/metallicroughnessmaterial.h>

QT_BEGIN_NAMESPACE

using namespace Kuesa::GLTF2Import;

namespace {

const QLatin1String KEY_NAME = QLatin1Literal("name");
const QLatin1String KEY_ALPHA_MODE = QLatin1Literal("alphaMode");
const QLatin1String KEY_ALPHA_CUTOFF = QLatin1Literal("alphaCutoff");
const QLatin1String KEY_DOUBLE_SIDED = QLatin1Literal("doubleSided");
const QLatin1String KEY_PBR_METALLIC_ROUGHNESS = QLatin1Literal("pbrMetallicRoughness");
const QLatin1String KEY_BASE_COLOR_FACTOR = QLatin1Literal("baseColorFactor");
const QLatin1String KEY_BASE_COLOR_TEXTURE = QLatin1Literal("baseColorTexture");
const QLatin1String KEY_ROUGHNESS_FACTOR = QLatin1Literal("roughnessFactor");
const QLatin1String KEY_ROUGHNESS_TEXTURE = QLatin1Literal("metallicRoughnessTexture");
const QLatin1String KEY_METALLIC_FACTOR = QLatin1Literal("metallicFactor");
const QLatin1String KEY_INDEX = QLatin1Literal("index");
const QLatin1String KEY_TEXCOORD = QLatin1Literal("texCoord");
const QLatin1String KEY_NORMAL_TEXTURE = QLatin1Literal("normalTexture");
const QLatin1String KEY_OCCLUSION_TEXTURE = QLatin1Literal("occlusionTexture");
const QLatin1String KEY_EMISSIVE_TEXTURE = QLatin1Literal("emissiveTexture");
const QLatin1String KEY_SCALE = QLatin1Literal("scale");
const QLatin1String KEY_EMISSIVE_FACTOR = QLatin1Literal("emissiveFactor");

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

Kuesa::MetallicRoughnessMaterial *createPbrMaterial(const Material &mat, const GLTF2Context *context)
{
    auto pbrMaterial = new Kuesa::MetallicRoughnessMaterial();
    pbrMaterial->setMetallicFactor(mat.pbr.metallicFactor);
    pbrMaterial->setRoughnessFactor(mat.pbr.roughtnessFactor);
    pbrMaterial->setNormalScale(mat.normalTexture.scale);
    pbrMaterial->setDoubleSided(mat.doubleSided);
    pbrMaterial->setBaseColorFactor(QColor::fromRgbF(
            mat.pbr.baseColorFactor[0],
            mat.pbr.baseColorFactor[1],
            mat.pbr.baseColorFactor[2],
            mat.pbr.baseColorFactor[3]));
    pbrMaterial->setEmissiveFactor(QColor::fromRgbF(
            mat.emissiveTexture.emissiveFactor[0],
            mat.emissiveTexture.emissiveFactor[1],
            mat.emissiveTexture.emissiveFactor[2]));

    const qint32 baseColorTextureIdx = mat.pbr.baseColorTexture.index;
    if (baseColorTextureIdx > -1)
        pbrMaterial->setBaseColorMap(context->texture(baseColorTextureIdx).texture);

    const qint32 metallicRoughnessTextureIdx = mat.pbr.metallicRoughnessTexture.index;
    if (metallicRoughnessTextureIdx > -1)
        pbrMaterial->setMetalRoughMap(context->texture(metallicRoughnessTextureIdx).texture);

    const qint32 normalMapTextureIdx = mat.normalTexture.index;
    if (normalMapTextureIdx > -1)
        pbrMaterial->setNormalMap(context->texture(normalMapTextureIdx).texture);

    const qint32 emissiveMapTextureIdx = mat.emissiveTexture.index;
    if (emissiveMapTextureIdx > -1)
        pbrMaterial->setEmissiveMap(context->texture(emissiveMapTextureIdx).texture);

    const qint32 occulsionMapTextureIdx = mat.occlusionTexture.index;
    if (occulsionMapTextureIdx > -1)
        pbrMaterial->setAmbientOcclusionMap(context->texture(occulsionMapTextureIdx).texture);

    switch (mat.alpha.mode) {
    case Material::Alpha::Opaque:
        pbrMaterial->setOpaque(true);
        break;
    case Material::Alpha::Blend:
        pbrMaterial->setOpaque(false);
        break;
    case Material::Alpha::Mask:
        pbrMaterial->setAlphaCutoffEnabled(true);
        pbrMaterial->setAlphaCutoff(mat.alpha.alphaCutoff);
    }

    return pbrMaterial;
}

} // namespace

Qt3DRender::QMaterial *Material::material(bool isSkinned, bool hasColorAttribute, const GLTF2Context *context)
{
    if (isSkinned) {
        if (m_skinnedMaterial == nullptr) {
            Kuesa::MetallicRoughnessMaterial *material = createPbrMaterial(*this, context);
            material->setUseSkinning(true);
            material->setUsingColorAttribute(hasColorAttribute);
            m_skinnedMaterial = material;
        }
        return m_skinnedMaterial;
    }

    if (m_regularMaterial == nullptr) {
        Kuesa::MetallicRoughnessMaterial *material = createPbrMaterial(*this, context);
        material->setUsingColorAttribute(hasColorAttribute);
        m_regularMaterial = material;
    }
    return m_regularMaterial;
}

Qt3DRender::QMaterial *Material::material(bool isSkinned) const
{
    if (isSkinned)
        return m_skinnedMaterial;
    return m_regularMaterial;
}

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
                mat.normalTexture.scale = normalTextureObject.value(KEY_SCALE).toDouble(1.0);
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

        context->addMaterial(mat);
    }

    return materials.size() > 0;
}

QT_END_NAMESPACE
