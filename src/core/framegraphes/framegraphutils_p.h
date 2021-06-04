/*
    framegraphutils_p.h

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

#ifndef KUESA_FRAMEGRAPHUTILS_P_H
#define KUESA_FRAMEGRAPHUTILS_P_H

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
#include <Qt3DRender/QRenderTargetOutput>
#include <QFlags>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QRenderTarget;
class QAbtractTexture;
} // namespace Qt3DRender

namespace Kuesa {

class KUESA_PRIVATE_EXPORT FrameGraphUtils
{
    Q_GADGET
public:
    struct RenderingFeatures {
        bool hasHalfFloatTexture = false;
        bool hasHalfFloatRenderable = false;
        bool hasMultisampledTexture = false;
        bool hasMultisampledFBO = false;

        bool hasCubeMapArrayTextures = false;
        bool hasGeometryShader = false;
    };

    static RenderingFeatures checkRenderingFeatures();
    static bool hasHalfFloatRenderable();
    static bool hasCubeMapArrayTextures();
    static bool hasGeometryShaderSupport();
    static bool hasMSAASupport();

    enum RenderTargetFlag {
        IncludeDepth = (1 << 0),
        Multisampled = (1 << 1),
        HalfFloatAttachments = (1 << 2),
        IncludeStencil = (1 << 3)
    };
    Q_DECLARE_FLAGS(RenderTargetFlags, RenderTargetFlag)
    Q_FLAG(RenderTargetFlags)
    Q_ENUM(RenderTargetFlag)

    static Qt3DRender::QRenderTarget *createRenderTarget(RenderTargetFlags flags,
                                                         Qt3DCore::QNode *owner,
                                                         const QSize surfaceSize,
                                                         int samples = 0);

    static bool renderTargetHasAttachmentOfType(const Qt3DRender::QRenderTarget *target,
                                                const Qt3DRender::QRenderTargetOutput::AttachmentPoint attachmentType);

    static Qt3DRender::QAbstractTexture *findRenderTargetTexture(Qt3DRender::QRenderTarget *target,
                                                                 Qt3DRender::QRenderTargetOutput::AttachmentPoint attachmentPoint);

private:
    static RenderingFeatures m_renderingFeatures;
};

} // namespace Kuesa

Q_DECLARE_OPERATORS_FOR_FLAGS(Kuesa::FrameGraphUtils::RenderTargetFlags)

QT_END_NAMESPACE

#endif // KUESA_FRAMEGRAPHUTILS_P_H
