/*
    abstractpostprocessingeffect.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_ABSTRACTPOSTPROCESSINGEFFECT_H
#define KUESA_ABSTRACTPOSTPROCESSINGEFFECT_H

#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DCore/QEntity>
#include <QSharedPointer>
#include <Kuesa/kuesa_global.h>

namespace Qt3DRender {
class QLayer;
class QAbstractTexture;
} // namespace Qt3DRender

QT_BEGIN_NAMESPACE
namespace Kuesa {

class KUESASHARED_EXPORT AbstractPostProcessingEffect : public Qt3DCore::QNode
{
    Q_OBJECT

public:
    using FrameGraphNodePtr = QSharedPointer<Qt3DRender::QFrameGraphNode>;

    enum Type {
        Custom = 0
    };

    ~AbstractPostProcessingEffect();

    virtual FrameGraphNodePtr frameGraphSubTree() const = 0;
    Type type() const;

    //set texture containing scene to be post-processed
    virtual void setInputTexture(Qt3DRender::QAbstractTexture *texture) = 0;
    virtual void setDepthTexture(Qt3DRender::QAbstractTexture *texture);
    virtual void setCamera(Qt3DCore::QEntity *camera);
    virtual void setWindowSize(const QSize &sceneSize);
    virtual QVector<Qt3DRender::QLayer *> layers() const = 0;

protected:
    explicit AbstractPostProcessingEffect(Qt3DCore::QNode *parent = nullptr, Type = Custom);
    const Type m_type;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_ABSTRACTPOSTPROCESSINGEFFECT_H
