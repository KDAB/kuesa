# Kuesa Runtime module for Qt 3D

![alt text](https://github.com/KDAB/kuesa/blob/dev/examples/kuesa/assets/misc/kuesa_carscene.png)


### Kuesa Runtime is a module for Qt 3D which provides:
* Support for glTF 2 importing
* Helpers for accessing and manipulating loaded content
* PBR and custom material support
* Custom forward renderer for adding effects
* A Qt Creator template for quickly creating new Kuesa based projects

It is the developer facing part of the Kuesa 3D design and integration workflow.

### Kuesa Studio is the designer facing part of Kuesa.

It provides:

* glTF2 exporters and Kuesa specific glTF extensions for major 3D design softwares:
    * Kuesa for Blender
    * Kuesa for 3ds Max
    * Kuesa for Maya (coming soon)
* A library of real time materials that can be exported to glTF and previewed in Blender
* Tooling around asset conditionning to decrease asset size and optimize for embedded use cases.

More information about Kuesa Studio can be found [here](https://www.kdab.com/kuesa/).

## Requirements

**Note:** this repository requires [git-lfs](https://git-lfs.github.com) support.

Kuesa Runtime requires Qt 3D from Qt 5.12.8, Qt 5.14.2 or Qt 5.15.

Earlier Qt 3D versions can still be used but might be suffering from bugs
that have been identified and fixed since.

### Hardware

#### Desktop

Anything with OpenGL 3.2 support (or more recent) is adequate.

Currently supported operating systems are:
* Linux
* Windows
* Mac OS

#### Embedded

Here's a list of Chipsets/Devices Kuesa has successfully been tested on:
* Apple iPad 5 (PowerVR GT7600)
* Apple iPhone 7
* OnePlus 3T (Android, Qualcolmm Snapdragon 821/Adreno 430)
* NVidia Tegra K1 (embedded Linux)
* iMX8 (embedded Linux)

Kuesa Runtime supports anything with OpenGL ES 3.2 or above.

OpenGL ES 3.1 or lower is supported but with a limited feature set depending on available OpenGL extensions.
* For HDR support, GL_OES_texture_half_float is required
* For anti aliasing, ARB_texture_multisample and GL_EXT_color_buffer_half_float are required

## Optional Dependencies

Kuesa can optionnally use the [Draco](https://github.com/google/draco) mesh compression library,
either through an embedded version, or a version present on the system.
Draco can be used to dramatically decrease the size of glTF files.

Kuesa supports the ``KHR_draco_mesh_compression`` glTF extension as defined [here](https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_draco_mesh_compression/).

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

The glTF editor, provided with Kuesa, is able to compress existing glTF 2.0
assets with Draco.

## Components

Kuesa Runtime is composed of:
* A Qt module with core classes
* A QML plugin
* The gltfViewer tool to preview glTF 2.0 files, including selecting cameras and animations

## Installation

Kuesa builds as a Qt module and will be installed alongside Qt. The main advantage
is that once installed, using Kuesa in your projects is as easy as adding ``QT += kuesa``
to your project file. The down side is that building Kuesa from source can be a little
more complicated if you're not used to building Qt itself already.

*Note*: you DO NOT have to build Qt from source, building Kuesa against a pre-installed
version of Qt should work fine.

But building a module means that Kuesa has dependencies that are similar to those that
apply when building Qt from source (Instructions on how to build Qt from source can be
found [here](https://wiki.qt.io/Building_Qt_5_from_Git)). In particular:
* perl must be installed and included in the PATH
* if you have downloaded Kuesa as an archive from github, you must create an
  empty ``.git`` folder at the root of the kuesa source directory.

Once these are installed, ``syncqt`` will be able to generate the module headers at
build time. Simply run:

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

## Documentation

The reference documentation is available at: https://kdab.github.io/kuesa/

## 3rd-party
Kuesa uses the following 3rd-party software:
* Qt 5.12 or later, licensed under LGPLv3
www.qt.io

* Draco, licensed under Apache License 2.0 and authored by Google Inc.
https://github.com/google/draco

* MikkTSpace, licensed under a custom license and authored by Morten S. Mikkelsen
http://mmikkelsen3d.blogspot.com/

## Python Binding

A python binding it available, based on PySide 2. Please check the build instructions
in the ``src/python`` and the sample application in ``examples/kuesa/python``.

## Support

Please file issues [here](https://github.com/KDAB/Kuesa/issues)

## About KDAB

Kuesa is maintained by Klarälvdalens Datakonsult AB (KDAB).

KDAB, the Qt experts, provide consulting and mentoring for developing
Qt applications from scratch and in porting from all popular and legacy
frameworks to Qt. We continue to help develop parts of Qt and are one
of the major contributors to the Qt Project. KDAB is also behind the development effort of the Qt 3D module. We can give advanced or standard trainings anywhere around the globe.
