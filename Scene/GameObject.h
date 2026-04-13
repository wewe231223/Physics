#pragma once

#include "Mesh.h"
#include "Transform.h"
#include "PhysicsLib/Core/PhysicsActor.h"
#include "PhysicsLib/Core/PhysicsDynamicActor.h"
#include "PhysicsLib/Core/PhysicsKinematicActor.h"
#include "PhysicsLib/Core/PhysicsTerrainActor.h"

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
    void SetBoundingBoxMesh(const std::shared_ptr<Mesh>& MeshData);
    const std::shared_ptr<Mesh>& GetBoundingBoxMesh() const;
    void SetBoundingBoxVisible(bool IsVisible);
    bool GetBoundingBoxVisible() const;

    void UpdateWorldMatrix();
    const glm::mat4& GetWorldMatrix() const;
    const glm::mat4& GetBoundingBoxWorldMatrix() const;

    bool IsTerrainObject() const;
    PhysicsDynamicActor::ActorDesc GetPhysicsDynamicActorDesc() const;
    PhysicsTerrainActor::ActorDesc GetPhysicsTerrainActorDesc() const;
    void SetPhysicsActor(PhysicsActor* PhysicsActorPointer);
    PhysicsActor* GetPhysicsActor();
    const PhysicsActor* GetPhysicsActor() const;

    void PullTransformFromPhysicsActor();
    void UpdateBoundingBoxWorldMatrix();

private:
    std::string mName;
    bool mIsActive;
    Transform mTransform;
    std::shared_ptr<Mesh> mMesh;
    glm::mat4 mWorldMatrix;
    std::shared_ptr<Mesh> mBoundingBoxMesh;
    bool mBoundingBoxVisible;
    glm::mat4 mBoundingBoxWorldMatrix;
    PhysicsActor* mPhysicsActor;
};
