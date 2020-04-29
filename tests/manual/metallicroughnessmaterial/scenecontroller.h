/*
    scenecontroller.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Kevin Ottens <kevin.ottens@kdab.com>

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

#ifndef SCENECONTROLLER_H
#define SCENECONTROLLER_H

#include <QObject>
#include <QColor>
#include <QMatrix3x3>
#include <Kuesa/ToneMappingAndGammaCorrectionEffect>

class SceneController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor baseColorFactor READ baseColorFactor WRITE setBaseColorFactor NOTIFY baseColorFactorChanged)
    Q_PROPERTY(bool useBaseColorMap READ useBaseColorMap WRITE setUseBaseColorMap NOTIFY useBaseColorMapChanged)
    Q_PROPERTY(float metallicFactor READ metallicFactor WRITE setMetallicFactor NOTIFY metallicFactorChanged)
    Q_PROPERTY(float roughnessFactor READ roughnessFactor WRITE setRoughnessFactor NOTIFY roughnessFactorChanged)
    Q_PROPERTY(bool useMetalRoughMap READ useMetalRoughMap WRITE setUseMetalRoughMap NOTIFY useMetalRoughMapChanged)
    Q_PROPERTY(float normalScale READ normalScale WRITE setNormalScale NOTIFY normalScaleChanged)
    Q_PROPERTY(bool useNormalMap READ useNormalMap WRITE setUseNormalMap NOTIFY useNormalMapChanged)
    Q_PROPERTY(bool useAmbientOcclusionMap READ useAmbientOcclusionMap WRITE setUseAmbientOcclusionMap NOTIFY useAmbientOcclusionMapChanged)
    Q_PROPERTY(QColor emissiveFactor READ emissiveFactor WRITE setEmissiveFactor NOTIFY emissiveFactorChanged)
    Q_PROPERTY(bool useEmissiveMap READ useEmissiveMap WRITE setUseEmissiveMap NOTIFY useEmissiveMapChanged)
    Q_PROPERTY(QMatrix3x3 textureTransform READ textureTransform WRITE setTextureTransform NOTIFY textureTransformChanged)
    Q_PROPERTY(bool useColorAttribute READ useColorAttribute WRITE setUseColorAttribute NOTIFY useColorAttributeChanged)
    Q_PROPERTY(bool doubleSided READ doubleSided WRITE setDoubleSided NOTIFY doubleSidedChanged)
    Q_PROPERTY(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm READ toneMappingAlgorithm WRITE setToneMappingAlgorithm NOTIFY toneMappingAlgorithmChanged)

public:
    explicit SceneController(QObject *parent = nullptr);

    QColor baseColorFactor() const;
    bool useBaseColorMap() const;
    float metallicFactor() const;
    float roughnessFactor() const;
    bool useMetalRoughMap() const;
    float normalScale() const;
    bool useNormalMap() const;
    bool useAmbientOcclusionMap() const;
    QColor emissiveFactor() const;
    bool useEmissiveMap() const;
    QMatrix3x3 textureTransform() const;
    bool useColorAttribute() const;
    bool doubleSided() const;
    Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm() const;

public slots:
    void setBaseColorFactor(const QColor &baseColorFactor);
    void setUseBaseColorMap(bool useBaseColorMap);
    void setMetallicFactor(float metallicFactor);
    void setRoughnessFactor(float roughnessFactor);
    void setUseMetalRoughMap(bool useMetalRoughMap);
    void setNormalScale(float normalScale);
    void setUseNormalMap(bool useNormalMap);
    void setUseAmbientOcclusionMap(bool useAmbientOcclusionMap);
    void setEmissiveFactor(const QColor &emissiveFactor);
    void setUseEmissiveMap(bool useEmissiveMap);
    void setTextureTransform(const QMatrix3x3 &textureTransform);
    void setUseColorAttribute(bool useColorAttribute);
    void setDoubleSided(bool doubleSided);
    void setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping algorithm);

signals:
    void baseColorFactorChanged(const QColor &baseColorFactor);
    void useBaseColorMapChanged(bool useBaseColorMap);
    void metallicFactorChanged(float metallicFactor);
    void roughnessFactorChanged(float roughnessFactor);
    void useMetalRoughMapChanged(bool useMetalRoughMap);
    void normalScaleChanged(bool normalScale);
    void useNormalMapChanged(bool useNormalMap);
    void useAmbientOcclusionMapChanged(bool useAmbienOcclusionMap);
    void emissiveFactorChanged(const QColor &emissiveFactor);
    void useEmissiveMapChanged(bool useEmissiveMap);
    void textureTransformChanged(const QMatrix3x3 &textureTransform);
    void useColorAttributeChanged(bool useColorAttribute);
    void doubleSidedChanged(bool doubleSided);
    void toneMappingAlgorithmChanged(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping algorithm);

private:
    QColor m_baseColorFactor;
    bool m_useBaseColorMap;
    float m_metallicFactor;
    float m_roughnessFactor;
    bool m_useMetalRoughMap;
    float m_normalScale;
    bool m_useNormalMap;
    bool m_useAmbientOcclusionMap;
    QColor m_emissiveFactor;
    bool m_useEmissiveMap;
    QMatrix3x3 m_textureTransform;
    bool m_useColorAttribute;
    bool m_doubleSided;
    Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping m_toneMappingAlgorithm;
};

#endif // SCENECONTROLLER_H
