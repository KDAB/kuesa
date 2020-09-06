/*
    boundingvolumerenderer.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef BOUNDINGVOLUMERENDERER_H
#define BOUNDINGVOLUMERENDERER_H

#include <KuesaUtils/kuesa_utils_global.h>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>

#include <QColor>

#include <vector>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
class QMouseDevice;
class QKeyboardDevice;
} // namespace Qt3DInput

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
namespace Qt3DGeometry = Qt3DCore;
namespace Qt3DCore {
#else
namespace Qt3DGeometry = Qt3DRender;
namespace Qt3DRender {
#endif
class QBuffer;
class QGeometry;
}

namespace Qt3DRender {
class QGeometryRenderer;
class QParameter;
} // namespace Qt3DRender

namespace KuesaUtils {

class KUESAUTILS_SHARED_EXPORT BoundingVolumeRenderer : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    enum SelectionMode {
        Replace = 1 << 0,
        Add = 1 << 1,
        Recurse = 1 << 2,
    };
    Q_DECLARE_FLAGS(SelectionModes, SelectionMode)

    BoundingVolumeRenderer(Qt3DCore::QNode *parent = nullptr);
    ~BoundingVolumeRenderer();

    Q_INVOKABLE void select(Qt3DCore::QEntity *entity);
    Q_INVOKABLE void select(Qt3DCore::QEntity *entity, SelectionModes mode);
    Q_INVOKABLE void deselect(Qt3DCore::QEntity *entity);
    Q_INVOKABLE void clearSelection();

    QColor color() const;
    void setColor(const QColor &color);

private:
    struct SelectionDetails {
        Qt3DCore::QNodeId m_id;
        QMatrix4x4 m_matrix;
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        Qt3DCore::QEntity *m_entity;
#endif
        Qt3DCore::QTransform *m_transform;
        Qt3DGeometry::QGeometry *m_geometry;
        std::vector<QMetaObject::Connection> m_changeTrackers;
    };

    void initRendering();
    void updateRecord(const Qt3DCore::QEntity *entity);
    void updateRecord(SelectionDetails &record, bool updateBuffer = true);
    void scheduleBufferUpdate();
    Q_INVOKABLE void updateInstanceBuffer();
    void deselect(const Qt3DCore::QNodeId &id);

    std::vector<SelectionDetails> m_selectionData;
    Qt3DInput::QMouseDevice *m_mouseDevice;
    Qt3DInput::QKeyboardDevice *m_keyboardDevice;
    Qt3DGeometry::QBuffer *m_instanceBuffer;
    Qt3DRender::QGeometryRenderer *m_view;
    Qt3DRender::QParameter *m_colorParameter;
    QColor m_color;
    bool m_updateScheduled;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(BoundingVolumeRenderer::SelectionModes)

} // namespace KuesaUtils

QT_END_NAMESPACE

#endif // BOUNDINGVOLUMERENDERER_H
