/*
    cameraparser.cpp

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

#include "cameraparser_p.h"

#include "kuesa_p.h"
#include "gltf2context_p.h"

#include <cmath>
#include <memory>

#include <QJsonArray>
#include <QJsonObject>
#include <QtMath>

#include <Qt3DRender/QCamera>

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {

const QLatin1String KEY_NAME = QLatin1Literal("name");
const QLatin1String KEY_TYPE = QLatin1Literal("type");
const QLatin1String KEY_Z_NEAR = QLatin1Literal("znear");
const QLatin1String KEY_Z_FAR = QLatin1Literal("zfar");
const QLatin1String KEY_X_MAG = QLatin1Literal("xmag");
const QLatin1String KEY_Y_MAG = QLatin1Literal("ymag");
const QLatin1String KEY_ASPECT_RATIO = QLatin1Literal("aspectRatio");
const QLatin1String KEY_Y_FOV = QLatin1Literal("yfov");
const QLatin1String KEY_PERSPECTIVE = QLatin1Literal("perspective");
const QLatin1String KEY_ORTHOGRAPHIC = QLatin1Literal("orthographic");
} // namespace

CameraParser::CameraParser()
{
}

CameraParser::~CameraParser()
{
}

bool CameraParser::parse(const QJsonArray &cameras, Kuesa::GLTF2Import::GLTF2Context *context)
{
    const qint32 nbCameras = cameras.size();

    for (qint32 i = 0; i < nbCameras; ++i) {
        const QJsonObject &cameraObject = cameras.at(i).toObject();
        Camera camera;
        if (cameraObject.contains(KEY_NAME))
            camera.name = cameraObject.value(KEY_NAME).toString();

        std::unique_ptr<Qt3DRender::QCameraLens> lens(new Qt3DRender::QCameraLens());
        if (cameraObject.value(KEY_TYPE).toString() == QStringLiteral("perspective")) {
            if (!cameraObject.contains(KEY_PERSPECTIVE))
                return false;
            const QJsonObject &perspectiveObject = cameraObject.value(KEY_PERSPECTIVE).toObject();
            if (!perspectiveObject.contains(KEY_Y_FOV) ||
                !perspectiveObject.contains(KEY_Z_NEAR))
                return false;
            lens->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
            float v = perspectiveObject.value(KEY_Z_NEAR).toDouble();
            if (v <= 0.f)
                return false;
            lens->setNearPlane(v);
            v = perspectiveObject.value(KEY_Y_FOV).toDouble();
            if (v <= 0.f)
                return false;
            lens->setFieldOfView(qRadiansToDegrees(v));
            if (perspectiveObject.contains(KEY_ASPECT_RATIO)) {
                v = perspectiveObject.value(KEY_ASPECT_RATIO).toDouble();
                if (v <= 0.f)
                    return false;
                lens->setAspectRatio(v);
            }
            if (perspectiveObject.contains(KEY_Z_FAR)) {
                v = perspectiveObject.value(KEY_Z_FAR).toDouble();
                if (v <= 0.f)
                    return false;
                lens->setFarPlane(v);
            } else {
                v = std::tan(qDegreesToRadians(lens->fieldOfView()) * 0.5f);
                QMatrix4x4 projectionMatrix;
                projectionMatrix(0, 0) = 1.f / (lens->aspectRatio() * v);
                projectionMatrix(1, 1) = 1.f / v;
                projectionMatrix(2, 2) = -1.f;
                projectionMatrix(2, 3) = -2.f * lens->nearPlane();
                projectionMatrix(3, 2) = -1.f;
                lens->setProjectionMatrix(projectionMatrix);
            }
        } else if (cameraObject.value(KEY_TYPE).toString() == QStringLiteral("orthographic")) {
            if (!cameraObject.contains(KEY_ORTHOGRAPHIC))
                return false;
            const QJsonObject &orthographicObject = cameraObject.value(KEY_ORTHOGRAPHIC).toObject();
            if (!orthographicObject.contains(KEY_Z_NEAR) ||
                !orthographicObject.contains(KEY_Z_FAR) ||
                !orthographicObject.contains(KEY_X_MAG) ||
                !orthographicObject.contains(KEY_Y_MAG))
                return false;

            float v = orthographicObject.value(KEY_Z_NEAR).toDouble();
            if (v < 0.f)
                return false;
            lens->setNearPlane(v);
            v = orthographicObject.value(KEY_Z_FAR).toDouble();
            if (v <= 0.f)
                return false;
            lens->setFarPlane(v);

            QMatrix4x4 projectionMatrix;
            const float zFar = lens->farPlane();
            const float zNear = lens->nearPlane();
            v = orthographicObject.value(KEY_X_MAG).toDouble(1.0);
            if (qFuzzyIsNull(v))
                return false;
            const float xMag = v;
            v = orthographicObject.value(KEY_Y_MAG).toDouble(1.0);
            if (qFuzzyIsNull(v))
                return false;
            const float yMag = v;
            projectionMatrix(0, 0) = 1.0f / xMag;
            projectionMatrix(1, 1) = 1.0f / yMag;
            projectionMatrix(2, 2) = 2.0f / (zNear - zFar);
            projectionMatrix(3, 3) = 1.0f;
            projectionMatrix(2, 3) = (zFar + zNear) / (zNear - zFar);

            lens->setProjectionMatrix(projectionMatrix);
        } else {
            return false;
        }

        camera.lens = lens.release();
        context->addCamera(camera);
    }

    return true;
}

QT_END_NAMESPACE
