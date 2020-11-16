
/*
    iro2mattealphashaderdata_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Nicolas Guichard <nicolas.guichard@kdab.com>

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

#ifndef KUESA_IRO2MATTEALPHASHADERDATA_P_H
#define KUESA_IRO2MATTEALPHASHADERDATA_P_H


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

class Iro2MatteAlphaShaderData : public Qt3DRender::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(bool usesMatteMap READ usesMatteMap WRITE setUsesMatteMap NOTIFY usesMatteMapChanged)
    Q_PROPERTY(float matteGain READ matteGain WRITE setMatteGain NOTIFY matteGainChanged)
    Q_PROPERTY(QVector3D matteInnerFilter READ matteInnerFilter WRITE setMatteInnerFilter NOTIFY matteInnerFilterChanged)
    Q_PROPERTY(QVector3D matteOuterFilter READ matteOuterFilter WRITE setMatteOuterFilter NOTIFY matteOuterFilterChanged)
    Q_PROPERTY(QVector2D uvOffset READ uvOffset WRITE setUvOffset NOTIFY uvOffsetChanged)
    Q_PROPERTY(float innerAlpha READ innerAlpha WRITE setInnerAlpha NOTIFY innerAlphaChanged)
    Q_PROPERTY(float outerAlpha READ outerAlpha WRITE setOuterAlpha NOTIFY outerAlphaChanged)
    Q_PROPERTY(float alphaGain READ alphaGain WRITE setAlphaGain NOTIFY alphaGainChanged)
    Q_PROPERTY(bool usesNormalMap READ usesNormalMap WRITE setUsesNormalMap NOTIFY usesNormalMapChanged)
    Q_PROPERTY(float normalMapGain READ normalMapGain WRITE setNormalMapGain NOTIFY normalMapGainChanged)
    Q_PROPERTY(QVector3D normalScaling READ normalScaling WRITE setNormalScaling NOTIFY normalScalingChanged)
    Q_PROPERTY(QVector2D normalDisturb READ normalDisturb WRITE setNormalDisturb NOTIFY normalDisturbChanged)
    Q_PROPERTY(float postVertexColor READ postVertexColor WRITE setPostVertexColor NOTIFY postVertexColorChanged)
    Q_PROPERTY(QVector3D postHemiFilter READ postHemiFilter WRITE setPostHemiFilter NOTIFY postHemiFilterChanged)
    Q_PROPERTY(float postGain READ postGain WRITE setPostGain NOTIFY postGainChanged)
    Q_PROPERTY(bool gltfYUp READ gltfYUp WRITE setGltfYUp NOTIFY gltfYUpChanged)

public:
    explicit Iro2MatteAlphaShaderData(Qt3DCore::QNode *parent = nullptr);
    ~Iro2MatteAlphaShaderData();
    bool usesMatteMap() const;
    float matteGain() const;
    QVector3D matteInnerFilter() const;
    QVector3D matteOuterFilter() const;
    QVector2D uvOffset() const;
    float innerAlpha() const;
    float outerAlpha() const;
    float alphaGain() const;
    bool usesNormalMap() const;
    float normalMapGain() const;
    QVector3D normalScaling() const;
    QVector2D normalDisturb() const;
    float postVertexColor() const;
    QVector3D postHemiFilter() const;
    float postGain() const;
    bool gltfYUp() const;

public Q_SLOTS:
    void setUsesMatteMap(bool usesMatteMap);
    void setMatteGain(float matteGain);
    void setMatteInnerFilter(const QVector3D &matteInnerFilter);
    void setMatteOuterFilter(const QVector3D &matteOuterFilter);
    void setUvOffset(const QVector2D &uvOffset);
    void setInnerAlpha(float innerAlpha);
    void setOuterAlpha(float outerAlpha);
    void setAlphaGain(float alphaGain);
    void setUsesNormalMap(bool usesNormalMap);
    void setNormalMapGain(float normalMapGain);
    void setNormalScaling(const QVector3D &normalScaling);
    void setNormalDisturb(const QVector2D &normalDisturb);
    void setPostVertexColor(float postVertexColor);
    void setPostHemiFilter(const QVector3D &postHemiFilter);
    void setPostGain(float postGain);
    void setGltfYUp(bool gltfYUp);

Q_SIGNALS:
    void usesMatteMapChanged(bool);
    void matteGainChanged(float);
    void matteInnerFilterChanged(QVector3D);
    void matteOuterFilterChanged(QVector3D);
    void uvOffsetChanged(QVector2D);
    void innerAlphaChanged(float);
    void outerAlphaChanged(float);
    void alphaGainChanged(float);
    void usesNormalMapChanged(bool);
    void normalMapGainChanged(float);
    void normalScalingChanged(QVector3D);
    void normalDisturbChanged(QVector2D);
    void postVertexColorChanged(float);
    void postHemiFilterChanged(QVector3D);
    void postGainChanged(float);
    void gltfYUpChanged(bool);

private:
    bool m_usesMatteMap;
    float m_matteGain;
    QVector3D m_matteInnerFilter;
    QVector3D m_matteOuterFilter;
    QVector2D m_uvOffset;
    float m_innerAlpha;
    float m_outerAlpha;
    float m_alphaGain;
    bool m_usesNormalMap;
    float m_normalMapGain;
    QVector3D m_normalScaling;
    QVector2D m_normalDisturb;
    float m_postVertexColor;
    QVector3D m_postHemiFilter;
    float m_postGain;
    bool m_gltfYUp;

};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_IRO2MATTEALPHASHADERDATA_P_H
