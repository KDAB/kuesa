/*
    ddsheaderparser.cpp

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

#include "ddsheaderparser.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMetaEnum>
#include <qendian.h>

namespace {

struct DdsPixelFormat {
    quint32 size;
    quint32 flags;
    quint32 fourCC;
    quint32 rgbBitCount;
    quint32 redMask;
    quint32 greenMask;
    quint32 blueMask;
    quint32 alphaMask;
};

struct DdsHeader {
    char magic[4];
    quint32 size;
    quint32 flags;
    quint32 height;
    quint32 width;
    quint32 pitchOrLinearSize;
    quint32 depth;
    quint32 mipmapCount;
    quint32 reserved[11];
    DdsPixelFormat pixelFormat;
    quint32 caps;
    quint32 caps2;
    quint32 caps3;
    quint32 caps4;
    quint32 reserved2;
};

struct DdsDX10Header {
    quint32 format;
    quint32 dimension;
    quint32 miscFlag;
    quint32 arraySize;
    quint32 miscFlags2;
};

constexpr int DdsFourCC(int a, int b, int c, int d)
{
    return a | (b << 8) | (c << 16) | (d << 24);
};

QString valueToEnumName(uint v, const char *enumName)
{
    const int enumIdx = DDSHeaderParser::staticMetaObject.indexOfEnumerator(enumName);
    const QMetaEnum metaEnum = DDSHeaderParser::staticMetaObject.enumerator(enumIdx);
    const int keysCount = metaEnum.keyCount();

    for (int i = 0; i < keysCount; ++i) {
        const uint enumValue = uint(metaEnum.value(i));
        if (v == enumValue)
            return QStringLiteral("%1 (%2)").arg(metaEnum.key(i)).arg(v);
    }
    return QString::number(v);
}

QString flagsToEnumName(uint v, const char *enumName)
{
    QString str;

    const int enumIdx = DDSHeaderParser::staticMetaObject.indexOfEnumerator(enumName);
    const QMetaEnum metaEnum = DDSHeaderParser::staticMetaObject.enumerator(enumIdx);
    const int keysCount = metaEnum.keyCount();

    for (int i = 0; i < keysCount; ++i) {
        const uint enumValue = uint(metaEnum.value(i));
        if ((v & enumValue) == enumValue) {
            if (!str.isEmpty())
                str += QStringLiteral(", ");
            str += QStringLiteral("%1 (0x%2)").arg(metaEnum.key(i)).arg(enumValue, 0, 16);
        }
    }

    if (!str.isEmpty())
        return QStringLiteral("%1 - %2").arg(v, 0, 2).arg(str);
    return QStringLiteral("%1").arg(v, 0, 2);
}

} // namespace

DDSHeaderParser::DDSHeaderParser(QObject *parent)
    : QObject(parent)
{
    QObject::connect(this, &DDSHeaderParser::filePathChanged,
                     this, &DDSHeaderParser::parseTextureHeader);
}

DDSHeaderParser::~DDSHeaderParser()
{
}

QUrl DDSHeaderParser::filePath() const
{
    return m_filePath;
}

QString DDSHeaderParser::ddsHeader() const
{
    return m_ddsHeader;
}

void DDSHeaderParser::setFilePath(const QUrl &filePath)
{
    if (m_filePath == filePath)
        return;

    m_filePath = filePath;
    emit filePathChanged(m_filePath);
}

void DDSHeaderParser::parseTextureHeader()
{
    m_ddsHeader.clear();
    emit ddsHeaderChanged();

    // 1) Check path and that file exists
    const QString path = m_filePath.toLocalFile();
    QFileInfo fInfo(path);
    // 2) Check extension is .dds
    if (!fInfo.exists() || fInfo.suffix() != QStringLiteral("dds")) {
        qWarning() << path << "doesn't exist or is not a DDS file";
        return;
    }

    QFile source(path);
    if (!source.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open DDS File";
        return;
    }

    // 3) Parse header
    DdsHeader header;
    if ((source.read(reinterpret_cast<char *>(&header), sizeof header) != sizeof header) || (qstrncmp(header.magic, "DDS ", 4) != 0)) {
        qWarning() << "Invalid DDS Header";
        return;
    }

    const quint32 pixelFlags = qFromLittleEndian(header.pixelFormat.flags);
    const quint32 fourCC = qFromLittleEndian(header.pixelFormat.fourCC);

    QString dx10HeaderStr = QStringLiteral("{}");

    constexpr quint32 dx10FourCC = DdsFourCC('D', 'X', '1', '0');
    if (fourCC == dx10FourCC) {
        // DX10 texture
        DdsDX10Header dx10Header;
        if (source.read(reinterpret_cast<char *>(&dx10Header), sizeof dx10Header) != sizeof dx10Header) {
            qWarning() << "Invalid DX10 Header";
            return;
        }

        dx10HeaderStr = QStringLiteral("\n"
                                       "    Format: %1\n"
                                       "    Dimension: %2\n"
                                       "    MiscFlags: %3\n"
                                       "    ArraySize: %4\n"
                                       "    MiscFlags2: %5\n")
                                .arg(valueToEnumName(dx10Header.format, "DXGIFormat"))
                                .arg(dx10Header.dimension)
                                .arg(dx10Header.miscFlag, 0, 2)
                                .arg(dx10Header.arraySize)
                                .arg(dx10Header.miscFlags2, 0, 2);
    }

    // 4) Create representation for header
    m_ddsHeader = QStringLiteral("Magic(%1 {%2}, %3 {%4}, %5 {%6}, %7 {%8})\n"
                                 "Size: %9\n"
                                 "Flags: %10\n"
                                 "Width: %11, Height: %12, Depth: %13\n"
                                 "PitchOrLinearSize: %14\n"
                                 "MipmapCount: %15\n"
                                 "Caps: "
                                 "  [0] %16\n"
                                 "  [1] %17\n"
                                 "  [2] %18\n"
                                 "  [3] %19\n"
                                 "DdsPixelFormat:\n"
                                 "    Size: %20\n"
                                 "    Flags: %21\n"
                                 "    FourCC: %22\n"
                                 "    RGB BitCount: %23\n"
                                 "    Red Mask: %24\n"
                                 "    Green Mask: %25\n"
                                 "    Blue Mask: %26\n"
                                 "    Alpha Mask: %27\n"
                                 "DdsDX10Header: %28\n")
                          .arg(QString::number(header.magic[0]))
                          .arg(header.magic[0])
                          .arg(QString::number(header.magic[1]))
                          .arg(header.magic[1])
                          .arg(QString::number(header.magic[2]))
                          .arg(header.magic[2])
                          .arg(QString::number(header.magic[3]))
                          .arg(header.magic[3])
                          .arg(header.size)
                          .arg(flagsToEnumName(header.flags, "DdsFlags"))
                          .arg(header.width)
                          .arg(header.height)
                          .arg(header.depth)
                          .arg(header.pitchOrLinearSize)
                          .arg(header.mipmapCount)
                          .arg(flagsToEnumName(header.caps, "Caps2Flags"),
                               flagsToEnumName(header.caps2, "Caps2Flags"),
                               flagsToEnumName(header.caps3, "Caps2Flags"),
                               flagsToEnumName(header.caps4, "Caps2Flags"))
                          .arg(header.pixelFormat.size)
                          .arg(flagsToEnumName(pixelFlags, "PixelFormatFlag"))
                          .arg(fourCC, 0, 2)
                          .arg(header.pixelFormat.rgbBitCount)
                          .arg(header.pixelFormat.redMask, 0, 2)
                          .arg(header.pixelFormat.greenMask, 0, 2)
                          .arg(header.pixelFormat.blueMask, 0, 2)
                          .arg(header.pixelFormat.alphaMask, 0, 2)
                          .arg(dx10HeaderStr);
    emit ddsHeaderChanged();
}
