/*
    gltf2parser_p.h

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

#ifndef KUESA_GLTF2IMPORT_GLTF2PARSER_P_H
#define KUESA_GLTF2IMPORT_GLTF2PARSER_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QtCore/qglobal.h>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <Kuesa/private/gltf2context_p.h>

QT_BEGIN_NAMESPACE

class QJsonArray;

namespace Qt3DCore {
class QEntity;
class QSkeleton;
} // namespace Qt3DCore

namespace Qt3DAnimation {
class QAnimationClip;
class QChannelMapper;
} // namespace Qt3DAnimation

namespace Kuesa {
class SceneEntity;
class AbstractAssetCollection;

namespace GLTF2Import {

struct TreeNode;
struct Mesh;

struct HierarchyNode {
    int nodeIdx = -1;
    HierarchyNode *parent = nullptr;
    QVector<HierarchyNode *> children;
};

using KeyParserFuncPair = QPair<QLatin1String, std::function<bool(const QJsonValue &)>>;

class Q_AUTOTEST_EXPORT GLTF2Parser
{
public:
    GLTF2Parser(SceneEntity *sceneEntity = nullptr, bool assignNames = false);
    virtual ~GLTF2Parser();

    virtual QVector<KeyParserFuncPair> prepareParsers();
    Qt3DCore::QEntity *parse(const QString &filePath);
    Qt3DCore::QEntity *parse(const QByteArray &jsonData, const QString &basePath);

    void setContext(GLTF2Context *);
    const GLTF2Context *context() const;

private:
    void buildEntitiesAndJointsGraph();
    void buildJointHierarchy(const HierarchyNode *node, int &jointAccessor, const Skin &skin, int skinIdx, Qt3DCore::QJoint *parentJoint = nullptr);
    void generateTreeNodeContent();
    void generateSkeletonContent();
    void generateAnimationContent();

    template<class T>
    void updateDataForJointsAttr(Qt3DRender::QAttribute *attr, int skinId);

    template<typename Asset>
    void addAssetsIntoCollection(std::function<void(const Asset &, int)> namedAdd,
                                 std::function<void(const Asset &, int)> unnamedAdd)
    {
        const int m = m_context->count<Asset>();
        for (int i = 0; i < m; ++i) {
            const Asset &asset = m_context->assetAt<Asset>(i);
            if (!asset.name.isEmpty())
                namedAdd(asset, i);
        }
        if (!m_assignNames)
            return;
        for (int i = 0; i < m; ++i) {
            const Asset &asset = m_context->assetAt<Asset>(i);
            if (asset.name.isEmpty())
                unnamedAdd(asset, i);
        }
    }

    struct AnimationDetails {
        Qt3DAnimation::QAnimationClip *clip;
        Qt3DAnimation::QChannelMapper *mapper;
    };

    Qt3DCore::QEntity *scene(const int id);

    QString m_basePath;
    GLTF2Context *m_context;
    QVector<TreeNode> m_treeNodes;
    QVector<Qt3DCore::QSkeleton *> m_skeletons;
    QVector<AnimationDetails> m_animators;
    SceneEntity *m_sceneEntity;
    Qt3DCore::QEntity *m_sceneRootEntity;
    int m_defaultSceneIdx;
    bool m_assignNames;
    QVector<QHash<int, int>> m_gltfJointIdxToSkeletonJointIdxPerSkeleton;
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_GLTF2PARSER_P_H
