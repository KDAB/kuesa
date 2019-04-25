/*
    textureparser.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "textureparser_p.h"

#include "gltf2context_p.h"
#include "kuesa_p.h"
#include "texturesamplerparser_p.h"

#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>
#include <Qt3DRender/QTextureImage>

QT_BEGIN_NAMESPACE
using namespace Kuesa::GLTF2Import;

namespace {
const QLatin1String KEY_SAMPLER = QLatin1Literal("sampler");
const QLatin1String KEY_SOURCE = QLatin1Literal("source");
const QLatin1String KEY_NAME = QLatin1Literal("name");
const QLatin1String KEY_GLTF_EXTENSION = QLatin1String("extensions");
const QLatin1String KEY_MSFT_DDS_EXTENSION = QLatin1String("MSFT_texture_dds");

class EmbeddedTextureImageFunctor : public Qt3DRender::QTextureImageDataGenerator
{
public:
    EmbeddedTextureImageFunctor(const QImage &image)
        : m_image(image)
    {
    }

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

    QT3D_FUNCTOR(EmbeddedTextureImageFunctor)

private:
    QImage m_image;
};

class EmbeddedTextureImage : public Qt3DRender::QAbstractTextureImage
{
public:
    EmbeddedTextureImage(const QImage &image, QNode *parent = nullptr)
        : Qt3DRender::QAbstractTextureImage(parent), m_image(image)
    {
    }

    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const
    {
        return Qt3DRender::QTextureImageDataGeneratorPtr(new EmbeddedTextureImageFunctor(m_image));
    }

private:
    QImage m_image;
};

} // namespace

bool TextureParser::parse(const QJsonArray &texturesArray, GLTF2Context *context) const
{
    QHash<QString, Qt3DRender::QAbstractTextureImage *> sharedImages;

    for (const auto &textureValue : texturesArray) {
        const auto &textureObject = textureValue.toObject();

        auto texture = Texture();
        texture.name = textureObject[KEY_NAME].toString();

        auto sourceValue = textureObject[KEY_SOURCE];

        //Check whether there is a compressed texture available and if so, use it instead
        bool isDDSTexture = false;
        if (textureObject.contains(KEY_GLTF_EXTENSION)) {
            const auto &ddsExtension = textureObject[KEY_GLTF_EXTENSION][KEY_MSFT_DDS_EXTENSION];
            if (!ddsExtension.isUndefined()) {
                isDDSTexture = true;
                sourceValue = ddsExtension[KEY_SOURCE];
            }
        }

        if (sourceValue.isUndefined()) {
            qCWarning(kuesa, "Unknown image source for texture");
            context->addTexture(texture);
            continue;
        }

        const auto image = context->image(sourceValue.toInt());
        if (image.url.isEmpty() && image.data.isEmpty()) {
            qCWarning(kuesa) << "Invalid image source index for texture:" << sourceValue.toInt();
            return false; // Not a valid image
        }

        auto texture2d = std::unique_ptr<Qt3DRender::QAbstractTexture>(nullptr);
        if (isDDSTexture) {
            if (image.data.isEmpty()) {
                auto textureLoader = new Qt3DRender::QTextureLoader;
                texture2d.reset(textureLoader);
                textureLoader->setSource(image.url);
            } else {
                // embedded DDS images not supported for now
                qCWarning(kuesa) << "Embedded DDS images are not currently supported";
                return false;
            }
        } else {
            texture2d.reset(new Qt3DRender::QTexture2D);
            auto *textureImage = image.key.isEmpty() ? nullptr : sharedImages.value(image.key);

            if (textureImage == nullptr) {
                if (image.data.isEmpty()) {
                    auto ti = new Qt3DRender::QTextureImage();
                    ti->setSource(image.url);
                    ti->setMirrored(false);
                    textureImage = ti;
                } else {
                    QImage qimage;
                    if (!qimage.loadFromData(image.data)) {
                        qCWarning(kuesa) << "Failed to decode image " << sourceValue.toInt() << "from buffer";
                        return false;
                    }
                    textureImage = new EmbeddedTextureImage(qimage);
                }
                if (!image.key.isEmpty())
                    sharedImages.insert(image.key, textureImage);
            }

            // Add Image to Texture if compatible
            if (ensureImageIsCompatibleWithTexture(textureImage, texture2d.get()))
                texture2d->addTextureImage(textureImage);
            else
                qCWarning(kuesa) << "Image with source" << image.url << "is incompatbile with texture" << texture2d->objectName();
        }

        const auto &samplerValue = textureObject[KEY_SAMPLER];
        if (samplerValue.isUndefined()) {
            // Repeat wrappring and auto filtering should be used
            texture2d->setWrapMode(Qt3DRender::QTextureWrapMode(Qt3DRender::QTextureWrapMode::Repeat));
            texture2d->setGenerateMipMaps(true);
            texture2d->setMinificationFilter(Qt3DRender::QAbstractTexture::LinearMipMapLinear);
            texture2d->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
        } else {
            const auto sampler = context->textureSampler(samplerValue.toInt());
            if (!sampler.textureWrapMode)
                return false; // We could use default wrapping, but the file is wrong, so we reject it

            texture2d->setWrapMode(*sampler.textureWrapMode.get());
            texture2d->setMinificationFilter(sampler.minificationFilter);
            texture2d->setMagnificationFilter(sampler.magnificationFilter);
        }

        texture2d->setObjectName(textureObject[KEY_NAME].toString());
        texture.texture = texture2d.release();

        context->addTexture(texture);
    }

    return texturesArray.size() > 0;
}

bool TextureParser::ensureImageIsCompatibleWithTexture(Qt3DRender::QAbstractTextureImage *image,
                                                       Qt3DRender::QAbstractTexture *texture)
{
    const Qt3DRender::QAbstractTexture::Target arrayTargets[] = {
        Qt3DRender::QAbstractTexture::Target1DArray,
        Qt3DRender::QAbstractTexture::Target2DArray,
        Qt3DRender::QAbstractTexture::Target3D,
        Qt3DRender::QAbstractTexture::TargetCubeMapArray,
        Qt3DRender::QAbstractTexture::Target2DMultisampleArray
    };

    if (image->layer() != 0 &&
        std::find(std::begin(arrayTargets),
                  std::end(arrayTargets),
                  texture->target()) == std::end(arrayTargets)) {
        qCWarning(kuesa) << "Using an image with layer specified for a non array texture" << texture->objectName();
        return false;
    } else {
        const auto textureImages = texture->textureImages();
        for (const Qt3DRender::QAbstractTextureImage *img : textureImages) {
            // Non critical error, just warn user
            if (img->layer() == image->layer() &&
                img->mipLevel() == image->mipLevel() &&
                img->face() == image->face()) {
                qCWarning(kuesa) << "Texture contains two images for the same layer and mipmap level";
                break;
            }
        }
    }
    return true;
}

QT_END_NAMESPACE
