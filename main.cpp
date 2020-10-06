#include "WindowManagement.h"

using namespace std;

int main()
{
    WindowManagement window_management;

    window_management.init();
    window_management.main_loop();

    return 0;
}