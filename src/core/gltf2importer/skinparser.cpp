/*
    skinparser.cpp

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

#include "skinparser_p.h"
#include <kuesa_p.h>
#include <gltf2context_p.h>

#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;
using namespace Qt3DGeometry;

namespace {

const QLatin1String KEY_JOINTS = QLatin1String("joints");
const QLatin1String KEY_INVERSE_BIND_MATRICES = QLatin1String("inverseBindMatrices");
const QLatin1String KEY_SKELETON = QLatin1String("skeleton");
const QLatin1String KEY_NAME = QLatin1String("name");

} // namespace

bool SkinParser::parse(const QJsonArray &skinsArray, GLTF2Context *context) const
{
    const qint32 nbSkins = skinsArray.size();
    for (const QJsonValue &skinValue : skinsArray) {
        const QJsonObject skinObj = skinValue.toObject();

        const QJsonArray jointsArray = skinObj.value(KEY_JOINTS).toArray();

        // Joints is the only required property for a skin
        if (jointsArray.size() < 1) {
            qCWarning(kuesa, "A skin must define an array of joint indices with a least one entry");
            return false;
        }

        QVector<qint32> jointsIdx;
        jointsIdx.reserve(jointsArray.size());
        for (const QJsonValue &jointValue : jointsArray) {
            const qint32 nodeIdx = jointValue.toInt(-1);
            if (nodeIdx < 0 || nodeIdx > context->treeNodeCount()) {
                qCWarning(kuesa, "Skin joints references invalid node");
                return false;
            }
            if (jointsIdx.contains(nodeIdx)) {
                qCWarning(kuesa) << "Skin joints references" << jointsIdx << "more than once";
                return false;
            }
            jointsIdx.push_back(nodeIdx);
        }

        Skin skin;
        skin.jointsIndices = jointsIdx;
        skin.name = skinObj.value(KEY_NAME).toString();

        // Defaults to scene root if undefined
        const QJsonValue skeletonValue = skinObj.value(KEY_SKELETON);
        if (!skeletonValue.isUndefined()) {
            const qint32 skeletonIdx = skeletonValue.toInt(-1);
            if (skeletonIdx < 0 || skeletonIdx > context->treeNodeCount()) {
                qCWarning(kuesa, "Skin skeleton references invalid node");
                return false;
            }
            skin.skeletonIdx = skeletonIdx;
        }

        const QJsonValue inverseBindMatricesValue = skinObj.value(KEY_INVERSE_BIND_MATRICES);
        if (!inverseBindMatricesValue.isUndefined()) {
            const qint32 inverseBindMatricesIdx = inverseBindMatricesValue.toInt(-1);
            if (inverseBindMatricesIdx < 0 || inverseBindMatricesIdx > context->accessorCount()) {
                qCWarning(kuesa, "Skin inverseBindMatrices references invalid accessor");
                return false;
            }
            skin.inverseBindMatricesAccessorIdx = inverseBindMatricesIdx;

            const Accessor &accessor = context->accessor(inverseBindMatricesIdx);

            if (accessor.count != static_cast<quint32>(skin.jointsIndices.size())) {
                qCWarning(kuesa, "InverseBindMatrix Accessor's count differs from number of joints");
                return false;
            }
            if (accessor.dataSize != 16 || accessor.type != QAttribute::Float) {
                qCWarning(kuesa, "InverseBindMatrix Accessor types or datasize inappropriate");
                return false;
            }

            const BufferView &bufferViewData = context->bufferView(accessor.bufferViewIndex);
            const quint32 byteOffset = accessor.offset;
            const qint8 elemByteSize = sizeof(float);
            const quint32 byteStride = (!bufferViewData.bufferData.isEmpty() && bufferViewData.byteStride > 0 ? bufferViewData.byteStride : accessor.dataSize * elemByteSize);

            if (byteStride < accessor.dataSize * elemByteSize) {
                qCWarning(kuesa, "InverseBindMatrix Buffer data byteStride doesn't match accessor dataSize and byte size for type");
                return false;
            }

            // bufferData was generated using the bufferView's byteOffset applied
            const QByteArray bufferData = accessor.bufferData;

            if (byteOffset + accessor.count * byteStride > static_cast<quint32>(bufferData.size())) {
                qCWarning(kuesa, "InverseBindMatrix Buffer data is too small");
                return false;
            }

            const char *rawBytes = bufferData.constData() + byteOffset;
            for (quint32 i = 0; i < accessor.count; ++i) {
                QMatrix4x4 inverseBindMatrix;
                memcpy(inverseBindMatrix.data(), reinterpret_cast<const float *>(rawBytes), sizeof(float) * 16);
                rawBytes += byteStride;
                skin.inverseBindMatrices.push_back(inverseBindMatrix);
            }
        }

        context->addSkin(skin);
    }
    return nbSkins > 0;
}

QT_END_NAMESPACE
