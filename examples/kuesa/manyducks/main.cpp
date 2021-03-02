/*
    main.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <MeshInstantiator>
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
#include <QTimer>
#include <QRandomGenerator>
#include <array>
#ifdef Q_OS_ANDROID
#include <QOpenGLContext>
#endif

namespace {

template<typename ComponentType>
inline ComponentType *componentFromEntity(Qt3DCore::QEntity *e)
{
    const auto cmps = e->componentsOfType<ComponentType>();
    return cmps.size() > 0 ? cmps.first() : nullptr;
}

static QString envmap(QString name)
{
    return QStringLiteral("qrc:///pink_sunrise_16f_%1").arg(name);
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

    explicit Window(const bool usesInstancing)
        : m_usesInstancing(usesInstancing)
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
        fg->setSkinning(true);
        // We disable frustum culling if using instancing
        fg->setFrustumCulling(!usesInstancing);
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
        auto dof = new Kuesa::DepthOfFieldEffect();
        dof->setRadius(15.0f);
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

            if (!m_usesInstancing) {
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

                QRandomGenerator *rand = QRandomGenerator::global();
                for (int i = 0; i < Ducks; i++) {
                    auto new_entity = new Qt3DCore::QEntity{ parent };
                    auto new_transform = new Qt3DCore::QTransform;
                    new_transform->setScale(0.2f);
                    new_transform->setTranslation(QVector3D(rand->generate() % r - r / 2, rand->generate() % r - r / 2, rand->generate() % r - r / 2));

                    new_transform->setRotationX(rand->generate() % 360);
                    new_transform->setRotationY(rand->generate() % 360);
                    new_transform->setRotationZ(rand->generate() % 360);

                    new_entity->addComponent(new_transform);
                    new_entity->addComponent(orig_geometry);
                    new_entity->addComponent(orig_material);

                    m_transforms[i] = new_transform;
                }
                //! [1.3]

            } else {
                //! [1.4]
                // Build base transformation matrices for each instance

                QRandomGenerator *rand = QRandomGenerator::global();
                m_matrices.reserve(Ducks + 1);
                for (int i = 0; i < Ducks; i++) {
                    QMatrix4x4 m;

                    const int extent = r / 20;
                    m.translate(QVector3D(rand->generate() % extent - extent * 0.5, rand->generate() % extent - extent * 0.5, rand->generate() % extent - extent * 0.5));
                    m.rotate(rand->generate() % 360, QVector3D(1.0f, 0.0f, 0.0f));
                    m.rotate(rand->generate() % 360, QVector3D(0.0f, 1.0f, 0.0f));
                    m.rotate(rand->generate() % 360, QVector3D(0.0f, 0.0f, 1.0f));

                    m_matrices.push_back(m);
                }
                // Add an extra matrices to have one instance placed at the origin
                m_matrices.push_back({});
                //! [1.4]

                //! [1.5]
                // Create MeshInstantiator
                m_meshInstantiator = new Kuesa::MeshInstantiator(m_scene);
                // Specify which Entity with a Mesh needs to be instanced
                m_meshInstantiator->setEntityName(QStringLiteral("KuesaEntity_2"));
                // Set transformation matrices
                m_meshInstantiator->setTransformationMatrices(m_matrices);
                //! [1.5]
            }

            // Animate everyone
            qreal ms = 1000. / this->screen()->refreshRate();
            startTimer(static_cast<int>(ms));
        }
    }

    //! [2]
    void timerEvent(QTimerEvent *event) override
    {
        Q_UNUSED(event)
        if (!m_usesInstancing) {
            for (auto transform : m_transforms) {
                transform->setRotationX(transform->rotationX() + 0.1f);
                transform->setRotationY(transform->rotationY() + 0.1f);
                transform->setRotationZ(transform->rotationZ() + 0.1f);
            }
        } else {
            static bool wasInitialized = false;
            static QMatrix4x4 rotationIncrementMatrix;

            if (!wasInitialized) {
                rotationIncrementMatrix.rotate(0.1f, QVector3D(1.0f, 0.0f, 0.0f));
                rotationIncrementMatrix.rotate(0.1f, QVector3D(0.0f, 1.0f, 0.0f));
                rotationIncrementMatrix.rotate(0.1f, QVector3D(0.0f, 0.0f, 1.0f));
                wasInitialized = true;
            }

            // Apply rotation transform to all matrices
            // This accumulates over time
            for (QMatrix4x4 &m : m_matrices)
                m *= rotationIncrementMatrix;

            m_meshInstantiator->setTransformationMatrices(m_matrices);
        }
    }
    //! [2]

    const bool m_usesInstancing;
    Kuesa::SceneEntity *m_scene{};
    std::array<Qt3DCore::QTransform *, Ducks> m_transforms;
    std::vector<QMatrix4x4> m_matrices;
    Kuesa::MeshInstantiator *m_meshInstantiator = nullptr;
};

int main(int argc, char *argv[])
{
    {
        // Set OpenGL requirements
        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
#ifndef KUESA_OPENGL_ES_2
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

    constexpr bool useInstancing = true;
    Window window(useInstancing);

    window.show();

    return app.exec();
}
