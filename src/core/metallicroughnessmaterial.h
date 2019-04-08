/*
    metallicroughnessmaterial.h

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

#ifndef KUESA_METALLICROUGHNESSMATERIAL_H
#define KUESA_METALLICROUGHNESSMATERIAL_H

#include <QtGui/qcolor.h>
#include <QtGui/qgenericmatrix.h>
#include <Qt3DRender/qmaterial.h>
#include <Kuesa/kuesa_global.h>
#include <Kuesa/metallicroughnesseffect.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QFilterKey;
class QAbstractTexture;
class QTechnique;
class QParameter;
class QShaderProgram;
class QShaderProgramBuilder;
class QRenderPass;
class QCullFace;
} // namespace Qt3DRender

namespace Kuesa {

class MorphController;

class KUESASHARED_EXPORT MetallicRoughnessMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT

    Q_PROPERTY(bool baseColorUsesTexCoord1 READ isBaseColorUsingTexCoord1 WRITE setBaseColorUsesTexCoord1 NOTIFY baseColorUsesTexCoord1Changed)
    Q_PROPERTY(bool metallicRoughnessUsesTexCoord1 READ isMetallicRoughnessUsingTexCoord1 WRITE setMetallicRoughnessUsesTexCoord1 NOTIFY metallicRoughnessUsesTexCoord1Changed)
    Q_PROPERTY(bool normalUsesTexCoord1 READ isNormalUsingTexCoord1 WRITE setNormalUsesTexCoord1 NOTIFY normalUsesTexCoord1Changed)
    Q_PROPERTY(bool aoUsesTexCoord1 READ isAOUsingTexCoord1 WRITE setAOUsesTexCoord1 NOTIFY aoUsesTexCoord1Changed)
    Q_PROPERTY(bool emissiveUsesTexCoord1 READ isEmissiveUsingTexCoord1 WRITE setEmissiveUsesTexCoord1 NOTIFY emissiveUsesTexCoord1Changed)

    Q_PROPERTY(QColor baseColorFactor READ baseColorFactor WRITE setBaseColorFactor NOTIFY baseColorFactorChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *baseColorMap READ baseColorMap WRITE setBaseColorMap NOTIFY baseColorMapChanged)

    Q_PROPERTY(float metallicFactor READ metallicFactor WRITE setMetallicFactor NOTIFY metallicFactorChanged)
    Q_PROPERTY(float roughnessFactor READ roughnessFactor WRITE setRoughnessFactor NOTIFY roughnessFactorChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *metalRoughMap READ metalRoughMap WRITE setMetalRoughMap NOTIFY metalRoughMapChanged)

    Q_PROPERTY(float normalScale READ normalScale WRITE setNormalScale NOTIFY normalScaleChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)

    Q_PROPERTY(Qt3DRender::QAbstractTexture *ambientOcclusionMap READ ambientOcclusionMap WRITE setAmbientOcclusionMap NOTIFY ambientOcclusionMapChanged)

    Q_PROPERTY(QColor emissiveFactor READ emissiveFactor WRITE setEmissiveFactor NOTIFY emissiveFactorChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *emissiveMap READ emissiveMap WRITE setEmissiveMap NOTIFY emissiveMapChanged)

    Q_PROPERTY(QMatrix3x3 textureTransform READ textureTransform WRITE setTextureTransform NOTIFY textureTransformChanged)
    Q_PROPERTY(bool usingColorAttribute READ isUsingColorAttribute WRITE setUsingColorAttribute NOTIFY usingColorAttributeChanged)
    Q_PROPERTY(bool doubleSided READ isDoubleSided WRITE setDoubleSided NOTIFY doubleSidedChanged)
    Q_PROPERTY(bool useSkinning READ useSkinning WRITE setUseSkinning NOTIFY useSkinningChanged)
    Q_PROPERTY(bool opaque READ isOpaque WRITE setOpaque NOTIFY opaqueChanged)
    Q_PROPERTY(float alphaCutoff READ alphaCutoff WRITE setAlphaCutoff NOTIFY alphaCutoffChanged)
    Q_PROPERTY(bool alphaCutoffEnabled READ isAlphaCutoffEnabled WRITE setAlphaCutoffEnabled NOTIFY alphaCutoffEnabledChanged)
    Q_PROPERTY(Kuesa::MetallicRoughnessEffect::ToneMapping toneMappingAlgorithm READ toneMappingAlgorithm WRITE setToneMappingAlgorithm NOTIFY toneMappingAlgorithmChanged REVISION 1)
    Q_PROPERTY(Kuesa::MorphController *morphController READ morphController NOTIFY morphControllerChanged)

public:
    explicit MetallicRoughnessMaterial(Qt3DCore::QNode *parent = nullptr);
    ~MetallicRoughnessMaterial();

    bool isBaseColorUsingTexCoord1() const;
    bool isMetallicRoughnessUsingTexCoord1() const;
    bool isNormalUsingTexCoord1() const;
    bool isAOUsingTexCoord1() const;
    bool isEmissiveUsingTexCoord1() const;

    QColor baseColorFactor() const;
    Qt3DRender::QAbstractTexture *baseColorMap() const;

    float metallicFactor() const;
    float roughnessFactor() const;
    Qt3DRender::QAbstractTexture *metalRoughMap() const;

    float normalScale() const;
    Qt3DRender::QAbstractTexture *normalMap() const;

    Qt3DRender::QAbstractTexture *ambientOcclusionMap() const;

    QColor emissiveFactor() const;
    Qt3DRender::QAbstractTexture *emissiveMap() const;

    QMatrix3x3 textureTransform() const;
    bool isUsingColorAttribute() const;
    bool isDoubleSided() const;
    bool useSkinning() const;
    bool isOpaque() const;
    bool isAlphaCutoffEnabled() const;
    float alphaCutoff() const;

    MetallicRoughnessEffect::ToneMapping toneMappingAlgorithm() const;
    MorphController *morphController() const;

public Q_SLOTS:
    void setBaseColorUsesTexCoord1(bool baseColorUsesTexCoord1);
    void setMetallicRoughnessUsesTexCoord1(bool metallicRoughnessUsesTexCoord1);
    void setNormalUsesTexCoord1(bool normalUsesTexCoord1);
    void setAOUsesTexCoord1(bool aoUsesTexCoord1);
    void setEmissiveUsesTexCoord1(bool emissiveUsesTexCoord1);

    void setBaseColorFactor(const QColor &baseColorFactor);
    void setBaseColorMap(Qt3DRender::QAbstractTexture *baseColorMap);

    void setMetallicFactor(float metallicFactor);
    void setRoughnessFactor(float roughnessFactor);
    void setMetalRoughMap(Qt3DRender::QAbstractTexture *metalRoughMap);

    void setNormalScale(float normalScale);
    void setNormalMap(Qt3DRender::QAbstractTexture *normalMap);

    void setAmbientOcclusionMap(Qt3DRender::QAbstractTexture *ambientOcclusionMap);

    void setEmissiveFactor(const QColor &emissiveFactor);
    void setEmissiveMap(Qt3DRender::QAbstractTexture *emissiveMap);

    void setTextureTransform(const QMatrix3x3 &textureTransform);
    void setUsingColorAttribute(bool usingColorAttribute);
    void setDoubleSided(bool doubleSided);
    void setUseSkinning(bool useSkinning);
    void setOpaque(bool opaque);
    void setAlphaCutoffEnabled(bool enabled);
    void setAlphaCutoff(float alphaCutoff);
    void setToneMappingAlgorithm(MetallicRoughnessEffect::ToneMapping algorithm);
    void setMorphController(MorphController *morphController);

Q_SIGNALS:
    void baseColorUsesTexCoord1Changed(bool);
    void metallicRoughnessUsesTexCoord1Changed(bool);
    void normalUsesTexCoord1Changed(bool);
    void aoUsesTexCoord1Changed(bool);
    void emissiveUsesTexCoord1Changed(bool);

    void baseColorFactorChanged(const QColor &baseColorFactor);
    void baseColorMapChanged(Qt3DRender::QAbstractTexture *baseColorMap);

    void metallicFactorChanged(float metallicFactor);
    void roughnessFactorChanged(float roughnessFactor);
    void metalRoughMapChanged(Qt3DRender::QAbstractTexture *metalRoughMap);

    void normalScaleChanged(float normalScale);
    void normalMapChanged(Qt3DRender::QAbstractTexture *normalMapChanged);

    void ambientOcclusionMapChanged(Qt3DRender::QAbstractTexture *ambientOcclusionMap);

    void emissiveFactorChanged(const QColor &emissiveFactor);
    void emissiveMapChanged(Qt3DRender::QAbstractTexture *emissiveMap);

    void textureTransformChanged(const QMatrix3x3 &textureTransform);
    void usingColorAttributeChanged(bool usingColorAttribute);
    void doubleSidedChanged(bool doubleSided);
    void useSkinningChanged(bool useSkinning);
    void opaqueChanged(bool opaque);
    void alphaCutoffEnabledChanged(bool enabled);
    void alphaCutoffChanged(float value);
    void toneMappingAlgorithmChanged(MetallicRoughnessEffect::ToneMapping algorithm);
    void morphControllerChanged(MorphController *morphController);

private:
    void updateEffect();

    Qt3DRender::QParameter *m_baseColorUsesTexCoord1;
    Qt3DRender::QParameter *m_metallicRoughnessUsesTexCoord1;
    Qt3DRender::QParameter *m_normalUsesTexCoord1;
    Qt3DRender::QParameter *m_aoUsesTexCoord1;
    Qt3DRender::QParameter *m_emissiveUsesTexCoord1;

    Qt3DRender::QParameter *m_baseColorFactorParameter;
    Qt3DRender::QParameter *m_baseColorMapParameter;

    Qt3DRender::QParameter *m_metallicFactorParameter;
    Qt3DRender::QParameter *m_roughnessFactorParameter;
    Qt3DRender::QParameter *m_metalRoughMapParameter;

    Qt3DRender::QParameter *m_normalScaleParameter;
    Qt3DRender::QParameter *m_normalMapParameter;

    Qt3DRender::QParameter *m_ambientOcclusionMapParameter;

    Qt3DRender::QParameter *m_emissiveFactorParameter;
    Qt3DRender::QParameter *m_emissiveMapParameter;

    Qt3DRender::QParameter *m_alphaCutoffParameter;

    Qt3DRender::QParameter *m_textureTransformParameter;

    Qt3DRender::QParameter *m_morphControllerParameter;

    MetallicRoughnessEffect *m_effect;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_METALLICROUGHNESSMATERIAL_H
