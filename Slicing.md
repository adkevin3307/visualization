# Slicing Method
class `Slicing` is inheriting from class `Method`

## Explain
![](https://i.imgur.com/gGOXQ71.png)
* each slice will locate in volume data, from the farest position to nearest position
* generate every slices' vertices and texture coordinates

## Special
* generate all condition for axis aligned slicing method when class initilaize, speed up

## Dillemma
* glTexImage3D arguments meaning, shape (z, y ,x)
* transfer function for pretty volume render

## Class Structure
### Member
```
_index (int):
    decide which axis and direction should use

_vertex (vector<vector<GLfloat>>):
    shape should be (6, ), store 6 conditions for axis aligned slicing vertices

_texture_1d (vector<float>):
    per element should be (r, g, b, a), transfer function for all different intensity color and opacity

_texture_3d (vector<float>):
    per element should be (normal_x, normal_y, normal_z, intensity), look up texture for every point

_template (vector<vector<float>>):
    template for three axis (per axis ) slices coordinate
```

### Member Function
#### Private
```
generate_texture_1d:
    generate texture 1D and store at _texture_1d

generate_texture_3d:
    generate texture 3D and store at _texture_3d

push:
    push data into _vertex[index]
    - parameters:
        data (glm::vec3): the data will be push_back into _vertex[index]
        index (int): value should be [0, 1, 2, 3, 4, 5], decide which _vertex should be use

calculate:
    calculate 6 different slices vertices
```

#### Public
```
Slicing:
    empty constructor

Slicing:
    the constructor use super constructor to intialize Volume
    - parameters:
        inf_file (string): inf file path
        raw_file (string): raw file path

~Slicing:
    destructor

run:
    calculate which _vertex should use
    - parameters:
        view_position (glm::vec3): camera position
    - return:
        (bool): if _index change, return true, else false

texture_1d:
    get texture 1D
    - return:
        (vector<float>&): return _texture_1d

texture_3d:
    get texture 3D
    - return:
        (vertor<float>&): return _texture_3d

texture_1d_shape:
    get texture 1D shape
    - return:
        (glm::ivec3): return amount of _texture_1d element (per element is 4 float), shape should be (x, 0, 0)

texture_3d_shape:
    get texture 3D shape
    - return:
        (glm::ivec3): return amount of _texture_3d element (per element is 4 float), shape should be (x, y, z)

vertex:
    override parent class virtual function, get vertices let will be render
    - return:
        (vector<GLfloat>&): return _vertex[_index]

attribute:
    override parent class virtual function, get attribute let will use in VAO
    - return:
        (vector<int>): return { 3, 3 }, means position (x, y, z) and texture coordinate (s, r, t)

render_mode:
    override parent class virtual function, get render mode let will use in glDrawArrays
    - return:
        (GLenum): return GL_TRIANGLES, value should be [GL_TRIANGLES, GL_LINES, ...]
```