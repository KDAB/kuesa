/*
    embeddedtextureimage.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "embeddedtextureimage_p.h"

QT_BEGIN_NAMESPACE
using namespace Kuesa;
using namespace GLTF2Import;

namespace {

class EmbeddedTextureImageFunctor : public Qt3DRender::QTextureImageDataGenerator
{
public:
    QT_WARNING_PUSH
    QT_WARNING_DISABLE_DEPRECATED
    EmbeddedTextureImageFunctor(const QImage &image)
        : m_image(image)
    {
    }
    QT_WARNING_POP

    Qt3DRender::QTextureImageDataPtr operator()() override
    {
        Qt3DRender::QTextureImageDataPtr dataPtr = Qt3DRender::QTextureImageDataPtr::create();
        dataPtr->setImage(m_image);
        return dataPtr;
    }

    bool operator==(const Qt3DRender::QTextureImageDataGenerator &other) const override
    {
        const EmbeddedTextureImageFunctor *otherFunctor = functor_cast<EmbeddedTextureImageFunctor>(&other);
        return (otherFunctor != nullptr && otherFunctor->m_image == m_image);
    }

    QT_WARNING_PUSH
    QT_WARNING_DISABLE_DEPRECATED
    QT3D_FUNCTOR(EmbeddedTextureImageFunctor)
    QT_WARNING_POP

private:
    QImage m_image;
};

} // namespace

EmbeddedTextureImage::EmbeddedTextureImage(const QImage &image, Qt3DCore::QNode *parent)
    : Qt3DRender::QAbstractTextureImage(parent), m_image(image)
{
}

EmbeddedTextureImage::~EmbeddedTextureImage()
{
}

Qt3DRender::QTextureImageDataGeneratorPtr EmbeddedTextureImage::dataGenerator() const
{
    return Qt3DRender::QTextureImageDataGeneratorPtr(new EmbeddedTextureImageFunctor(m_image));
}

QImage EmbeddedTextureImage::image()
{
    return m_image;
}

QT_END_NAMESPACE
