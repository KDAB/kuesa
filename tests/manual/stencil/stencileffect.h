/*
    stencileffect.h

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

#ifndef STENCILEFFECT_H
#define STENCILEFFECT_H

#include <Kuesa/AbstractPostProcessingEffect>
#include <Qt3DRender/QParameter>
#include <QColor>

class StencilEffect : public Kuesa::AbstractPostProcessingEffect
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
public:
    explicit StencilEffect();

    QColor color() const;
    FrameGraphNodePtr frameGraphSubTree() const;
    void setInputTexture(Qt3DRender::QAbstractTexture *texture);
    void setDepthTexture(Qt3DRender::QAbstractTexture *texture);
    QVector<Qt3DRender::QLayer *> layers() const;

public Q_SLOTS:
    void setColor(QColor color);

Q_SIGNALS:
    void colorChanged(QColor color);

private:
    Qt3DRender::QParameter *m_color;
    FrameGraphNodePtr m_rootFramegraphNode;
    Qt3DRender::QParameter *m_inputTexture;
    Qt3DRender::QParameter *m_depthTexture;
    QVector<Qt3DRender::QLayer *> m_layers;
};

#endif // STENCILEFFECT_H
