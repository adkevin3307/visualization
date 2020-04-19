#include "WindowManagement.h"
#include "IsoSurface.h"
#include "Shader.h"
#include "Mesh.h"

#include <iostream>
#include <string>

using namespace std;

int main()
{
    string filename;
    while (cout << "filename: ", cin >> filename) {
        cout << "==================== INFO ====================" << '\n';

        IsoSurface iso_surface("./Data/Scalar/" + filename + ".inf", "./Data/Scalar/" + filename + ".raw");
        iso_surface.run();

        WindowManagement window_management;
        window_management.init();

        Shader shader("./src/shader/vertex.glsl", "./src/shader/fragment.glsl");
        shader.use();

        Mesh mesh(iso_surface.vertex(), iso_surface.attribute(), iso_surface.volume_shape(), iso_surface.render_mode());
        mesh.init();

        window_management.main_loop(mesh, shader);

        cout << "==============================================" << '\n';
    }

    return 0;
}