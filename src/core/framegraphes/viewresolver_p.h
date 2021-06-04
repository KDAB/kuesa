/*
    viewresolver_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_VIEWRESOLVER_P_H
#define KUESA_VIEWRESOLVER_P_H

#include <Kuesa/private/kuesa_global_p.h>
#include <Qt3DRender/qframegraphnode.h>

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

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QMaterial;
class QParameter;
class QAbstractTexture;
class QShaderProgram;
} // namespace Qt3DRender

namespace Kuesa {

class View;
class FullScreenQuad;

class KUESA_PRIVATE_EXPORT ViewResolver : public Qt3DRender::QFrameGraphNode
{
    Q_OBJECT
public:
    explicit ViewResolver(Qt3DCore::QNode *parent = nullptr);

    void setView(View *view);
    View *view() const;

private:
    Qt3DRender::QMaterial *m_material = nullptr;
    Qt3DRender::QParameter *m_sourceTextureParameter = nullptr;
    Qt3DRender::QShaderProgram *m_shader = nullptr;
    FullScreenQuad *m_fsQuad = nullptr;
    View *m_view = nullptr;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_VIEWRESOLVER_P_H
