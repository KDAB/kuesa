/*
    ShadowCastingLight.h

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#ifndef KUESA_SHADOWCASTINGLIGHT_H
#define KUESA_SHADOWCASTINGLIGHT_H

#include <QAbstractLight>
#include <QSize>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QAbstractTexture;
}

namespace Kuesa {
class ShadowCastingLightPrivate;

class KUESASHARED_EXPORT ShadowCastingLight : public Qt3DRender::QAbstractLight
{
    Q_OBJECT
    Q_PROPERTY(bool castsShadows READ castsShadows WRITE setCastsShadows NOTIFY castsShadowsChanged)
    Q_PROPERTY(bool softShadows READ softShadows WRITE setSoftShadows NOTIFY softShadowsChanged)
    Q_PROPERTY(QSize textureSize READ textureSize WRITE setTextureSize NOTIFY textureSizeChanged)
    Q_PROPERTY(float shadowMapBias READ shadowMapBias WRITE setShadowMapBias NOTIFY biasChanged)
    Q_PROPERTY(float nearPlane READ nearPlane WRITE setNearPlane NOTIFY nearPlaneChanged)

public:
    ~ShadowCastingLight();

    bool castsShadows() const;
    QSize textureSize() const;

    float shadowMapBias() const;
    float nearPlane() const;

    bool softShadows() const;

public Q_SLOTS:
    void setCastsShadows(bool castsShadows);
    void setLightViewProjectionMatrix(const QMatrix4x4 &matrix);
    void setTextureSize(const QSize &textureSize);
    void setShadowMapBias(float bias);
    void setNearPlane(float nearPlane);

    void setSoftShadows(bool softShadows);
    void setLightIndex(int index);

    // used for point lights. Holds actual camera clip planes
    void setNearFarPlanes(float near, float far);

Q_SIGNALS:
    void castsShadowsChanged(bool castsShadows);
    void textureSizeChanged(const QSize &textureSize);
    void depthTextureChanged(Qt3DRender::QAbstractTexture *depthTexture);
    void biasChanged(float bias);
    void nearPlaneChanged(float nearPlane);

    void softShadowsChanged(bool softShadows);

protected:
    explicit ShadowCastingLight(ShadowCastingLightPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(ShadowCastingLight)

    QSize m_textureSize;
    float m_nearPlane = 0.0f;
};

} // namespace Kuesa

QT_END_NAMESPACE
Q_DECLARE_METATYPE(Kuesa::ShadowCastingLight *)

#endif // KUESA_SHADOWCASTINGLIGHT_H
