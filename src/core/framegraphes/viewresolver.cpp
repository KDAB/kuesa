/*
    viewresolver.cpp

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

#include "viewresolver_p.h"
#include <QUrl>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qnodepthmask.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qlayerfilter.h>

#include <Kuesa/fullscreenquad.h>
#include <Kuesa/view.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/effectsstages_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

ViewResolver::ViewResolver(Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(parent)
    , m_material(new Qt3DRender::QMaterial(this))
    , m_sourceTextureParameter(new Qt3DRender::QParameter(this))
    , m_shader(new Qt3DRender::QShaderProgram())
{
    m_sourceTextureParameter->setName(QStringLiteral("source"));

    Qt3DRender::QEffect *effect = new Qt3DRender::QEffect();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    Qt3DRender::QTechnique *rhiTechnique = new Qt3DRender::QTechnique();
    rhiTechnique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::RHI);
    rhiTechnique->graphicsApiFilter()->setMajorVersion(1);
    rhiTechnique->graphicsApiFilter()->setMinorVersion(0);
    rhiTechnique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::NoProfile);

    Qt3DRender::QRenderPass *pass = new Qt3DRender::QRenderPass();

    m_shader->setShaderCode(Qt3DRender::QShaderProgram::Vertex,
                            Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl45/fullscreen.vert"))));
    m_shader->setShaderCode(Qt3DRender::QShaderProgram::Fragment,
                            Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl45/fboresolver.frag"))));

    Qt3DRender::QDepthTest *depthTest = new Qt3DRender::QDepthTest();
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Always);
    pass->addRenderState(new Qt3DRender::QNoDepthMask());
    pass->addRenderState(depthTest);

    pass->setShaderProgram(m_shader);
    rhiTechnique->addRenderPass(pass);
    effect->addTechnique(rhiTechnique);
#endif

    m_material->setEffect(effect);
    m_material->addParameter(m_sourceTextureParameter);

    m_fsQuad = new FullScreenQuad(m_material, this);

    Qt3DRender::QLayerFilter *layerFilter = new Qt3DRender::QLayerFilter(this);
    layerFilter->addLayer(m_fsQuad->layer());
}

void ViewResolver::setView(View *view)
{
    if (view == m_view)
        return;

    auto d = Qt3DCore::QNodePrivate::get(this);
    if (m_view) {
        d->unregisterDestructionHelper(m_view);
        m_view->disconnect();
    }

    m_view = view;

    if (m_view) {
        d->registerDestructionHelper(view, &ViewResolver::setView, view);
        QObject::connect(m_view, &View::viewportRectChanged, this, [this] {
            m_fsQuad->setViewportRect(m_view->viewportRect());
        });
        QObject::connect(m_view, &View::frameGraphTreeReconfigured, this, [this] {
            m_sourceTextureParameter->setValue(QVariant::fromValue(m_view->m_internalFXStages->finalColorTexture()));
        });
        m_fsQuad->setViewportRect(m_view->viewportRect());
        m_sourceTextureParameter->setValue(QVariant::fromValue(m_view->m_internalFXStages->finalColorTexture()));
    }
}

View *ViewResolver::view() const
{
    return m_view;
}

} // namespace Kuesa

QT_END_NAMESPACE
