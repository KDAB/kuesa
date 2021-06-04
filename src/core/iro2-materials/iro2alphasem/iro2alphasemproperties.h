/*
    iro2alphasemproperties.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    This file was auto-generated

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

#ifndef KUESA_IRO2ALPHASEMPROPERTIES_H
#define KUESA_IRO2ALPHASEMPROPERTIES_H

#include <QVector2D>
#include <QVector3D>
#include <Qt3DRender/QAbstractTexture>
#include <Kuesa/GLTF2MaterialProperties>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {
class Iro2AlphaSemShaderData;

class KUESASHARED_EXPORT Iro2AlphaSemProperties : public GLTF2MaterialProperties
{
    Q_OBJECT
    Q_PROPERTY(bool usesReflectionMap READ usesReflectionMap WRITE setUsesReflectionMap NOTIFY usesReflectionMapChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *reflectionMap READ reflectionMap WRITE setReflectionMap NOTIFY reflectionMapChanged)
    Q_PROPERTY(float reflectionGain READ reflectionGain WRITE setReflectionGain NOTIFY reflectionGainChanged)
    Q_PROPERTY(QVector3D reflectionInnerFilter READ reflectionInnerFilter WRITE setReflectionInnerFilter NOTIFY reflectionInnerFilterChanged)
    Q_PROPERTY(QVector3D reflectionOuterFilter READ reflectionOuterFilter WRITE setReflectionOuterFilter NOTIFY reflectionOuterFilterChanged)
    Q_PROPERTY(float innerAlpha READ innerAlpha WRITE setInnerAlpha NOTIFY innerAlphaChanged)
    Q_PROPERTY(float outerAlpha READ outerAlpha WRITE setOuterAlpha NOTIFY outerAlphaChanged)
    Q_PROPERTY(float alphaGain READ alphaGain WRITE setAlphaGain NOTIFY alphaGainChanged)
    Q_PROPERTY(bool usesNormalMap READ usesNormalMap WRITE setUsesNormalMap NOTIFY usesNormalMapChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)
    Q_PROPERTY(float normalMapGain READ normalMapGain WRITE setNormalMapGain NOTIFY normalMapGainChanged)
    Q_PROPERTY(QVector3D normalScaling READ normalScaling WRITE setNormalScaling NOTIFY normalScalingChanged)
    Q_PROPERTY(QVector2D normalDisturb READ normalDisturb WRITE setNormalDisturb NOTIFY normalDisturbChanged)
    Q_PROPERTY(float postVertexColor READ postVertexColor WRITE setPostVertexColor NOTIFY postVertexColorChanged)
    Q_PROPERTY(QVector3D postHemiFilter READ postHemiFilter WRITE setPostHemiFilter NOTIFY postHemiFilterChanged)
    Q_PROPERTY(float postGain READ postGain WRITE setPostGain NOTIFY postGainChanged)
    Q_PROPERTY(bool gltfYUp READ gltfYUp WRITE setGltfYUp NOTIFY gltfYUpChanged)

public:
    Q_INVOKABLE explicit Iro2AlphaSemProperties(Qt3DCore::QNode *parent = nullptr);
    ~Iro2AlphaSemProperties();

    Qt3DRender::QShaderData *shaderData() const override;
    bool usesReflectionMap() const;
    Qt3DRender::QAbstractTexture *reflectionMap() const;
    float reflectionGain() const;
    QVector3D reflectionInnerFilter() const;
    QVector3D reflectionOuterFilter() const;
    float innerAlpha() const;
    float outerAlpha() const;
    float alphaGain() const;
    bool usesNormalMap() const;
    Qt3DRender::QAbstractTexture *normalMap() const;
    float normalMapGain() const;
    QVector3D normalScaling() const;
    QVector2D normalDisturb() const;
    float postVertexColor() const;
    QVector3D postHemiFilter() const;
    float postGain() const;
    bool gltfYUp() const;

public Q_SLOTS:
    void setUsesReflectionMap(bool usesReflectionMap);
    void setReflectionMap(Qt3DRender::QAbstractTexture *reflectionMap);
    void setReflectionGain(float reflectionGain);
    void setReflectionInnerFilter(const QVector3D &reflectionInnerFilter);
    void setReflectionOuterFilter(const QVector3D &reflectionOuterFilter);
    void setInnerAlpha(float innerAlpha);
    void setOuterAlpha(float outerAlpha);
    void setAlphaGain(float alphaGain);
    void setUsesNormalMap(bool usesNormalMap);
    void setNormalMap(Qt3DRender::QAbstractTexture *normalMap);
    void setNormalMapGain(float normalMapGain);
    void setNormalScaling(const QVector3D &normalScaling);
    void setNormalDisturb(const QVector2D &normalDisturb);
    void setPostVertexColor(float postVertexColor);
    void setPostHemiFilter(const QVector3D &postHemiFilter);
    void setPostGain(float postGain);
    void setGltfYUp(bool gltfYUp);

Q_SIGNALS:
    void usesReflectionMapChanged(bool);
    void reflectionMapChanged(Qt3DRender::QAbstractTexture *);
    void reflectionGainChanged(float);
    void reflectionInnerFilterChanged(QVector3D);
    void reflectionOuterFilterChanged(QVector3D);
    void innerAlphaChanged(float);
    void outerAlphaChanged(float);
    void alphaGainChanged(float);
    void usesNormalMapChanged(bool);
    void normalMapChanged(Qt3DRender::QAbstractTexture *);
    void normalMapGainChanged(float);
    void normalScalingChanged(QVector3D);
    void normalDisturbChanged(QVector2D);
    void postVertexColorChanged(float);
    void postHemiFilterChanged(QVector3D);
    void postGainChanged(float);
    void gltfYUpChanged(bool);

private:
    Iro2AlphaSemShaderData *m_shaderData;
    Qt3DRender::QAbstractTexture *m_reflectionMap;
    Qt3DRender::QAbstractTexture *m_normalMap;
};

} // namespace Kuesa

QT_END_NAMESPACE
Q_DECLARE_METATYPE(Kuesa::Iro2AlphaSemProperties *)
#endif // KUESA_IRO2ALPHASEMPROPERTIES_H
