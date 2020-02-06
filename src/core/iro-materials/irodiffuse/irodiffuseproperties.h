
/*
    irodiffuseproperties.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_IRODIFFUSEPROPERTIES_H
#define KUESA_IRODIFFUSEPROPERTIES_H

#include <Qt3DRender/QAbstractTexture>
#include <QVector2D>
#include <QVector3D>
#include <Kuesa/GLTF2MaterialProperties>
#include <Kuesa/kuesa_global.h>


QT_BEGIN_NAMESPACE

namespace Kuesa {

class IroDiffuseShaderData;

class KUESASHARED_EXPORT IroDiffuseProperties : public GLTF2MaterialProperties
{
    Q_OBJECT
    Q_PROPERTY(QVector3D normalScaling READ normalScaling WRITE setNormalScaling NOTIFY normalScalingChanged)
    Q_PROPERTY(QVector2D normalDisturb READ normalDisturb WRITE setNormalDisturb NOTIFY normalDisturbChanged)
    Q_PROPERTY(float postVertexColor READ postVertexColor WRITE setPostVertexColor NOTIFY postVertexColorChanged)
    Q_PROPERTY(float postGain READ postGain WRITE setPostGain NOTIFY postGainChanged)
    Q_PROPERTY(float reflectionGain READ reflectionGain WRITE setReflectionGain NOTIFY reflectionGainChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture * reflectionMap READ reflectionMap WRITE setReflectionMap NOTIFY reflectionMapChanged)
    Q_PROPERTY(QVector3D reflectionInnerFilter READ reflectionInnerFilter WRITE setReflectionInnerFilter NOTIFY reflectionInnerFilterChanged)
    Q_PROPERTY(QVector3D reflectionOuterFilter READ reflectionOuterFilter WRITE setReflectionOuterFilter NOTIFY reflectionOuterFilterChanged)
    Q_PROPERTY(QVector3D diffuseInnerFilter READ diffuseInnerFilter WRITE setDiffuseInnerFilter NOTIFY diffuseInnerFilterChanged)
    Q_PROPERTY(QVector3D diffuseOuterFilter READ diffuseOuterFilter WRITE setDiffuseOuterFilter NOTIFY diffuseOuterFilterChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture * diffuseMap READ diffuseMap WRITE setDiffuseMap NOTIFY diffuseMapChanged)
    Q_PROPERTY(QVector3D diffuseFactor READ diffuseFactor WRITE setDiffuseFactor NOTIFY diffuseFactorChanged)
    Q_PROPERTY(bool usesDiffuseMap READ usesDiffuseMap WRITE setUsesDiffuseMap NOTIFY usesDiffuseMapChanged)

public:
    Q_INVOKABLE explicit IroDiffuseProperties(Qt3DCore::QNode *parent = nullptr);
    ~IroDiffuseProperties();

    Qt3DRender::QShaderData *shaderData() const override;
    QVector3D normalScaling() const;
    QVector2D normalDisturb() const;
    float postVertexColor() const;
    float postGain() const;
    float reflectionGain() const;
    Qt3DRender::QAbstractTexture * reflectionMap() const;
    QVector3D reflectionInnerFilter() const;
    QVector3D reflectionOuterFilter() const;
    QVector3D diffuseInnerFilter() const;
    QVector3D diffuseOuterFilter() const;
    Qt3DRender::QAbstractTexture * diffuseMap() const;
    QVector3D diffuseFactor() const;
    bool usesDiffuseMap() const;

public Q_SLOTS:
    void setNormalScaling(const QVector3D &normalScaling);
    void setNormalDisturb(const QVector2D &normalDisturb);
    void setPostVertexColor(float postVertexColor);
    void setPostGain(float postGain);
    void setReflectionGain(float reflectionGain);
    void setReflectionMap(Qt3DRender::QAbstractTexture * reflectionMap);
    void setReflectionInnerFilter(const QVector3D &reflectionInnerFilter);
    void setReflectionOuterFilter(const QVector3D &reflectionOuterFilter);
    void setDiffuseInnerFilter(const QVector3D &diffuseInnerFilter);
    void setDiffuseOuterFilter(const QVector3D &diffuseOuterFilter);
    void setDiffuseMap(Qt3DRender::QAbstractTexture * diffuseMap);
    void setDiffuseFactor(const QVector3D &diffuseFactor);
    void setUsesDiffuseMap(bool usesDiffuseMap);

Q_SIGNALS:
    void normalScalingChanged(QVector3D);
    void normalDisturbChanged(QVector2D);
    void postVertexColorChanged(float);
    void postGainChanged(float);
    void reflectionGainChanged(float);
    void reflectionMapChanged(Qt3DRender::QAbstractTexture *);
    void reflectionInnerFilterChanged(QVector3D);
    void reflectionOuterFilterChanged(QVector3D);
    void diffuseInnerFilterChanged(QVector3D);
    void diffuseOuterFilterChanged(QVector3D);
    void diffuseMapChanged(Qt3DRender::QAbstractTexture *);
    void diffuseFactorChanged(QVector3D);
    void usesDiffuseMapChanged(bool);

private:
    IroDiffuseShaderData *m_shaderData;
};
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_IRODIFFUSEPROPERTIES_H
