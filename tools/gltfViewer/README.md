# Simple glTL Viewer

The gltfViewer application can be used to view glTF files, including
selecting cameras and playing animations.

Usage: `gltf [-f] [-c name] [-p name] [-l] file.gltf`

## Options

*  `-h, --help`               : Display help.
*  `-v, --version`            : Display version information.
*  `-f, --fullScreen`         : Show full screen.
*  `-p, --play <animations>`  : Play named animation
*  `-l, --loop`               : Loop animations
*  `-c, --camera <camera>`    : Use named camera


## Operation

The application will render the glTF file passed as an
argument on the command line.

On macOS, the application is associated to .gltf and .glb files
so double clicking on one such file in Finder will launch the
application.

To load another file, simply drag & drop it onto the main window.

The `--fullscreen` argument can be used to render the scene in full screen.
Using the `Ctrl-F` key combination will toggle the full screen window.

If one or more cameras are present in the file, the first one
will initially be used to render the scene. A specific camera can
be selected using the `--camera <name>` argument. Using the `Tab` 
and `BackTab` keys will cycle through available cameras.

To play an animation, use the `--play <name>` argument. The animation
will play once. To play it continuously, use the `--loop` argument.
The `Space` key can be used to pause or resume the current animation.
