#include "Method.h"
#include "IsoSurface.h"
#include "WindowManagement.h"
#include "Shader.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main()
{
    string filename;
    while (cout << "filename: ", cin >> filename) {
        cout << "==================== INFO ====================" << '\n';

        Method *method = new IsoSurface("./Data/Scalar/" + filename + ".inf", "./Data/Scalar/" + filename + ".raw");
        method->run();

        cout << "==============================================" << '\n';

        WindowManagement window_management;
        window_management.init();

        Shader shader("./src/shader/vertex.glsl", "./src/shader/fragment.glsl");
        shader.use();

        vector<Mesh> mesh{Mesh(method, glm::vec4(0.41, 0.37, 0.89, 1.0))};
        for (size_t i = 0; i < mesh.size(); i++) {
            mesh[i].init();
        }

        window_management.main_loop(mesh, shader);

        delete method;
    }

    return 0;
}