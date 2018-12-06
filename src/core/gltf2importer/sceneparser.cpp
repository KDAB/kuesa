/*
    sceneparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sceneparser_p.h"
#include "gltf2context_p.h"
#include "kuesa_p.h"

#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {

const QLatin1String KEY_NODES = QLatin1Literal("nodes");
const QLatin1String KEY_NAME = QLatin1Literal("name");

} // namespace

bool SceneParser::parse(const QJsonArray &scenesArray, GLTF2Context *context) const
{
    const int nbScenes = scenesArray.size();
    for (const QJsonValue &sceneValue : scenesArray) {
        const QJsonObject sceneObj = sceneValue.toObject();

        const QJsonArray nodesArray = sceneObj.value(KEY_NODES).toArray();
        if (nodesArray.size() < 1) {
            qCWarning(kuesa, "A scene must define at least one root node");
            return false;
        }
        QVector<int> rootIndices;
        rootIndices.reserve(nodesArray.size());
        for (const QJsonValue &nodeValue : nodesArray) {
            const int nodeIdx = nodeValue.toInt(-1);
            if (nodeIdx < 0 || nodeIdx > context->treeNodeCount()) {
                qCWarning(kuesa, "Scene root node references invalid node");
                return false;
            }
            if (rootIndices.contains(nodeIdx)) {
                qCWarning(kuesa) << "Scene root node declared more than once";
                return false;
            }
            rootIndices.push_back(nodeIdx);
        }

        Scene scene;
        scene.name = sceneObj.value(KEY_NAME).toString();
        scene.rootNodeIndices = rootIndices;
        context->addScene(scene);
    }
    return nbScenes > 0;
}

QT_END_NAMESPACE
