
/*
    alphasphericalenvmapproperties.h

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

#ifndef KUESA_ALPHASPHERICALENVMAPPROPERTIES_H
#define KUESA_ALPHASPHERICALENVMAPPROPERTIES_H

#include <Qt3DRender/QAbstractTexture>
#include <QVector2D>
#include <QVector3D>
#include <Kuesa/GLTF2MaterialProperties>
#include <Kuesa/kuesa_global.h>


QT_BEGIN_NAMESPACE

namespace Kuesa {

class AlphaSphericalEnvMapShaderData;

class KUESASHARED_EXPORT AlphaSphericalEnvMapProperties : public GLTF2MaterialProperties
{
    Q_OBJECT
    Q_PROPERTY(QVector3D factors READ factors WRITE setFactors NOTIFY factorsChanged)
    Q_PROPERTY(QVector2D disturbation READ disturbation WRITE setDisturbation NOTIFY disturbationChanged)
    Q_PROPERTY(float postVertexColor READ postVertexColor WRITE setPostVertexColor NOTIFY postVertexColorChanged)
    Q_PROPERTY(float postGain READ postGain WRITE setPostGain NOTIFY postGainChanged)
    Q_PROPERTY(float semGain READ semGain WRITE setSemGain NOTIFY semGainChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture * sem READ sem WRITE setSem NOTIFY semChanged)
    Q_PROPERTY(QVector3D semInnerFilter READ semInnerFilter WRITE setSemInnerFilter NOTIFY semInnerFilterChanged)
    Q_PROPERTY(QVector3D semOuterFilter READ semOuterFilter WRITE setSemOuterFilter NOTIFY semOuterFilterChanged)
    Q_PROPERTY(float semInnerAlpha READ semInnerAlpha WRITE setSemInnerAlpha NOTIFY semInnerAlphaChanged)
    Q_PROPERTY(float semOuterAlpha READ semOuterAlpha WRITE setSemOuterAlpha NOTIFY semOuterAlphaChanged)
    Q_PROPERTY(QVector3D glassInnerFilter READ glassInnerFilter WRITE setGlassInnerFilter NOTIFY glassInnerFilterChanged)
    Q_PROPERTY(QVector3D glassOuterFilter READ glassOuterFilter WRITE setGlassOuterFilter NOTIFY glassOuterFilterChanged)

public:
    Q_INVOKABLE explicit AlphaSphericalEnvMapProperties(Qt3DCore::QNode *parent = nullptr);
    ~AlphaSphericalEnvMapProperties();

    Qt3DRender::QShaderData *shaderData() const override;
    QVector3D factors() const;
    QVector2D disturbation() const;
    float postVertexColor() const;
    float postGain() const;
    float semGain() const;
    Qt3DRender::QAbstractTexture * sem() const;
    QVector3D semInnerFilter() const;
    QVector3D semOuterFilter() const;
    float semInnerAlpha() const;
    float semOuterAlpha() const;
    QVector3D glassInnerFilter() const;
    QVector3D glassOuterFilter() const;

public Q_SLOTS:
    void setFactors(const QVector3D &factors);
    void setDisturbation(const QVector2D &disturbation);
    void setPostVertexColor(float postVertexColor);
    void setPostGain(float postGain);
    void setSemGain(float semGain);
    void setSem(Qt3DRender::QAbstractTexture * sem);
    void setSemInnerFilter(const QVector3D &semInnerFilter);
    void setSemOuterFilter(const QVector3D &semOuterFilter);
    void setSemInnerAlpha(float semInnerAlpha);
    void setSemOuterAlpha(float semOuterAlpha);
    void setGlassInnerFilter(const QVector3D &glassInnerFilter);
    void setGlassOuterFilter(const QVector3D &glassOuterFilter);

Q_SIGNALS:
    void factorsChanged(QVector3D);
    void disturbationChanged(QVector2D);
    void postVertexColorChanged(float);
    void postGainChanged(float);
    void semGainChanged(float);
    void semChanged(Qt3DRender::QAbstractTexture *);
    void semInnerFilterChanged(QVector3D);
    void semOuterFilterChanged(QVector3D);
    void semInnerAlphaChanged(float);
    void semOuterAlphaChanged(float);
    void glassInnerFilterChanged(QVector3D);
    void glassOuterFilterChanged(QVector3D);

private:
    AlphaSphericalEnvMapShaderData *m_shaderData;
};
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_ALPHASPHERICALENVMAPPROPERTIES_H
