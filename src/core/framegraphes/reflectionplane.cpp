/*
    reflectionplane.cpp

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

#include "reflectionplane.h"
#include <Qt3DRender/QLayer>
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {


/*!
    \class Kuesa::ReflectionPlane
    \brief Specifies a reflection plane for a View of the scene.
    \inmodule Kuesa
    \since Kuesa 1.3
    \inherits Qt3DRender::QFrameGraphNode

    The ReflectionPlane class allows to provide the information required to
    perform planar reflections. It expects a plane equation to be provided.
    Aditionally, a set of visible layers can also be provided to restrict
    what the reflections will display.

    \badcode
    Kuesa::ForwardRenderer *frameGraph = new Kuesa::ForwardRenderer();
    Kuesa::ReflectionPlane *reflectionPlane = new Kuesa::ReflectionPlane();

    reflectionPlane->setEquation(QVector4D(0.0f, 1.0f, 0.0f, 0.0f);

    frameGraph->addReflectionPlane(reflectionPlane);
    \endcode

*/

/*!
    \qmltype ReflectionPlane
    \brief Specifies a reflection plane for a View of the scene.
    \inqmlmodule Kuesa
    \since Kuesa 1.3
    \instantiates Kuesa::ReflectionPlane

    The ReflectionPlane element allows to provide the information required to
    perform planar reflections. It expects a plane equation to be provided.
    Aditionally, a set of visible layers can also be provided to restrict what
    the reflections will display.

    \badcode
    ForwardRenderer {
        reflectionPlanes: [
            ReflectionPlane {
                equation: Qt.vector4d(0.0, 1.0, 0.0, 0.0)
                layers: [ sceneLayer ]
            }
        ]
    }
    \endcode
*/

ReflectionPlane::ReflectionPlane(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
{
}

/*!
    \property Kuesa::ReflectionPlane::equation

    Holds the plane equation as a QVector4D in the form Ax + By + Cz = D
    where the plane normal is (A, B, C).
*/

/*!
    \qmlproperty vector4d Kuesa::ReflectionPlane::equation

    Holds the plane equation as a vector4d in the form Ax + By + Cz = D
    where the plane normal is (A, B, C).
*/
void ReflectionPlane::setEquation(const QVector4D &equation)
{
    if (equation == m_equation)
        return;
    m_equation = equation;
    emit equationChanged();
}

QVector4D ReflectionPlane::equation() const
{
    return m_equation;
}

void ReflectionPlane::addLayer(Qt3DRender::QLayer *layer)
{
    if (layer) {
        // Parent it to the View if they have no parent
        // We want to avoid them ever being parented to the SceneStages
        // As those might be destroyed when rebuilding the FG
        if (!layer->parent())
            layer->setParent(this);

        auto d = Qt3DCore::QNodePrivate::get(this);
        d->registerDestructionHelper(layer, &ReflectionPlane::removeLayer, m_layers);
        m_layers.push_back(layer);
        emit layersChanged();
    }
}

void ReflectionPlane::removeLayer(Qt3DRender::QLayer *layer)
{
    auto it = std::find(std::begin(m_layers), std::end(m_layers), layer);
    if (it != std::end(m_layers)) {
        auto d = Qt3DCore::QNodePrivate::get(this);
        d->unregisterDestructionHelper(layer);
        m_layers.erase(it);
        emit layersChanged();
    }
}


/*!
    \qmlproperty list Kuesa::ReflectionPlane::layers

    The list of visible layers that are used to select what is
    reflected. If no layers are specified, uses the same set of layers as the
    view.
*/

/*!
    Returns the list of visible layers that are used to select what is
    reflected. If no layers are specified, uses the same set of layers as the
    view.
*/
const std::vector<Qt3DRender::QLayer *> &ReflectionPlane::layers() const
{
    return m_layers;
}

} // Kuesa

QT_END_NAMESPACE
