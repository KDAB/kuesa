/*
    GuidedDrillingUI.qml

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

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Drill 1.0

Item {
    property int selectedMode
    property int selectedMaterial

    property GuidedDrillingScreenController controller
    readonly property int currentStep: controller.currentStep

    readonly property int _MODE_SELECTOR_STEP: 0
    readonly property int _MATERIAL_SELECTOR_STEP: 1
    readonly property int _BIT_SELECTOR_STEP: 2
    readonly property int _INSERT_DRILL_STEP: 3
    readonly property int _SET_MODE_STEP: 4
    readonly property int _DRILL_STEP: 5

    function viewForStep(step) {
        // TO DO: Sync with 3D view based on the currentStep
        switch (step) {
        case GuidedDrillingScreenController.ModeSelection:
            return modeSelector;
        case GuidedDrillingScreenController.MaterialSelection:
            return materialSelector;
        case GuidedDrillingScreenController.BitSelection:
            return bitSelector;
        case GuidedDrillingScreenController.SetupBit:
            return insertDrillStep;
        case GuidedDrillingScreenController.SetupClutch:
            return setModeStep;
        case GuidedDrillingScreenController.SetupSpeed:
            return setSpeedStep;
        case GuidedDrillingScreenController.SetupDirection:
            return setDirectionStep;
        case GuidedDrillingScreenController.CompletionStep:
            return drillStep;
        }
    }

    function next() {
        guidedDrillingWizardStackView.push(viewForStep(controller.nextStep()))
    }

    function back() {
        guidedDrillingWizardStackView.pop();
        controller.previousStep()
    }

    function restart() {
        guidedDrillingWizardStackView.clear()
        controller.reset();
        guidedDrillingWizardStackView.push(modeSelector)
    }

    StackView {
        id: guidedDrillingWizardStackView
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            margins: 50
        }
        width: parent.width * 0.3
        initialItem: modeSelector

        Component {
            id: modeSelector
            Frame {
                ColumnLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                        text: "Mode Selector"
                        font.pointSize: 20
                    }

                    Button {
                        id: drillingModeButton
                        Layout.alignment: Qt.AlignHCenter
                        text: "Drilling"
                        checkable: true
                        autoExclusive: true
                    }
                    Button {
                        id: screwDrivingModeButton
                        Layout.alignment: Qt.AlignHCenter
                        text: "Screw Driving"
                        checkable: true
                        autoExclusive: true
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignRight|Qt.AlignBottom
                        Button {
                            text: "Next"
                            enabled: screwDrivingModeButton.checked || drillingModeButton.checked
                            onClicked: {
                                next()
                            }
                        }
                    }
                }
            }
        }

        Component {
            id: materialSelector
            Frame {
                ColumnLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                        text: "Material Selector"
                        font.pointSize: 20
                    }

                    Button {
                        id: woodMaterialButton
                        Layout.alignment: Qt.AlignHCenter
                        text: "Wood"
                        checkable: true
                        autoExclusive: true
                    }
                    Button {
                        id: concreteMaterialButton
                        Layout.alignment: Qt.AlignHCenter
                        text: "Concrete"
                        checkable: true
                        autoExclusive: true
                    }
                    Button {
                        id: metalMaterialButton
                        Layout.alignment: Qt.AlignHCenter
                        text: "Metal"
                        checkable: true
                        autoExclusive: true
                    }


                    RowLayout {
                        Layout.alignment: Qt.AlignBottom
                        Button {
                            Layout.alignment: Qt.AlignLeft
                            text: "Back"
                            onClicked: back()
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        Button {
                            enabled: woodMaterialButton.checked || concreteMaterialButton.checked || metalMaterialButton.checked
                            Layout.alignment: Qt.AlignRight
                            text: "Next"
                            onClicked: next()
                        }
                    }
                }
            }
        }

        Component {
            id: bitSelector
            Frame {
                ColumnLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                        text: "Bit Selector"
                        font.pointSize: 20
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignBottom
                        Button {
                            Layout.alignment: Qt.AlignLeft
                            text: "Back"
                            onClicked: back()
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        Button {
                            Layout.alignment: Qt.AlignRight
                            text: "Next"
                            onClicked: next()
                        }
                    }
                }
            }
        }

        Component {
            id: insertDrillStep
            Frame {
                ColumnLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                        text: "Insert Drill Bit"
                        font.pointSize: 20
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignBottom
                        Button {
                            Layout.alignment: Qt.AlignLeft
                            text: "Back"
                            onClicked: back()
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        Button {
                            Layout.alignment: Qt.AlignRight
                            text: "Next"
                            onClicked: next()
                        }
                    }
                }
            }
        }

        Component {
            id: setModeStep
            Frame {
                ColumnLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                        text: "Set Drill Mode"
                        font.pointSize: 20
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignBottom
                        Button {
                            Layout.alignment: Qt.AlignLeft
                            text: "Back"
                            onClicked: back()
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        Button {
                            Layout.alignment: Qt.AlignRight
                            text: "Next"
                            onClicked: next()
                        }
                    }
                }
            }
        }

        Component {
            id: setSpeedStep
            Frame {
                ColumnLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                        text: "Set Speed"
                        font.pointSize: 20
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignBottom
                        Button {
                            Layout.alignment: Qt.AlignLeft
                            text: "Back"
                            onClicked: back()
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        Button {
                            Layout.alignment: Qt.AlignRight
                            text: "Next"
                            onClicked: next()
                        }
                    }
                }
            }
        }

        Component {
            id: setDirectionStep
            Frame {
                ColumnLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                        text: "Set Direction"
                        font.pointSize: 20
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignBottom
                        Button {
                            Layout.alignment: Qt.AlignLeft
                            text: "Back"
                            onClicked: back()
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        Button {
                            Layout.alignment: Qt.AlignRight
                            text: "Next"
                            onClicked: next()
                        }
                    }
                }
            }
        }

        Component {
            id: drillStep
            Frame {
                ColumnLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                        text: "Start Drilling"
                        font.pointSize: 20
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignBottom
                        Button {
                            Layout.alignment: Qt.AlignLeft
                            text: "Back"
                            onClicked: back()
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        Button {
                            Layout.alignment: Qt.AlignRight
                            text: "Restart"
                            onClicked: restart()
                        }
                    }
                }
            }
        }
    }
}
