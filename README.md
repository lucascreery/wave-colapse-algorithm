# wave-colapse-algorithm

My implimentation of the [Wave Function Collapse Algorithm](https://github.com/mxgmn/WaveFunctionCollapse) in c++.

## Requirements

-SFML

## Usage
To build:
```
make
```

To use:
```
wfc.exe (output width) (output height) (n) (input image) (output image) (optional: match boundary patterns)
```
The ouput width and height specify the desired output size of the image in pixels and n represents the pattern size. Supported imput and output formats are any file extension supported by SFML image library. The `match boundary condition` specifies whether or not the ouput image will use the same patterns on the top and bottom border as the input image. Simply putting any value in for the parameter will enable matching boundary conditions.

## Examples
A few output images are shown in the example images. Complimentary input images can be found in the original Wave Function Collapse Algorithm repository.
