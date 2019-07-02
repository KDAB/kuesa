/*
    morphcontroller.cpp

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "morphcontroller.h"
#include <algorithm>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::MorphController
    \inheaderfile Kuesa/MorphController
    \inherits Qt3DRender::QShaderData
    \inmodule Kuesa
    \since Kuesa 1.1

    \brief Kuesa::MorphController allows to control the morph target weights of
    an entity. Though the glTF 2.0 specifications do not impose a limit on the
    number of weights, we have limited it to 8.

    The weight values are set to 0.0f by default.

    \note Given this class subclasses Qt3DRender::QShaderData, its properties
    can be exposed as uniforms to a material.
 */

/*!
    \property MorphController::count

    Holds the actual number of weights being handled by the MorphController.
 */

/*!
    \property MorphController::weight0

    Specifies the weight value at index 0.
 */

/*!
    \property MorphController::weight1

    Specifies the weight value at index 1.
 */

/*!
    \property MorphController::weight2

    Specifies the weight value at index 2.
 */

/*!
    \property MorphController::weight3

    Specifies the weight value at index 3.
 */

/*!
    \property MorphController::weight4

    Specifies the weight value at index 4.
 */

/*!
    \property MorphController::weight5

    Specifies the weight value at index 5.
 */

/*!
    \property MorphController::weight6

    Specifies the weight value at index 6.
 */

/*!
    \property MorphController::weight7

    Specifies the weight value at index 7.
 */

/*!
    \property MorphController::morphWeights
    \internal

    Used by the animation aspect to set all weights at once.
 */

/*!
    \qmltype MorphController
    \instantiates Kuesa::MorphController
    \inqmlmodule Kuesa
    \inherits Qt3DRender::QShaderData
    \since Kuesa 1.1

    \brief Kuesa::MorphController allows to control the morph target weights of
    an entity. Though the glTF 2.0 specifications do not impose a limit on the
    number of weights, we have limited it to 8.

    The weight values are set to 0.0f by default.

    \note Given this class subclasses Render.ShaderData, its properties can be
    exposed as uniforms to a material.
 */

/*!
    \qmlproperty int MorphController::count

    Holds the actual number of weights being handled by the MorphController.
 */

/*!
    \qmlproperty real MorphController::weight0

    Specifies the weight value at index 0.
 */

/*!
    \qmlproperty real MorphController::weight1

    Specifies the weight value at index 1.
 */

/*!
    \qmlproperty real MorphController::weight2

    Specifies the weight value at index 2.
 */

/*!
    \qmlproperty real MorphController::weight3

    Specifies the weight value at index 3.
 */

/*!
    \qmlproperty real MorphController::weight4

    Specifies the weight value at index 4.
 */

/*!
    \qmlproperty real MorphController::weight5

    Specifies the weight value at index 5.
 */

/*!
    \qmlproperty real MorphController::weight6

    Specifies the weight value at index 6.
 */

/*!
    \qmlproperty real MorphController::weight7

    Specifies the weight value at index 7.
 */

/*!
    \qmlproperty variantlist MorphController::morphWeights
    \internal

    Used by the animation aspect to set all weights at once.
 */

MorphController::MorphController(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_count(0)
    , m_shouldEmitMorphWeights(true)
{
    std::fill(std::begin(m_weights), std::end(m_weights), 0.0f);

    auto shouldEmitMorphWeightsChanged = [this]() {
        if (m_shouldEmitMorphWeights)
            emit morphWeightsChanged();
    };

    QObject::connect(this, &MorphController::weight0Changed,
                     this, shouldEmitMorphWeightsChanged);
    QObject::connect(this, &MorphController::weight1Changed,
                     this, shouldEmitMorphWeightsChanged);
    QObject::connect(this, &MorphController::weight2Changed,
                     this, shouldEmitMorphWeightsChanged);
    QObject::connect(this, &MorphController::weight3Changed,
                     this, shouldEmitMorphWeightsChanged);
    QObject::connect(this, &MorphController::weight4Changed,
                     this, shouldEmitMorphWeightsChanged);
    QObject::connect(this, &MorphController::weight5Changed,
                     this, shouldEmitMorphWeightsChanged);
    QObject::connect(this, &MorphController::weight6Changed,
                     this, shouldEmitMorphWeightsChanged);
    QObject::connect(this, &MorphController::weight7Changed,
                     this, shouldEmitMorphWeightsChanged);
}

MorphController::~MorphController()
{
}

float MorphController::weight0() const
{
    return m_weights[0];
}

float MorphController::weight1() const
{
    return m_weights[1];
}

float MorphController::weight2() const
{
    return m_weights[2];
}

float MorphController::weight3() const
{
    return m_weights[3];
}

float MorphController::weight4() const
{
    return m_weights[4];
}

float MorphController::weight5() const
{
    return m_weights[5];
}

float MorphController::weight6() const
{
    return m_weights[6];
}

float MorphController::weight7() const
{
    return m_weights[7];
}

int MorphController::count() const
{
    return m_count;
}

QVariantList MorphController::morphWeights() const
{
    QVariantList weights;
    weights.reserve(m_count);

    for (int i = 0; i < m_count; ++i)
        weights.push_back(m_weights[i]);

    return weights;
}

void MorphController::setWeight0(float weight0)
{
    if (weight0 == m_weights[0])
        return;
    m_weights[0] = weight0;
    emit weight0Changed(weight0);
}

void MorphController::setWeight1(float weight1)
{
    if (weight1 == m_weights[1])
        return;
    m_weights[1] = weight1;
    emit weight1Changed(weight1);
}

void MorphController::setWeight2(float weight2)
{
    if (weight2 == m_weights[2])
        return;
    m_weights[2] = weight2;
    emit weight2Changed(weight2);
}

void MorphController::setWeight3(float weight3)
{
    if (weight3 == m_weights[3])
        return;
    m_weights[3] = weight3;
    emit weight3Changed(weight3);
}

void MorphController::setWeight4(float weight4)
{
    if (weight4 == m_weights[4])
        return;
    m_weights[4] = weight4;
    emit weight4Changed(weight4);
}

void MorphController::setWeight5(float weight5)
{
    if (weight5 == m_weights[5])
        return;
    m_weights[5] = weight5;
    emit weight5Changed(weight5);
}

void MorphController::setWeight6(float weight6)
{
    if (weight6 == m_weights[6])
        return;
    m_weights[6] = weight6;
    emit weight6Changed(weight6);
}

void MorphController::setWeight7(float weight7)
{
    if (weight7 == m_weights[7])
        return;
    m_weights[7] = weight7;
    emit weight7Changed(weight7);
}

void MorphController::setCount(int count)
{
    Q_ASSERT(count <= 8);
    if (count == m_count)
        return;
    m_count = count;
    emit countChanged(count);
}

void MorphController::setMorphWeights(const QVariantList &morphWeights)
{
    using WeightSetterFunc = void (MorphController::*)(float);
    const WeightSetterFunc weightSetterFuncs[]{
        &MorphController::setWeight0,
        &MorphController::setWeight1,
        &MorphController::setWeight2,
        &MorphController::setWeight3,
        &MorphController::setWeight4,
        &MorphController::setWeight5,
        &MorphController::setWeight6,
        &MorphController::setWeight7,
    };

    m_shouldEmitMorphWeights = false;
    for (int i = 0, m = std::min(morphWeights.size(), m_count); i < m; ++i) {
        const float w = morphWeights.at(i).toFloat();
        (this->*weightSetterFuncs[i])(w);
    }

    emit morphWeightsChanged();
    m_shouldEmitMorphWeights = true;
}

} // namespace Kuesa

QT_END_NAMESPACE
