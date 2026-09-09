# cubicSplineReconstruction

The sign to be reconstructed should be a png file named img.png in the same directory as binary.
The points to be reconstructed are to be stored in a file called data.txt in the local directory.

The visibility of points can be toggled by pressing 'P' key.
The visibility of the Spline Curve can be toggle by pressing the 'L' key.
The visibility of the curve to be reconstructed i.e. img.png can be toggled by pressing 'I' key.
Pressing Ctrl+S saves the current points of the current spline in data.txt, and also saves a screenshot of the spline as image.png in current directory.
Pressing Ctrl+L loads the points from data.txt.

Compilation requires installation of SDL3, SDL3_image and SDL3_TTF libraries.
Running requires the corresponding dynamic libraries/shared libraries and also a TTF file in the local directory. A static standalone portable windows executable has also been provided for convenience. Makefile has been provided as well.

Compiling with the static flag produces a static binary(only on windows, still requires ttf file). Compiling with debug flag on windows allows console to be opened as well.
