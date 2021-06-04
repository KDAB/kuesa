/*
    gltf2parser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <Kuesa/private/kuesa_global_p.h>
#include <QtCore/qglobal.h>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <Qt3DCore/QEntity>
#include <QThread>
#include <QPointer>
#include "gltf2context_p.h"

class tst_GLTFExporter;
class tst_GLTFParser;

QT_BEGIN_NAMESPACE

class QJsonArray;

namespace Qt3DCore {
class QSkeleton;
} // namespace Qt3DCore

namespace Qt3DAnimation {
class QAnimationClip;
class QChannelMapper;
} // namespace Qt3DAnimation

namespace Kuesa {
class SceneEntity;
class AbstractAssetCollection;
class MorphController;
class GLTF2Material;

namespace GLTF2Import {

struct TreeNode;
struct Mesh;
struct Primitive;
class PrimitiveBuilder;

struct HierarchyNode {
    int nodeIdx = -1;
    HierarchyNode *parent = nullptr;
    QVector<HierarchyNode *> children;
    bool hasJoints = false;
};

class SceneRootEntity : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit SceneRootEntity(const QVector<Qt3DCore::QEntity *> rootNodes,
                             Qt3DCore::QNode *parent = nullptr)
        : Qt3DCore::QEntity(parent)
        , m_rootNodes(rootNodes)
    {
        for (Qt3DCore::QEntity *e : qAsConst(m_rootNodes))
            e->setParent(this);
    }

    void makeActive()
    {
        for (Qt3DCore::QEntity *e : qAsConst(m_rootNodes))
            e->setParent(this);
    }

private:
    QVector<Qt3DCore::QEntity *> m_rootNodes;
};

class KUESA_PRIVATE_EXPORT ParseWorker : public QObject
{
    Q_OBJECT
public:
    explicit ParseWorker(GLTF2Parser *parser, const std::function<bool(GLTF2Parser *)> &parseFunc);
    ~ParseWorker();

public Q_SLOTS:
    bool work();

Q_SIGNALS:
    void workDone(bool result);

private:
    QPointer<GLTF2Parser> m_parser;
    std::function<bool(GLTF2Parser *)> m_func;
};

using KeyParserFuncPair = QPair<QLatin1String, std::function<bool(const QJsonValue &)>>;
class KUESA_PRIVATE_EXPORT GLTF2Parser
    : public QObject
{
    Q_OBJECT
public:
    GLTF2Parser(SceneEntity *sceneEntity = nullptr, bool assignNames = false);
    virtual ~GLTF2Parser();

    virtual QVector<KeyParserFuncPair> prepareParsers();
    bool parse(const QString &filePath, bool async = false);
    bool parse(const QByteArray &data, const QString &basePath, const QString &filename = {}, bool async = false);

    void generateContent();

    void setContext(GLTF2Context *);
    const GLTF2Context *context() const;

    Qt3DCore::QEntity *contentRoot() const;
    QVector<SceneRootEntity *> sceneRoots() const;

signals:
    void gltfFileParsingCompleted(bool parsingSucceeded);

private:
    QThread m_workerThread;

    bool performParsing(const std::function<bool(GLTF2Parser *)> &parsingFunc, bool async);
    bool doParse(const QString &filePath);
    bool doParse(const QByteArray &data, const QString &basePath, const QString &filename = {});
    bool isBinaryGLTF(const QByteArray &data, bool &isValid);
    friend class ::tst_GLTFExporter;
    friend class ::tst_GLTFParser;
    bool detectTypeAndParse(const QByteArray &data, const QString &basePath, const QString &filename = {});
    bool parseJSON(const QByteArray &jsonData, const QString &basePath, const QString &filename = {});
    bool parseBinary(const QByteArray &data, const QString &basePath, const QString &filename = {});

    void addResourcesToSceneEntityCollections();

    void buildSceneRootEntities();
    void buildEntitiesAndJointsGraph();
    void buildJointHierarchy(const HierarchyNode *node, int &jointAccessor, const Skin &skin, int skinIdx, Qt3DCore::QJoint *parentJoint = nullptr);
    void generateTreeNodeContent();
    void generateSkeletonContent();
    void generateAnimationContent();

    void createTransform(const TreeNode &node);
    void createPlaceholder(const TreeNode &node);
    void createLayers(const TreeNode &node);
    void createLight(const TreeNode &node);
    void createMesh(const TreeNode &node);
    void createReflectionPlane(TreeNode &node);

    void createSkin(const TreeNode &node,
                    Qt3DCore::QArmature **armaturePtr,
                    Qt3DCore::QEntity **skinRootJointEntityPtr);
    MorphController *createMorphTarget(const TreeNode &node);
    GLTF2Material *createMaterial(const Mesh &meshData,
                                  const Primitive &primitiveData,
                                  bool isSkinned,
                                  bool hasMorphTarget);

    template<class T>
    void updateDataForJointsAttr(Qt3DGeometry::QAttribute *attr, int skinId);

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

    QString m_basePath;
    GLTF2Context *m_context;
    SceneEntity *m_sceneEntity;
    QVector<SceneRootEntity *> m_sceneRootEntities;
    Qt3DCore::QEntity *m_contentRootEntity;
    int m_defaultSceneIdx;
    bool m_assignNames;
    QVector<QHash<int, int>> m_gltfJointIdxToSkeletonJointIdxPerSkeleton;

    friend class ParseWorker;
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_GLTF2PARSER_P_H
