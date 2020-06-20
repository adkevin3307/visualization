#pragma once

#include "constant.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"

using namespace std;

class Transformation {
private:
    glm::mat4 _model, _view, _projection;
    Shader shader;

public:
    Transformation(Shader shader);
    ~Transformation();

    void init(TRANSFORMATION type);
    void set_model(TRANSFORMATION type, glm::vec3 v, float angle = 0.0);
    void set_view(glm::mat4 view);
    void set_projection(GLfloat width, GLfloat height, GLfloat rate, GLfloat near, GLfloat far);
    void set(bool set_model = false);
};