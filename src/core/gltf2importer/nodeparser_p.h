/*
    nodeparser_p.h

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

#ifndef KUESA_GLTF2IMPORT_NODEPARSER_P_H
#define KUESA_GLTF2IMPORT_NODEPARSER_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QtCore/qglobal.h>
#include <QtCore/QString>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtGui/QMatrix4x4>

QT_BEGIN_NAMESPACE

class QJsonArray;
namespace Qt3DCore {
class QEntity;
class QJoint;
} // namespace Qt3DCore

namespace Kuesa {
namespace GLTF2Import {

class GLTF2Context;

struct TreeNode {
    Qt3DCore::QEntity *entity = nullptr;
    QVector<Qt3DCore::QJoint *> joints;

    struct TransformInfo {
        enum TransformBit {
            None = 0x0,
            MatrixSet = 0x1,
            ScaleSet = 0x2,
            RotationSet = 0x4,
            TranslationSet = 0x8
        };
        Q_DECLARE_FLAGS(TransformBits, TransformBit)

        QMatrix4x4 matrix;
        QVector3D scale3D;
        QQuaternion rotation;
        QVector3D translation;
        TransformBits bits = 0x0;
    };

    QString name;
    TransformInfo transformInfo;
    qint32 meshIdx = -1;
    qint32 skinIdx = -1;
    qint32 cameraIdx = -1;
    QVector<int> childrenIndices;
    QVector<int> layerIndices;
    QVector<float> morphTargetWeights;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TreeNode::TransformInfo::TransformBits)

class Q_AUTOTEST_EXPORT NodeParser
{
public:
    NodeParser();

    bool parse(const QJsonArray &nodes, GLTF2Context *context) const;
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_NODEPARSER_P_H
