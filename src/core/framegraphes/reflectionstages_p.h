/*
    reflectionstages_p.h

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

#ifndef KUESA_REFLECTIONSTAGES_P_H
#define KUESA_REFLECTIONSTAGES_P_H

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
#include <Kuesa/private/scenestages_p.h>
#include <QRectF>
#include <QMetaObject>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QNode;
class QEntity;
}

namespace Qt3DRender {
class QTechniqueFilter;
class QRenderPassFilter;
class QViewport;
class QCameraSelector;
class QNoDraw;
class QSortPolicy;
class QRenderTarget;
class QBlitFramebuffer;
class QLayerFilter;
class QClearBuffers;
class QFrameGraphNode;
class QLayer;
class QParameter;
} // namespace Qt3DRender

namespace Kuesa {

class AbstractRenderStage;
class OpaqueRenderStage;
class TransparentRenderStage;
class ZFillRenderStage;

class KUESA_PRIVATE_EXPORT ReflectionStages : public SceneStages
{
    Q_OBJECT
public:
    explicit ReflectionStages(Qt3DRender::QFrameGraphNode *parent = nullptr);
    ~ReflectionStages();

    void setReflectivePlaneEquation(const QVector4D &planeEquation);

private:
    void reconfigure(const Features features) override;

    Qt3DRender::QClearBuffers *m_clearDepth;
};
using ReflectionStagesPtr = QSharedPointer<ReflectionStages>;

} // namespace Kuesa
QT_END_NAMESPACE


#endif // KUESA_REFLECTIONSTAGES_P_H
