/*
    fovadaptor.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "fovadaptor_p.h"

#include <qmath.h>

/*!
  \internal
 This class is just an adaptor to convert radians coming from the yFov animation to degrees, expected by the Qt3DRender::QCamera
 We need a radians property so the Qt3DAnimation aspect can find it and therefore, we need read/write function, but we dont need to
 actually store any content.

 When the radians writer is called, we just convert the radians to degrees and emit the degreesChanged function which is connected
 to the QCamera::setFieldOfView in the importer
 */
FOVAdaptor::FOVAdaptor(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
{
}

float FOVAdaptor::radians() const
{
    return 0.0;
}

void FOVAdaptor::setRadians(float radians)
{
    Q_EMIT degreesChanged(qRadiansToDegrees(radians));
}
