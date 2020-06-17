# Stream Line Method
class `StreamLine` is inheriting from class `Method`

## Explain
![](https://i.imgur.com/exBZkM0.png)
* trace stream line position with current position vector
![](https://i.imgur.com/5CEm5rp.png)
* calculate next position with rk2 method

## Special
* use transfer function to show vector magnitude

## Dillemma
* `glLineWidth` has been deprecated

## Member
```
min_vector_magnitude (float):
    minium magnitude for vector

max_vector_magnitude (float):
    maxium magnitude for vector

data (vector<vector<glm::vec2>>):
    vector data

_vertex (vector<GLfloat>):
    stream line vertices
```

## Member Function
### Private
```
load_data:
    load data from filename
    - parameters:
        filename (string): filename

check:
    check whether position is in data range
    - parameters:
        position (glm::vec2): position want to check

vector_interpolation:
    interpolate vector with bilinear interpolation
    - parameters:
        position (glm::vec2): position that will be calculate
    - return:
        (glm::vec2): position's vector

rk2:
    use rk2 method to calculate next position
    - parameters:
        position (glm::vec2): initial position
        h (float): h in rk2 method
    - return:
        (glm::vec2): new position after rk2 method

calculate:
    use rk2 method to calculate position and generate stream line
    - parameters:
        position (glm::vec2): initial position
        delta (float): decide trace stream line with positive or negative way
        table (vector<vector<bool>>&): record position already be used
        scale (int): current scale for pyramid of grids
    - return:
        (vector<GLfloat>): stream line points
```

### Public
```
StreamLine:
    constructor
    - parameters:
        filename (string): vector filename

~StreamLine:
    destructor

run:
    calculate stream line vertices

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
        (vector<int>): return { 2, 3 }, means position (x, y) and color (r, g, b)

render_mode:
    override parent class virtual function, get render mode let will use in glDrawArrays
    - return:
        (GLenum): return GL_POINTS, value should be [GL_TRIANGLES, GL_LINES, GL_POINTS, ...]
```