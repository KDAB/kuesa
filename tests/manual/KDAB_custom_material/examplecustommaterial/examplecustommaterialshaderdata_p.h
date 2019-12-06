
/*
    examplecustommaterialshaderdata_p.h

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

#ifndef KUESA_EXAMPLECUSTOMMATERIALSHADERDATA_P_H
#define KUESA_EXAMPLECUSTOMMATERIALSHADERDATA_P_H

#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QShaderData>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class ExampleCustomMaterialShaderData : public Qt3DRender::QShaderData
{
   Q_OBJECT
    Q_PROPERTY(float modulator READ modulator WRITE setModulator NOTIFY modulatorChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture * inputTexture READ inputTexture WRITE setInputTexture NOTIFY inputTextureChanged)

public:
    explicit ExampleCustomMaterialShaderData(Qt3DCore::QNode *parent = nullptr);
    ~ExampleCustomMaterialShaderData();
    float modulator() const;
    Qt3DRender::QAbstractTexture * inputTexture() const;

public Q_SLOTS:
    void setModulator(float modulator);
    void setInputTexture(Qt3DRender::QAbstractTexture * inputTexture);

Q_SIGNALS:
    void modulatorChanged(float);
    void inputTextureChanged(Qt3DRender::QAbstractTexture *);

private:
    float m_modulator;
    Qt3DRender::QAbstractTexture * m_inputTexture;

};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_EXAMPLECUSTOMMATERIALSHADERDATA_P_H
