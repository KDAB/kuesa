/*
    materialparser_p.h

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

#ifndef KUESA_GLTF2IMPORT_MATERIALPARSER_P_H
#define KUESA_GLTF2IMPORT_MATERIALPARSER_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QVector4D>
#include <QVector3D>
#include <QVector2D>
#include <QString>

#include <Kuesa/private/effectslibrary_p.h>
#include <Kuesa/private/textureinfoparser_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QShaderData;
}

namespace Kuesa {

class GLTF2MaterialProperties;

namespace GLTF2Import {

class GLTF2Context;
class AnimationParser;

class Material
{
public:
    QString name;
    bool doubleSided = false;

    struct PBRMetallicRoughness {
        QVector4D baseColorFactor = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
        float metallicFactor = 1.0f;
        float roughtnessFactor = 1.0f;
        TextureInfo baseColorTexture;
        TextureInfo metallicRoughnessTexture;
    } pbr;

    struct NormalTextureInfo : public TextureInfo {
        NormalTextureInfo() = default;
        NormalTextureInfo(const TextureInfo &t)
            : TextureInfo(t) {}
        float scale = 0.25f;
    } normalTexture;

    struct OcclusionTextureInfo : public TextureInfo {
        OcclusionTextureInfo() = default;
        OcclusionTextureInfo(const TextureInfo &t)
            : TextureInfo(t) {}
        float strength = 1.0f;
    } occlusionTexture;

    struct EmissiveTextureInfo : public TextureInfo {
        EmissiveTextureInfo() = default;
        EmissiveTextureInfo(const TextureInfo &t)
            : TextureInfo(t) {}
        QVector3D emissiveFactor = QVector3D(0.0f, 0.0f, 0.0f);
    } emissiveTexture;

    struct Alpha {
        enum Mode {
            Opaque = 0,
            Mask,
            Blend
        };
        Mode mode = Opaque;
        float alphaCutoff = 0.5f;
    } alpha;

    struct Extensions {
        bool KHR_materials_unlit = false;
        bool KDAB_custom_material = false;
    } extensions;

    struct KDABCustomMaterial {
        QString type;
        const QMetaObject *materialClassMetaObject;
        const QMetaObject *propertiesClassMetaObject;
        const QMetaObject *effectClassMetaObject;

        struct Property {
            QString name;
            QVariant value;
        };
        QVector<Property> properties;
    } customMaterial;

    Kuesa::GLTF2MaterialProperties *getOrAllocateProperties(GLTF2Context &context);
    Kuesa::GLTF2MaterialProperties *materialProperties() const;

    Kuesa::GLTF2MaterialProperties *m_materialProperties = nullptr;

    static Kuesa::EffectProperties::Properties effectPropertiesFromMaterial(const Material &material);
};

class Q_AUTOTEST_EXPORT MaterialParser
{
public:
    MaterialParser() = default;

    bool parse(const QJsonArray &materials, GLTF2Context *context);
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_MATERIALPARSER_P_H
