/*
    metallicroughnessshaderdata_p.h

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_METALLICROUGHNESSSHADERDATA_P_H
#define KUESA_METALLICROUGHNESSSHADERDATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/QComponent>
#include <Qt3DRender/QShaderData>
#include <Qt3DRender/QAbstractTexture>

#include <QMatrix3x3>

#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class MetallicRoughnessShaderData : public Qt3DRender::QShaderData
{
    Q_OBJECT

    Q_PROPERTY(bool baseColorUsesTexCoord1 READ isBaseColorUsingTexCoord1 WRITE setBaseColorUsesTexCoord1 NOTIFY baseColorUsesTexCoord1Changed)
    Q_PROPERTY(bool metallicRoughnessUsesTexCoord1 READ isMetallicRoughnessUsingTexCoord1 WRITE setMetallicRoughnessUsesTexCoord1 NOTIFY metallicRoughnessUsesTexCoord1Changed)
    Q_PROPERTY(bool normalUsesTexCoord1 READ isNormalUsingTexCoord1 WRITE setNormalUsesTexCoord1 NOTIFY normalUsesTexCoord1Changed)
    Q_PROPERTY(bool aoUsesTexCoord1 READ isAOUsingTexCoord1 WRITE setAOUsesTexCoord1 NOTIFY aoUsesTexCoord1Changed)
    Q_PROPERTY(bool emissiveUsesTexCoord1 READ isEmissiveUsingTexCoord1 WRITE setEmissiveUsesTexCoord1 NOTIFY emissiveUsesTexCoord1Changed)
    Q_PROPERTY(bool receivesShadows READ receivesShadows WRITE setReceivesShadows NOTIFY receivesShadowsChanged)

    Q_PROPERTY(QColor baseColorFactor READ baseColorFactor WRITE setBaseColorFactor NOTIFY baseColorFactorChanged)
    Q_PROPERTY(QMatrix3x3 baseColorMapTextureTransform READ baseColorMapTextureTransform WRITE setBaseColorMapTextureTransform NOTIFY baseColorMapTextureTransformChanged)
    Q_PROPERTY(float metallicFactor READ metallicFactor WRITE setMetallicFactor NOTIFY metallicFactorChanged)
    Q_PROPERTY(float roughnessFactor READ roughnessFactor WRITE setRoughnessFactor NOTIFY roughnessFactorChanged)
    Q_PROPERTY(QMatrix3x3 metalRoughMapTextureTransform READ metalRoughMapTextureTransform WRITE setMetalRoughMapTextureTransform NOTIFY metalRoughMapTextureTransformChanged)

    Q_PROPERTY(float normalScale READ normalScale WRITE setNormalScale NOTIFY normalScaleChanged)
    Q_PROPERTY(QMatrix3x3 normalMapTextureTransform READ normalMapTextureTransform WRITE setNormalMapTextureTransform NOTIFY normalMapTextureTransformChanged)

    Q_PROPERTY(QMatrix3x3 ambientOcclusionMapTextureTransform READ ambientOcclusionMapTextureTransform WRITE setAmbientOcclusionMapTextureTransform NOTIFY ambientOcclusionMapTextureTransformChanged)

    Q_PROPERTY(QColor emissiveFactor READ emissiveFactor WRITE setEmissiveFactor NOTIFY emissiveFactorChanged)
    Q_PROPERTY(QMatrix3x3 emissiveMapTextureTransform READ emissiveMapTextureTransform WRITE setEmissiveMapTextureTransform NOTIFY emissiveMapTextureTransformChanged)

    Q_PROPERTY(float alphaCutoff READ alphaCutoff WRITE setAlphaCutoff NOTIFY alphaCutoffChanged)

public:
    explicit MetallicRoughnessShaderData(Qt3DCore::QNode *parent = nullptr);
    ~MetallicRoughnessShaderData();

    bool isBaseColorUsingTexCoord1() const;
    bool isMetallicRoughnessUsingTexCoord1() const;
    bool isNormalUsingTexCoord1() const;
    bool isAOUsingTexCoord1() const;
    bool isEmissiveUsingTexCoord1() const;

    QColor baseColorFactor() const;
    Qt3DRender::QAbstractTexture *baseColorMap() const;
    QMatrix3x3 baseColorMapTextureTransform() const;

    float metallicFactor() const;
    float roughnessFactor() const;
    Qt3DRender::QAbstractTexture *metalRoughMap() const;
    QMatrix3x3 metalRoughMapTextureTransform() const;

    float normalScale() const;
    Qt3DRender::QAbstractTexture *normalMap() const;
    QMatrix3x3 normalMapTextureTransform() const;

    Qt3DRender::QAbstractTexture *ambientOcclusionMap() const;
    QMatrix3x3 ambientOcclusionMapTextureTransform() const;

    QColor emissiveFactor() const;
    Qt3DRender::QAbstractTexture *emissiveMap() const;
    QMatrix3x3 emissiveMapTextureTransform() const;

    float alphaCutoff() const;

    bool receivesShadows() const;

public Q_SLOTS:
    void setBaseColorUsesTexCoord1(bool baseColorUsesTexCoord1);
    void setMetallicRoughnessUsesTexCoord1(bool metallicRoughnessUsesTexCoord1);
    void setNormalUsesTexCoord1(bool normalUsesTexCoord1);
    void setAOUsesTexCoord1(bool aoUsesTexCoord1);
    void setEmissiveUsesTexCoord1(bool emissiveUsesTexCoord1);

    void setBaseColorFactor(const QColor &baseColorFactor);
    void setBaseColorMapTextureTransform(const QMatrix3x3 &m);

    void setMetallicFactor(float metallicFactor);
    void setRoughnessFactor(float roughnessFactor);
    void setMetalRoughMapTextureTransform(const QMatrix3x3 &m);

    void setNormalScale(float normalScale);
    void setNormalMapTextureTransform(const QMatrix3x3 &m);

    void setAmbientOcclusionMapTextureTransform(const QMatrix3x3 &m);

    void setEmissiveFactor(const QColor &emissiveFactor);
    void setEmissiveMapTextureTransform(const QMatrix3x3 &m);

    void setAlphaCutoff(float alphaCutoff);

    void setReceivesShadows(bool receivesShadows);

Q_SIGNALS:
    void baseColorUsesTexCoord1Changed(bool);
    void metallicRoughnessUsesTexCoord1Changed(bool);
    void normalUsesTexCoord1Changed(bool);
    void aoUsesTexCoord1Changed(bool);
    void emissiveUsesTexCoord1Changed(bool);

    void baseColorFactorChanged(const QColor &baseColorFactor);
    void baseColorMapTextureTransformChanged(const QMatrix3x3 &m);

    void metallicFactorChanged(float metallicFactor);
    void roughnessFactorChanged(float roughnessFactor);
    void metalRoughMapTextureTransformChanged(const QMatrix3x3 &m);

    void normalScaleChanged(float normalScale);
    void normalMapTextureTransformChanged(const QMatrix3x3 &m);

    void ambientOcclusionMapTextureTransformChanged(const QMatrix3x3 &m);

    void emissiveFactorChanged(const QColor &emissiveFactor);
    void emissiveMapTextureTransformChanged(const QMatrix3x3 &m);

    void alphaCutoffChanged(float alphaCutoff);

    void receivesShadowsChanged(bool receivesShadows);

private:
    bool m_baseColorUsesTexCoord1;
    bool m_metallicRoughnessUsesTexCoord1;
    bool m_normalUsesTexCoord1;
    bool m_aoUsesTexCoord1;
    bool m_emissiveUsesTexCoord1;

    QColor m_baseColorFactor;
    QMatrix3x3 m_baseColorMapTextureTransform;

    float m_metallicFactor;
    float m_roughnessFactor;
    QMatrix3x3 m_metalRoughMapTextureTransform;

    float m_normalScale;
    QMatrix3x3 m_normalMapTextureTransform;

    QMatrix3x3 m_ambientOcclusionMapTextureTransform;

    QColor m_emissiveFactor;
    QMatrix3x3 m_emissiveMapTextureTransform;

    float m_alphaCutoff;
    bool m_receivesShadows;
};
} // namespace Kuesa
QT_END_NAMESPACE

#endif // KUESA_METALLICROUGHNESSSHADERDATA_P_H
