#include "GameObject.h"

#include <utility>

GameObject::GameObject()
    : mName{ "GameObject" },
      mIsActive{ true },
      mTransform{},
      mMesh{},
      mWorldMatrix{ 1.0F } {
}

GameObject::~GameObject() {
}

GameObject::GameObject(const GameObject& Other)
    : mName{ Other.mName },
      mIsActive{ Other.mIsActive },
      mTransform{ Other.mTransform },
      mMesh{ Other.mMesh },
      mWorldMatrix{ Other.mWorldMatrix } {
}

GameObject& GameObject::operator=(const GameObject& Other) {
    if (this == &Other) {
        return *this;
    }

    mName = Other.mName;
    mIsActive = Other.mIsActive;
    mTransform = Other.mTransform;
    mMesh = Other.mMesh;
    mWorldMatrix = Other.mWorldMatrix;

    return *this;
}

GameObject::GameObject(GameObject&& Other) noexcept
    : mName{ std::move(Other.mName) },
      mIsActive{ Other.mIsActive },
      mTransform{ std::move(Other.mTransform) },
      mMesh{ std::move(Other.mMesh) },
      mWorldMatrix{ Other.mWorldMatrix } {
    Other.mName = "";
    Other.mIsActive = false;
    Other.mWorldMatrix = glm::mat4{ 1.0F };
}

GameObject& GameObject::operator=(GameObject&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mName = std::move(Other.mName);
    mIsActive = Other.mIsActive;
    mTransform = std::move(Other.mTransform);
    mMesh = std::move(Other.mMesh);
    mWorldMatrix = Other.mWorldMatrix;

    Other.mName = "";
    Other.mIsActive = false;
    Other.mWorldMatrix = glm::mat4{ 1.0F };

    return *this;
}

GameObject::GameObject(std::string Name)
    : mName{ std::move(Name) },
      mIsActive{ true },
      mTransform{},
      mMesh{},
      mWorldMatrix{ 1.0F } {
}

void GameObject::SetName(std::string Name) {
    mName = std::move(Name);
}

const std::string& GameObject::GetName() const {
    return mName;
}

void GameObject::SetIsActive(bool IsActive) {
    mIsActive = IsActive;
}

bool GameObject::GetIsActive() const {
    return mIsActive;
}

Transform& GameObject::GetTransform() {
    return mTransform;
}

const Transform& GameObject::GetTransform() const {
    return mTransform;
}

void GameObject::SetMesh(const std::shared_ptr<Mesh>& MeshData) {
    mMesh = MeshData;
}

const std::shared_ptr<Mesh>& GameObject::GetMesh() const {
    return mMesh;
}

void GameObject::UpdateWorldMatrix() {
    mWorldMatrix = mTransform.GetWorldMatrix();
}

const glm::mat4& GameObject::GetWorldMatrix() const {
    return mWorldMatrix;
}
