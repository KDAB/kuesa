/*
    fullscreenquad.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "fullscreenquad.h"
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DExtras/qplanemesh.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qcameralens.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
 * \class FullScreenQuad
 * \inheaderfile Kuesa/FullScreenQuad
 * \inmodule Kuesa
 * \since 1.0
 * \brief FullScreenQuad is a convenience Qt3DCore::QEntity subclass
 * use to create a full screen quad.
 *
 * Its useful for creating background which must be rendered independently from the scene
 * or to create screen space effects on an image.
 *
 * A Qt3DRender::QLayer is added to the FullScreenQuad entity which can be retrieved with layer().
 */


/*!
 * \brief FullScreenQuad::FullScreenQuad
 * \param material A material to be aggregated to the quad entity.
 *
 * This material can be used to implement a gradient color background or a screen space effect
 *
 * \param parent The parent node for the quad entity.
 */
FullScreenQuad::FullScreenQuad(Qt3DRender::QMaterial *material, Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
{
    m_layer = new Qt3DRender::QLayer(this);

    auto planeMesh = new Qt3DExtras::QPlaneMesh(this);
    planeMesh->setMirrored(true);
    planeMesh->setWidth(2.0f);
    planeMesh->setHeight(2.0f);

    auto xform = new Qt3DCore::QTransform(this);
    xform->setRotationX(90);

    addComponent(m_layer);
    addComponent(planeMesh);
    addComponent(xform);
    addComponent(material);
}

FullScreenQuad::~FullScreenQuad()
{
}

/*!
 * \brief FullScreenQuad::layer
 * \return The Qt3DRender::QLayer added to this quad entity.
 *
 * This layer can be used to filter render views and separate the quad entity
 * render from the scene render.
 */
Qt3DRender::QLayer* FullScreenQuad::layer() const
{
    return m_layer;
}

} // namespace Kuesa

QT_END_NAMESPACE
