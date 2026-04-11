#pragma once

#include "Mesh.h"
#include "Transform.h"

#include <memory>
#include <string>

#include <glm/mat4x4.hpp>

class GameObject final {
public:
    GameObject();
    ~GameObject();
    GameObject(const GameObject& Other);
    GameObject& operator=(const GameObject& Other);
    GameObject(GameObject&& Other) noexcept;
    GameObject& operator=(GameObject&& Other) noexcept;

    explicit GameObject(std::string Name);

public:
    void SetName(std::string Name);
    const std::string& GetName() const;

    void SetIsActive(bool IsActive);
    bool GetIsActive() const;

    Transform& GetTransform();
    const Transform& GetTransform() const;

    void SetMesh(const std::shared_ptr<Mesh>& MeshData);
    const std::shared_ptr<Mesh>& GetMesh() const;

    void UpdateWorldMatrix();
    const glm::mat4& GetWorldMatrix() const;

private:
    std::string mName;
    bool mIsActive;
    Transform mTransform;
    std::shared_ptr<Mesh> mMesh;
    glm::mat4 mWorldMatrix;
};
