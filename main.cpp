#include "Renderer/Renderer.h"

#include <utility>

#ifdef DEBUG
#pragma comment(lib, "debug/glfw3.lib")
#else
#pragma comment(lib, "release/glfw3.lib")
#endif

int main() {
    Scene MainScene{};

    GameObject MainObject{ "MainObject" };
    MainScene.AddGameObject(std::move(MainObject));

    Camera& MainCamera{ MainScene.GetMainCamera() };
    MainCamera.SetClearColor(0.1F, 0.2F, 0.3F, 1.0F);

    Renderer MainRenderer{};

    if (!MainRenderer.Initialize()) {
        return -1;
    }

    MainRenderer.Run(MainScene);
    MainRenderer.Shutdown();

    return 0;
}
