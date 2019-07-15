/*
    fxutils.cpp

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

#include "fxutils_p.h"
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QShaderProgramBuilder>

QT_BEGIN_NAMESPACE

namespace Kuesa {

namespace FXUtils {

/*!
    \internal
 */

Qt3DRender::QTechnique *makeTechnique(Qt3DRender::QGraphicsApiFilter::Api api,
                                      int majorVersion, int minorVersion,
                                      Qt3DRender::QGraphicsApiFilter::OpenGLProfile profile,
                                      const QString &vertexShader,
                                      const QString &fragShaderGraphPath,
                                      const QString &passFilterName, const QString &passFilterValue)
{
    auto shaderBuilder = new Qt3DRender::QShaderProgramBuilder;
    shaderBuilder->setFragmentShaderGraph(QUrl(fragShaderGraphPath));
    return makeTechnique(api, majorVersion, minorVersion, profile, vertexShader, shaderBuilder, passFilterName, passFilterValue);
}

Qt3DRender::QTechnique *makeTechnique(Qt3DRender::QGraphicsApiFilter::Api api,
                                      int majorVersion, int minorVersion,
                                      Qt3DRender::QGraphicsApiFilter::OpenGLProfile profile,
                                      const QString &vertexShader,
                                      Qt3DRender::QShaderProgramBuilder *shaderBuilder,
                                      const QString &passFilterName, const QString &passFilterValue)
{
    auto technique = makeTechnique(api, majorVersion, minorVersion, profile);

    auto renderPass = createRenderPass(passFilterName, passFilterValue);

    auto *shaderProgram = new Qt3DRender::QShaderProgram(renderPass);

    if (!shaderBuilder->parent())
        shaderBuilder->setParent(renderPass);

    shaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(vertexShader)));
    shaderBuilder->setShaderProgram(shaderProgram);
    renderPass->setShaderProgram(shaderProgram);

    technique->addRenderPass(renderPass);

    return technique;
}

Qt3DRender::QTechnique *makeTechnique(Qt3DRender::QGraphicsApiFilter::Api api, int majorVersion, int minorVersion, Qt3DRender::QGraphicsApiFilter::OpenGLProfile profile)
{
    auto technique = new Qt3DRender::QTechnique;

    technique->graphicsApiFilter()->setApi(api);
    technique->graphicsApiFilter()->setMajorVersion(majorVersion);
    technique->graphicsApiFilter()->setMinorVersion(minorVersion);
    technique->graphicsApiFilter()->setProfile(profile);

    auto techniqueFilterKey = new Qt3DRender::QFilterKey;
    techniqueFilterKey->setName(QStringLiteral("renderingStyle"));
    techniqueFilterKey->setValue(QStringLiteral("forward"));
    technique->addFilterKey(techniqueFilterKey);

    return technique;
}

Qt3DRender::QRenderPass *createRenderPass(const QString &passName, const QVariant &passValue)
{
    auto renderPass = new Qt3DRender::QRenderPass;
    auto passFilterKey = new Qt3DRender::QFilterKey;
    passFilterKey->setName(passName);
    passFilterKey->setValue(passValue);
    renderPass->addFilterKey(passFilterKey);
    return renderPass;
}

Qt3DRender::QRenderPassFilter *createRenderPassFilter(const QString &name, const QVariant &value, Qt3DCore::QNode *parent)
{
    auto filter = new Qt3DRender::QRenderPassFilter(parent);
    auto filterKey = new Qt3DRender::QFilterKey;
    filterKey->setName(name);
    filterKey->setValue(value);
    filter->addMatch(filterKey);
    return filter;
}

} // namespace FXUtils

} // namespace Kuesa

QT_END_NAMESPACE
