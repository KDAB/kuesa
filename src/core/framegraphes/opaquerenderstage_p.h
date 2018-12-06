/*
    opaquerenderstage_p.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_OPAQUERENDERSTAGE_P_H
#define KUESA_OPAQUERENDERSTAGE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "abstractrenderstage_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QDepthTest;
class QNoDepthMask;
class QRenderStateSet;
} // namespace Qt3DRender

namespace Kuesa {

class Q_AUTOTEST_EXPORT OpaqueRenderStage : public AbstractRenderStage
{
    Q_OBJECT
public:
    OpaqueRenderStage(Qt3DCore::QNode *parent = nullptr);
    ~OpaqueRenderStage();

    void setZFilling(bool zFill);
    bool zFilling() const;

private:
    Qt3DRender::QRenderStateSet *m_states;
    Qt3DRender::QDepthTest *m_depthTest;
    Qt3DRender::QNoDepthMask *m_noDepthWrite;
};
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_OPAQUERENDERSTAGE_P_H
