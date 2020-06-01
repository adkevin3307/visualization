#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iomanip>

#include "Method.h"
#include "Volume.h"

using namespace std;

class IsoSurface : public Method {
	using super = Method;

private:
	float iso_value;
	vector<GLfloat> _vertex;

	pair<glm::vec3, glm::vec3> interpolation(glm::ivec3 p1, glm::ivec3 p2);

public:
	IsoSurface(Volume &volume);
	IsoSurface(string inf_file, string raw_file);
	IsoSurface(string inf_file, string raw_file, float iso_value);
	~IsoSurface();

	void run();
	vector<GLfloat>& vertex() override;
	vector<int> attribute() override;
	GLenum render_mode() override;
};