
/*
    examplecustommaterialshaderdata.cpp

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

#include "examplecustommaterialshaderdata_p.h"
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Kuesa {

ExampleCustomMaterialShaderData::ExampleCustomMaterialShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_modulator()
    , m_inputTexture(nullptr)
{
}

ExampleCustomMaterialShaderData::~ExampleCustomMaterialShaderData() = default;

float ExampleCustomMaterialShaderData::modulator() const
{
    return m_modulator;
}

Qt3DRender::QAbstractTexture *ExampleCustomMaterialShaderData::inputTexture() const
{
    return m_inputTexture;
}

void ExampleCustomMaterialShaderData::setModulator(float modulator)
{
    if (m_modulator == modulator)
        return;
    m_modulator = modulator;
    emit modulatorChanged(modulator);
}

void ExampleCustomMaterialShaderData::setInputTexture(Qt3DRender::QAbstractTexture *inputTexture)
{
    if (m_inputTexture == inputTexture)
        return;

    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_inputTexture != nullptr)
        d->unregisterDestructionHelper(m_inputTexture);
    m_inputTexture = inputTexture;
    if (m_inputTexture != nullptr) {
        if (m_inputTexture->parent() == nullptr)
            m_inputTexture->setParent(this);
        d->registerDestructionHelper(m_inputTexture, &ExampleCustomMaterialShaderData::setInputTexture, m_inputTexture);
    }
    emit inputTextureChanged(m_inputTexture);
}

} // namespace Kuesa

QT_END_NAMESPACE
