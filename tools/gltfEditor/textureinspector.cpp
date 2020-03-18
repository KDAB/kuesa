/*
    textureinspector.cpp

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

#include "textureinspector.h"
#include <QMetaEnum>
#include <QFileInfo>

#include <Kuesa/private/textureparser_p.h>

namespace {

template<typename Obj>
QHash<int, QString> enumToNameMap(const char *enumName)
{
    const QMetaObject metaObj = Obj::staticMetaObject;
    const int indexOfEnum = metaObj.indexOfEnumerator(enumName);
    const QMetaEnum metaEnum = metaObj.enumerator(indexOfEnum);
    const int keysCount = metaEnum.keyCount();

    QHash<int, QString> v;
    v.reserve(keysCount);
    for (int i = 0; i < keysCount; ++i)
        v[metaEnum.value(i)] = metaEnum.key(i);
    return v;
}

} // namespace

TextureInspector::TextureInspector(QObject *parent)
    : QObject(parent)
    , m_texture(nullptr)
    , m_model(new TextureImagesModel(this))
{
}

void TextureInspector::setTexture(const QString &name, Qt3DRender::QAbstractTexture *texture)
{
    if (m_texture == texture)
        return;

    m_assetName = name;
    m_texture = texture;
    m_model->setTexture(texture);

    disconnect(m_textureConnection);

    if (m_texture)
        m_textureConnection = QObject::connect(texture,
                                               &Qt3DCore::QNode::nodeDestroyed,
                                               this, [this] { setTexture(QString(), nullptr); });

    emit textureParamsChanged();
}

Qt3DRender::QAbstractTexture *TextureInspector::texture() const
{
    return m_texture;
}

TextureImagesModel *TextureInspector::textureImagesModel() const
{
    return m_model;
}

QString TextureInspector::assetName() const
{
    return m_assetName;
}

int TextureInspector::width() const
{
    return (m_texture != nullptr) ? m_texture->width() : 0;
}

int TextureInspector::height() const
{
    return (m_texture != nullptr) ? m_texture->height() : 0;
}

int TextureInspector::depth() const
{
    return (m_texture != nullptr) ? m_texture->depth() : 0;
}

int TextureInspector::layers() const
{
    return (m_texture != nullptr) ? m_texture->layers() : 0;
}

int TextureInspector::samples() const
{
    return (m_texture != nullptr) ? m_texture->samples() : 0;
}

QString TextureInspector::target() const
{
    static const auto targetsToName = enumToNameMap<Qt3DRender::QAbstractTexture>("Target");
    return (m_texture != nullptr) ? targetsToName.value(m_texture->target(), QStringLiteral("Undefined")) : QStringLiteral("Undefined");
}

QString TextureInspector::format() const
{
    static const auto formatsToName = enumToNameMap<Qt3DRender::QAbstractTexture>("TextureFormat");
    return (m_texture != nullptr) ? formatsToName.value(m_texture->format(), QStringLiteral("Undefined")) : QStringLiteral("Undefined");
}

QString TextureInspector::wrapXMode() const
{
    static const auto wrapModesToName = enumToNameMap<Qt3DRender::QTextureWrapMode>("WrapMode");
    return (m_texture != nullptr) ? wrapModesToName.value(m_texture->wrapMode()->x(), QStringLiteral("Undefined")) : QStringLiteral("Undefined");
}

QString TextureInspector::wrapYMode() const
{
    static const auto wrapModesToName = enumToNameMap<Qt3DRender::QTextureWrapMode>("WrapMode");
    return (m_texture != nullptr) ? wrapModesToName.value(m_texture->wrapMode()->y(), QStringLiteral("Undefined")) : QStringLiteral("Undefined");
}

QString TextureInspector::wrapZMode() const
{
    static const auto wrapModesToName = enumToNameMap<Qt3DRender::QTextureWrapMode>("WrapMode");
    return (m_texture != nullptr) ? wrapModesToName.value(m_texture->wrapMode()->z(), QStringLiteral("Undefined")) : QStringLiteral("Undefined");
}

QString TextureInspector::minificationFilter() const
{
    static const auto filtersToName = enumToNameMap<Qt3DRender::QAbstractTexture>("Filter");
    return (m_texture != nullptr) ? filtersToName.value(m_texture->minificationFilter(), QStringLiteral("Undefined")) : QStringLiteral("Undefined");
}

QString TextureInspector::magnificationFilter() const
{
    static const auto filtersToName = enumToNameMap<Qt3DRender::QAbstractTexture>("Filter");
    return (m_texture != nullptr) ? filtersToName.value(m_texture->magnificationFilter(), QStringLiteral("Undefined")) : QStringLiteral("Undefined");
}

int TextureInspector::textureSize() const
{
    return ::textureSizeInBytes(m_texture);
}

TextureImagesModel::TextureImagesModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_texture(nullptr)
{
}

void TextureImagesModel::setTexture(Qt3DRender::QAbstractTexture *texture)
{
    if (m_texture == texture)
        return;

    m_texture = texture;
    beginResetModel();
    m_images = (m_texture != nullptr) ? m_texture->textureImages() : QVector<Qt3DRender::QAbstractTextureImage *>();
    endResetModel();
}

int TextureImagesModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_images.size();
}

int TextureImagesModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : LastColumn;
}

QVariant TextureImagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (!(role == Qt::DisplayRole || role == Qt::EditRole))
        return QVariant();

    const Qt3DRender::QAbstractTextureImage *img = m_images.at(index.row());
    const Qt3DRender::QTextureImage *sourceImg = qobject_cast<const Qt3DRender::QTextureImage *>(img);

    switch (index.column()) {
    case ImageNameColumn:
        return img->objectName();
    case MipLevelColumn:
        return img->mipLevel();
    case LayerColumn:
        return img->layer();
    case FaceColumn:
        return img->face();
    case SourceColumn:
        if (sourceImg != nullptr)
            return sourceImg->source();
        break;
    case MirroredColumn:
        if (sourceImg != nullptr)
            return sourceImg->isMirrored();
        return false;
    default:
        break;
    }

    return QVariant();
}

QVariant TextureImagesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const auto columnsToName = enumToNameMap<TextureImagesModel>("AttributeColumns");

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return columnsToName.value(section);
    return QVariant();
}

int textureSizeInBytes(Qt3DRender::QAbstractTexture *texture)
{
    if (!texture)
        return 0;

    auto mipmapMultiplier = 1.0;
    if (texture->generateMipMaps())
        // A mipmap chain uses 1/3 more space than the normal image
        mipmapMultiplier = 4.0/3.0;
    // texture can be of two types. QTextureLoader or QTexture2D
    // We use QTextureLoader for dds files and QTexture2D for png/jpg files
    auto textureLoader = qobject_cast<Qt3DRender::QTextureLoader *>(texture);
    if (textureLoader) {
        QFileInfo fInfo;
        fInfo.setFile(textureLoader->source().toString());
        if (fInfo.exists())
            return static_cast<int>(mipmapMultiplier*fInfo.size());
    } else {
        // FIXME Check that all textures we upload coming from a glTF file have 4 channels
        return static_cast<int>(mipmapMultiplier * texture->width() * texture->height() * 4);
    }
    return std::numeric_limits<int>::quiet_NaN();
}
