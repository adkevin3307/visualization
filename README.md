# Iso Surface
讀入 Volume 後，藉由 iso value 產生 iso surface，iso surface 以大量三角形模擬，其中運用 linear interpolation 求得三角形頂點座標及法向量。

## Environment
* Windows
* MinGW-W64
    * `g++ 8.1.0`
* `opengl 4.4`
* `glfw 3.3.2`
* `glad` with correct version
* `glm 0.9.9.7`

> If you want to show histogram, you'll need
> * Python 3.7.3
> * matplotlib 3.1.2

## Usage

### Compile and Execute
* `make`
    * Compile all file
    * Generate and Execute`main.exe`
* `make compile`
    * Compile all file
    * Generate `main.exe`
* `make execute`
    * Execute `main.exe`
* `make clean`
    * Remove all `.exe`
    * Remove `./Data/Scalar/histogram.txt`

> If you want to show histogram, you'll need to run
> * `make histogram execute`
>     * Compile all file
>     * Generate `histogram.txt` at `./Data/Scalar`
>     * Generate and Execute `main.exe`
> * `python histogram.py`

### Control
* Scroll mouse wheel: Zoom in and Zoom out
* Click and Drag mouse left button: Move camera position
    * Spherical Coordinate System
* Click and Drag mouse right button: Move camera look at position
* GUI: Control clip plane

## Class Structure

#### `Volume`
* read inf file and raw file
* calculate vertices and gradient
* generate **float** type data

#### `IsoSurface`
* calculate iso surface after volume read data
* generate triangles' vertices and normals

#### `Mesh`
* setting VBO and VAO with triangles vertices and normals
* use `BufferManagement` to manage VAO and VBO

#### `BufferManagement`
* provide static function to operate VAO and VBO

#### `Transformation`
* provide function to operate model, view, projection matrix

#### `Camera`
* handle all operation about camera

#### `Shader`
* read shader file
* provide function to set uniform

#### `WindowManagement`
* manage window
* initialize glfw
* set callback function
* run main loop for window

## Special
* `Volume` class can handle different data type
* light position will follow camera position

## Challenge
* well-defined structure for reuse