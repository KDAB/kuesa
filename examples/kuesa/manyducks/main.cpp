/*
    main.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jean-Michaël Celerier <jean-michael.celerier@kdab.com>

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

// Kuesa libraries
#include <ForwardRenderer>
#include <SceneEntity>
#include <DepthOfFieldEffect>
#include <GLTF2Importer>
#include <Skybox>
#include <QScreen>
// Qt3D
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QEnvironmentLight>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>

// Qt & std
#include <QGuiApplication>
#include <QResource>
#include <QDir>
#include <QTimer>
#include <array>
#ifdef Q_OS_ANDROID
#include <QOpenGLContext>
#endif

namespace {

template<typename ComponentType>
inline ComponentType *componentFromEntity(Qt3DCore::QEntity *e)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
   const auto cmps = e->componentsOfType<ComponentType>();
   return cmps.size() > 0 ? cmps.first() : nullptr;
#else
   ComponentType *typedComponent = nullptr;
   const Qt3DCore::QComponentVector cmps = e->components();

   for (Qt3DCore::QComponent *c : cmps) {
       typedComponent = qobject_cast<ComponentType *>(c);
       if (typedComponent != nullptr)
           break;
   }

   return typedComponent;
#endif
}

// Helper to load .qrb files, used for large environment maps
bool initializeAssetResources(const QVector<QString> &fileNames)
{
#ifdef Q_OS_MACOS
    QDir resourceDir(qApp->applicationDirPath() + QStringLiteral("/../Resources"));
#elif defined(Q_OS_IOS)
    QDir resourceDir(qApp->applicationDirPath() + QStringLiteral("/Library/Application Support"));
#else
    QDir resourceDir(qApp->applicationDirPath() + QStringLiteral("/resources"));
#endif
    bool b = true;
    for (const auto &fileName : fileNames) {
        auto f = resourceDir.path() + QDir::separator() + fileName;
        b &= QResource::registerResource(f);
        if (!b)
            qDebug() << "Failed to load assets from" << f;
    }
    return b;
}

static QString envmap(QString name)
{
    return QStringLiteral("qrc:///pink_sunrise_%1%2")
#if defined(Q_OS_MACOS)
            .arg("16f_")
#else
            .arg("")
#endif
            .arg(name);
}

} // namespace

class DefaultEnvMap : public Qt3DRender::QEnvironmentLight
{
public:
    DefaultEnvMap(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QEnvironmentLight{ parent }
    {
        auto tli = new Qt3DRender::QTextureLoader(this);
        tli->setSource(QUrl(envmap("irradiance.dds")));
        tli->setMinificationFilter(Qt3DRender::QAbstractTexture::LinearMipMapLinear);
        tli->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
        tli->wrapMode()->setX(Qt3DRender::QTextureWrapMode::ClampToEdge);
        tli->wrapMode()->setY(Qt3DRender::QTextureWrapMode::ClampToEdge);
        tli->setGenerateMipMaps(0);
        setIrradiance(tli);

        auto tls = new Qt3DRender::QTextureLoader(this);
        tls->setSource(QUrl(envmap("specular.dds")));
        tls->setMinificationFilter(Qt3DRender::QAbstractTexture::LinearMipMapLinear);
        tls->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
        tls->wrapMode()->setX(Qt3DRender::QTextureWrapMode::ClampToEdge);
        tls->wrapMode()->setY(Qt3DRender::QTextureWrapMode::ClampToEdge);
        tls->setGenerateMipMaps(0);
        setSpecular(tls);
    }
};


//! [0]
class Window : public Qt3DExtras::Qt3DWindow
//! [0]
{
public:
    static constexpr int Ducks = 2000;
    static constexpr int r = 2000;

    Window()
    {
        // Default set-up - here we create an empty entity,
        // which will contain one scene per glTF object.
        //! [0.1]
        m_scene = new Kuesa::SceneEntity();
        m_scene->addComponent(new DefaultEnvMap(m_scene));
        //! [0.1]

        //! [0.2]
        camera()->setPosition(QVector3D(5, 1.5, 5));
        camera()->setViewCenter(QVector3D(0, 0.5, 0));
        camera()->setUpVector(QVector3D(0, 1, 0));
        camera()->setAspectRatio(16.f / 9.f);

        auto camController = new Qt3DExtras::QOrbitCameraController(m_scene);
        camController->setCamera(camera());
        //! [0.2]

        //! [0.3]
        auto fg = new Kuesa::ForwardRenderer();
        fg->setCamera(camera());
        fg->setGamma(2.2f);
        fg->setExposure(1.f);
        fg->setClearColor("white");
        setActiveFrameGraph(fg);
        //! [0.3]

        // Load a glTF models
        //! [0.4]
        auto gltfImporter = new Kuesa::GLTF2Importer(m_scene);
        gltfImporter->setSceneEntity(m_scene);
        gltfImporter->setSource(QUrl{ "qrc:///assets/models/duck/Duck.glb" });

        connect(gltfImporter, &Kuesa::GLTF2Importer::statusChanged,
                this, &Window::on_sceneLoaded);
        //! [0.4]

        //! [0.5]
        // Skybox creation
        auto skybox = new Kuesa::Skybox(m_scene);
        skybox->setBaseName(envmap("radiance"));
        skybox->setExtension(".dds");
        //! [0.5]

        //! [0.6]
        // Depth-of-field
        auto dof = new Kuesa::DepthOfFieldEffect;
        dof->setRadius(35.0f);
        dof->setFocusRange(2.0f);
        dof->setFocusDistance(6.5f);
        fg->addPostProcessingEffect(dof);
        //! [0.6]

        //! [0.7]
        setRootEntity(m_scene);
        //! [0.7]
    }

private:
    //! [1]
    void on_sceneLoaded(Kuesa::GLTF2Importer::Status status)
    {
        if (status == Kuesa::GLTF2Importer::Ready) {
    //! [1]
            //! [1.1]
            // First let's take the components that we are going to use to create our clones
            // Gotten from gammaray
            auto parent = m_scene->entity("KuesaEntity_0");
            //! [1.1]

            //! [1.2]
            auto *orig_entity = qobject_cast<Qt3DCore::QEntity *>(m_scene->entity("KuesaEntity_2")->childNodes()[1]);
            auto *orig_geometry = componentFromEntity<Qt3DRender::QGeometryRenderer>(orig_entity);
            auto *orig_material = componentFromEntity<Qt3DRender::QMaterial>(orig_entity);
            //! [1.2]

            //! [1.3]
            // Then create clones by giving them a custom transform, and the same components than before
            for (int i = 0; i < Ducks; i++) {
                auto new_entity = new Qt3DCore::QEntity{ parent };
                auto new_transform = new Qt3DCore::QTransform;
                new_transform->setScale(0.2f);
                new_transform->setTranslation(QVector3D(rand() % r - r / 2, rand() % r - r / 2, rand() % r - r / 2));

                new_transform->setRotationX(rand() % 360);
                new_transform->setRotationY(rand() % 360);
                new_transform->setRotationZ(rand() % 360);

                new_entity->addComponent(new_transform);
                new_entity->addComponent(orig_geometry);
                new_entity->addComponent(orig_material);

                m_transforms[i] = new_transform;
            }
            //! [1.3]

            // Animate everyone
            qreal ms = 1000. / this->screen()->refreshRate();
            startTimer(static_cast<int>(ms));
        }
    }

    //! [2]
    void timerEvent(QTimerEvent *event) override
    {
        Q_UNUSED(event)
        for (auto transform : m_transforms) {
            transform->setRotationX(transform->rotationX() + 0.1f);
            transform->setRotationY(transform->rotationY() + 0.1f);
            transform->setRotationZ(transform->rotationZ() + 0.1f);
        }
    }
    //! [2]

    Kuesa::SceneEntity *m_scene{};
    std::array<Qt3DCore::QTransform *, Ducks> m_transforms;
};

int main(int argc, char *argv[])
{
    {
        // Set OpenGL requirements
        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
#ifndef QT_OPENGL_ES_2
        format.setVersion(4, 1);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSamples(4);
#else
        format.setVersion(3, 0);
        format.setProfile(QSurfaceFormat::NoProfile);
        format.setRenderableType(QSurfaceFormat::OpenGLES);
#endif
        QSurfaceFormat::setDefaultFormat(format);
    }

    QGuiApplication app(argc, argv);

    initializeAssetResources({ QStringLiteral("Duck.qrb") });
#if defined(Q_OS_IOS) || defined(Q_OS_MACOS) || defined(Q_OS_ANDROID)
    initializeAssetResources({ QStringLiteral("envmap-pink-sunrise-16f.qrb") });
#else
    initializeAssetResources({ QStringLiteral("envmap-pink-sunrise.qrb") });
#endif

#ifdef Q_OS_ANDROID
    const QString assetsPrefix = QStringLiteral("assets:/");
    // Qt builds for android may not define QT_OPENGL_ES_3
    // Therefore we need a runtime check to see whether we can use ES 3.0 or not
    QOpenGLContext ctx;
    ctx.setFormat(QSurfaceFormat::defaultFormat());
    if (ctx.create())
        const QSurfaceFormat androidFormat = ctx.format();
#else
    const QString assetsPrefix = QStringLiteral("qrc:/");
#endif

    Window window;

    window.show();

    return app.exec();
}
