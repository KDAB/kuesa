/*
    metallicroughnesseffect.h

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

#ifndef KUESA_METALLICROUGHNESSEFFECT_H
#define KUESA_METALLICROUGHNESSEFFECT_H

#include <QtGui/qcolor.h>
#include <Kuesa/gltf2materialeffect.h>
#include <Kuesa/kuesa_global.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/qtechnique.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QAbstractTexture;
class QBlendEquation;
class QBlendEquationArguments;
class QShaderProgramBuilder;
class QShaderProgram;
class QRenderPass;
class QFilterKey;
} // namespace Qt3DRender

namespace Kuesa {

class KUESASHARED_EXPORT MetallicRoughnessTechnique : public Qt3DRender::QTechnique
{
    Q_OBJECT
public:
    enum Version {
        GL3 = 0,
        ES3,
        ES2,
        RHI
    };

    explicit MetallicRoughnessTechnique(Version version, Qt3DCore::QNode *parent = nullptr);

    QStringList enabledLayers() const;
    void setEnabledLayers(const QStringList &layers);
    void setOpaque(bool opaque);
    void setCullingMode(Qt3DRender::QCullFace::CullingMode mode);
    void setAllowCulling(bool allowCulling);

    Qt3DRender::QShaderProgramBuilder *metalRoughShaderBuilder() const;

private:
    Qt3DRender::QCullFace *m_backFaceCulling;
    Qt3DRender::QBlendEquation *m_blendEquation;
    Qt3DRender::QBlendEquationArguments *m_blendArguments;
    Qt3DRender::QShaderProgramBuilder *m_metalRoughShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_zfillShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_cubeMapShadowShaderBuilder;
    Qt3DRender::QShaderProgram *m_metalRoughShader;
    Qt3DRender::QShaderProgram *m_zfillShader;
    Qt3DRender::QRenderPass *m_zfillRenderPass;
    Qt3DRender::QRenderPass *m_opaqueRenderPass;
    Qt3DRender::QRenderPass *m_transparentRenderPass;
    Qt3DRender::QRenderPass *m_cubeMapShadowRenderPass;
    Qt3DRender::QFilterKey *m_techniqueAllowFrustumCullingFilterKey;
};

class KUESASHARED_EXPORT MetallicRoughnessEffect : public GLTF2MaterialEffect
{
    Q_OBJECT
    Q_PROPERTY(bool baseColorMapEnabled READ isBaseColorMapEnabled WRITE setBaseColorMapEnabled NOTIFY baseColorMapEnabledChanged)
    Q_PROPERTY(bool metalRoughMapEnabled READ isMetalRoughMapEnabled WRITE setMetalRoughMapEnabled NOTIFY metalRoughMapEnabledChanged)
    Q_PROPERTY(bool normalMapEnabled READ isNormalMapEnabled WRITE setNormalMapEnabled NOTIFY normalMapEnabledChanged)
    Q_PROPERTY(bool ambientOcclusionMapEnabled READ isAmbientOcclusionMapEnabled WRITE setAmbientOcclusionMapEnabled NOTIFY ambientOcclusionMapEnabledChanged)
    Q_PROPERTY(bool emissiveMapEnabled READ isEmissiveMapEnabled WRITE setEmissiveMapEnabled NOTIFY emissiveMapEnabledChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *brdfLUT READ brdfLUT WRITE setBrdfLUT NOTIFY brdfLUTChanged)

public:
    explicit MetallicRoughnessEffect(Qt3DCore::QNode *parent = nullptr);
    ~MetallicRoughnessEffect();

    bool isBaseColorMapEnabled() const;
    bool isMetalRoughMapEnabled() const;
    bool isNormalMapEnabled() const;
    bool isAmbientOcclusionMapEnabled() const;
    bool isEmissiveMapEnabled() const;
    Qt3DRender::QAbstractTexture *brdfLUT() const;

public Q_SLOTS:
    void setBaseColorMapEnabled(bool enabled);
    void setMetalRoughMapEnabled(bool enabled);
    void setNormalMapEnabled(bool enabled);
    void setAmbientOcclusionMapEnabled(bool enabled);
    void setEmissiveMapEnabled(bool enabled);
    void setBrdfLUT(Qt3DRender::QAbstractTexture *brdfLUT);

Q_SIGNALS:
    void baseColorMapEnabledChanged(bool enabled);
    void metalRoughMapEnabledChanged(bool enabled);
    void normalMapEnabledChanged(bool enabled);
    void ambientOcclusionMapEnabledChanged(bool enabled);
    void emissiveMapEnabledChanged(bool enabled);
    void brdfLUTChanged(Qt3DRender::QAbstractTexture *brdfLUT);

private:
    bool m_baseColorMapEnabled;
    bool m_metalRoughMapEnabled;
    bool m_normalMapEnabled;
    bool m_ambientOcclusionMapEnabled;
    bool m_emissiveMapEnabled;

    MetallicRoughnessTechnique *m_metalRoughGL3Technique;
    MetallicRoughnessTechnique *m_metalRoughES3Technique;
    MetallicRoughnessTechnique *m_metalRoughES2Technique;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    MetallicRoughnessTechnique *m_metalRoughRHITechnique;
#endif
    Qt3DRender::QParameter *m_brdfLUTParameter;

    void updateLayersOnTechniques(const QStringList &layers);

    void updateDoubleSided(bool doubleSided) override;
    void updateUsingSkinning(bool useSkinning) override;
    void updateOpaque(bool opaque) override;
    void updateAlphaCutoffEnabled(bool enabled) override;
    void updateUsingColorAttribute(bool enabled) override;
    void updateUsingNormalAttribute(bool enabled) override;
    void updateUsingTangentAttribute(bool enabled) override;
    void updateUsingTexCoordAttribute(bool enabled) override;
    void updateUsingTexCoord1Attribute(bool enabled) override;
    void updateUsingMorphTargets(bool usingMorphTargets) override;
    void updateUsingCubeMapArrays(bool usingCubeMapArrays) override;
    void updateInstanced(bool instanced) override;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_METALLICROUGHNESSEFFECT_H
