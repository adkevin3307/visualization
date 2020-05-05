#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "Method.h"

using namespace std;

class Slicing : public Method {
    using super = Method;

private:
    vector<GLfloat> _vertex;
	vector<float> _texture_1d, _texture_3d;
	vector<vector<float>> _template;

	void generate_texture_1d();
	void generate_texture_3d();
	void push(glm::vec3 data);
	void calculate(double index, int max_index, glm::ivec3 shape);

public:
	Slicing();
	Slicing(string inf_file, string raw_file);
	~Slicing();

	void run(glm::vec3 view_position);
	vector<float>& texture_1d();
	vector<float>& texture_3d();
	glm::ivec3 texture_1d_shape();
	glm::ivec3 texture_3d_shape();

	vector<GLfloat>& vertex() override;
	vector<int> attribute() override;
	GLenum render_mode() override;
};