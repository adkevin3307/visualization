# Iso Surface
讀入 Volume 後，藉由 iso value 產生 iso surface，iso surface 以大量三角形模擬，其中運用 linear interpolation 求得三角形頂點座標及法向量。

## Environment
* Windows
* MinGW
    * `g++ 9.2.0`
* `opengl 4.4`
* `glfw 3.3.2`
* `glad` with correct version
* `glm 0.9.9.7`

```
if you want to show histogram, you'll need
* Python 3.7.3
* matplotlib 3.1.2
```

## Usage

### Compile and Execute
* `make`
    * compile all file
    * generate and run `main.exe`
* `make compile`
    * compile all file
    * generate `main.exe`
* `make execute`
    * execute `main.exe`
* `make clean`
    * remove all `.exe`
    * remove `./Data/Scalar/histogram.txt`

```
if you want to show histogram, you'll need to run
* `make histogram execute`
    * compile all file
    * generate `histogram.txt` at `./Data/Scalar`
    * generate and execute `main.exe`
* `python histogram.py`
```

### Control
* scroll mouse wheel for zoom in and zoom out
* click mouse left button and drag for move camera position
    * Spherical Coordinate System
* click mouse right button and drag for move camera look at position

## Class Structure

#### `Volume`
* read inf file and raw file
* calculate vertices gradient
* generate **float** type data

#### `IsoSurface`
* calculate iso surface after volume read data
* generate triangles vertices and normals

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