/*
    collectionbrowser.cpp

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

#include "collectionbrowser.h"
#include "collectionmodel.h"

#include <Kuesa/CameraCollection>
#include <Kuesa/AnimationClipCollection>
#include <Kuesa/AnimationMappingCollection>
#include <Kuesa/SceneEntity>
#include <Kuesa/AnimationPlayer>
#include <Qt3DAnimation/QClipAnimator>
#include <Qt3DAnimation/QChannelMapping>
#include <QItemSelectionModel>

CollectionBrowser::CollectionBrowser(QWidget *parent)
    : QTreeView(parent)
{
    connect(this, &QTreeView::doubleClicked, this, &CollectionBrowser::onAssetDoubleClicked);
}

void CollectionBrowser::setModel(QAbstractItemModel *model)
{
    QTreeView::setModel(model);
    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &CollectionBrowser::onSelectionChanged);
}

void CollectionBrowser::onAssetDoubleClicked(const QModelIndex &index)
{
    auto data = assetForIndex(index);
    if (!data.first || data.second.isEmpty())
        return;

    Kuesa::CameraCollection *cameras = qobject_cast<Kuesa::CameraCollection *>(data.first);
    if (cameras && cameras->names().contains(data.second)) {
        emit viewCamera(data.second);
        return;
    }

    Kuesa::AnimationClipCollection *animationClips = qobject_cast<Kuesa::AnimationClipCollection *>(data.first);
    if (animationClips && animationClips->names().contains(data.second)) {
        Qt3DAnimation::QAbstractAnimationClip *clip = animationClips->find(data.second);
        if (clip) {
            const CollectionModel *model = qobject_cast<const CollectionModel *>(index.model());

            auto player = new Kuesa::AnimationPlayer(model->entity());
            player->setSceneEntity(model->entity());
            player->setClip(data.second);
            if (player->status() == Kuesa::AnimationPlayer::Ready) {
                player->start();
                connect(player, &Kuesa::AnimationPlayer::runningChanged, [player](bool running) {
                    if (!running)
                        player->deleteLater();
                });
            } else {
                delete player;
            }
        }
        return;
    }
}

void CollectionBrowser::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    const auto index = selected.indexes().isEmpty() ? QModelIndex() : selected.indexes().first();
    auto data = assetForIndex(index);
    emit assetSelected(data.second, data.first);
}

QPair<Kuesa::AbstractAssetCollection *, QString> CollectionBrowser::assetForIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return {};

    const CollectionModel *model = qobject_cast<const CollectionModel *>(index.model());
    if (!model)
        return {};

    auto collection = model->collection(index);
    if (!collection)
        return {};

    return qMakePair(collection, index.data().toString());
}
