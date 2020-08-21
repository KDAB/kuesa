/*
    unliteffect.h

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

#ifndef KUESA_UNLITEFFECT_H
#define KUESA_UNLITEFFECT_H

#include <QtGui/qcolor.h>
#include <Kuesa/gltf2materialeffect.h>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QAbstractTexture;
class QShaderProgramBuilder;
class QShaderProgram;
class QCullFace;
class QRenderPass;
} // namespace Qt3DRender

namespace Kuesa {
class UnlitTechnique;

class KUESASHARED_EXPORT UnlitEffect : public GLTF2MaterialEffect
{
    Q_OBJECT
    Q_PROPERTY(bool baseColorMapEnabled READ isBaseColorMapEnabled WRITE setBaseColorMapEnabled NOTIFY baseColorMapEnabledChanged)
    Q_PROPERTY(bool usingColorAttribute READ isUsingColorAttribute WRITE setUsingColorAttribute NOTIFY usingColorAttributeChanged)

public:
    explicit UnlitEffect(Qt3DCore::QNode *parent = nullptr);
    ~UnlitEffect();

    bool isBaseColorMapEnabled() const;
    bool isUsingColorAttribute() const;

public Q_SLOTS:
    void setBaseColorMapEnabled(bool enabled);
    void setUsingColorAttribute(bool usingColorAttribute);

Q_SIGNALS:
    void baseColorMapEnabledChanged(bool enabled);
    void usingColorAttributeChanged(bool usingColorAttribute);

private:
    bool m_baseColorMapEnabled;
    bool m_usingColorAttribute;

    UnlitTechnique *m_unlitGL3Technique;
    UnlitTechnique *m_unlitES3Technique;
    UnlitTechnique *m_unlitES2Technique;
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    UnlitTechnique *m_unlitRHITechnique;
#endif
    void updateLayersOnTechniques(const QStringList &layers);

    void updateDoubleSided(bool doubleSided) override;
    void updateSkinning(bool useSkinning) override;
    void updateOpaque(bool opaque) override;
    void updateAlphaCutoffEnabled(bool enabled) override;
    void updateUsingColorAttribute(bool enabled) override;
    void updateUsingNormalAttribute(bool enabled) override;
    void updateUsingTangentAttribute(bool enabled) override;
    void updateUsingTexCoordAttribute(bool enabled) override;
    void updateUsingTexCoord1Attribute(bool enabled) override;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_UNLITEFFECT_H
