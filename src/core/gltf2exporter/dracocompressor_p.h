/*
    dracocompressor_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Jose Casafranca <juan.casafranca@kdab.com>

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

#ifndef KUESA_GLTF2EXPORTER_DRACOCOMPRESSOR_P_H
#define KUESA_GLTF2EXPORTER_DRACOCOMPRESSOR_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <Kuesa/private/draco_prefix_p.h>
#include <qtkuesa-config.h>
#include <draco/compression/encode.h>
#include <memory>
#include <vector>
#include <QString>

namespace draco {
class EncoderBuffer;
}

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QGeometry;
}
namespace Kuesa {
class GLTF2ExportConfiguration;
namespace DracoCompression {

struct CompressedMesh {
    std::unique_ptr<draco::EncoderBuffer> buffer;
    std::vector<std::pair<QString, int>> attributes;
};

CompressedMesh compressMesh(const Qt3DRender::QGeometry &geometry, const GLTF2ExportConfiguration &);
} // namespace DracoCompression
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2EXPORTER_DRACOCOMPRESSOR_H
