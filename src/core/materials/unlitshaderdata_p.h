/*
    unlitshaderdata_p.h

    This file is part of Kuesa.

    Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#ifndef KUESA_UNLITSHADERDATA_P_H
#define KUESA_UNLITSHADERDATA_P_H

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

#include <Qt3DCore/QComponent>
#include <Qt3DRender/QShaderData>
#include <Qt3DRender/QAbstractTexture>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class UnlitShaderData : public Qt3DRender::QShaderData
{
    Q_OBJECT

    Q_PROPERTY(bool baseColorUsesTexCoord1 READ isBaseColorUsingTexCoord1 WRITE setBaseColorUsesTexCoord1 NOTIFY baseColorUsesTexCoord1Changed)

    Q_PROPERTY(QColor baseColorFactor READ baseColorFactor WRITE setBaseColorFactor NOTIFY baseColorFactorChanged)

    Q_PROPERTY(float alphaCutoff READ alphaCutoff WRITE setAlphaCutoff NOTIFY alphaCutoffChanged)

public:
    explicit UnlitShaderData(Qt3DCore::QNode *parent = nullptr);
    ~UnlitShaderData();

    bool isBaseColorUsingTexCoord1() const;

    QColor baseColorFactor() const;
    Qt3DRender::QAbstractTexture *baseColorMap() const;

    float alphaCutoff() const;

public Q_SLOTS:
    void setBaseColorUsesTexCoord1(bool baseColorUsesTexCoord1);

    void setBaseColorFactor(const QColor &baseColorFactor);
    void setBaseColorMap(Qt3DRender::QAbstractTexture *baseColorMap);

    void setAlphaCutoff(float alphaCutoff);

Q_SIGNALS:
    void baseColorUsesTexCoord1Changed(bool);

    void baseColorFactorChanged(const QColor &baseColorFactor);
    void baseColorMapChanged(Qt3DRender::QAbstractTexture *baseColorMap);

    void alphaCutoffChanged(float alphaCutoff);

private:
    bool m_baseColorUsesTexCoord1;

    QColor m_baseColorFactor;
    Qt3DRender::QAbstractTexture *m_baseColorMap;

    float m_alphaCutoff;
};
} // namespace Kuesa
QT_END_NAMESPACE

#endif // KUESA_UNLITSHADERDATA_H
