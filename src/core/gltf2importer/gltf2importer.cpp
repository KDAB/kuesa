/*
    gltf2importer.cpp

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

#include "gltf2context_p.h"
#include "gltf2importer.h"
#include "gltf2importer/gltf2parser_p.h"

#include "collections/meshcollection.h"
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
 * \since 1.0
 * \brief Imports glTF 2 scenes into a Qt 3D Scene.
 *
 * GLTF2Importer imports glTF 2 scenes into a Qt 3D scene.
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
 * \list
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
 * \inmodule Kuesa
 * \since 1.0
 * \brief Imports glTF 2 scenes into a Qt 3D Scene.
 *
 * GLTF2Importer imports glTF 2 scenes into a Qt 3D scene.
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
 * \list
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
    \enum GLTF2Importer::Status

    This enum type describes state of the importer.

    \value None  Unknown state (default).
    \value Loading  Importer is currently loading a file.
    \value Ready  A glTF file was successfully loaded.
    \value Error  An error occurred when loading the current glTF file.
*/

/*!
    \property GLTF2Importer::source
    \brief the source of the glTF file

    \sa GLTF2Importer::source()
 */

/*!
    \property GLTF2Importer::status
    \brief the current status of the importer

    \sa GLTF2Importer::status(), GLTF2Importer::Status
 */

/*!
    \property GLTF2Importer::sceneEntity
    \brief pointer to the SceneEntity with which assets will be registered as they are loaded from the glTF file.

    \sa GLTF2Importer::sceneEntity()
 */

/*!
    \property GLTF2Importer::assignNames
    \brief if true, assets with no names will be added to collections with default names (default is false)

    \sa GLTF2Importer::assignNames()
 */

/*!
    \qmlproperty GLTF2Importer::source
    \brief the source of the glTF file
 */

/*!
    \qmlproperty GLTF2Importer::status
    \brief the current status of the importer
 */

/*!
    \qmlproperty GLTF2Importer::sceneEntity
    \brief pointer to the SceneEntity with which assets will be registered as they are loaded from the glTF file.
 */

/*!
    \property GLTF2Importer::assignNames
    \brief if true, assets with no names will be added to collections with default names (default is false)
 */

GLTF2Importer::GLTF2Importer(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
    , m_context(new GLTF2Import::GLTF2Context)
    , m_root(nullptr)
    , m_status(None)
    , m_sceneEntity(nullptr)
    , m_assignNames(false)
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
 * Load the glTF file from the given url.
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
 * Returns the SceneEntity used to access asset collections
 */
SceneEntity *GLTF2Importer::sceneEntity() const
{
    return m_sceneEntity;
}

/*!
 * Set the SceneEntity instance which should be used to access
 * the collections loaded assets will be registered with.
 */
void GLTF2Importer::setSceneEntity(SceneEntity *sceneEntity)
{
    if (m_sceneEntity == sceneEntity)
        return;

    if (m_sceneEntity)
        disconnect(m_sceneEntityDestructionConnection);

    m_sceneEntity = sceneEntity;
    if (m_sceneEntity) {
        auto f = [this]() { setSceneEntity(nullptr); };
        m_sceneEntityDestructionConnection = connect(m_sceneEntity, &Qt3DCore::QNode::nodeDestroyed, this, f);
    }

    emit sceneEntityChanged(m_sceneEntity);
}

/*!
 * Returns \c true if assets with no names should be added to collections
 */
bool GLTF2Importer::assignNames() const
{
    return m_assignNames;
}

/*!
 * if \param assignNames is true, assets with no names will be added to
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

void GLTF2Importer::load()
{
    setStatus(GLTF2Importer::Status::Loading);
    *m_context = {};

    const QString path = urlToLocalFileOrQrc(m_source);

    GLTF2Import::GLTF2Parser parser(m_sceneEntity, m_assignNames);
    parser.setContext(m_context);

    Q_ASSERT(m_root == nullptr);
    m_root = parser.parse(path);
    if (m_root) {
        m_root->setParent(this);
        if (m_sceneEntity)
            emit m_sceneEntity->loadingDone();
    }

    setStatus(m_root ? GLTF2Importer::Status::Ready : GLTF2Importer::Status::Error);
}

void GLTF2Importer::clear()
{
    if (m_root != nullptr) {
        m_root->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
        m_root->deleteLater();
    }
    m_root = nullptr;
}

QT_END_NAMESPACE
