#include "Renderer/Renderer.h"

#ifdef DEBUG
#pragma comment(lib, "debug/glfw3.lib")
#else   
#pragma comment(lib, "release/glfw3.lib")
#endif

int main() {
    Renderer MainRenderer{};

    if (!MainRenderer.Initialize()) {
        return -1;
    }

    MainRenderer.Run();
    MainRenderer.Shutdown();

    return 0;
}
