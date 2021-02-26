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

    // Always start in a correct state
    Component.onCompleted: restart()

    function restart() {
        guidedDrillingWizardStackView.clear()
        controller.reset();
        guidedDrillingWizardStackView.push(modeSelector)
    }

    property Item content: guidedDrillingWizardStackView

    StackView {
        id: guidedDrillingWizardStackView
        anchors {
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
        height: currentItem ? currentItem.implicitHeight : 0
        initialItem: modeSelector

        Component {
            id: modeSelector
            Frame {
                ButtonGroup {
                    id: modeSelectorButtons

                    onClicked: (button) => {
                                   controller.mode = button.value
                               }
                }

                ColumnLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                        text: "Mode Selector"
                        font.pointSize: 20
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter

                        Button {
                            ButtonGroup.group: modeSelectorButtons
                            text: "Drilling"
                            checkable: true

                            readonly property int value: GuidedDrillingScreenController.Mode.Drill
                            checked: controller.mode === value
                        }
                        Button {
                            ButtonGroup.group: modeSelectorButtons
                            text: "Screw Driving"
                            checkable: true

                            readonly property int value: GuidedDrillingScreenController.Mode.Screw
                            checked: controller.mode === value
                        }
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignRight|Qt.AlignBottom
                        Button {
                            text: "Next"
                            enabled: controller.mode !== GuidedDrillingScreenController.Mode.None
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
                ButtonGroup {
                    id: materialSelectorButtons

                    onClicked: (button) => {
                                   controller.material = button.value
                               }
                }

                ColumnLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                        text: "Material Selector"
                        font.pointSize: 20
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter

                        Button {
                            ButtonGroup.group: materialSelectorButtons
                            text: "Wood"
                            checkable: true

                            readonly property int value: GuidedDrillingScreenController.MaterialType.Wood
                            checked: controller.material === value
                        }
                        Button {
                            ButtonGroup.group: materialSelectorButtons
                            text: "Concrete"
                            checkable: true

                            readonly property int value: GuidedDrillingScreenController.MaterialType.Concrete
                            checked: controller.material === value
                        }
                        Button {
                            ButtonGroup.group: materialSelectorButtons
                            text: "Metal"
                            checkable: true

                            readonly property int value: GuidedDrillingScreenController.MaterialType.Metal
                            checked: controller.material === value
                        }
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
                            enabled: controller.material !== GuidedDrillingScreenController.MaterialType.None
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

                    ComboBox {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 250

                        model: Array(...controller.filteredBits)

                        delegate: ItemDelegate {
                            width: parent.width
                            text: controller.bitName(modelData)
                        }

                        displayText: controller.bitName(currentValue)

                        onActivated: (index) => controller.bit = model[index]
                        currentIndex: indexOfValue(controller.bit)
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
                            enabled: controller.bit !== GuidedDrillingScreenController.Bit.None
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
