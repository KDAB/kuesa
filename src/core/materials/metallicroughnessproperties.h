/*
    metallicroughnessproperties.h

    This file is part of Kuesa.

    Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_METALLICROUGHNESSPROPERTIES_H
#define KUESA_METALLICROUGHNESSPROPERTIES_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/GLTF2MaterialProperties>
#include <QColor>
#include <Qt3DRender/qabstracttexture.h>
#include <Kuesa/TextureTransform>

QT_BEGIN_NAMESPACE

namespace Kuesa {
class MetallicRoughnessMaterial;
class MetallicRoughnessShaderData;

class KUESASHARED_EXPORT MetallicRoughnessProperties : public GLTF2MaterialProperties
{
    Q_OBJECT
    Q_PROPERTY(bool metallicRoughnessUsesTexCoord1 READ isMetallicRoughnessUsingTexCoord1 WRITE setMetallicRoughnessUsesTexCoord1 NOTIFY metallicRoughnessUsesTexCoord1Changed)
    Q_PROPERTY(bool normalUsesTexCoord1 READ isNormalUsingTexCoord1 WRITE setNormalUsesTexCoord1 NOTIFY normalUsesTexCoord1Changed)
    Q_PROPERTY(bool aoUsesTexCoord1 READ isAOUsingTexCoord1 WRITE setAOUsesTexCoord1 NOTIFY aoUsesTexCoord1Changed)
    Q_PROPERTY(bool emissiveUsesTexCoord1 READ isEmissiveUsingTexCoord1 WRITE setEmissiveUsesTexCoord1 NOTIFY emissiveUsesTexCoord1Changed)
    Q_PROPERTY(Kuesa::TextureTransform *baseColorMapTextureTransform READ baseColorMapTextureTransform)

    Q_PROPERTY(float metallicFactor READ metallicFactor WRITE setMetallicFactor NOTIFY metallicFactorChanged)
    Q_PROPERTY(float roughnessFactor READ roughnessFactor WRITE setRoughnessFactor NOTIFY roughnessFactorChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *metalRoughMap READ metalRoughMap WRITE setMetalRoughMap NOTIFY metalRoughMapChanged)
    Q_PROPERTY(Kuesa::TextureTransform *metalRoughMapTextureTransform READ metalRoughMapTextureTransform)

    Q_PROPERTY(float normalScale READ normalScale WRITE setNormalScale NOTIFY normalScaleChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)
    Q_PROPERTY(Kuesa::TextureTransform *normalMapTextureTransform READ normalMapTextureTransform)

    Q_PROPERTY(Qt3DRender::QAbstractTexture *ambientOcclusionMap READ ambientOcclusionMap WRITE setAmbientOcclusionMap NOTIFY ambientOcclusionMapChanged)
    Q_PROPERTY(Kuesa::TextureTransform *ambientOcclusionMapTextureTransform READ ambientOcclusionMapTextureTransform)

    Q_PROPERTY(QColor emissiveFactor READ emissiveFactor WRITE setEmissiveFactor NOTIFY emissiveFactorChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *emissiveMap READ emissiveMap WRITE setEmissiveMap NOTIFY emissiveMapChanged)
    Q_PROPERTY(Kuesa::TextureTransform *emissiveMapTextureTransform READ emissiveMapTextureTransform)

public:
    explicit MetallicRoughnessProperties(Qt3DCore::QNode *parent = nullptr);
    ~MetallicRoughnessProperties();

    Qt3DRender::QShaderData *shaderData() const override;

    bool isMetallicRoughnessUsingTexCoord1() const;
    bool isNormalUsingTexCoord1() const;
    bool isAOUsingTexCoord1() const;
    bool isEmissiveUsingTexCoord1() const;

    Kuesa::TextureTransform *baseColorMapTextureTransform() const;

    float metallicFactor() const;
    float roughnessFactor() const;
    Qt3DRender::QAbstractTexture *metalRoughMap() const;
    Kuesa::TextureTransform *metalRoughMapTextureTransform() const;

    float normalScale() const;
    Qt3DRender::QAbstractTexture *normalMap() const;
    Kuesa::TextureTransform *normalMapTextureTransform() const;

    Qt3DRender::QAbstractTexture *ambientOcclusionMap() const;
    Kuesa::TextureTransform *ambientOcclusionMapTextureTransform() const;

    QColor emissiveFactor() const;
    Qt3DRender::QAbstractTexture *emissiveMap() const;
    Kuesa::TextureTransform *emissiveMapTextureTransform() const;

public Q_SLOTS:
    void setMetallicRoughnessUsesTexCoord1(bool metallicRoughnessUsesTexCoord1);
    void setNormalUsesTexCoord1(bool normalUsesTexCoord1);
    void setAOUsesTexCoord1(bool aoUsesTexCoord1);
    void setEmissiveUsesTexCoord1(bool emissiveUsesTexCoord1);

    void setMetallicFactor(float metallicFactor);
    void setRoughnessFactor(float roughnessFactor);
    void setMetalRoughMap(Qt3DRender::QAbstractTexture *metalRoughMap);

    void setNormalScale(float normalScale);
    void setNormalMap(Qt3DRender::QAbstractTexture *normalMap);

    void setAmbientOcclusionMap(Qt3DRender::QAbstractTexture *ambientOcclusionMap);

    void setEmissiveFactor(const QColor &emissiveFactor);
    void setEmissiveMap(Qt3DRender::QAbstractTexture *emissiveMap);

Q_SIGNALS:
    void metallicRoughnessUsesTexCoord1Changed(bool);
    void normalUsesTexCoord1Changed(bool);
    void aoUsesTexCoord1Changed(bool);
    void emissiveUsesTexCoord1Changed(bool);

    void metallicFactorChanged(float metallicFactor);
    void roughnessFactorChanged(float roughnessFactor);
    void metalRoughMapChanged(Qt3DRender::QAbstractTexture *metalRoughMap);

    void normalScaleChanged(float normalScale);
    void normalMapChanged(Qt3DRender::QAbstractTexture *normalMapChanged);

    void ambientOcclusionMapChanged(Qt3DRender::QAbstractTexture *ambientOcclusionMap);

    void emissiveFactorChanged(const QColor &emissiveFactor);
    void emissiveMapChanged(Qt3DRender::QAbstractTexture *emissiveMap);

private:
    friend class MetallicRoughnessMaterial;
    MetallicRoughnessShaderData *m_metallicRoughnessShaderData;

    Kuesa::TextureTransform *m_baseColorMapTextureTransform;
    Kuesa::TextureTransform *m_metalRoughMapTextureTransform;
    Kuesa::TextureTransform *m_normalMapTextureTransform;
    Kuesa::TextureTransform *m_ambientOcclusionMapTextureTransform;
    Kuesa::TextureTransform *m_emissiveMapTextureTransform;

    Qt3DRender::QAbstractTexture *m_emissiveMap;
    Qt3DRender::QAbstractTexture *m_normalMap;
    Qt3DRender::QAbstractTexture *m_metalRoughMap;
    Qt3DRender::QAbstractTexture *m_ambientOcclusionMap;
};
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_METALLICROUGHNESSPROPERTIES_H
