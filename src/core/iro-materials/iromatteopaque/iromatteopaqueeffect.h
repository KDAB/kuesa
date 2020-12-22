
/*
    iromatteopaqueeffect.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_IROMATTEOPAQUEEFFECT_H
#define KUESA_IROMATTEOPAQUEEFFECT_H

#include <Kuesa/gltf2materialeffect.h>
#include <Kuesa/kuesa_global.h>


QT_BEGIN_NAMESPACE

namespace Kuesa {

class IroMatteOpaqueTechnique;

class KUESASHARED_EXPORT IroMatteOpaqueEffect : public GLTF2MaterialEffect
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit IroMatteOpaqueEffect(Qt3DCore::QNode *parent = nullptr);
    ~IroMatteOpaqueEffect();

private:
    IroMatteOpaqueTechnique *m_gl3Technique;
    IroMatteOpaqueTechnique *m_es3Technique;
    IroMatteOpaqueTechnique *m_es2Technique;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    IroMatteOpaqueTechnique *m_rhiTechnique;
#endif
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
    void updateUsingMorphTargets(bool enabled) override;
    void updateInstanced(bool instanced) override;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_IROMATTEOPAQUEEFFECT_H
