#include "Method.h"

using namespace std;

Method::Method()
{

}

Method::Method(string inf_file, string raw_file)
{
    this->volume = Volume(inf_file, raw_file);
}

Method::Method(Volume &volume)
{
    this->volume = volume;
}

Method::~Method()
{

}