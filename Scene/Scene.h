#pragma once

#include "Camera.h"
#include "GameObject.h"

#include <cstddef>
#include <vector>

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

    GameObject* GetGameObject(std::size_t Index);
    const GameObject* GetGameObject(std::size_t Index) const;

    std::size_t GetGameObjectCount() const;

private:
    Camera mMainCamera;
    std::vector<GameObject> mGameObjects;
};
