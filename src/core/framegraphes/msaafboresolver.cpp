/*
    msaafboresolver.cpp

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

#include "msaafboresolver_p.h"
#include <QUrl>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qnodepthmask.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <fullscreenquad.h>


QT_BEGIN_NAMESPACE

namespace Kuesa {

MSAAFBOResolver::MSAAFBOResolver(Qt3DCore::QNode *parent)
    : Qt3DRender::QRenderTargetSelector(parent)
    , m_material(new Qt3DRender::QMaterial(this))
    , m_sourceTextureParameter(new Qt3DRender::QParameter(this))
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
    Qt3DRender::QShaderProgram *shader = new Qt3DRender::QShaderProgram();

    shader->setShaderCode(Qt3DRender::QShaderProgram::Vertex,
                          Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl45/msaaresolver.vert"))));
    shader->setShaderCode(Qt3DRender::QShaderProgram::Fragment,
                          Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/kuesa/shaders/gl45/msaaresolver.frag"))));

    Qt3DRender::QDepthTest *depthTest = new Qt3DRender::QDepthTest();
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Always);
    pass->addRenderState(new Qt3DRender::QNoDepthMask());
    pass->addRenderState(depthTest);

    pass->setShaderProgram(shader);
    rhiTechnique->addRenderPass(pass);
    effect->addTechnique(rhiTechnique);
#endif

    m_material->setEffect(effect);
    m_material->addParameter(m_sourceTextureParameter);

    FullScreenQuad *fsQuad = new FullScreenQuad(m_material, this);

    Qt3DRender::QLayerFilter *layerFilter = new Qt3DRender::QLayerFilter(this);
    layerFilter->addLayer(fsQuad->layer());
}

void MSAAFBOResolver::setSource(Qt3DRender::QAbstractTexture *source)
{
    m_sourceTextureParameter->setValue(QVariant::fromValue(source));
}

void MSAAFBOResolver::setDestination(Qt3DRender::QRenderTarget *destination)
{
    setTarget(destination);
}

} // namespace Kuesa
QT_END_NAMESPACE
