/*
    meshparser_utils_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_GLTF2IMPORT_MESHPARSER_UTILS_P_H
#define KUESA_GLTF2IMPORT_MESHPARSER_UTILS_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <Kuesa/kuesa_global.h>
#include <Qt3DRender/QGeometryRenderer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QGeometry;
class QAttribute;
} // namespace Qt3DRender

namespace Kuesa {
namespace GLTF2Import {

class GLTF2Context;

namespace MeshParserUtils {

KUESASHARED_EXPORT void createTangentForGeometry(Qt3DRender::QGeometry *geometry, Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType);
KUESASHARED_EXPORT bool needsTangentAttribute(const Qt3DRender::QGeometry *geometry,
                                              Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType);
KUESASHARED_EXPORT void createNormalsForGeometry(Qt3DRender::QGeometry *geometry,
                                                 Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType);
KUESASHARED_EXPORT bool needsNormalAttribute(const Qt3DRender::QGeometry *geometry,
                                             Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType);
KUESASHARED_EXPORT bool generatePrecomputedTangentAttribute(Qt3DRender::QGeometryRenderer *mesh,
                                                            GLTF2Context *context);
KUESASHARED_EXPORT bool generatePrecomputedNormalAttribute(Qt3DRender::QGeometryRenderer *mesh,
                                                           GLTF2Context *context);
bool geometryIsGLTF2Valid(Qt3DRender::QGeometry *geometry);

} // namespace MeshParserUtils
} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_MESHPARSER_UTILS_P_H
