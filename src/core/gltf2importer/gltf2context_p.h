/*
    gltf2context_p.h

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

#ifndef KUESA_GLTF2IMPORT_GLTF2CONTEXT_P_H
#define KUESA_GLTF2IMPORT_GLTF2CONTEXT_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QVector>
#include <QJsonDocument>
#include <Kuesa/private/kuesa_global_p.h>
#include "bufferparser_p.h"
#include "bufferviewsparser_p.h"
#include "cameraparser_p.h"
#include "nodeparser_p.h"
#include "bufferaccessorparser_p.h"
#include "meshparser_p.h"
#include "layerparser_p.h"
#include "lightparser_p.h"
#include "imageparser_p.h"
#include "texturesamplerparser_p.h"
#include "textureparser_p.h"
#include "animationparser_p.h"
#include "sceneparser_p.h"
#include "materialparser_p.h"
#include "skinparser_p.h"

#include "gltf2options.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QLayer;
}

namespace Kuesa {

class GLTF2Importer;
class EffectsLibrary;
class SceneEntity;

namespace GLTF2Import {

class KUESA_PRIVATE_EXPORT GLTF2Context
{
public:
    GLTF2Context();
    ~GLTF2Context();

    template<typename Asset>
    int count() const
    {
        return 0;
    }

    template<typename Asset>
    Asset assetAt(qint32) const
    {
        return {};
    }

    int bufferCount() const;
    const QByteArray buffer(qint32 id) const;
    void addBuffer(const QByteArray &buffer);

    int bufferViewCount() const;
    const BufferView bufferView(qint32 id) const;
    void addBufferView(const BufferView &bufferView);

    int cameraCount() const;
    const Camera camera(qint32 id) const;
    void addCamera(const Camera &camera);

    int accessorCount() const;
    void addAccessor(const Accessor &accessor);
    const Accessor accessor(qint32 id) const;

    int meshesCount() const;
    void addMesh(const Mesh &mesh);
    const Mesh mesh(int id) const;

    int treeNodeCount() const;
    void addTreeNode(const TreeNode &treeNode);
    const TreeNode treeNode(qint32 id) const;
    TreeNode &treeNode(qint32 id);
    const QVector<TreeNode> &treeNodes() const;

    int layersCount() const;
    void addLayer(const Layer &layer);
    Layer layer(qint32 id) const;

    int imagesCount() const;
    void addImage(const Image &image);
    const Image image(qint32 id) const;

    int textureSamplersCount() const;
    void addTextureSampler(const TextureSampler &textureSampler);
    const TextureSampler textureSampler(qint32 id) const;

    int texturesCount() const;
    void addTexture(const Texture &texture);
    const Texture texture(qint32 id) const;

    int animationsCount() const;
    void addAnimation(const Animation &animation);
    const Animation animation(qint32 id) const;
    Animation &animation(qint32 id);

    int scenesCount() const;
    void addScene(const Scene &scene);
    const Scene scene(qint32 id) const;

    void setDefaultScene(qint32 id);
    qint32 defaultScene() const;

    int materialsCount() const;
    void addMaterial(const Material &material);
    const Material material(qint32 id) const;
    Material &material(qint32 id);

    int skinsCount() const;
    void addSkin(const Skin &skin);
    const Skin skin(qint32 id) const;
    Skin &skin(qint32 id);

    int lightCount() const;
    void addLight(const Light &light);
    const Light light(qint32 id) const;
    Light &light(qint32 id);

    QStringList usedExtension() const;
    void setUsedExtensions(const QStringList &usedExtensions);
    QStringList requiredExtensions() const;
    void setRequiredExtensions(const QStringList &requiredExtensions);

    const QString &filename() const;
    void setFilename(const QString &);

    const QString &basePath() const;
    void setBasePath(const QString &);

    const QJsonDocument &json() const;
    void setJson(const QJsonDocument &doc);

    const QStringList &localFiles() const;
    void addLocalFile(const QString &file);

    QByteArray bufferChunk() const;
    void setBufferChunk(const QByteArray &bufferChunk);

    Kuesa::GLTF2Import::GLTF2Options *options();
    const Kuesa::GLTF2Import::GLTF2Options *options() const;

    void reset(Kuesa::SceneEntity *sceneEntity);

    static GLTF2Context *fromImporter(GLTF2Importer *importer);

    EffectsLibrary *effectLibrary() const;

    QVector<Qt3DCore::QEntity *> primitiveEntitiesForEntity(Qt3DCore::QEntity *e) const;
    void addPrimitiveEntityToEntity(Qt3DCore::QEntity *e, Qt3DCore::QEntity *primitive);

private:
    QVector<Accessor> m_accessors;
    QVector<QByteArray> m_buffers;
    QVector<BufferView> m_bufferViews;
    QVector<Camera> m_cameras;
    QVector<Mesh> m_meshes;
    QVector<TreeNode> m_treeNodes;
    QVector<Layer> m_layers;
    QVector<Image> m_images;
    QVector<TextureSampler> m_textureSamplers;
    QVector<Texture> m_textures;
    QVector<Animation> m_animations;
    QVector<Scene> m_scenes;
    QVector<Material> m_materials;
    QVector<Skin> m_skins;
    QVector<Light> m_lights;
    QStringList m_usedExtensions;
    QStringList m_requiredExtensions;
    QString m_filename;
    QString m_basePath;
    QJsonDocument m_json;
    QStringList m_localFiles;
    QByteArray m_bufferChunk;

    Kuesa::GLTF2Import::GLTF2Options m_options;
    qint32 m_defaultScene;
    QScopedPointer<EffectsLibrary> m_effectLibrary;

    QHash<Qt3DCore::QEntity *, QVector<Qt3DCore::QEntity *>> m_treeNodeIdToPrimitiveEntities;
};

template<>
int GLTF2Context::count<Mesh>() const;

template<>
Mesh GLTF2Context::assetAt<Mesh>(int i) const;

template<>
int GLTF2Context::count<Layer>() const;

template<>
Layer GLTF2Context::assetAt<Layer>(int i) const;

template<>
int GLTF2Context::count<TreeNode>() const;

template<>
TreeNode GLTF2Context::assetAt<TreeNode>(int i) const;

template<>
int GLTF2Context::count<Texture>() const;

template<>
Texture GLTF2Context::assetAt<Texture>(int i) const;

template<>
int GLTF2Context::count<Animation>() const;

template<>
Animation GLTF2Context::assetAt<Animation>(int i) const;

template<>
int GLTF2Context::count<Material>() const;

template<>
Material GLTF2Context::assetAt<Material>(int i) const;

template<>
int GLTF2Context::count<Skin>() const;

template<>
Skin GLTF2Context::assetAt<Skin>(int i) const;

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLFTF2IMPORT_GLTF2CONTEXT_P_H
