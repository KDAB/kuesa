/*
    iromattealphaproperties.h

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

#ifndef KUESA_IROMATTEALPHAPROPERTIES_H
#define KUESA_IROMATTEALPHAPROPERTIES_H

#include <QVector2D>
#include <QVector3D>
#include <Qt3DRender/QAbstractTexture>
#include <Kuesa/GLTF2MaterialProperties>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {
class IroMatteAlphaShaderData;

class KUESASHARED_EXPORT IroMatteAlphaProperties : public GLTF2MaterialProperties
{
    Q_OBJECT
    Q_PROPERTY(float postVertexColor READ postVertexColor WRITE setPostVertexColor NOTIFY postVertexColorChanged)
    Q_PROPERTY(float postGain READ postGain WRITE setPostGain NOTIFY postGainChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *matteMap READ matteMap WRITE setMatteMap NOTIFY matteMapChanged)
    Q_PROPERTY(bool usesMatteMap READ usesMatteMap WRITE setUsesMatteMap NOTIFY usesMatteMapChanged)
    Q_PROPERTY(QVector3D matteFilter READ matteFilter WRITE setMatteFilter NOTIFY matteFilterChanged)
    Q_PROPERTY(float matteGain READ matteGain WRITE setMatteGain NOTIFY matteGainChanged)
    Q_PROPERTY(float matteAlphaGain READ matteAlphaGain WRITE setMatteAlphaGain NOTIFY matteAlphaGainChanged)
    Q_PROPERTY(QVector2D uvOffset READ uvOffset WRITE setUvOffset NOTIFY uvOffsetChanged)

public:
    Q_INVOKABLE explicit IroMatteAlphaProperties(Qt3DCore::QNode *parent = nullptr);
    ~IroMatteAlphaProperties();

    Qt3DRender::QShaderData *shaderData() const override;
    float postVertexColor() const;
    float postGain() const;
    Qt3DRender::QAbstractTexture *matteMap() const;
    bool usesMatteMap() const;
    QVector3D matteFilter() const;
    float matteGain() const;
    float matteAlphaGain() const;
    QVector2D uvOffset() const;

public Q_SLOTS:
    void setPostVertexColor(float postVertexColor);
    void setPostGain(float postGain);
    void setMatteMap(Qt3DRender::QAbstractTexture *matteMap);
    void setUsesMatteMap(bool usesMatteMap);
    void setMatteFilter(const QVector3D &matteFilter);
    void setMatteGain(float matteGain);
    void setMatteAlphaGain(float matteAlphaGain);
    void setUvOffset(const QVector2D &uvOffset);

Q_SIGNALS:
    void postVertexColorChanged(float);
    void postGainChanged(float);
    void matteMapChanged(Qt3DRender::QAbstractTexture *);
    void usesMatteMapChanged(bool);
    void matteFilterChanged(QVector3D);
    void matteGainChanged(float);
    void matteAlphaGainChanged(float);
    void uvOffsetChanged(QVector2D);

private:
    IroMatteAlphaShaderData *m_shaderData;
    Qt3DRender::QAbstractTexture *m_matteMap;
};

} // namespace Kuesa

QT_END_NAMESPACE
Q_DECLARE_METATYPE(Kuesa::IroMatteAlphaProperties *)
#endif // KUESA_IROMATTEALPHAPROPERTIES_H
