# Visualization Method
* Iso Surface
* Slicing Method
* Stream Line

## Environment
* Windows
* MinGW-W64
    * `g++ 8.1.0`
    * `c++ 17`
* `opengl 4.4`
* `glfw 3.3.2`
* `glad` with correct version
* `glm 0.9.9.7`
* `Dear ImGui 1.76`
* `ImPlot 0.1`

## Usage
### Compile and Execute
* `make`
    * Compile all file (exclude test.cpp)
    * Generate and Execute `main.exe`
* `make static`
    * Compile all file with `g++` static mode (exclude test.cpp)
    * Generate and Execute `main.exe`
* `make test`
    * Compile all file (exclude main.cpp)
    * Generate and Execute `main.exe`
* `make clean`
    * Remove all `.exe`, `out`
    * Remove `imgui.ini`
    * Remove `transfer_function.txt`
    * Remove `obj` directory

### Control
* Scroll mouse wheel: Zoom in and Zoom out
* Click and Drag mouse left button: Move camera position
    * Spherical Coordinate System
* Click and Drag mouse right button: Move camera look at position
* GUI
    * Select visualization method
    * Select data
    * Control Clip Plane

## Class Structure
#### `Volume`
* read inf file and raw file
* calculate vertices and gradient
* generate **float** type data

#### `Method`
* provide basic function
* set attribute_size virtual function

#### `IsoSurface`
* inheritance `Method`
* implement virtual function
* calculate iso surface after `Volume` read data
* generate triangles' vertices and normals

#### `Slicing`
* inheritance `Method`
* implement virtual function
* calculate slicing after `Volume` read data
* generate triangles' and textures' coordinates for render

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