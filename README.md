# Kuesa module for Qt 3D

Kuesa is a module for Qt 3D which provides:
* Support for glTF 2 importing
* Helpers for accessing and manipulating loaded content
* Custom PBR material
* Custom forward renderer for adding effects
* A Qt Creator template for quickly creating new Kuesa based projects

## Requirements

**Note:** this repository requires [git-lfs](https://git-lfs.github.com) support.

Kuesa requires Qt 3D 5.12.0 or later.

## Optional Dependencies

Kuesa can optionnally use the [Draco](https://github.com/google/draco) mesh compression library, 
either through an embedded version, or a version present on the system.
Draco can be used to dramatically decrease the size of glTF files.

Kuesa supports the ``KHR_draco_mesh_compression`` glTF extension as defined here:
https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_draco_mesh_compression/

By default, Kuesa will build with its own embedded version of the Draco library.
This can be enforced with:

    qmake kuesa.pro -- --draco=qt
    
To build Kuesa without any support for Draco, run qmake like this:

    qmake kuesa.pro -- --draco=no
    
To build Kuesa with an external version of Draco, run qmake like this:

    qmake kuesa.pro -- --draco=system
    
If Draco is not installed in the default location on your system, you can
set the `DRACOSDK` environment variable to point to where Draco is installed.
Use the `DRACOSDK_LIBS` environment variable to point to the folder containing
the Draco libraries.

Note that Kuesa has last been tested with Draco at commit 8833cf878e6fd43c5a3fd6e4231212e25e25e632.

The asset pipeline editor, provided with Kuesa, is able to compress existing glTF 2.0 
assets with Draco.

## Components

Kuesa is composed of:
* A Qt module with core classes
* A QML plugin
* The assetpipelineeditor tool to preview and process glTF 2.0 files
* The assetprocessor, a command line tool to compress meshes, embed or extract binary assets, etc
* The cubemaptooctahedralmap tool to convert cube maps to octahedral maps
  to enable PBR rendering on ES 2 platforms

### Asset Pipeline Editor

The Asset Pipeline Editor can be used both by the designer and in order
to preview and pre-process the content of glTF files.

The designers will use the editor to check the rendering of the scene,
verify that materials are correct, preview animations, etc.

The developers will also use the editor check model properties such as sizes,
asset names, etc.

The editor can also be used to compress meshes, embed (or extract) binary assets, etc.

In the 3d view, use the mouse to click on meshes to select them in the collection browser.
Use:
* Simple click to select meshes
* Shift-click to select materials
* Alt-click to select entities

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

## 3rd-party
Kuesa uses the following 3rd-party software:
* Qt 5.12 or later, licensed under LGPLv3
www.qt.io

* Draco, licensed under Apache License 2.0 and authored by Google Inc.
https://github.com/google/draco

* MikkTSpace, licensed under a custom license and authored by Morten S. Mikkelsen
http://mmikkelsen3d.blogspot.com/

