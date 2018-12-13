#include "ssbobuffer.h"
#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <cstring>
#include <gli.hpp>
#include <convert.hpp>
#include <QTimer>

SSBOBuffer::SSBOBuffer(Qt3DCore::QNode *parent)
    : Qt3DRender::QBuffer(parent)
    , m_wasInitialized(false)
    , m_isInteractive(false)
    , m_isComplete(false)
{
    setAccessType(Qt3DRender::QBuffer::ReadWrite);
    setType(Qt3DRender::QBuffer::ShaderStorageBuffer);
    setUsage(Qt3DRender::QBuffer::DynamicRead);
    QObject::connect(this, &SSBOBuffer::dataChanged,
                     this, &SSBOBuffer::onDataChanged);
}

void SSBOBuffer::setTextureSize(QSize size)
{
    if (m_textureSize == size)
        return;

    m_textureSize = size;
    emit textureSizeChanged();

    if (m_textureSize.width() <= 1 || m_textureSize.height() <= 1)
        return;

    QByteArray cleanData;
    cleanData.resize(size.width() * size.height() * 4 * sizeof(float));
    std::memset(cleanData.data(), 0, cleanData.size());
    setData(cleanData);
    m_wasInitialized = true;
    emit isInitializedChanged();
}

void SSBOBuffer::setIsInteractive(bool interactive)
{
    if (m_isInteractive == interactive)
        return;
    m_isInteractive = interactive;
    emit isInteractiveChanged();
}

void SSBOBuffer::setOutputFileName(const QString &name)
{
    if (m_outputFileName == name)
        return;
    m_outputFileName = name;
    emit outputFileNameChanged();
}

void SSBOBuffer::onDataChanged(const QByteArray &newData)
{
    if (!m_wasInitialized || m_isComplete)
        return;

    // We have receive data containing our captured image
    Q_ASSERT(newData.size() == m_textureSize.width() * m_textureSize.height() * 4 * sizeof(float));

    m_isComplete = true;
    emit isCompleteChanged();

    QMetaObject::invokeMethod(this, "saveImage");
}

void SSBOBuffer::saveImage()
{
    gli::texture2d outputTexture(gli::FORMAT_RGBA32_SFLOAT_PACK32,
                                 gli::extent2d(m_textureSize.width(), m_textureSize.height()),
                                 1);

    const QByteArray newData = data();
    const float *rawFloats = reinterpret_cast<const float *>(newData.constData());
    std::memcpy(outputTexture.data(), rawFloats, newData.size());

    // Convert to 16 Bits
    gli::texture2d convertedTexture = gli::convert(outputTexture, gli::FORMAT_RGBA16_SFLOAT_PACK16);

    // Save to dds
    if (!gli::save(convertedTexture, m_outputFileName.toLatin1()))
        qWarning() << "Failed to write output texture";

    if (!m_isInteractive)
        QTimer::singleShot(1000, qApp, &QCoreApplication ::quit);
}
