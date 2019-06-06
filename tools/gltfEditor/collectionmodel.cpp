/*
    collectionmodel.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "collectionmodel.h"
#include "mainwindow.h"

#include <Kuesa/AbstractAssetCollection>
#include <Kuesa/SceneEntity>

CollectionModel::CollectionModel(MainWindow *mw, QObject *parent)
    : QAbstractItemModel(parent)
    , m_entity(nullptr)
    , m_window(mw)
{
    connect(m_window, &MainWindow::activeCameraChanged, this, [this]() {
        emit dataChanged(index(0, 0, {}), index(m_collections.size() - 1, 1, {}));
    });
}

Kuesa::SceneEntity *CollectionModel::entity() const
{
    return m_entity;
}

void CollectionModel::update(Kuesa::SceneEntity *entity)
{
    beginResetModel();
    m_entity = entity;
    m_collections.clear();
    if (m_entity) {
        if (m_entity->entities()->size())
            m_collections << m_entity->entities();
        if (m_entity->meshes()->size())
            m_collections << m_entity->meshes();
        if (m_entity->layers()->size())
            m_collections << m_entity->layers();
        if (m_entity->materials()->size())
            m_collections << m_entity->materials();
        if (m_entity->effects()->size())
            m_collections << m_entity->effects();
        if (m_entity->textures()->size())
            m_collections << m_entity->textures();
        if (m_entity->textureImages()->size())
            m_collections << m_entity->textureImages();
        if (m_entity->animationClips()->size())
            m_collections << m_entity->animationClips();
        if (m_entity->animationMappings()->size())
            m_collections << m_entity->animationMappings();
        if (m_entity->armatures()->size())
            m_collections << m_entity->armatures();
        if (m_entity->skeletons()->size())
            m_collections << m_entity->skeletons();
        if (m_entity->cameras()->size())
            m_collections << m_entity->cameras();
    }
    endResetModel();
}

Kuesa::AbstractAssetCollection *CollectionModel::collection(const QModelIndex &index) const
{
    if (index.isValid())
        return reinterpret_cast<Kuesa::AbstractAssetCollection *>(index.internalPointer());
    return nullptr;
}

QModelIndex CollectionModel::index(Kuesa::AbstractAssetCollection *collection, QString name) const
{
    QModelIndex parent = index(m_collections.indexOf(collection), 0, {});
    return index(collection->names().indexOf(name), 0, parent);
}

QModelIndex CollectionModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!m_entity)
        return {};

    auto r = createIndex(row, column);
    if (parent.isValid()) {
        Q_ASSERT(parent.row() >= 0 && parent.row() < m_collections.size());
        r = createIndex(row, column, (void *)m_collections.at(parent.row()));
    }
    return r;
}

QModelIndex CollectionModel::parent(const QModelIndex &child) const
{
    Kuesa::AbstractAssetCollection *c = collection(child);
    if (c)
        return createIndex(m_collections.indexOf(c), child.column());
    return {};
}

int CollectionModel::rowCount(const QModelIndex &parent) const
{
    if (!m_entity)
        return 0;

    int r = 0;
    if (parent.isValid()) {
        Kuesa::AbstractAssetCollection *c = collection(parent);
        if (!c)
            r = m_collections[parent.row()]->size();
    } else {
        r = m_collections.size();
    }

    return r;
}

int CollectionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant CollectionModel::data(const QModelIndex &index, int role) const
{
    if (!m_entity || !index.isValid())
        return {};

    Kuesa::AbstractAssetCollection *c = collection(index);
    auto boldFont = [this]() -> QFont {
        auto f = m_window->font();
        f.setBold(true);
        return f;
    };

    if (c) {
        // leaf node
        switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return c->names().at(index.row());
        case Qt::FontRole: {
            Kuesa::CameraCollection *cameraCollection = qobject_cast<Kuesa::CameraCollection *>(c);
            if (cameraCollection && index.row() == m_window->activeCamera())
                return boldFont();
            return {};
        }
        default:
            break;
        }
    } else {
        // collection node
        switch (role) {
        case Qt::DisplayRole:
            return QString(QLatin1String("%1 (%2)"))
                    .arg(m_collections.at(index.row())->metaObject()->className())
                    .arg(m_collections.at(index.row())->size());
        default:
            break;
        }
    }

    return {};
}
