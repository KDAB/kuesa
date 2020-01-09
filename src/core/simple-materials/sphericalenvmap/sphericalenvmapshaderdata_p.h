
/*
    sphericalenvmapshaderdata_p.h

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

#ifndef KUESA_SPHERICALENVMAPSHADERDATA_P_H
#define KUESA_SPHERICALENVMAPSHADERDATA_P_H

#include <Qt3DRender/QAbstractTexture>
#include <QVector2D>
#include <QVector3D>
#include <Qt3DRender/QShaderData>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class SphericalEnvMapShaderData : public Qt3DRender::QShaderData
{
   Q_OBJECT
    Q_PROPERTY(QVector3D factors READ factors WRITE setFactors NOTIFY factorsChanged)
    Q_PROPERTY(QVector2D disturbation READ disturbation WRITE setDisturbation NOTIFY disturbationChanged)
    Q_PROPERTY(float postVertexColor READ postVertexColor WRITE setPostVertexColor NOTIFY postVertexColorChanged)
    Q_PROPERTY(float postGain READ postGain WRITE setPostGain NOTIFY postGainChanged)
    Q_PROPERTY(float semGain READ semGain WRITE setSemGain NOTIFY semGainChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture * sem READ sem WRITE setSem NOTIFY semChanged)

public:
    explicit SphericalEnvMapShaderData(Qt3DCore::QNode *parent = nullptr);
    ~SphericalEnvMapShaderData();
    QVector3D factors() const;
    QVector2D disturbation() const;
    float postVertexColor() const;
    float postGain() const;
    float semGain() const;
    Qt3DRender::QAbstractTexture * sem() const;

public Q_SLOTS:
    void setFactors(const QVector3D &factors);
    void setDisturbation(const QVector2D &disturbation);
    void setPostVertexColor(float postVertexColor);
    void setPostGain(float postGain);
    void setSemGain(float semGain);
    void setSem(Qt3DRender::QAbstractTexture * sem);

Q_SIGNALS:
    void factorsChanged(QVector3D);
    void disturbationChanged(QVector2D);
    void postVertexColorChanged(float);
    void postGainChanged(float);
    void semGainChanged(float);
    void semChanged(Qt3DRender::QAbstractTexture *);

private:
    QVector3D m_factors;
    QVector2D m_disturbation;
    float m_postVertexColor;
    float m_postGain;
    float m_semGain;
    Qt3DRender::QAbstractTexture * m_sem;

};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_SPHERICALENVMAPSHADERDATA_P_H
