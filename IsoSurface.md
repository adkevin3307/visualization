# IsoSurface Method
class `IsoSurface` is inheriting from class `Method`

## Explain
![](https://i.imgur.com/dwyGm3Y.png)
* use marching cube
* calculate iso surface each cube (8 voxel per cube) and generate triangles
* all triangle vertex position use look up table

## Special
* iso value will be `Volume` voxel intensity average, when iso value is not defined

## Dillemma
* handle triangles' vertices with special structure

## Member
```
iso_value (float):
    iso value for calculating iso surface

_vertex (vector<GLfloat>):
    iso surface vertices compose by triangles
```

## Member Function
### Private
```
interpolation:
    calculate position and normal with interpolation
    - parameters:
        p1 (glm::ivec3): point one position
        p2 (glm::ivec3): point tow position
    - return:
        (pair<glm::vec3, glm::vec3>): return position calculate with interpolation, get and calcualte normal from correspond voxel
```

### Public
```
IsoSurface:
    the constructor use super constructor to initialize Volume
    - parameters:
        inf_file (string): inf file path
        raw_file (string): raw file path

IsoSurface:
    the constructor use super constructor to initialize Volume
    - parameters:
        inf_file (string): inf file path
        raw_file (string): raw file path
        iso_value (float): iso value

~IsoSurface:
    destructor

run:
    calculate iso surface triangles' vertices

shape:
    override parent class virtual function, get shape let will be shift result
    - return:
        (glm::vec3): return data shape

vertex:
    override parent class virtual function, get vertices let will be render
    - return:
        (vector<GLfloat>&): return _vertex

attribute:
    override parent class virtual function, get attribute let will use in VAO
    - return:
        (vector<int>): return { 3, 3 }, means position (x, y, z) and normal (x, y, z)

render_mode:
    override parent class virtual function, get render mode let will use in glDrawArrays
    - return:
        (GLenum): return GL_TRIANGLES, value should be [GL_TRIANGLES, GL_LINES, ...]
```