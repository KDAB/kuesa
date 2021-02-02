/*
    StyledRadioButton.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import "."

RadioButton {
    style: RadioButtonStyle {
        indicator: Rectangle {
            color: control.checked ? SharedAttributes.controlsContentColor : SharedAttributes.controlsBackgroundColor
            Behavior on color { animation: SharedAttributes.defaultColorAnimation }
            implicitWidth: implicitHeight
            implicitHeight: SharedAttributes.controlsHeight
            radius: width * 0.5
            border {
                color: SharedAttributes.controlsBorderColor
                width: 1
            }
        }

        label: StyledLabel {
            text: control.text
        }
    }
}
