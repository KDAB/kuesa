
/*
    iromattebackgroundproperties.h

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

#ifndef KUESA_IROMATTEBACKGROUNDPROPERTIES_H
#define KUESA_IROMATTEBACKGROUNDPROPERTIES_H

#include <QVector3D>
#include <Qt3DRender/QAbstractTexture>
#include <Kuesa/GLTF2MaterialProperties>
#include <Kuesa/kuesa_global.h>


QT_BEGIN_NAMESPACE

namespace Kuesa {

class IroMatteBackgroundShaderData;

class KUESASHARED_EXPORT IroMatteBackgroundProperties : public GLTF2MaterialProperties
{
    Q_OBJECT
    Q_PROPERTY(float postVertexColor READ postVertexColor WRITE setPostVertexColor NOTIFY postVertexColorChanged)
    Q_PROPERTY(float postGain READ postGain WRITE setPostGain NOTIFY postGainChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture * matteMap READ matteMap WRITE setMatteMap NOTIFY matteMapChanged)
    Q_PROPERTY(bool usesMatteMap READ usesMatteMap WRITE setUsesMatteMap NOTIFY usesMatteMapChanged)
    Q_PROPERTY(QVector3D matteFilter READ matteFilter WRITE setMatteFilter NOTIFY matteFilterChanged)
    Q_PROPERTY(float matteGain READ matteGain WRITE setMatteGain NOTIFY matteGainChanged)
    Q_PROPERTY(bool gltfYUp READ gltfYUp WRITE setGltfYUp NOTIFY gltfYUpChanged)

public:
    Q_INVOKABLE explicit IroMatteBackgroundProperties(Qt3DCore::QNode *parent = nullptr);
    ~IroMatteBackgroundProperties();

    Qt3DRender::QShaderData *shaderData() const override;
    float postVertexColor() const;
    float postGain() const;
    Qt3DRender::QAbstractTexture * matteMap() const;
    bool usesMatteMap() const;
    QVector3D matteFilter() const;
    float matteGain() const;
    bool gltfYUp() const;

public Q_SLOTS:
    void setPostVertexColor(float postVertexColor);
    void setPostGain(float postGain);
    void setMatteMap(Qt3DRender::QAbstractTexture * matteMap);
    void setUsesMatteMap(bool usesMatteMap);
    void setMatteFilter(const QVector3D &matteFilter);
    void setMatteGain(float matteGain);
    void setGltfYUp(bool gltfYUp);

Q_SIGNALS:
    void postVertexColorChanged(float);
    void postGainChanged(float);
    void matteMapChanged(Qt3DRender::QAbstractTexture *);
    void usesMatteMapChanged(bool);
    void matteFilterChanged(QVector3D);
    void matteGainChanged(float);
    void gltfYUpChanged(bool);

private:
    IroMatteBackgroundShaderData *m_shaderData;
};
} // namespace Kuesa

QT_END_NAMESPACE
Q_DECLARE_METATYPE(Kuesa::IroMatteBackgroundProperties*)
#endif // KUESA_IROMATTEBACKGROUNDPROPERTIES_H
