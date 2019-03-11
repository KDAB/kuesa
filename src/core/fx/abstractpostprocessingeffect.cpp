/*
    abstractpostprocessingeffect.cpp

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

#include "abstractpostprocessingeffect.h"

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
 * \class AbstractPostProcessingEffect
 *
 * \brief AbstractPostProcessingEffect is the abstract base class for Kuesa
 * post processing effects.
 */

AbstractPostProcessingEffect::AbstractPostProcessingEffect(Qt3DCore::QNode *parent,
                                                           AbstractPostProcessingEffect::Type type)
    : Qt3DCore::QNode(parent)
    , m_type(type)
{
}

AbstractPostProcessingEffect::~AbstractPostProcessingEffect()
{
}

/*!
 * \fn Qt3DCore::QNode *AbstractPostProcessingEffect::frameGraphSubTree() const
 *
 * Returns a FrameGraph subtree corresponding to the effect's implementation.
 *
 * \note The lifetime of the returned subtree is assumed to be managed by the shared
 * pointer. Any caller reparenting the subtree to add to a QFrameGraph will need to
 * unparent it to avoid it being deleted twice.
 */

/*!
 * \fn void AbstractPostProcessingEffect::setInputTexture(Qt3DRender::QAbstractTexture *)
 *
 * Sets the input texture for this effect.  The texture contain the rendered scene that
 * the effect will be applied to. This is set automatically by the ForwardRenderer when the
 * effect is added.
 */

/*!
 * Returns the post processing effect's type.
 */
AbstractPostProcessingEffect::Type AbstractPostProcessingEffect::type() const
{
    return m_type;
}

/*!
 * Sets the size of the rendered scene (in pixels) that the effect will be applied to.
 * This is necessary for effects to render correctly.
 */
void AbstractPostProcessingEffect::setSceneSize(const QSize &size)
{
    Q_UNUSED(size);
}

} // namespace Kuesa
QT_END_NAMESPACE
