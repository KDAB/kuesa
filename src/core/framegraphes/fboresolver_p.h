/*
    fboresolver_p.h

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

#ifndef KUESA_FBORESOLVER_P_H
#define KUESA_FBORESOLVER_P_H

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

#include <Kuesa/private/kuesa_global_p.h>
#include <Qt3DRender/qrendertargetselector.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QMaterial;
class QParameter;
class QAbstractTexture;
class QShaderProgram;
} // namespace Qt3DRender

namespace Kuesa {

class FullScreenQuad;

class KUESA_PRIVATE_EXPORT FBOResolver : public Qt3DRender::QRenderTargetSelector
{
    Q_OBJECT
public:
    explicit FBOResolver(Qt3DCore::QNode *parent = nullptr);

    void setSource(Qt3DRender::QAbstractTexture *source);
    void setDestination(Qt3DRender::QRenderTarget *destination);

    void setViewportRect(const QRectF &vp);
    QRectF viewportRect() const;

    Qt3DRender::QAbstractTexture *source() const;
    Qt3DRender::QRenderTarget *destination() const;
    bool targetHasSamples() const;

private:
    Qt3DRender::QMaterial *m_material = nullptr;
    Qt3DRender::QParameter *m_sourceTextureParameter = nullptr;
    Qt3DRender::QShaderProgram *m_shader = nullptr;

    void updateFragmentShader();

    FullScreenQuad *m_fsQuad = nullptr;
};

} // namespace Kuesa
QT_END_NAMESPACE

#endif // KUESA_FBORESOLVER_P_H
