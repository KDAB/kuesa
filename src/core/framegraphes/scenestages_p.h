/*
    scenestages_p.h

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

#ifndef KUESA_SCENESTAGES_P_H
#define KUESA_SCENESTAGES_P_H

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
#include <QRectF>
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
class QLayer;
} // namespace Qt3DRender

namespace Kuesa {

class AbstractRenderStage;
class OpaqueRenderStage;
class TransparentRenderStage;
class ZFillRenderStage;

class KUESA_PRIVATE_EXPORT SceneStages
{
public:
    SceneStages();
    ~SceneStages();

    SceneStages(const SceneStages &) = delete;

    void init();
    void setZFilling(bool zFilling);
    void setBackToFrontSorting(bool backToFrontSorting);
    void reconfigure(Qt3DRender::QLayer *layer, Qt3DCore::QEntity *camera, QRectF viewport);

    bool zFilling();
    bool backToFrontSorting();
    Qt3DRender::QLayer *layer() const;

    void unParent();
    void setParent(Qt3DCore::QNode *opaqueRoot, Qt3DCore::QNode *transparentRoot);

    Qt3DRender::QLayerFilter *opaqueStagesRoot() const;
    Qt3DRender::QLayerFilter *transparentStagesRoot() const;

    OpaqueRenderStage *opaqueStage() const;
    ZFillRenderStage *zFillStage() const;
    TransparentRenderStage *transparentStage() const;

private:
    struct StageConfiguration {
        Qt3DRender::QLayerFilter *m_root = nullptr;
        Qt3DRender::QCameraSelector *m_cameraSelector = nullptr;
        Qt3DRender::QViewport *m_viewport = nullptr;
    };
    StageConfiguration m_opaqueStagesConfiguration;
    StageConfiguration m_transparentStagesConfiguration;
    OpaqueRenderStage *m_opaqueStage;
    TransparentRenderStage *m_transparentStage;
    ZFillRenderStage *m_zFillStage;
    QMetaObject::Connection m_zFillDestroyedConnection;
};
using SceneStagesPtr = QSharedPointer<SceneStages>;


} // namespace Kuesa
QT_END_NAMESPACE

#endif // KUESA_SCENESTAGES_P_H
