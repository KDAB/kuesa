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
    if "-h" in sys.argv or "-help" in sys.argv or "--help" in sys.argv:
        help()
    else:
        checkImageMagick()
        gleslint.gleslint.GlesLint(os.getcwd(), sys.argv)

if (__name__ == "__main__"):
    main()
