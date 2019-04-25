# GLES Linter

This Python script checks if assets are ready to be used under GLES.

It's extensible by writing modules. Currently only the ES2 module is implemented.

## Requirements

* [Python 2 or Python 3](http://www.python.org)
* [ImageMagick](https://www.imagemagick.org)

## Usage

    python gleslint.py <path/pattern>[,<path/pattern>,...] [-r]

* -r: Globs recursively

Example:

    python gleslint.py ../../examples/kuesa/assets/* -r

This would glob all subdirs of the specified path for files with known extensions and check them. By now these are:

* .dds
* .png
* .jpg

glTF files and mesh data is not checked yet.

## Modules

Currently only the es2 module (gleslint/module_es2.py) is implemented. You can modify it or you can implement your own module by inheriting from Module (gleslint/module.py) and overriding those methods:

* lint_dds(self, path)
* lint_png(self, path)
* lint_jpg(self, path)

If you are writing new lint modules you'll need to wire them up and delegate in gleslint/gleslint.py!

## ES2 Module

The current implementation throws...

Errors:

* File invalid.
* Texture format not supported.
* File not 8 bit.

Warnings:

* Cubemap with mipmaps. Can't be used as specular env map for PBR. Convert to octahedron map instead!
* Bytes per pixel > 4. Check format!
* Compression detected. Check if supported!
* sRGB detected. Must not end up in *SRGB* GL texture!

## 3rd party code

GLES linter is using 'dds_loader' to read dds headers.
