/*
    textureinspector.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef TEXTUREINSPECTOR_H
#define TEXTUREINSPECTOR_H

#include <QObject>
#include <QString>
#include <QAbstractTableModel>
#include <Qt3DRender/QTexture>

namespace Kuesa {
class AbstractAssetCollection;
}

class TextureImagesModel;

class TextureInspector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QAbstractTexture *texture READ texture NOTIFY textureParamsChanged)
public:
    explicit TextureInspector(QObject *parent = nullptr);

    void setTexture(const QString &name, Qt3DRender::QAbstractTexture *texture);

    Qt3DRender::QAbstractTexture *texture() const;
    TextureImagesModel *textureImagesModel() const;

    QString assetName() const;
    int width() const;
    int height() const;
    int depth() const;
    int layers() const;
    int samples() const;
    QString target() const;
    QString format() const;
    QString wrapXMode() const;
    QString wrapYMode() const;
    QString wrapZMode() const;
    QString minificationFilter() const;
    QString magnificationFilter() const;

Q_SIGNALS:
    void textureParamsChanged();

private:
    QString m_assetName;
    Qt3DRender::QAbstractTexture *m_texture;
    TextureImagesModel *m_model;
    QMetaObject::Connection m_textureConnection;
};

class TextureImagesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum AttributeColumns {
        ImageNameColumn = 0,
        MipLevelColumn,
        LayerColumn,
        FaceColumn,
        SourceColumn,
        MirroredColumn,
        LastColumn
    };
    Q_ENUM(AttributeColumns)

    explicit TextureImagesModel(QObject *parent = nullptr);

    void setTexture(Qt3DRender::QAbstractTexture *texture);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    Qt3DRender::QAbstractTexture *m_texture;
    QVector<Qt3DRender::QAbstractTextureImage *> m_images;
};

#endif // TEXTUREINSPECTOR_H
