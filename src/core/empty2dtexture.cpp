/*
    empty2dtexture.cpp

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

#include "empty2dtexture_p.h"
#include <Qt3DRender/QTextureDataUpdate>
#include <Qt3DRender/QTextureWrapMode>

QT_BEGIN_NAMESPACE

namespace Kuesa {

Empty2DTexture::Empty2DTexture(Qt3DCore::QNode *parent)
    : Qt3DRender::QTexture2D(parent)
    , m_color(Qt::black)
{
    setFormat(Qt3DRender::QAbstractTexture::RGBA8_UNorm);
    setWrapMode(Qt3DRender::QTextureWrapMode(Qt3DRender::QTextureWrapMode::Repeat));

    QObject::connect(this, &Empty2DTexture::colorChanged,
                     this, &Empty2DTexture::updateTextureData);
    updateTextureData();
}

Empty2DTexture::~Empty2DTexture()
{
}

QColor Empty2DTexture::color() const
{
    return m_color;
}

void Empty2DTexture::setColor(const QColor &color)
{
    if (color == m_color)
        return;
    m_color = color;
    emit colorChanged();
}

void Empty2DTexture::updateTextureData()
{
    QByteArray rawData;
    rawData.resize(4 * sizeof(uchar));
    uchar *data = reinterpret_cast<uchar *>(rawData.data());
    data[0] = uchar(m_color.red());
    data[1] = uchar(m_color.green());
    data[2] = uchar(m_color.blue());
    data[3] = uchar(m_color.alpha());

    Qt3DRender::QTextureImageDataPtr imageData = Qt3DRender::QTextureImageDataPtr::create();
    imageData->setData(rawData, 1, false);
    imageData->setWidth(1);
    imageData->setHeight(1);
    imageData->setDepth(1);
    imageData->setFaces(1);
    imageData->setLayers(1);
    imageData->setMipLevels(1);
    imageData->setPixelFormat(QOpenGLTexture::RGBA);
    imageData->setPixelType(QOpenGLTexture::UInt8);
    imageData->setFormat(QOpenGLTexture::RGBA8_UNorm);

    Qt3DRender::QTextureDataUpdate update;
    update.setData(imageData);

    updateData(update);
}

} // namespace Kuesa

QT_END_NAMESPACE
