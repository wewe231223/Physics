#pragma once

#include "Camera.h"
#include "GameObject.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

enum class PrimitiveMeshType {
    Cube,
    Sphere,
    TriangularPyramid,
    SquarePyramid,
    Grid
};

class Scene final {
public:
    Scene();
    ~Scene();
    Scene(const Scene& Other);
    Scene& operator=(const Scene& Other);
    Scene(Scene&& Other) noexcept;
    Scene& operator=(Scene&& Other) noexcept;

public:
    Camera& GetMainCamera();
    const Camera& GetMainCamera() const;

    std::size_t AddGameObject(const GameObject& Object);
    std::size_t AddGameObject(GameObject&& Object);
    std::size_t CreatePrimitiveGameObject(std::string Name, PrimitiveMeshType PrimitiveType);

    GameObject* GetGameObject(std::size_t Index);
    const GameObject* GetGameObject(std::size_t Index) const;
    std::size_t GetGameObjectCount() const;

    void Update();

private:
    std::shared_ptr<Mesh> GetPrimitiveMesh(PrimitiveMeshType PrimitiveType);

private:
    Camera mMainCamera;
    std::vector<GameObject> mGameObjects;
    std::shared_ptr<Mesh> mCubeMesh;
    std::shared_ptr<Mesh> mSphereMesh;
    std::shared_ptr<Mesh> mTriangularPyramidMesh;
    std::shared_ptr<Mesh> mSquarePyramidMesh;
    std::shared_ptr<Mesh> mGridMesh;
};
