import gleslint.gleslint
import sys
import os
import subprocess

def checkPythonVersion():
    if not sys.version.startswith("3"):
        print("Python 3 is needed! This is Python {}.".format(sys.version.split(" ")[0]))
        sys.exit(1)

def checkImageMagick():
    try:
        subprocess.run(["magick", "-help"], capture_output=True)
    except:
        print("ImageMagick not available!")
        print("Please install!")
        sys.exit(1)

def main():
    checkPythonVersion()
    checkImageMagick()
    gleslint.gleslint.GlesLint(os.getcwd(), sys.argv)

if (__name__ == "__main__"):
    main()
