/*
    tonemappingandgammacorrectioneffect.h

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

#ifndef KUESA_TONEMAPPINGANDGAMMACORRECTIONEFFECT_H
#define KUESA_TONEMAPPINGANDGAMMACORRECTIONEFFECT_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/abstractpostprocessingeffect.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QLayer;
class QAbstractTexture;
class QParameter;
class QShaderProgramBuilder;
} // namespace Qt3DRender

namespace Kuesa {

class KUESASHARED_EXPORT ToneMappingAndGammaCorrectionEffect : public AbstractPostProcessingEffect
{
    Q_OBJECT
    Q_PROPERTY(float exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
    Q_PROPERTY(ToneMapping toneMappingAlgorithm READ toneMappingAlgorithm WRITE setToneMappingAlgorithm NOTIFY toneMappingAlgorithmChanged REVISION 1)

public:
    enum ToneMapping {
        None = 0,
        Reinhard,
        Filmic,
        Uncharted
    };
    Q_ENUM(ToneMapping)

    explicit ToneMappingAndGammaCorrectionEffect(Qt3DCore::QNode *parent = nullptr);
    ~ToneMappingAndGammaCorrectionEffect();

    float exposure() const;
    float gamma() const;
    ToneMapping toneMappingAlgorithm() const;

    // AbstractPostProcessingEffect interface
    FrameGraphNodePtr frameGraphSubTree() const override;
    void setInputTexture(Qt3DRender::QAbstractTexture *texture) override;
    QVector<Qt3DRender::QLayer *> layers() const override;

public Q_SLOTS:
    void setExposure(float exposure);
    void setGamma(float gamma);
    void setToneMappingAlgorithm(ToneMapping toneMappingAlgorithm);

Q_SIGNALS:
    void exposureChanged(float exposure);
    void gammaChanged(float gamma);
    void toneMappingAlgorithmChanged(ToneMapping toneMappingAlgorithm);

private:
    FrameGraphNodePtr m_rootFrameGraphNode;
    Qt3DRender::QLayer *m_layer;
    Qt3DRender::QShaderProgramBuilder *m_gl3ShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_es3ShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_es2ShaderBuilder;
    Qt3DRender::QParameter *m_exposureParameter;
    Qt3DRender::QParameter *m_gammaParameter;
    Qt3DRender::QParameter *m_inputTextureParameter;
    ToneMapping m_toneMappingAlgorithm;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_TONEMAPPINGANDGAMMACORRECTIONEFFECT_H
