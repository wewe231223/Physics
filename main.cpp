#include "Renderer/Renderer.h"

#include <iostream>

#ifdef DEBUG
#pragma comment(lib, "debug/glfw3.lib")
#else
#pragma comment(lib, "release/glfw3.lib")
#endif

int main() {
    std::cout << "가나다" << std::endl;

    Scene MainScene{};

    Camera& MainCamera{ MainScene.GetMainCamera() };
    MainCamera.SetClearColor(0.1F, 0.2F, 0.3F, 1.0F);
    MainCamera.GetTransform().SetPosition(glm::vec3{ 0.0F, 3.0F, 8.0F });
    MainCamera.GetTransform().SetRotation(glm::vec3{ -0.35F, 0.0F, 0.0F });

    std::size_t GridIndex{ MainScene.CreatePrimitiveGameObject("Grid", PrimitiveMeshType::Grid) };
    std::size_t CubeIndex{ MainScene.CreatePrimitiveGameObject("Cube", PrimitiveMeshType::Cube) };
    std::size_t SphereIndex{ MainScene.CreatePrimitiveGameObject("Sphere", PrimitiveMeshType::Sphere) };
    std::size_t TriangularPyramidIndex{ MainScene.CreatePrimitiveGameObject("TriangularPyramid", PrimitiveMeshType::TriangularPyramid) };
    std::size_t SquarePyramidIndex{ MainScene.CreatePrimitiveGameObject("SquarePyramid", PrimitiveMeshType::SquarePyramid) };

    GameObject* GridObject{ MainScene.GetGameObject(GridIndex) };
    if (GridObject != nullptr) {
        GridObject->GetTransform().SetPosition(glm::vec3{ 0.0F, 0.0F, 0.0F });
    }

    GameObject* CubeObject{ MainScene.GetGameObject(CubeIndex) };
    if (CubeObject != nullptr) {
        CubeObject->GetTransform().SetPosition(glm::vec3{ -2.0F, 0.5F, 0.0F });
    }

    GameObject* SphereObject{ MainScene.GetGameObject(SphereIndex) };
    if (SphereObject != nullptr) {
        SphereObject->GetTransform().SetPosition(glm::vec3{ 2.0F, 0.5F, 0.0F });
    }

    GameObject* TriangularPyramidObject{ MainScene.GetGameObject(TriangularPyramidIndex) };
    if (TriangularPyramidObject != nullptr) {
        TriangularPyramidObject->GetTransform().SetPosition(glm::vec3{ 0.0F, 0.5F, -2.0F });
    }

    GameObject* SquarePyramidObject{ MainScene.GetGameObject(SquarePyramidIndex) };
    if (SquarePyramidObject != nullptr) {
        SquarePyramidObject->GetTransform().SetPosition(glm::vec3{ 0.0F, 0.5F, 2.0F });
    }

    Renderer MainRenderer{};

    if (!MainRenderer.Initialize()) {
        return -1;
    }

    while (!MainRenderer.ShouldClose()) {
        MainRenderer.ProcessInput(MainScene);
        MainScene.Update();
        MainRenderer.RenderFrame(MainScene);
        MainRenderer.Present();
        MainRenderer.PollEvents();
    }

    MainRenderer.Shutdown();

    return 0;
}
