/*
    materialinspector.h

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

#ifndef MATERIALINSPECTOR_H
#define MATERIALINSPECTOR_H

#include<QObject>
#include <Kuesa/GLTF2Material>

namespace Qt3DCore {
class QEntity;
}

namespace Qt3DRender {
class QPickEvent;
}

class MaterialInspector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Kuesa::GLTF2Material *material READ material NOTIFY materialChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
public:
    explicit MaterialInspector(QObject *parent = nullptr);

    Kuesa::GLTF2Material *material() const;
    QString description() const;

    Q_INVOKABLE void inspect(Qt3DRender::QPickEvent *pick);

signals:
    void materialChanged();
    void descriptionChanged();

private:
    void setMaterial(Kuesa::GLTF2Material *material);
    void introspect();

    Kuesa::GLTF2Material *m_material;
    QString m_description;
};

#endif // MATERIALINSPECTOR_H
