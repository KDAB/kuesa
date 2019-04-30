/*
    unliteffect.h

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Qt3DRender/qeffect.h>
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

class KUESASHARED_EXPORT UnlitEffect : public Qt3DRender::QEffect
{
    Q_OBJECT
    Q_PROPERTY(bool baseColorMapEnabled READ isBaseColorMapEnabled WRITE setBaseColorMapEnabled NOTIFY baseColorMapEnabledChanged)
    Q_PROPERTY(bool usingColorAttribute READ isUsingColorAttribute WRITE setUsingColorAttribute NOTIFY usingColorAttributeChanged)
    Q_PROPERTY(bool doubleSided READ isDoubleSided WRITE setDoubleSided NOTIFY doubleSidedChanged)
    Q_PROPERTY(bool useSkinning READ useSkinning WRITE setUseSkinning NOTIFY useSkinningChanged)
    Q_PROPERTY(bool opaque READ isOpaque WRITE setOpaque NOTIFY opaqueChanged)
    Q_PROPERTY(bool alphaCutoffEnabled READ isAlphaCutoffEnabled WRITE setAlphaCutoffEnabled NOTIFY alphaCutoffEnabledChanged)

public:
    explicit UnlitEffect(Qt3DCore::QNode *parent = nullptr);
    ~UnlitEffect();

    bool isBaseColorMapEnabled() const;
    bool isUsingColorAttribute() const;
    bool isDoubleSided() const;
    bool useSkinning() const;
    bool isOpaque() const;
    bool isAlphaCutoffEnabled() const;

public Q_SLOTS:
    void setBaseColorMapEnabled(bool enabled);
    void setUsingColorAttribute(bool usingColorAttribute);
    void setDoubleSided(bool doubleSided);
    void setUseSkinning(bool useSkinning);
    void setOpaque(bool opaque);
    void setAlphaCutoffEnabled(bool enabled);

Q_SIGNALS:
    void baseColorMapEnabledChanged(bool enabled);
    void usingColorAttributeChanged(bool usingColorAttribute);
    void doubleSidedChanged(bool doubleSided);
    void useSkinningChanged(bool useSkinning);
    void opaqueChanged(bool opaque);
    void alphaCutoffEnabledChanged(bool enabled);

private:
    bool m_baseColorMapEnabled;
    bool m_usingColorAttribute;
    bool m_useSkinning;
    bool m_invokeInitVertexShaderRequested;
    bool m_opaque;
    bool m_alphaCutoffEnabled;

    UnlitTechnique *m_unlitGL3Technique;
    UnlitTechnique *m_unlitES3Technique;
    UnlitTechnique *m_unlitES2Technique;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_UNLITEFFECT_H
