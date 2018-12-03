#Kuesa module for Qt 3D

Kuesa is a module for Qt 3D which provides:
* Support for glTF 2 importing
* Helpers for accessing and manipulating loaded content
* Custom PBR material
* Custom forward renderer for adding effects
* A Qt Creator template for quickly creating new Kuesa based projects

## Requirements

Kuesa requires Qt 3D 5.12.0 or later.

## Components

Kuesa is composed of:
* A Qt module with core classes
* A QML plugin
* The assetpipelineeditor tool to preview and process glTF 2.0 files
* The cubemaptooctahedralmap tool to convert cube maps to octahedral maps
  to enable PBR rendering on ES 2 platforms

## Installation

Kuesa builds as a Qt module and will be installed alongside Qt.

    qmake
    make
    make install

Should examples not be built, try:

    make sub-examples
    cd examples
    make install

To install the Qt Creator template, copy the entire kuesa folder
from ``tools/qtc_templates`` to the ``templates/wizards`` folder inside
the Qt Creator install location.
