# Sammon Mapping
class `SammonMapping` is inheriting from class `Method`

## Explain
![](https://i.imgur.com/YZ4rNsk.png)
* Sammon Mapping will consider origin distance

## Special
* use kmeans to auto divide group

## Dilemma
* if amount of data greater than 1000, `Sammon Mapping` will need a lot of time

## Member
```
group (int):
    how many group will be divided

label (vector<int>):
    store each data's group

data (vector<vector<float>>):
    store data

_distance (vector<vector<float>>):
    store distance for each data

_vertex (vector<GLfloat>):
    mapping vertices
```

## Member Function
### Private
```
distance:
    calculate distance with multiple features data
    - parameters:
        p1 (vector<float>): point one position
        p2 (vector<float>): point two position
    - return:
        float: return distance between two position

normalize:
    normalize data with each feature

kmeans:
    divide data to group
    - parameters:
        group (int): data will be divided to group amount

calculate_distance:
    if amount of data less than 1000, it will generate _distance table

load_custom:
    load custom file to data
    - parameters:
        filename (string): custom filename

load_scalar:
    load scalar file to data, it will random sample

descent:
    calculate delta for update mapping position
    - parameters:
        lambda (float): lambda for calculate delta
        index (glm::ivec2): index for data and mapping point
        mapping_point (vector<glm::vec2>): store all mapping points
    - return:
        (glm::vec2): return delta for update
```

### Public
```
SammonMapping:
    empty constructor

SammonMapping:
    constructor for load custom data
    - parameters:
        filename (string): custom filename

SammonMapping:
    constructor use super constructor to initialize Volume
    - parameters:
        inf_file (string): inf file path
        raw_file (string): raw file path

~SammonMapping:
    destructor

run:
    calculate mapping point position
    - parameters:
        alpha (float): update lambda with alpha

shape:
    override parent class virtual function, get shape let will be shift result
    - return:
        (glm::vec3): return glm::vec3(0.0)

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