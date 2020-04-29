/*
    directionallight_p.h

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

#ifndef KUESA_DIRECTIONALLIGHT_P_H
#define KUESA_DIRECTIONALLIGHT_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <Qt3DRender/private/qabstractlight_p.h>
#include <Kuesa/private/kuesa_global_p.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {

class DirectionalLight;

class KUESA_PRIVATE_EXPORT DirectionalLightPrivate : Qt3DRender::QAbstractLightPrivate
{
public:
    DirectionalLightPrivate();

    Q_DECLARE_PUBLIC(DirectionalLight)
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_DIRECTIONALLIGHT_P_H
