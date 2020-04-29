/*
    main.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#include <Kuesa/SceneEntity>

#include <Qt3DAnimation/QAnimationAspect>
#include <Qt3DQuickExtras/qt3dquickwindow.h>

#include <QQmlAspectEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QGuiApplication>

using namespace Kuesa;

class SceneInitializer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SceneEntity *scene READ scene WRITE setScene NOTIFY sceneChanged)
public:
    explicit SceneInitializer(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    SceneEntity *scene() const
    {
        return m_scene;
    }

    void setScene(SceneEntity *scene)
    {
        // TODO: Find somewhere nicer to do this
        // Layers
        scene->layers()->add("depthPass", new Qt3DRender::QLayer);
        scene->layers()->add("forwardPass", new Qt3DRender::QLayer);
        scene->layer("forwardPass")->setRecursive(true);

        m_scene = scene;
        emit sceneChanged();
        emit scene->loadingDone();
    }

signals:
    void sceneChanged();

private:
    SceneEntity *m_scene;
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Qt3DExtras::Quick::Qt3DQuickWindow view;
    view.registerAspect(new Qt3DAnimation::QAnimationAspect());

    qmlRegisterType<SceneInitializer>("GLTF2Importer", 1, 0, "SceneInitializer");

    view.engine()->qmlEngine()->rootContext()->setContextProperty("ASSETS", ASSETS);

    view.setSource(QUrl("qrc:/main.qml"));
    view.resize(1920, 1080);
    view.show();

    return app.exec();
}

#include "main.moc"
