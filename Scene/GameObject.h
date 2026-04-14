#pragma once

#include "Mesh.h"
#include "PhysicsExchangeTypes.h"
#include "Transform.h"
#include "PhysicsLib/Actors/PhysicsDynamicActor.h"
#include "PhysicsLib/Actors/PhysicsTerrainActor.h"

#include <SimpleMath/SimpleMath.h>

#include <memory>
#include <string>
#include <vector>

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

    void SetActorId(ActorId ActorIdValue);
    ActorId GetActorId() const;
    bool HasActorId() const;

    void SetPhysicsMass(float PhysicsMass);
    float GetPhysicsMass() const;
    void SetInitialImpulse(const DirectX::SimpleMath::Vector3& InitialImpulse);
    void ClearInitialImpulse();
    bool HasInitialImpulse() const;
    const DirectX::SimpleMath::Vector3& GetInitialImpulse() const;

    PhysicsDynamicActor::ActorDesc GetPhysicsDynamicActorDesc() const;
    PhysicsTerrainActor::ActorDesc GetPhysicsTerrainActorDesc() const;

    void ApplyPhysicsState(const DirectX::SimpleMath::Vector3& Position, const DirectX::SimpleMath::Vector3& Rotation, const DirectX::SimpleMath::Vector3& Scale);
    void SetBoundingBoxFromPhysicsState(const DirectX::BoundingOrientedBox& WorldBoundingBox, const DirectX::SimpleMath::Vector3& Rotation);
    void ClearBoundingBoxWorldMatrix();

private:
    std::string mName;
    bool mIsActive;
    Transform mTransform;
    std::shared_ptr<Mesh> mMesh;
    glm::mat4 mWorldMatrix;
    std::shared_ptr<Mesh> mBoundingBoxMesh;
    bool mBoundingBoxVisible;
    glm::mat4 mBoundingBoxWorldMatrix;
    ActorId mActorId;
    float mPhysicsMass;
    bool mHasInitialImpulse;
    DirectX::SimpleMath::Vector3 mInitialImpulse;
};


