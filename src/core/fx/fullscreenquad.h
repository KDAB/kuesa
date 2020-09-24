/*
    fullscreenquad.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_FULLSCREENQUAD_H
#define KUESA_FULLSCREENQUAD_H

#include <Kuesa/kuesa_global.h>
#include <Qt3DCore/QEntity>
#include <QRectF>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QLayer;
class QCamera;
class QMaterial;
class QBuffer;
} // namespace Qt3DRender

namespace Kuesa {

class KUESASHARED_EXPORT FullScreenQuad : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit FullScreenQuad(Qt3DRender::QMaterial *material, Qt3DCore::QNode *parent = nullptr);
    ~FullScreenQuad();

    Qt3DRender::QLayer *layer() const;
    virtual void setViewportRect(const QRectF &vp);
    QRectF viewportRect() const;
    Qt3DRender::QBuffer *buffer() const;

private:
    Qt3DRender::QBuffer *m_buffer;
    Qt3DRender::QLayer *m_layer;
    QRectF m_viewportRect;

    void updateBufferData();
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_FULLSCREENQUAD_H
