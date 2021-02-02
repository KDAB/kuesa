/*
    gltf2materialeffect.cpp

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

#ifndef KUESA_GLTF2MATERIALEFFECT_H
#define KUESA_GLTF2MATERIALEFFECT_H

#include <Qt3DRender/QEffect>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class KUESASHARED_EXPORT GLTF2MaterialEffect : public Qt3DRender::QEffect
{
    Q_OBJECT
    Q_PROPERTY(bool doubleSided READ isDoubleSided WRITE setDoubleSided NOTIFY doubleSidedChanged)
    Q_PROPERTY(bool usingSkinning READ isUsingSkinning WRITE setUsingSkinning NOTIFY usingSkinningChanged)
    Q_PROPERTY(bool opaque READ isOpaque WRITE setOpaque NOTIFY opaqueChanged)
    Q_PROPERTY(bool alphaCutoffEnabled READ isAlphaCutoffEnabled WRITE setAlphaCutoffEnabled NOTIFY alphaCutoffEnabledChanged)
    Q_PROPERTY(bool usingColorAttribute READ isUsingColorAttribute WRITE setUsingColorAttribute NOTIFY usingColorAttributeChanged)
    Q_PROPERTY(bool usingNormalAttribute READ isUsingNormalAttribute WRITE setUsingNormalAttribute NOTIFY usingNormalAttributeChanged)
    Q_PROPERTY(bool usingTangentAttribute READ isUsingTangentAttribute WRITE setUsingTangentAttribute NOTIFY usingTangentAttributeChanged)
    Q_PROPERTY(bool usingTexCoordAttribute READ isUsingTexCoordAttribute WRITE setUsingTexCoordAttribute NOTIFY usingTexCoordAttributeChanged)
    Q_PROPERTY(bool usingTexCoord1Attribute READ isUsingTexCoord1Attribute WRITE setUsingTexCoord1Attribute NOTIFY usingTexCoord1AttributeChanged)
    Q_PROPERTY(bool usingMorphTargets READ isUsingMorphTargets WRITE setUsingMorphTargets NOTIFY usingMorphTargetsChanged)
    Q_PROPERTY(bool instanced READ isInstanced WRITE setInstanced NOTIFY instancedChanged)

public:
    explicit GLTF2MaterialEffect(Qt3DCore::QNode *parent = nullptr);
    ~GLTF2MaterialEffect();

    bool isDoubleSided() const;
    bool isUsingSkinning() const;
    bool isOpaque() const;
    bool isAlphaCutoffEnabled() const;
    bool isUsingColorAttribute() const;
    bool isUsingNormalAttribute() const;
    bool isUsingTangentAttribute() const;
    bool isUsingTexCoordAttribute() const;
    bool isUsingTexCoord1Attribute() const;
    bool isUsingMorphTargets() const;
    bool isInstanced() const;

public Q_SLOTS:
    void setDoubleSided(bool doubleSided);
    void setUsingSkinning(bool useSkinning);
    void setOpaque(bool opaque);
    void setAlphaCutoffEnabled(bool enabled);
    void setUsingColorAttribute(bool usingColorAttribute);
    void setUsingNormalAttribute(bool usingNormalAttribute);
    void setUsingTangentAttribute(bool usingTangentAttribute);
    void setUsingTexCoordAttribute(bool usingTexCoordAttribute);
    void setUsingTexCoord1Attribute(bool usingTexCoord1Attribute);
    void setUsingMorphTargets(bool usingMorphTargets);
    void setInstanced(bool instanced);

protected:
    virtual void updateDoubleSided(bool doubleSided) = 0;
    virtual void updateUsingSkinning(bool enabled) = 0;
    virtual void updateOpaque(bool opaque) = 0;
    virtual void updateAlphaCutoffEnabled(bool enabled) = 0;
    virtual void updateUsingColorAttribute(bool enabled) = 0;
    virtual void updateUsingNormalAttribute(bool enabled) = 0;
    virtual void updateUsingTangentAttribute(bool enabled) = 0;
    virtual void updateUsingTexCoordAttribute(bool enabled) = 0;
    virtual void updateUsingTexCoord1Attribute(bool enabled) = 0;
    virtual void updateUsingMorphTargets(bool usingMorphTargets) = 0;
    virtual void updateInstanced(bool instanced) = 0;

    // Todo: make this pure virtual and implement in iro materials
    virtual void updateUsingCubeMapArrays(bool usingCubeMapArrays);

Q_SIGNALS:
    void doubleSidedChanged(bool doubleSided);
    void usingSkinningChanged(bool useSkinning);
    void opaqueChanged(bool opaque);
    void alphaCutoffEnabledChanged(bool enabled);
    void usingColorAttributeChanged(bool usingColorAttribute);
    void usingNormalAttributeChanged(bool usingNormalAttribute);
    void usingTangentAttributeChanged(bool usingTangentAttribute);
    void usingTexCoordAttributeChanged(bool usingTexCoordAttribute);
    void usingTexCoord1AttributeChanged(bool usingTexCoord1Attribute);
    void usingMorphTargetsChanged(bool usingMorphTargets);
    void instancedChanged(bool instanced);

private:
    bool m_usingSkinning;
    bool m_opaque;
    bool m_alphaCutoffEnabled;
    bool m_doubleSided;
    bool m_usingColorAttribute;
    bool m_usingNormalAttribute;
    bool m_usingTangentAttribute;
    bool m_usingTexCoordAttribute;
    bool m_usingTexCoord1Attribute;
    bool m_usingMorphTargets;
    bool m_instanced;

    void initialize();
};

} // Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2MATERIALEFFECT_H
