/*
    collectionbrowser.h

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

#ifndef COLLECTIONBROWSER_H
#define COLLECTIONBROWSER_H

#include <QTreeView>

namespace Kuesa {
class AbstractAssetCollection;
class AnimationPlayer;
} // namespace Kuesa

class CollectionBrowser : public QTreeView
{
    Q_OBJECT
public:
    explicit CollectionBrowser(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model) override;

signals:
    void viewCamera(const QString &cameraName);
    void assetSelected(const QString &assetName, Kuesa::AbstractAssetCollection *collection);

private:
    void onAssetDoubleClicked(const QModelIndex &index);
    QPair<Kuesa::AbstractAssetCollection *, QString> assetForIndex(const QModelIndex &index);
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif // COLLECTIONBROWSER_H
