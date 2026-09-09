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

Known Issues:
1) When dragging the points too fast, they may move out of the cursor and stop being dragged.
2) If a point, which is ahead of another point in terms of parameterisation, gets dragged over the point, they collapse into a single point and the reconstruction temporarily fails. This can be immediately fixed by dragging the now combined point once again, which separates the two, thus bringing back the spline. No such error occurs when the point being dragged is behind the other point.
3) There are some memory leaks during cleanup
4) Static build only works for linux and even on windows does now include a packaged font.
