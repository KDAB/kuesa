/*
    noisetextureimage_p.h

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mauro Persano <mauro.persano@kdab.com>

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

#include "noisetextureimage_p.h"

#include <algorithm>
#include <random>

using namespace Qt3DRender;

QT_BEGIN_NAMESPACE

namespace Kuesa {

namespace {

constexpr const auto TextureSize = 1024;

class NoiseTextureDataFunctor : public QTextureImageDataGenerator
{
public:
    QTextureImageDataPtr operator()() override;
    bool operator==(const QTextureImageDataGenerator &other) const override;

    QT3D_FUNCTOR(NoiseTextureDataFunctor)
};

QTextureImageDataPtr NoiseTextureDataFunctor::operator()()
{
    auto data = QTextureImageDataPtr::create();

    data->setTarget(QOpenGLTexture::Target2D);

    data->setWidth(TextureSize);
    data->setHeight(TextureSize);
    data->setDepth(1);

    data->setLayers(1);
    data->setMipLevels(1);
    data->setFaces(1);

    data->setPixelFormat(QOpenGLTexture::RGBA);
    data->setFormat(QOpenGLTexture::RGBA32F);
    data->setPixelType(QOpenGLTexture::Float32);

    QByteArray pixelBytes(TextureSize * TextureSize * 4 * sizeof(float), 0);

    auto *pixelData = reinterpret_cast<float *>(pixelBytes.data());

    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    std::generate(pixelData, pixelData + TextureSize * TextureSize * 4, [&distribution, &generator] {
        return distribution(generator);
    });

    data->setData(pixelBytes, 4 * sizeof(float), false);

    return data;
}

bool NoiseTextureDataFunctor::operator==(const QTextureImageDataGenerator &other) const
{
    return functor_cast<NoiseTextureDataFunctor>(&other) != nullptr;
}

} // namespace

QTextureImageDataGeneratorPtr NoiseTextureImage::dataGenerator() const
{
    return QTextureImageDataGeneratorPtr(new NoiseTextureDataFunctor);
}

} // namespace Kuesa

QT_END_NAMESPACE
