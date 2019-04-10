/*
    metallicroughnesseffect.h

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

#ifndef KUESA_METALLICROUGHNESSEFFECT_H
#define KUESA_METALLICROUGHNESSEFFECT_H

#include <QtGui/qcolor.h>
#include <Qt3DRender/qeffect.h>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QAbstractTexture;
} // namespace Qt3DRender

namespace Kuesa {
class MetallicRoughnessTechnique;

class KUESASHARED_EXPORT MetallicRoughnessEffect : public Qt3DRender::QEffect
{
    Q_OBJECT
    Q_PROPERTY(bool baseColorMapEnabled READ isBaseColorMapEnabled WRITE setBaseColorMapEnabled NOTIFY baseColorMapEnabledChanged)
    Q_PROPERTY(bool metalRoughMapEnabled READ isMetalRoughMapEnabled WRITE setMetalRoughMapEnabled NOTIFY metalRoughMapEnabledChanged)
    Q_PROPERTY(bool normalMapEnabled READ isNormalMapEnabled WRITE setNormalMapEnabled NOTIFY normalMapEnabledChanged)
    Q_PROPERTY(bool ambientOcclusionMapEnabled READ isAmbientOcclusionMapEnabled WRITE setAmbientOcclusionMapEnabled NOTIFY ambientOcclusionMapEnabledChanged)
    Q_PROPERTY(bool emissiveMapEnabled READ isEmissiveMapEnabled WRITE setEmissiveMapEnabled NOTIFY emissiveMapEnabledChanged)
    Q_PROPERTY(bool usingColorAttribute READ isUsingColorAttribute WRITE setUsingColorAttribute NOTIFY usingColorAttributeChanged)
    Q_PROPERTY(bool doubleSided READ isDoubleSided WRITE setDoubleSided NOTIFY doubleSidedChanged)
    Q_PROPERTY(bool useSkinning READ useSkinning WRITE setUseSkinning NOTIFY useSkinningChanged)
    Q_PROPERTY(bool opaque READ isOpaque WRITE setOpaque NOTIFY opaqueChanged)
    Q_PROPERTY(bool alphaCutoffEnabled READ isAlphaCutoffEnabled WRITE setAlphaCutoffEnabled NOTIFY alphaCutoffEnabledChanged)
    Q_PROPERTY(Kuesa::MetallicRoughnessEffect::ToneMapping toneMappingAlgorithm READ toneMappingAlgorithm WRITE setToneMappingAlgorithm NOTIFY toneMappingAlgorithmChanged REVISION 1)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *brdfLUT READ brdfLUT WRITE setBrdfLUT NOTIFY brdfLUTChanged REVISION 1)

public:
    enum ToneMapping {
        Reinhard = 0,
        Filmic
    };
    Q_ENUM(ToneMapping)

    explicit MetallicRoughnessEffect(Qt3DCore::QNode *parent = nullptr);
    ~MetallicRoughnessEffect();

    bool isBaseColorMapEnabled() const;
    bool isMetalRoughMapEnabled() const;
    bool isNormalMapEnabled() const;
    bool isAmbientOcclusionMapEnabled() const;
    bool isEmissiveMapEnabled() const;
    bool isUsingColorAttribute() const;
    bool isDoubleSided() const;
    bool useSkinning() const;
    bool isOpaque() const;
    bool isAlphaCutoffEnabled() const;
    ToneMapping toneMappingAlgorithm() const;
    Qt3DRender::QAbstractTexture *brdfLUT() const;

public Q_SLOTS:
    void setBaseColorMapEnabled(bool enabled);
    void setMetalRoughMapEnabled(bool enabled);
    void setNormalMapEnabled(bool enabled);
    void setAmbientOcclusionMapEnabled(bool enabled);
    void setEmissiveMapEnabled(bool enabled);
    void setUsingColorAttribute(bool usingColorAttribute);
    void setDoubleSided(bool doubleSided);
    void setUseSkinning(bool useSkinning);
    void setOpaque(bool opaque);
    void setAlphaCutoffEnabled(bool enabled);
    void setToneMappingAlgorithm(ToneMapping algorithm);
    void setBrdfLUT(Qt3DRender::QAbstractTexture *brdfLUT);

Q_SIGNALS:
    void baseColorMapEnabledChanged(bool enabled);
    void metalRoughMapEnabledChanged(bool enabled);
    void normalMapEnabledChanged(bool enabled);
    void ambientOcclusionMapEnabledChanged(bool enabled);
    void emissiveMapEnabledChanged(bool enabled);
    void usingColorAttributeChanged(bool usingColorAttribute);
    void doubleSidedChanged(bool doubleSided);
    void useSkinningChanged(bool useSkinning);
    void opaqueChanged(bool opaque);
    void alphaCutoffEnabledChanged(bool enabled);
    void toneMappingAlgorithmChanged(ToneMapping algorithm);
    void brdfLUTChanged(Qt3DRender::QAbstractTexture *brdfLUT);

private:
    bool m_baseColorMapEnabled;
    bool m_metalRoughMapEnabled;
    bool m_normalMapEnabled;
    bool m_ambientOcclusionMapEnabled;
    bool m_emissiveMapEnabled;
    bool m_usingColorAttribute;
    bool m_doubleSided;
    bool m_useSkinning;
    bool m_opaque;
    bool m_alphaCutoffEnabled;
    ToneMapping m_toneMappingAlgorithm;

    MetallicRoughnessTechnique *m_metalRoughGL3Technique;
    MetallicRoughnessTechnique *m_metalRoughES3Technique;
    MetallicRoughnessTechnique *m_metalRoughES2Technique;
    Qt3DRender::QParameter *m_brdfLUTParameter;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_METALLICROUGHNESSEFFECT_H
