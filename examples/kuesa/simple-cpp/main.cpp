/*
    main.cpp

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

#include <QGuiApplication>
#include <KuesaUtils/window.h>

namespace {

KuesaUtils::SceneConfiguration *prepareScene()
{
    KuesaUtils::SceneConfiguration *sceneConfiguration = new KuesaUtils::SceneConfiguration;
    sceneConfiguration->setSource(QUrl("qrc:/car.gltf"));
    sceneConfiguration->setCameraName("CamSweep_Orientation");

    // List of animations to play
    const QString animationNames[] = {
        QStringLiteral("MatMotorBlockAction"),
        QStringLiteral("TriggerMotorInfoAction"),
        QStringLiteral("DoorLeftAction"),
        QStringLiteral("DoorRightAction"),
        QStringLiteral("HoodAction"),
        QStringLiteral("SweepCamAction"),
        QStringLiteral("SweepCamCenterAction"),
        QStringLiteral("SweepCamPitchAction"),
    };

    for (const QString &animationName : animationNames) {
        Kuesa::AnimationPlayer *player = new Kuesa::AnimationPlayer();
        player->setClip(animationName);
        player->setRunning(true);
        player->setLoopCount(Kuesa::AnimationPlayer::Infinite);
        sceneConfiguration->addAnimationPlayer(player);
    }
    return sceneConfiguration;
}

} // namespace

int main(int ac, char **av)
{
    // Set OpenGL requirements
    Kuesa::setupDefaultSurfaceFormat();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(ac, av);
    KuesaUtils::Window view;
#ifdef Q_OS_IOS
    view.setFlags(view.flags() | Qt::MaximizeUsingFullscreenGeometryHint);
#endif

    // Configure Scene Content
    KuesaUtils::SceneConfiguration *sceneConfiguration = prepareScene();

    // Set SceneConfiguration on 3D View
    view.scene()->setActiveScene(sceneConfiguration);

    // Show the window
    view.show();

    return app.exec();
}
