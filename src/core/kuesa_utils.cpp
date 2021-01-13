/*
    kuesa_utils.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#include "kuesa_utils_p.h"
#include <QAspectEngine>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/framegraphnode_p.h>

#include <Kuesa/forwardrenderer.h>
QT_BEGIN_NAMESPACE
namespace Kuesa {
namespace Utils {

Qt3DRender::Render::AbstractRenderer *getRenderer(Qt3DCore::QScene *scene)
{
    auto engine = scene->engine();
    auto aspects = engine->aspects();
    auto renderAspectIt = std::find_if(aspects.cbegin(), aspects.cend(), [](Qt3DCore::QAbstractAspect *aspect) {
        return qobject_cast<Qt3DRender::QRenderAspect *>(aspect);
    });

    Qt3DRender::QRenderAspect *const renderAspect = renderAspectIt == aspects.end() ? nullptr : qobject_cast<Qt3DRender::QRenderAspect *>(*renderAspectIt);
    Q_ASSERT(renderAspect);
    const auto *rap = static_cast<Qt3DRender::QRenderAspectPrivate *>(Qt3DRender::QRenderAspectPrivate::get(renderAspect));
    Q_ASSERT(rap->m_renderer);
    return rap->m_renderer;
}

Kuesa::ForwardRenderer *findForwardRenderer(Qt3DCore::QNode *nodeInScene)
{
    auto scene = Qt3DCore::QNodePrivate::get(nodeInScene)->scene();
    if (!scene)
        return nullptr;

    const auto renderer = getRenderer(scene);
    Qt3DRender::QFrameGraphNode *fg = qobject_cast<Qt3DRender::QFrameGraphNode *>(scene->lookupNode(renderer->frameGraphRoot()->peerId()));
    if (fg) {
        Kuesa::ForwardRenderer *fR = qobject_cast<Kuesa::ForwardRenderer *>(fg);
        if (fR)
            return fR;
        return fg->findChild<Kuesa::ForwardRenderer *>();
    }
    return nullptr;
}

Qt3DRender::Render::Entity *findBackendRootEntity(Qt3DCore::QNode *nodeInScene)
{
    auto scene = Qt3DCore::QNodePrivate::get(nodeInScene)->scene();
    if (!scene)
        return nullptr;

    const auto renderer = getRenderer(scene);
    return renderer->sceneRoot();
}

} // namespace Utils
} // namespace Kuesa
QT_END_NAMESPACE
