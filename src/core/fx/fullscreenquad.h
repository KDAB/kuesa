/*
    fullscreenquad.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
} // namespace Qt3DRender

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
namespace Qt3DGeometry = Qt3DCore;
namespace Qt3DCore {
#else
namespace Qt3DGeometry = Qt3DRender;
namespace Qt3DRender {
#endif
class QBuffer;
}

namespace Kuesa {

class KUESASHARED_EXPORT FullScreenQuad : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit FullScreenQuad(Qt3DRender::QMaterial *material, Qt3DCore::QNode *parent = nullptr);
    ~FullScreenQuad();

    Qt3DRender::QLayer *layer() const;
    Qt3DGeometry::QBuffer *buffer() const;

    void setViewportRect(const QRectF &vp);
    QRectF viewportRect() const;

private:
    Qt3DGeometry::QBuffer *m_buffer;
    Qt3DRender::QLayer *m_layer;
    QRectF m_viewportRect = QRectF(0.0f, 0.0f, 1.0f, 1.0f);

    void updateBufferData();
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_FULLSCREENQUAD_H
