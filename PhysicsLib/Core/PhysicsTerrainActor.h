#pragma once

#include <SimpleMath/SimpleMath.h>

#include <string>

#include "PhysicsActor.h"

class PhysicsTerrainActor final : public PhysicsActor {
public:
    struct ActorDesc {
        std::string Name;
        bool IsActive;
        PhysicsActorFlags Flags;
        DirectX::SimpleMath::Vector3 Position;
        DirectX::SimpleMath::Vector3 Rotation;
        DirectX::SimpleMath::Vector3 Scale;
        float HalfExtentX;
        float HalfExtentZ;
    };

public:
    PhysicsTerrainActor();
    ~PhysicsTerrainActor() override;
    PhysicsTerrainActor(const PhysicsTerrainActor& Other);
    PhysicsTerrainActor& operator=(const PhysicsTerrainActor& Other);
    PhysicsTerrainActor(PhysicsTerrainActor&& Other) noexcept;
    PhysicsTerrainActor& operator=(PhysicsTerrainActor&& Other) noexcept;

    explicit PhysicsTerrainActor(std::string Name);
    explicit PhysicsTerrainActor(const ActorDesc& Desc);

public:
    void SetPosition(const DirectX::SimpleMath::Vector3& Position);
    const DirectX::SimpleMath::Vector3& GetPosition() const;

    void SetRotation(const DirectX::SimpleMath::Vector3& Rotation);
    const DirectX::SimpleMath::Vector3& GetRotation() const;

    void SetScale(const DirectX::SimpleMath::Vector3& Scale);
    const DirectX::SimpleMath::Vector3& GetScale() const;

    void SetHalfExtentX(float HalfExtentX);
    float GetHalfExtentX() const;

    void SetHalfExtentZ(float HalfExtentZ);
    float GetHalfExtentZ() const;

private:
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector3 mRotation;
    DirectX::SimpleMath::Vector3 mScale;
    float mHalfExtentX;
    float mHalfExtentZ;
};
