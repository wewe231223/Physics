#include "Scene.h"

#include <utility>

Scene::Scene()
    : mMainCamera{},
      mGameObjects{} {
}

Scene::~Scene() {
}

Scene::Scene(const Scene& Other)
    : mMainCamera{ Other.mMainCamera },
      mGameObjects{ Other.mGameObjects } {
}

Scene& Scene::operator=(const Scene& Other) {
    if (this == &Other) {
        return *this;
    }

    mMainCamera = Other.mMainCamera;
    mGameObjects = Other.mGameObjects;

    return *this;
}

Scene::Scene(Scene&& Other) noexcept
    : mMainCamera{ std::move(Other.mMainCamera) },
      mGameObjects{ std::move(Other.mGameObjects) } {
}

Scene& Scene::operator=(Scene&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mMainCamera = std::move(Other.mMainCamera);
    mGameObjects = std::move(Other.mGameObjects);

    return *this;
}

Camera& Scene::GetMainCamera() {
    return mMainCamera;
}

const Camera& Scene::GetMainCamera() const {
    return mMainCamera;
}

std::size_t Scene::AddGameObject(const GameObject& Object) {
    mGameObjects.push_back(Object);
    std::size_t CreatedIndex{ mGameObjects.size() - 1U };
    return CreatedIndex;
}

std::size_t Scene::AddGameObject(GameObject&& Object) {
    mGameObjects.push_back(std::move(Object));
    std::size_t CreatedIndex{ mGameObjects.size() - 1U };
    return CreatedIndex;
}

GameObject* Scene::GetGameObject(std::size_t Index) {
    if (Index >= mGameObjects.size()) {
        return nullptr;
    }

    return &mGameObjects[Index];
}

const GameObject* Scene::GetGameObject(std::size_t Index) const {
    if (Index >= mGameObjects.size()) {
        return nullptr;
    }

    return &mGameObjects[Index];
}

std::size_t Scene::GetGameObjectCount() const {
    std::size_t Count{ mGameObjects.size() };
    return Count;
}
