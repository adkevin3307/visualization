#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "Method.h"
#include "Volume.h"

using namespace std;

class IsoSurface : public Method {
	using super = Method;

private:
	float iso_value;

	pair<glm::vec3, glm::vec3> interpolation(glm::ivec3 p1, glm::ivec3 p2);
	void calculate();

public:
	IsoSurface(string inf_file, string raw_file);
	~IsoSurface();

	void run();
	void run(float value);

	vector<int> attribute_size() override;
};