# coding: utf8

"""
    gleslint.py

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Timo Buske <timo.buske@kdab.com>

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
"""

import gleslint.gleslint
import sys
import os
import subprocess

def help():
    print("Usage:")
    print("  python gleslint.py pattern[,pattern][,pattern][...] [-r]")
    print("pattern examples:")
    print("  *")
    print("  *.dds")
    print("  ../envmaps/*.dds,../textures/*.png")

def checkImageMagick():
    try:
        subprocess.check_output(["magick", "-help"])
    except:
        print("ImageMagick not available!")
        print("Please install!")
        sys.exit(1)

def main():
    if len(sys.argv) <= 1 or "-h" in sys.argv or "-help" in sys.argv or "--help" in sys.argv:
        help()
    else:
        checkImageMagick()
        gleslint.gleslint.GlesLint(os.getcwd(), sys.argv)

if (__name__ == "__main__"):
    main()
