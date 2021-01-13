/*
    gltf2importer.h

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

#ifndef KUESA_GLTF2IMPORTER_H
#define KUESA_GLTF2IMPORTER_H

#include <Kuesa/kuesa_global.h>
#include <QUrl>
#include <QHash>
#include <Qt3DCore/QEntity>
#include <Kuesa/GLTF2Options>
#include <Kuesa/SceneEntity>
#include <Kuesa/KuesaNode>

QT_BEGIN_NAMESPACE

namespace Kuesa {
class GLTF2Exporter;

namespace GLTF2Import {
class GLTF2Context;
class GLTF2Parser;
class MaterialParser;
class SceneRootEntity;
}

class KUESASHARED_EXPORT GLTF2Importer : public KuesaNode
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Kuesa::GLTF2Importer::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool assignNames READ assignNames WRITE setAssignNames NOTIFY assignNamesChanged)
    Q_PROPERTY(Kuesa::GLTF2Import::GLTF2Options *options READ options CONSTANT)
    Q_PROPERTY(int activeSceneIndex READ activeSceneIndex WRITE setActiveSceneIndex NOTIFY activeSceneIndexChanged)
    Q_PROPERTY(QStringList availableScenes READ availableScenes NOTIFY availableScenesChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
public:
    enum Status {
        None,
        Loading,
        Ready,
        Error
    };
    Q_ENUM(Status)

    enum ActiveScene {
        DefaultScene = -2,
        EmptyScene = -1
    };
    Q_ENUM(ActiveScene)

    GLTF2Importer(Qt3DCore::QNode *parent = nullptr);
    ~GLTF2Importer();

    QUrl source() const;
    GLTF2Importer::Status status() const;
    bool assignNames() const;
    bool asynchronous() const;
    Kuesa::GLTF2Import::GLTF2Options *options();
    const Kuesa::GLTF2Import::GLTF2Options *options() const;
    int activeSceneIndex() const;
    QStringList availableScenes() const;

    template<class MaterialClass, class PropertiesClass, class EffectClass>
    static void registerCustomMaterial(const QString &name)
    {
        auto &cache = CustomMaterialCache::instance();

        cache.m_registeredCustomMaterial.insert(name, { &MaterialClass::staticMetaObject,
                                                        &PropertiesClass::staticMetaObject,
                                                        &EffectClass::staticMetaObject });
    }

public Q_SLOTS:
    void setSource(const QUrl &source);
    void setAssignNames(bool assignNames);
    void setOptions(const Kuesa::GLTF2Import::GLTF2Options &options);
    void setActiveSceneIndex(int index);
    void setAsynchronous(bool asynchronous);
    void reload();

Q_SIGNALS:
    void sourceChanged(const QUrl &source);
    void statusChanged(const Kuesa::GLTF2Importer::Status status);
    void assignNamesChanged(bool assignNames);
    void activeSceneIndexChanged(int activeSceneIndex);
    void availableScenesChanged(const QStringList &availableScenes);
    void asynchronousChanged(bool asynchronous);

private Q_SLOTS:
    void load();
    void setupActiveScene();

private:
    void clear();
    void setStatus(Status status);
    void handleGLTFParsingCompleted(bool parsingSucceeded);

    friend class GLTF2Exporter;
    friend class Kuesa::GLTF2Import::GLTF2Context;

    Kuesa::GLTF2Import::GLTF2Context *m_context;
    QUrl m_source;
    Qt3DCore::QEntity *m_root;
    GLTF2Import::SceneRootEntity *m_currentSceneEntity;
    QVector<GLTF2Import::SceneRootEntity *> m_sceneRootEntities;
    Status m_status;
    bool m_assignNames;
    Kuesa::GLTF2Import::GLTF2Options m_options;
    int m_activeSceneIndex;
    QStringList m_availableScenes;
    bool m_asynchronous = false;

    Kuesa::GLTF2Import::GLTF2Parser *m_parser = nullptr;


    friend class Kuesa::GLTF2Import::MaterialParser;
    struct CustomMaterialClassesTypeInfo {
        const QMetaObject *materialClassMetaObject;
        const QMetaObject *propertiesClassMetaObject;
        const QMetaObject *effectClassMetaObject;
    };

    struct CustomMaterialCache {
        QHash<QString, CustomMaterialClassesTypeInfo> m_registeredCustomMaterial;

        static CustomMaterialCache &instance()
        {
            static CustomMaterialCache c;
            return c;
        }
    };
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORTER_H
