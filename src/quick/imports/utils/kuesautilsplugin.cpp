/*
    kuesautilsplugin.cpp

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

#include "kuesautilsplugin.h"
#include "view3dsceneitem.h"
#include "sceneconfigurationitem.h"

#include <KuesaUtils/boundingvolumerenderer.h>
#include <KuesaUtils/orbitcameracontroller.h>

#include <QtQml/qqml.h>
#include <Qt3DQuick/private/qt3dquick_global_p.h>

QT_BEGIN_NAMESPACE

KuesaUtilsPlugin::KuesaUtilsPlugin(QObject *parent)
    : QQmlExtensionPlugin(parent)
{
}

KuesaUtilsPlugin::~KuesaUtilsPlugin()
{
}

void KuesaUtilsPlugin::registerTypes(const char *uri)
{
    // Make latest Kuesa import available (even if no new types are registered)
    qmlRegisterModule(uri, 1, 3);

    // Utils
    qmlRegisterType<KuesaUtils::BoundingVolumeRenderer>(uri, 1, 0, "BoundingVolumeRenderer");
    qmlRegisterType<KuesaUtils::OrbitCameraController>(uri, 1, 0, "OrbitCameraController");
    qmlRegisterExtendedType<KuesaUtils::View3DScene, KuesaUtils::View3DSceneItem>(uri, 1, 0, "View3DScene");
    qmlRegisterExtendedType<KuesaUtils::SceneConfiguration, KuesaUtils::SceneConfigurationItem>(uri, 1, 0, "SceneConfiguration");
}

QT_END_NAMESPACE
