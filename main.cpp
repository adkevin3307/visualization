#include "WindowManagement.h"
#include "Shader.h"

using namespace std;

int main()
{

    WindowManagement window_management;
    window_management.init();

    Shader shader("./src/shader/vertex.glsl", "./src/shader/fragment.glsl");
    shader.use();

    window_management.main_loop(shader);

    return 0;
}