
/*
    iromattealphashaderdata_p.h

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

#ifndef KUESA_IROMATTEALPHASHADERDATA_P_H
#define KUESA_IROMATTEALPHASHADERDATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QVector2D>
#include <QVector3D>
#include <Qt3DRender/QShaderData>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class IroMatteAlphaShaderData : public Qt3DRender::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(float postVertexColor READ postVertexColor WRITE setPostVertexColor NOTIFY postVertexColorChanged)
    Q_PROPERTY(float postGain READ postGain WRITE setPostGain NOTIFY postGainChanged)
    Q_PROPERTY(bool usesMatteMap READ usesMatteMap WRITE setUsesMatteMap NOTIFY usesMatteMapChanged)
    Q_PROPERTY(QVector3D matteFilter READ matteFilter WRITE setMatteFilter NOTIFY matteFilterChanged)
    Q_PROPERTY(float matteGain READ matteGain WRITE setMatteGain NOTIFY matteGainChanged)
    Q_PROPERTY(float matteAlphaGain READ matteAlphaGain WRITE setMatteAlphaGain NOTIFY matteAlphaGainChanged)
    Q_PROPERTY(QVector2D uvOffset READ uvOffset WRITE setUvOffset NOTIFY uvOffsetChanged)

public:
    explicit IroMatteAlphaShaderData(Qt3DCore::QNode *parent = nullptr);
    ~IroMatteAlphaShaderData();
    float postVertexColor() const;
    float postGain() const;
    bool usesMatteMap() const;
    QVector3D matteFilter() const;
    float matteGain() const;
    float matteAlphaGain() const;
    QVector2D uvOffset() const;

public Q_SLOTS:
    void setPostVertexColor(float postVertexColor);
    void setPostGain(float postGain);
    void setUsesMatteMap(bool usesMatteMap);
    void setMatteFilter(const QVector3D &matteFilter);
    void setMatteGain(float matteGain);
    void setMatteAlphaGain(float matteAlphaGain);
    void setUvOffset(const QVector2D &uvOffset);

Q_SIGNALS:
    void postVertexColorChanged(float);
    void postGainChanged(float);
    void usesMatteMapChanged(bool);
    void matteFilterChanged(QVector3D);
    void matteGainChanged(float);
    void matteAlphaGainChanged(float);
    void uvOffsetChanged(QVector2D);

private:
    float m_postVertexColor;
    float m_postGain;
    bool m_usesMatteMap;
    QVector3D m_matteFilter;
    float m_matteGain;
    float m_matteAlphaGain;
    QVector2D m_uvOffset;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_IROMATTEALPHASHADERDATA_P_H
