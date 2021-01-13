/*
    gltf2options.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "gltf2options.h"

/*!
 * \class Kuesa::GLTF2Import::GLTF2Options
 * \inheaderfile Kuesa/GLTF2Options
 * \inmodule Kuesa
 * \since Kuesa 1.1
 * \brief Different options to configure the glTF 2.0 importer.
 *
 * Avalaible options are
 * \list
 * \li generateTangents: If true, the importer will generate tangents
 * for all the primitives which don't have tangents.
 * \li generateNormals: If true, the importer will generate flat normals
 * for all the primitives which don't have normals.
 * \endlist
 */

/*!
 * \qmltype GLTF2Options
 * \instantiates Kuesa::GLTF2Import::GLTF2Options
 * \inqmlmodule Kuesa
 * \since Kuesa 1.1
 * \brief Different options to configure the glTF 2.0 importer.
 *
 * Avalaible options are
 * \list
 * \li generateTangents: If true, the importer will generate tangents
 * for all the primitives which don't have tangents.
 * \li generateNormals: If true, the importer will generate flat normals
 * for all the primitives which don't have normals.
 * \endlist
 */

QT_BEGIN_NAMESPACE

Kuesa::GLTF2Import::GLTF2Options::GLTF2Options()
    : QObject(nullptr)
    , m_generateTangents(false)
    , m_generateNormals(false)
{
}

bool Kuesa::GLTF2Import::GLTF2Options::generateTangents() const
{
    return m_generateTangents;
}

bool Kuesa::GLTF2Import::GLTF2Options::generateNormals() const
{
    return m_generateNormals;
}

void Kuesa::GLTF2Import::GLTF2Options::setGenerateTangents(bool generateTangents)
{
    if (generateTangents == m_generateTangents)
        return;
    m_generateTangents = generateTangents;
    emit generateTangentsChanged(m_generateTangents);
}

void Kuesa::GLTF2Import::GLTF2Options::setGenerateNormals(bool generateNormals)
{
    if (generateNormals == m_generateNormals)
        return;
    m_generateNormals = generateNormals;
    emit generateNormalsChanged(m_generateNormals);
}

QT_END_NAMESPACE
