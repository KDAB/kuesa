/*
    thresholdeffect.h

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

#ifndef KUESA_THRESHOLDEFFECT_H
#define KUESA_THRESHOLDEFFECT_H

#include <Kuesa/kuesa_global.h>
#include <Kuesa/abstractpostprocessingeffect.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QRenderPass;
class QParameter;
class QFrameGraphNode;
} // namespace Qt3DRender

namespace Kuesa {

class FullScreenQuad;

class KUESASHARED_EXPORT ThresholdEffect : public AbstractPostProcessingEffect
{
    Q_OBJECT

    Q_PROPERTY(float threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)

public:
    ThresholdEffect(Qt3DCore::QNode *parent = nullptr);

    FrameGraphNodePtr frameGraphSubTree() const override;
    QVector<Qt3DRender::QLayer *> layers() const override;
    void setInputTexture(Qt3DRender::QAbstractTexture *texture) override;

    float threshold() const;

public Q_SLOTS:
    void setThreshold(float threshold);

Q_SIGNALS:
    void thresholdChanged(float threshold);

private:
    FrameGraphNodePtr m_rootFrameGraphNode;
    Qt3DRender::QLayer *m_layer;

    Qt3DRender::QParameter *m_thresholdParameter;
    Qt3DRender::QParameter *m_textureParam;
    FullScreenQuad *m_fsQuad;
};
} // namespace Kuesa
QT_END_NAMESPACE

#endif // KUESA_THRESHOLDEFFECT_H
