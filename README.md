# Kuesa module for Qt 3D

Kuesa is a module for Qt 3D which provides:
* Support for glTF 2 importing
* Helpers for accessing and manipulating loaded content
* Custom PBR material
* Custom forward renderer for adding effects
* A Qt Creator template for quickly creating new Kuesa based projects

## Requirements

Kuesa requires Qt 3D 5.12.0 or later.

## Optional Dependencies

If present on the system, Kuesa will detect and use the Draco mesh compression library.
Draco can be used to dramatically decrease the size of glTF files.

Kuesa supports the ``KHR_draco_mesh_compression`` glTF extension as defined here:
https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_draco_mesh_compression/

In order to install Draco, please visit https://github.com/google/draco

If you have built Kuesa previously, you will need to run qmake again and
update the existing build after installing Draco.

If you have Draco installed but wish to build Kuesa without the support
for it, run qmake again like this:

    qmake kuesa.pro -- --draco=no

If Draco is not installed in the default location on your system, you can
set the DRACOSDK environment variable to point to where Draco is installed.
Use the DRACOSDK_LIBS environment variable to point to the folder containing
the Draco libraries.

In order to produce compressed glTF files, please use the gltf-pipeline tool at
https://github.com/AnalyticalGraphicsInc/gltf-pipeline

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
