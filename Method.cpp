#include "Method.h"

using namespace std;

Method::Method(string inf_file, string raw_file)
{
    this->volume = Volume(inf_file, raw_file);
}

Method::~Method()
{

}

vector<GLfloat>& Method::vertex()
{
    return this->_vertex;
}

glm::vec3 Method::volume_shape()
{
    return glm::vec3(this->volume.shape()) * this->volume.voxel_size();
}