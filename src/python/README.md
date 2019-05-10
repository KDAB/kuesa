# Python Binding for Kuesa

This project is built separately from Kuesa itself. It provides
a python binding for all of Kuesa's C++ features.

In particular, you can use python to load and render gltf files, access
asset collections, control animations, etc.

## Prerequisites

This requires [Qt For Python](https://wiki.qt.io/Qt_for_Python), built for the same version of Qt used to build Kuesa.

If you intend to use Animations, and Clocks in particular (to control the speed and direction of animations), please make sure you used Qt for Python 5.13 or later.

## Building

If the python environment is probably setup for Qt For Python,
then building is as simple as:
```
mkdir build
cd build
cmake ..
make
make install
```
If you build your own version of Qt For Python, we recommend
using ``virtualenv`` to isolate your build from the main
operating system.

## Example
``examples/kuesa/python`` contains a small example which loads
a gltf file and builds a widget based UI to control animations.
