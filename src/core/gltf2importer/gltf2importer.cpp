/*
    gltf2importer.cpp

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

#include "gltf2importer.h"
#include "gltf2context_p.h"
#include "gltf2parser_p.h"
#include "gltf2options.h"
#include "collections/meshcollection.h"

#include "kuesa_p.h"
#include <Qt3DCore/QEntity>
#include <Kuesa/SceneEntity>

namespace {

QString urlToLocalFileOrQrc(const QUrl &url)
{
    const QString scheme(url.scheme().toLower());
    if (scheme == QLatin1String("qrc")) {
        if (url.authority().isEmpty())
            return QLatin1Char(':') + url.path();
        return QString();
    }

#if defined(Q_OS_ANDROID)
    if (scheme == QLatin1String("assets")) {
        if (url.authority().isEmpty())
            return url.toString();
        return QString();
    }
#endif

    return url.toLocalFile();
}

} // namespace

QT_BEGIN_NAMESPACE
using namespace Kuesa;

/*!
 * \class Kuesa::GLTF2Importer
 * \inheaderfile Kuesa/GLTF2Importer
 * \inmodule Kuesa
 * \inherits KuesaNode
 * \since Kuesa 1.0
 * \brief Imports glTF 2 scenes into a Qt 3D Scene.
 *
 * GLTF2Importer imports glTF 2 scenes into a Qt 3D scene.
 *
 * \badcode
 * #include <GLTF2Importer>
 *
 * Kuesa::SceneEntity *scene = new Kuesa::SceneEntity();
 * Kuesa::GLTF2Importer *importer = new Kuesa::GLTF2Importer(scene);
 *
 * importer->setSceneEntity(scene);
 * importer->setSource(QUrl("file:///source.gltf"));
 * \endcode
 *
 * If a Kuesa::SceneEntity has been set on the importer, various Qt 3D
 * resources generated upon import will be registered into named collections.
 * If no name is specified and assignNames is true, a default name will be
 * assigned to the asset.
 * If several assets of the same type share the same name, subsequent assets
 * will be given unique names by appending an index.
 *
 * Below is a list of Qt 3D resources currently registered into collections:
 *
 * \list
 * \li Materials
 * \li Meshes
 * \li Armatures
 * \li Skeletons
 * \li Cameras
 * \li Entities
 * \li Textures
 * \li TextureImages
 * \li AnimationClips
 * \li AnimationMappings
 * \endlist
 *
 * As far as glTF is concerned, a material could be used for skinned as well as
 * regular meshes. This doesn't match with how Qt 3D works. If a material named
 * "Mat" is used for both skinned and regular meshes, the importer will
 * register two materials:
 *
 * \list
 * \li a material that doesn't perform skinning for use with regular meshes. It
 * will be added to the collection with the name "Mat"
 * \li a material that does perform skinning for use with skinned meshes. It will
 * be added to the collection with the name "skinned_Mat"
 * \endlist
 */

/*!
 * \qmltype GLTF2Importer
 * \instantiates Kuesa::GLTF2Importer
 * \inqmlmodule Kuesa
 * \inherits KuesaNode
 * \since Kuesa 1.0
 * \brief Imports glTF 2 scenes into a Qt 3D Scene.
 *
 * GLTF2Importer imports glTF 2 scenes into a Qt 3D scene.
 *
 * \badcode
 * import Kuesa 1.1 as Kuesa
 *
 * Kuesa.SceneEntity {
 *     id: scene
 *     Kuesa.GLTF2Importer {
 *         sceneEntity: scene
 *         source: "file:///source.gltf"
 *     }
 * }
 * \endcode
 *
 * If a Kuesa::SceneEntity has been set on the importer, various Qt 3D
 * resources generated upon import will be registered into named collections.
 * If no name is specified and assignNames is true, a default name will be
 * assigned to the asset.
 * If several assets of the same type share the same name, subsequent assets
 * will be given unique names by appending an index.
 *
 * Below is a list of Qt 3D resources currently registered into collections:
 *
 * \list
 * \li Materials
 * \li Meshes
 * \li Armatures
 * \li Skeletons
 * \li Cameras
 * \li Entities
 * \li Textures
 * \li TextureImages
 * \li AnimationClips
 * \li AnimationMappings
 * \endlist
 *
 * As far as glTF is concerned, a material could be used for skinned as well as
 * regular meshes. This doesn't match with how Qt 3D works. If a material named
 * "Mat" is used for both skinned and regular meshes, the importer will
 * register two materials:
 *
 * \list
 * \li a material that doesn't perform skinning for use with regular meshes. It
 * will be added to the collection with the name "Mat"
 * \li a material that does perform skinning for use with skinned meshes. It will
 * be added to the collection with the name "skinned_Mat"
 * \endlist
 *
 */

/*!
    \enum Kuesa::GLTF2Importer::Status

    This enum type describes state of the importer.

    \value None  Unknown state (default).
    \value Loading  Importer is currently loading a file.
    \value Ready  A glTF file was successfully loaded.
    \value Error  An error occurred when loading the current glTF file.
*/

/*!
    \property GLTF2Importer::source
    \brief the source of the glTF file
 */

/*!
    \property Kuesa::GLTF2Importer::status
    \brief the current status of the importer

    \sa Kuesa::GLTF2Importer::Status
 */

/*!
    \property Kuesa::GLTF2Importer::assignNames

    \brief if true, assets with no names will be added to collections with
    default names (default is false)
 */

/*!
    \qmlproperty url GLTF2Importer::source
    \brief the source of the glTF file
 */

/*!
    \qmlproperty GLTF2Importer::Status GLTF2Importer::status
    \brief the current status of the importer
 */

/*!
    \qmlproperty Kuesa.SceneEntity GLTF2Importer::sceneEntity

    \brief pointer to the SceneEntity with which assets will be registered as
    they are loaded from the glTF file.
 */

/*!
    \qmlproperty bool GLTF2Importer::assignNames

    \brief if true, assets with no names will be added to collections with
    default names (default is false)
 */

/*!
    \property GLTF2Importer::options

    \brief Holds the Kuesa::GLTF2Import::GLTF2Options used to select what to
    import/generate
 */

/*!
    \qmlproperty Kuesa.GLTF2Options GLTF2Importer::options
    \brief Holds the Kuesa.GLTF2Options used to select what to import/generate
 */

/*!
    \property GLTF2Importer::activeSceneIndex
    \brief Specifies which glTF scene should be loaded.
    Defaults to index GLTF2Importer::DefaultScene.
 */

/*!
    \qmlproperty int GLTF2Importer::activeSceneIndex
    \brief Specifies which glTF scene should be loaded.
    Defaults to index GLTF2Importer.DefaultScene.
 */

/*!
    \property GLTF2Importer::availableScenes
    \brief Holds the names of the available glTF scenes.
 */

/*!
    \qmlproperty list GLTF2Importer::availableScenes
    \brief Holds the names of the available glTF scenes.
 */

/*!
 * \enum GLTF2Importer::ActiveScene
 * \value DefaultScene
 * Makes the importer load the glTF scene specified as being the default one in
 * the glTF file. If the glTF file doesn't specify anything, nothing will be loaded
 * \value EmptyScene
 * Tell the importer to not load any scene by default
 */

GLTF2Importer::GLTF2Importer(Qt3DCore::QNode *parent)
    : KuesaNode(parent)
    , m_context(new GLTF2Import::GLTF2Context)
    , m_root(nullptr)
    , m_currentSceneEntity(nullptr)
    , m_status(None)
    , m_assignNames(true)
    , m_activeSceneIndex(DefaultScene)
{
}

GLTF2Importer::~GLTF2Importer()
{
    delete m_context;
}

/*!
 * Returns the url of the glTF file
 */
QUrl GLTF2Importer::source() const
{
    return m_source;
}

/*!
 * Load the glTF file from the given url \a source.
 *
 * \note The loading is asynchronous. When loading is complete the status
 * property will change.
 */
void GLTF2Importer::setSource(const QUrl &source)
{
    if (source != m_source) {
        m_source = source;

        emit sourceChanged(m_source);

        // Deletes the scene and reset it to nullptr
        clear();

        // Load only if the path is non empty
        if (!m_source.isEmpty())
            QMetaObject::invokeMethod(this, "load", Qt::QueuedConnection);
    }
}

/*!
 * Return the current state of the importer
 */
GLTF2Importer::Status GLTF2Importer::status() const
{
    return m_status;
}

void GLTF2Importer::setStatus(Status status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged(status);
    }
}

/*!
 * Returns \c true if assets with no names should be added to collections
 */
bool GLTF2Importer::assignNames() const
{
    return m_assignNames;
}

Kuesa::GLTF2Import::GLTF2Options *GLTF2Importer::options()
{
    return m_context->options();
}

const Kuesa::GLTF2Import::GLTF2Options *GLTF2Importer::options() const
{
    return m_context->options();
}

int GLTF2Importer::activeSceneIndex() const
{
    return m_activeSceneIndex;
}

QStringList GLTF2Importer::availableScenes() const
{
    return m_availableScenes;
}

/*!
 * If \a assignNames is true, assets with no names will be added to
 * collections with default names.
 *
 * Otherwise, assets will not be added to collections.
 */
void GLTF2Importer::setAssignNames(bool assignNames)
{
    if (m_assignNames == assignNames)
        return;

    m_assignNames = assignNames;
    emit assignNamesChanged(m_assignNames);
}

void GLTF2Importer::setOptions(const Kuesa::GLTF2Import::GLTF2Options &options)
{
    GLTF2Import::GLTF2Options *m_options = m_context->options();
    m_options->setGenerateTangents(options.generateTangents());
}

void GLTF2Importer::setActiveSceneIndex(int index)
{
    if (m_activeSceneIndex == index)
        return;
    m_activeSceneIndex = index;
    emit activeSceneIndexChanged(index);

    // Try to load scene tree if parsing has already been performed
    if (status() == Status::Ready) {
        setupActiveScene();
    }
}

/*!
 * Reloads the current glTF file.
 */
void GLTF2Importer::reload()
{
    // Deletes the scene and reset it to nullptr
    clear();

    // Load only if the path is non empty
    if (!m_source.isEmpty())
        load();
}

void GLTF2Importer::load()
{
    if (status() == Status::Loading)
        return;

    setStatus(GLTF2Importer::Status::Loading);
    // Reset context (except options)
    m_context->reset();

    // Clear list of available scenes
    m_availableScenes.clear();

    const QString path = urlToLocalFileOrQrc(m_source);

    // Ensure clear has been called before reloading
    Q_ASSERT(m_root == nullptr);
#ifdef USE_THREADED_PARSER
    auto parser = new GLTF2Import::ThreadedGLTF2Parser(m_context, m_sceneEntity, m_assignNames);

    connect(
            parser, &GLTF2Import::ThreadedGLTF2Parser::parsingFinished,
            this, [this, parser](Qt3DCore::QEntity *root) {
                m_root = root;

                if (m_root) {
                    m_root->setParent(this);
                    if (m_sceneEntity)
                        emit m_sceneEntity->loadingDone();
                    setStatus(GLTF2Importer::Status::Ready);
                } else {
                    setStatus(GLTF2Importer::Status::Error);
                }

                parser->deleteLater();
            },
            Qt::QueuedConnection);

    parser->parse(path);
#else
    GLTF2Import::GLTF2Parser parser(m_sceneEntity, m_assignNames);
    parser.setContext(m_context);
    const bool parsingSucceeded = parser.parse(path);
    m_root = parser.contentRoot();

    if (parsingSucceeded && m_root) {

        for (int i = 0, m = m_context->scenesCount(); i < m; ++i) {
            const GLTF2Import::Scene scene = m_context->scene(i);
            m_availableScenes << scene.name;
        }

        // Load scene
        m_sceneRootEntities = parser.sceneRoots();
        setupActiveScene();

        // Set parent on root content
        m_root->setParent(this);

        if (m_sceneEntity)
            emit m_sceneEntity->loadingDone();
        setStatus(GLTF2Importer::Status::Ready);
    } else {
        setStatus(GLTF2Importer::Status::Error);
    }
#endif

    emit availableScenesChanged(m_availableScenes);
}

void GLTF2Importer::setupActiveScene()
{
    Q_ASSERT(m_root);

    // Hide old scene tree
    if (m_currentSceneEntity != nullptr)
        m_currentSceneEntity->setParent(static_cast<Qt3DCore::QNode *>(nullptr));

    m_currentSceneEntity = nullptr;

    if (m_activeSceneIndex == DefaultScene) {
        m_activeSceneIndex = m_context->defaultScene();
        emit activeSceneIndexChanged(m_activeSceneIndex);
    }

    if (m_activeSceneIndex == EmptyScene)
        return;

    // Retrieve new scene tree
    if (m_activeSceneIndex > m_sceneRootEntities.size() ||
        m_activeSceneIndex < 0) {
        qCWarning(Kuesa::kuesa) << "Unable to load glTF scene for index" << m_activeSceneIndex;
        return;
    }

    m_currentSceneEntity = m_sceneRootEntities.at(m_activeSceneIndex);
    m_currentSceneEntity->makeActive();
    m_currentSceneEntity->setParent(m_root);
}

void GLTF2Importer::clear()
{
    if (m_root != nullptr) {
        for (GLTF2Import::SceneRootEntity *sceneRoot : qAsConst(m_sceneRootEntities))
            sceneRoot->deleteLater();
        m_root->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
        m_root->deleteLater();
    }
    m_sceneRootEntities.clear();
    m_root = nullptr;
    m_currentSceneEntity = nullptr;

    setStatus(GLTF2Importer::Status::None);
}

QT_END_NAMESPACE
