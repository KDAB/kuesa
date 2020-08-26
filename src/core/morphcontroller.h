/*
    morphcontroller.h

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_MORPHCONTROLLER_H
#define KUESA_MORPHCONTROLLER_H

#include <Qt3DCore/QComponent>
#include <Qt3DRender/QShaderData>
#include <Kuesa/kuesa_global.h>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class KUESASHARED_EXPORT MorphController : public Qt3DRender::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(float weight0 READ weight0 WRITE setWeight0 NOTIFY weight0Changed)
    Q_PROPERTY(float weight1 READ weight1 WRITE setWeight1 NOTIFY weight1Changed)
    Q_PROPERTY(float weight2 READ weight2 WRITE setWeight2 NOTIFY weight2Changed)
    Q_PROPERTY(float weight3 READ weight3 WRITE setWeight3 NOTIFY weight3Changed)
    Q_PROPERTY(float weight4 READ weight4 WRITE setWeight4 NOTIFY weight4Changed)
    Q_PROPERTY(float weight5 READ weight5 WRITE setWeight5 NOTIFY weight5Changed)
    Q_PROPERTY(float weight6 READ weight6 WRITE setWeight6 NOTIFY weight6Changed)
    Q_PROPERTY(float weight7 READ weight7 WRITE setWeight7 NOTIFY weight7Changed)
    Q_PROPERTY(QVariantList morphWeights READ morphWeights WRITE setMorphWeights NOTIFY morphWeightsChanged)

public:
    explicit MorphController(Qt3DCore::QNode *parent = nullptr);
    ~MorphController();

    float weight0() const;
    float weight1() const;
    float weight2() const;
    float weight3() const;
    float weight4() const;
    float weight5() const;
    float weight6() const;
    float weight7() const;
    int count() const;
    QVariantList morphWeights() const;

    void setWeight0(float weight0);
    void setWeight1(float weight1);
    void setWeight2(float weight2);
    void setWeight3(float weight3);
    void setWeight4(float weight4);
    void setWeight5(float weight5);
    void setWeight6(float weight6);
    void setWeight7(float weight7);
    void setCount(int count);
    void setMorphWeights(const QVariantList &weights);

Q_SIGNALS:
    void weight0Changed(float weight0);
    void weight1Changed(float weight1);
    void weight2Changed(float weight2);
    void weight3Changed(float weight3);
    void weight4Changed(float weight4);
    void weight5Changed(float weight5);
    void weight6Changed(float weight6);
    void weight7Changed(float weight7);
    void countChanged(int count);
    void morphWeightsChanged();

private:
    float m_weights[8];
    int m_count;
    bool m_shouldEmitMorphWeights;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_MORPHCONTROLLER_H
