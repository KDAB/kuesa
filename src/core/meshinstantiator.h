/*
    meshinstantiator.h

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

#ifndef KUESA_MESHINSTANTIATOR_H
#define KUESA_MESHINSTANTIATOR_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/KuesaNode>
#include <QMetaObject>

QT_BEGIN_NAMESPACE

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
namespace Qt3DGeometry = Qt3DCore;
namespace Qt3DCore {
#else
namespace Qt3DGeometry = Qt3DRender;
namespace Qt3DRender {
#endif
class QAttribute;
class QBuffer;
}

namespace Kuesa {

class KUESASHARED_EXPORT MeshInstantiator : public KuesaNode
{
    Q_OBJECT
    Q_PROPERTY(QString entityName READ entityName WRITE setEntityName NOTIFY entityNameChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit MeshInstantiator(Qt3DCore::QNode *parent = nullptr);
    ~MeshInstantiator() = default;

    int count() const;

    void setEntityName(const QString &entityName);
    QString entityName() const;

    void setTransformationMatrices(const std::vector<QMatrix4x4> &transformationMatrices);
    const std::vector<QMatrix4x4> &transformationMatrices() const;

Q_SIGNALS:
    void countChanged(int count);
    void transformationMatricesChanged(const std::vector<QMatrix4x4> &transformationMatrices);
    void entityNameChanged(const QString &entityName);

private:
    void updateTransformBuffer();
    void update();

    std::vector<QMatrix4x4> m_transformations;
    QString m_entityName;
    QMetaObject::Connection m_loadingDoneConnection;
    Qt3DGeometry::QBuffer *m_transformationsBuffer = nullptr;
    Qt3DGeometry::QAttribute *m_perInstanceTransformationAttribute = nullptr;
    Qt3DCore::QEntity *m_entity = nullptr;
    size_t m_instanceCount = 1;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_MESHINSTANTIATOR_H
