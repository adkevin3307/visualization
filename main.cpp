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
        float iso_value;

        cout << "iso value: ";
        cin >> iso_value;

        IsoSurface iso_surface("./Data/Scalar/" + filename + ".inf", "./Data/Scalar/" + filename + ".raw");
        iso_surface.run(iso_value);

        WindowManagement window_management;
        window_management.init();

        Shader shader("./src/shader/vertex.glsl", "./src/shader/fragment.glsl");
        shader.use();

        Mesh mesh(iso_surface.vertex(), iso_surface.volume_shape());
        mesh.init();

        window_management.main_loop(mesh, shader);
    }

    return 0;
}