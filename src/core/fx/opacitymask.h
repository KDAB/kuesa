/*
    opacitymask.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_OPACITYMASK_H
#define KUESA_OPACITYMASK_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/abstractpostprocessingeffect.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QLayer;
class QAbstractTexture;
class QParameter;
class QShaderProgramBuilder;
class QShaderProgram;
class QRenderStateSet;
} // namespace Qt3DRender

namespace Kuesa {

class KUESASHARED_EXPORT OpacityMask : public AbstractPostProcessingEffect
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QAbstractTexture *mask READ mask WRITE setMask NOTIFY maskChanged)
    Q_PROPERTY(bool premultipliedAlpha READ premultipliedAlpha WRITE setPremultipliedAlpha NOTIFY premultipliedAlphaChanged)
public:
    explicit OpacityMask(Qt3DCore::QNode *parent = nullptr);

    FrameGraphNodePtr frameGraphSubTree() const override;
    QVector<Qt3DRender::QLayer *> layers() const override;
    void setInputTexture(Qt3DRender::QAbstractTexture *texture) override;

    void setMask(Qt3DRender::QAbstractTexture *mask);
    Qt3DRender::QAbstractTexture *mask() const;

    void setPremultipliedAlpha(bool premultiplied);
    bool premultipliedAlpha() const;

Q_SIGNALS:
    void maskChanged(Qt3DRender::QAbstractTexture *mask);
    void premultipliedAlphaChanged(bool premultipliedAlpha);

private:
    FrameGraphNodePtr m_rootFrameGraphNode;
    bool m_premultipliedAlpha;
    Qt3DRender::QLayer *m_layer;
    Qt3DRender::QShaderProgramBuilder *m_gl3ShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_es3ShaderBuilder;
    Qt3DRender::QRenderStateSet *m_blendRenderState;
    Qt3DRender::QParameter *m_maskParameter;
    Qt3DRender::QParameter *m_inputTextureParameter;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_OPACITYMASK_H
