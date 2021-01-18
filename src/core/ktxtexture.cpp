/*
    ktxtexture.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include "ktxtexture.h"

#include <Qt3DRender/private/qabstracttexture_p.h>
#include <Qt3DRender/QTextureDataUpdate>
#include <Qt3DRender/private/qtextureimagedata_p.h>
#include <Kuesa/private/logging_p.h>
#include <QSharedPointer>
#include <QFile>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/private/qurlhelper_p.h>
#include <Qt3DRender/private/qtexturegenerator_p.h>
namespace QUrlHelperNS = Qt3DCore;
#else
#include <Qt3DRender/QTextureGenerator>
#include <Qt3DRender/private/qurlhelper_p.h>
namespace QUrlHelperNS = Qt3DRender;
#endif
#include <cmath>
#include <algorithm>

#include <vk_format.h>
#include <ktx.h>
#include <QMetaObject>

QT_BEGIN_NAMESPACE

namespace
{

bool isInternalFormatValid(const int internalFormat)
{
    const static int enumIdx = QOpenGLTexture::staticMetaObject.indexOfEnumerator("TextureFormat");
    const static auto &enumerator = QOpenGLTexture::staticMetaObject.enumerator(enumIdx);
    for (int i = 0, m = enumerator.keyCount(); i < m; ++i) {
        const auto enumValue = enumerator.value(i);
        if (enumValue == internalFormat)
            return true;
    }
    return false;
}

}

namespace Kuesa
{

/*!
    \class Kuesa::KTXTexture
    \brief Texture subclass that handles loading ktx files (both KTX1 and KTX2).

    \inmodule Kuesa
    \since Kuesa 1.3
    \inherits Qt3DRender::QAbstractTexture

    KTX is a container format backed by the Khronos Group. It allows bundling
    all types of textures (simple texture with no mipmaps to cube map arrays
    with mipmaps). Additionally it handles ASTC compressed content.
*/

/*!
    \qmltype KTXTexture
    \brief Texture subclass that handles loading ktx files (both KTX1 and KTX2).

    \inqmlmodule Kuesa
    \since Kuesa 1.3
    \instantiates Kuesa::KTXTexture

    KTX is a container format backed by the Khronos Group. It allows bundling
    all types of textures (simple texture with no mipmaps to cube map arrays
    with mipmaps). Additionally it handles ASTC compressed content.
*/

KTXTexture::KTXTexture(Qt3DCore::QNode *parent)
    : Qt3DRender::QAbstractTexture(parent)
    , m_ktxTexture(nullptr)
{
}

KTXTexture::~KTXTexture()
{
    ktxTexture_Destroy(m_ktxTexture);
}

QUrl KTXTexture::source() const
{
    return m_source;
}

void KTXTexture::setSource(const QUrl &source)
{
    if (m_source != source) {
        setStatus(KTXTexture::Status::Loading);
                m_source = source;
        if (m_source.isLocalFile() || m_source.scheme() == QLatin1String("qrc")
        #ifdef Q_OS_ANDROID
                || url.scheme() == QLatin1String("assets")
        #endif
                ) {
            const QString source = QUrlHelperNS::QUrlHelper::urlToLocalFileOrQrc(m_source);
            QFile f(source);
            if (!f.open(QIODevice::ReadOnly)) {
                setStatus(KTXTexture::Status::Error);
                qWarning() << "Failed to open" << source;
            }
            else {
                auto ktxStream = f.readAll();
                if (m_ktxTexture) {
                    ktxTexture_Destroy(m_ktxTexture);
                    m_ktxTexture = nullptr;
                }
                // This should return an error code, but we already check if the pointer is null below.
                ktxTexture_CreateFromMemory(reinterpret_cast<const unsigned char*>(ktxStream.constData()),
                                ktxStream.size(),
                                KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                &m_ktxTexture);
            }
        }
        emit sourceChanged(source);
        if (m_ktxTexture)
            generateData();
        else
            setStatus(KTXTexture::Status::Error);
    }
}

void KTXTexture::generateData()
{
    setWidth(m_ktxTexture->baseWidth);
    setHeight(m_ktxTexture->baseHeight);
    setDepth(m_ktxTexture->baseDepth);
    setLayers(m_ktxTexture->numLayers);
    setGenerateMipMaps(m_ktxTexture->generateMipmaps);
    setFormat(QAbstractTexture::TextureFormat::Automatic);

    auto pTexture = static_cast<Qt3DRender::QAbstractTexturePrivate*>(Qt3DRender::QAbstractTexturePrivate::get(this));
    pTexture->m_mipmapLevels = m_ktxTexture->numLevels;
    pTexture->update();

    QAbstractTexture::Target target;
    if (m_ktxTexture->isCubemap) {
        if (!m_ktxTexture->isArray)
            target = QAbstractTexture::TargetCubeMap;
        else
            target = QAbstractTexture::TargetCubeMapArray;
    } else {
        switch (m_ktxTexture->numDimensions) {
        case 1:  {
            if (!m_ktxTexture->isArray)
                target = QAbstractTexture::Target1D;
            else
                target = QAbstractTexture::Target1DArray;
            break;
        }
        case 2:  {
            if (!m_ktxTexture->isArray)
                target = QAbstractTexture::Target2D;
            else
                target = QAbstractTexture::Target2DArray;
            break;
        }
        case 3:  {
            target = QAbstractTexture::Target3D;
            break;
        }
        }
    }
    static_cast<Qt3DRender::QAbstractTexturePrivate*>(Qt3DRender::QAbstractTexturePrivate::get(this))->m_target = target;
    for (size_t level = 0; level < m_ktxTexture->numLevels; ++level) {
        for (size_t layer = 0; layer < m_ktxTexture->numLayers; ++layer) {
            for (size_t face = 0; face < m_ktxTexture->numFaces; ++face) {
                Qt3DRender::QTextureDataUpdate updateData;
                updateData.setFace(QAbstractTexture::CubeMapFace(QAbstractTexture::CubeMapFace::CubeMapPositiveX + face));
                updateData.setLayer(layer);
                updateData.setMipLevel(level);

                Qt3DRender::QTextureImageDataPtr imageData = Qt3DRender::QTextureImageDataPtr::create();

                imageData->setDepth(m_ktxTexture->baseDepth);
                imageData->setWidth(m_ktxTexture->baseWidth);
                imageData->setHeight(m_ktxTexture->baseHeight);
                imageData->setMipLevels(m_ktxTexture->numLevels);
                imageData->setFaces(m_ktxTexture->numFaces);
                imageData->setLayers(m_ktxTexture->numLayers);

                if (m_ktxTexture->classId == ktxTexture2_c) {
                    ktxTexture2 *m_ktxTexture2 = (ktxTexture2*)m_ktxTexture;
                    if (m_ktxTexture2->vkFormat == VkFormat::VK_FORMAT_UNDEFINED) {
                        qCWarning(kuesa) << "KTX v2 VK_FORMAT_UNDEFINED is not supported by libktx yet";
                        setStatus(KTXTexture::Status::Error);
                        return;
                    }

                    const auto internalFormat = glGetInternalFormatFromVkFormat((VkFormat)m_ktxTexture2->vkFormat);
                    const auto type = glGetTypeFromInternalFormat(internalFormat);
                    const auto format = glGetFormatFromInternalFormat(internalFormat);

                    if (!::isInternalFormatValid(internalFormat)) {
                        const QString source = QUrlHelperNS::QUrlHelper::urlToLocalFileOrQrc(m_source);
                        qCWarning(kuesa) << "Internal format used by KTX texture" << source << "is not supported";
                        setStatus(KTXTexture::Status::Error);
                        return;
                    }

                    imageData->setPixelFormat(static_cast<QOpenGLTexture::PixelFormat>(format));
                    imageData->setPixelType(static_cast<QOpenGLTexture::PixelType>(type));
                    imageData->setFormat(static_cast<QOpenGLTexture::TextureFormat>(internalFormat));
                } else {
                    ktxTexture1 *m_ktxTexture1 = (ktxTexture1*)m_ktxTexture;
                    if (!::isInternalFormatValid(m_ktxTexture1->glInternalformat)) {
                        const QString source = QUrlHelperNS::QUrlHelper::urlToLocalFileOrQrc(m_source);
                        qCWarning(kuesa) << "Internal format used by KTX texture" << source << "is not supported";
                        setStatus(KTXTexture::Status::Error);
                        return;
                    }

                    imageData->setPixelFormat(static_cast<QOpenGLTexture::PixelFormat>(m_ktxTexture1->glBaseInternalformat));
                    imageData->setPixelType(static_cast<QOpenGLTexture::PixelType>(m_ktxTexture1->glType));
                    imageData->setFormat(static_cast<QOpenGLTexture::TextureFormat>(m_ktxTexture1->glInternalformat));
                }
                imageData->setTarget(static_cast<QOpenGLTexture::Target>(target));
                setFormat(static_cast<Qt3DRender::QAbstractTexture::TextureFormat>(imageData->format()));


                Qt3DRender::QTextureImageDataPrivate *imagePrivate = nullptr;
                imagePrivate = Qt3DRender::QTextureImageDataPrivate::get(imageData.get());
                imagePrivate->m_alignment = m_ktxTexture->classId == ktxTexture1_c ? 4 : 1;


                ktx_size_t offset = 0;
                ktxTexture_GetImageOffset(this->m_ktxTexture, level, layer, face, &offset);
                int imageSize = ktxTexture_GetImageSize(this->m_ktxTexture, level);
                QByteArray data = QByteArray::fromRawData(reinterpret_cast<const char*>(ktxTexture_GetData(m_ktxTexture)) + offset, imageSize);
                imageData->setData(data, 1, m_ktxTexture->isCompressed);

                updateData.setData(imageData);

                this->updateData(updateData);
            }
        }
    }
    setStatus(KTXTexture::Status::Ready);
}

} // Kuesa

QT_END_NAMESPACE
