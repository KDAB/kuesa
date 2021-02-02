/*
    placeholder.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "placeholder.h"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>

#include <kuesa_utils_p.h>
#include <Qt3DCore/private/qmath3d_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Kuesa/private/logging_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::Placeholder
    \brief Specifies a placeholder which can be used to anchor QtQuick elements.
    \inmodule Kuesa
    \since Kuesa 1.3
    \inherits Qt3DCore::QEntity

    The Placeholder class allows to specify a placeholder to position QtQuick
    elements.

    It is meant to be used only from a glTF 2.0 file exported with
    KDAB_placeholder extension. In other words, you are not supposed to
    instantiate this object on your own but rather rely on retrieving it from
    the \l {Kuesa::PlaceholderCollection} upon loading.

    Read more about \l {Kuesa Placeholders}.

    \sa {Kuesa::PlaceholderTracker}
*/

/*!
    \qmltype Placeholder
    \brief Specifies a placeholder which can be used to anchor QtQuick elements.
    \inqmlmodule Kuesa
    \since Kuesa 1.3
    \instantiates Kuesa::Placeholder

    The Placeholder class allows to specify a placeholder to position QtQuick
    elements.

    It is meant to be used only from a glTF 2.0 file exported with
    KDAB_placeholder extension. In other words, you are not supposed to
    instantiate this object on your own but rather rely on retrieving it from
    the \l {Kuesa::PlaceholderCollection} upon loading.

    Read more about \l {Kuesa Placeholders}.

    \sa {Kuesa::PlaceholderTracker}
*/
Placeholder::Placeholder(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_transform(new Qt3DCore::QTransform)
{

    // Add a transform so we can pick the world transform of this entity
    addComponent(m_transform);

    QObject::connect(m_transform, &Qt3DCore::QTransform::worldMatrixChanged,
                     this, &Placeholder::updatePlaceholder);
}

void Placeholder::updatePlaceholder()
{

}

} // namespace Kuesa

QT_END_NAMESPACE
