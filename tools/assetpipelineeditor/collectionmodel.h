/*
    collectionmodel.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#ifndef COLLECTIONMODEL_H
#define COLLECTIONMODEL_H

#include <QAbstractItemModel>
#include <QVector>

namespace Kuesa {
class SceneEntity;
class AbstractAssetCollection;
}

class MainWindow;

class CollectionModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit CollectionModel(MainWindow *mw, QObject *parent = nullptr);

    Kuesa::SceneEntity *entity() const;
    void update(Kuesa::SceneEntity *entity);
    Kuesa::AbstractAssetCollection *collection(const QModelIndex &index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    Kuesa::SceneEntity *m_entity;
    MainWindow *m_window;
    QVector<Kuesa::AbstractAssetCollection*> m_collections;
};

#endif // COLLECTIONMODEL_H
