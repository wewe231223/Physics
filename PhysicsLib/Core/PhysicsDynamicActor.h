#pragma once

#include <SimpleMath/SimpleMath.h>
#include <DirectXCollision.h>

#include <string>

#include "PhysicsActor.h"

class PhysicsDynamicActor final : public PhysicsActor {
public:
    struct ActorDesc {
        std::string Name;
        bool IsActive;
        float Mass;
        PhysicsActorFlags Flags;
        DirectX::BoundingOrientedBox BoundingBoxValue;
        DirectX::SimpleMath::Vector3 Position;
        DirectX::SimpleMath::Vector3 Rotation;
        DirectX::SimpleMath::Vector3 Scale;
        DirectX::SimpleMath::Vector3 Velocity;
    };

public:
    PhysicsDynamicActor();
    ~PhysicsDynamicActor() override;
    PhysicsDynamicActor(const PhysicsDynamicActor& Other);
    PhysicsDynamicActor& operator=(const PhysicsDynamicActor& Other);
    PhysicsDynamicActor(PhysicsDynamicActor&& Other) noexcept;
    PhysicsDynamicActor& operator=(PhysicsDynamicActor&& Other) noexcept;

    explicit PhysicsDynamicActor(std::string Name);
    explicit PhysicsDynamicActor(const ActorDesc& Desc);

public:
    void SetBoundingBox(const DirectX::BoundingOrientedBox& BoundingBoxValue);
    const DirectX::BoundingOrientedBox& GetBoundingBox() const;

    void SetPosition(const DirectX::SimpleMath::Vector3& Position);
    const DirectX::SimpleMath::Vector3& GetPosition() const;

    void SetRotation(const DirectX::SimpleMath::Vector3& Rotation);
    const DirectX::SimpleMath::Vector3& GetRotation() const;

    void SetScale(const DirectX::SimpleMath::Vector3& Scale);
    const DirectX::SimpleMath::Vector3& GetScale() const;

    void SetVelocity(const DirectX::SimpleMath::Vector3& Velocity);
    const DirectX::SimpleMath::Vector3& GetVelocity() const;

private:
    DirectX::BoundingOrientedBox mBoundingBox;
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector3 mRotation;
    DirectX::SimpleMath::Vector3 mScale;
    DirectX::SimpleMath::Vector3 mVelocity;
};
