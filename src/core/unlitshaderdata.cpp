/*
    unlitshaderdata.cpp

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include "unlitshaderdata_p.h"
#include <Qt3DRender/private/qshaderdata_p.h>

QT_BEGIN_NAMESPACE
namespace Kuesa {

UnlitShaderData::UnlitShaderData(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
    , m_baseColorUsesTexCoord1(false)
    , m_baseColorFactor(QColor("gray"))
    , m_baseColorMap(nullptr)
{
}

UnlitShaderData::~UnlitShaderData()
{
}

bool UnlitShaderData::isBaseColorUsingTexCoord1() const
{
    return m_baseColorUsesTexCoord1;
}

QColor UnlitShaderData::baseColorFactor() const
{
    return m_baseColorFactor;
}

Qt3DRender::QAbstractTexture *UnlitShaderData::baseColorMap() const
{
    return m_baseColorMap;
}

float UnlitShaderData::alphaCutoff() const
{
    return m_alphaCutoff;
}

void UnlitShaderData::setBaseColorUsesTexCoord1(bool baseColorUsesTexCoord1)
{
    if (m_baseColorUsesTexCoord1 == baseColorUsesTexCoord1)
        return;
    m_baseColorUsesTexCoord1 = baseColorUsesTexCoord1;
    emit baseColorUsesTexCoord1Changed(baseColorUsesTexCoord1);
}

void UnlitShaderData::setBaseColorFactor(const QColor &baseColorFactor)
{
    if (m_baseColorFactor == baseColorFactor)
        return;
    m_baseColorFactor = baseColorFactor;
    emit baseColorFactorChanged(baseColorFactor);
}

void UnlitShaderData::setBaseColorMap(Qt3DRender::QAbstractTexture *baseColorMap)
{
    if (m_baseColorMap == baseColorMap)
        return;
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_baseColorMap != nullptr)
        d->unregisterDestructionHelper(m_baseColorMap);
    m_baseColorMap = baseColorMap;
    if (m_baseColorMap != nullptr) {
#ifndef QT_OPENGL_ES_2
        m_baseColorMap->setFormat(Qt3DRender::QAbstractTexture::TextureFormat::SRGB8_Alpha8);
#endif
        if (m_baseColorMap->parent() == nullptr)
            m_baseColorMap->setParent(this);
        d->registerDestructionHelper(m_baseColorMap, &UnlitShaderData::setBaseColorMap, m_baseColorMap);
    }
    baseColorMapChanged(baseColorMap);
}

void UnlitShaderData::setAlphaCutoff(float alphaCutoff)
{
    if (m_alphaCutoff == alphaCutoff)
        return;
    m_alphaCutoff = alphaCutoff;
    emit alphaCutoffChanged(alphaCutoff);
}

} // namespace Kuesa

QT_END_NAMESPACE
