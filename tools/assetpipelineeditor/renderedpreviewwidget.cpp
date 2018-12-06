/*
    renderedpreviewwidget.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#include "renderedpreviewwidget.h"
#include <QResizeEvent>
#include <Qt3DRender/QRenderCaptureReply>
#include <QByteArray>
#include <QtCore/QBuffer>

RenderedPreviewWidget::RenderedPreviewWidget(QWidget *parent)
    : QLabel(parent)
{
    setMinimumSize({ 128, 128 });
    setMaximumSize(previewSize());
}

RenderedPreviewWidget::~RenderedPreviewWidget()
{
}

void RenderedPreviewWidget::onCapture(Qt3DRender::QRenderCaptureReply *reply)
{
    m_pixmap = QPixmap::fromImage(reply->image());
    setPixmap(m_pixmap.scaled(size(), Qt::KeepAspectRatio));

    QByteArray imageData;
    QBuffer buffer(&imageData);
    m_pixmap.save(&buffer, "PNG");
    buffer.close();
    QString pixmapAsString = QString::fromLatin1(imageData.toBase64().data());
    setToolTip(QString::fromLatin1("<img src='data:image/png;base64, %1'><br/>").arg(pixmapAsString));
}

QSize RenderedPreviewWidget::previewSize() const
{
    return { 512, 512 };
}

void RenderedPreviewWidget::resizeEvent(QResizeEvent *)
{
    setPixmap(m_pixmap.scaled(size(), Qt::KeepAspectRatio));
}

QSize RenderedPreviewWidget::sizeHint() const
{
    return { 256, 256 };
}
