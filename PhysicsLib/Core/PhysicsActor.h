#pragma once

#include <SimpleMath/SimpleMath.h>
#include <DirectXCollision.h>

#include <cstdint>
#include <string>

#ifdef _DEBUG
#pragma comment(lib, "debug/DirectXTK12.lib")
#else 
#pragma comment(lib, "release/DirectXTK12.lib")
#endif

class PhysicsActor final {
public:
    enum class PhysicsActorFlags : std::uint32_t {
        None = 0U,
        Static = 1U << 0U,
        Kinematic = 1U << 1U,
        Trigger = 1U << 2U
    };

    struct ActorDesc {
        std::string Name;
        bool IsActive;
        float Mass;
        PhysicsActorFlags Flags;
        DirectX::BoundingOrientedBox BoundingBoxValue;
        DirectX::SimpleMath::Vector3 Position;
        DirectX::SimpleMath::Vector3 Rotation;
        DirectX::SimpleMath::Vector3 Scale;
    };

public:
    PhysicsActor();
    ~PhysicsActor();
    PhysicsActor(const PhysicsActor& Other);
    PhysicsActor& operator=(const PhysicsActor& Other);
    PhysicsActor(PhysicsActor&& Other) noexcept;
    PhysicsActor& operator=(PhysicsActor&& Other) noexcept;

    explicit PhysicsActor(std::string Name);
    explicit PhysicsActor(const ActorDesc& Desc);

public:
    void SetName(std::string Name);
    const std::string& GetName() const;

    void SetIsActive(bool IsActive);
    bool GetIsActive() const;

    void SetMass(float Mass);
    float GetMass() const;

    void SetFlags(PhysicsActorFlags Flags);
    PhysicsActorFlags GetFlags() const;
    bool HasFlag(PhysicsActorFlags Flag) const;

    void SetBoundingBox(const DirectX::BoundingOrientedBox& Box);
    const DirectX::BoundingOrientedBox& GetBoundingBox() const;

    void SetPosition(const DirectX::SimpleMath::Vector3& Position);
    const DirectX::SimpleMath::Vector3& GetPosition() const;

    void SetRotation(const DirectX::SimpleMath::Vector3& Rotation);
    const DirectX::SimpleMath::Vector3& GetRotation() const;

    void SetScale(const DirectX::SimpleMath::Vector3& Scale);
    const DirectX::SimpleMath::Vector3& GetScale() const;

private:
    std::string mName;
    bool mIsActive;
    float mMass;
    PhysicsActorFlags mFlags;
    DirectX::BoundingOrientedBox mBoundingBox;
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector3 mRotation;
    DirectX::SimpleMath::Vector3 mScale;
};

PhysicsActor::PhysicsActorFlags operator|(PhysicsActor::PhysicsActorFlags Left, PhysicsActor::PhysicsActorFlags Right);
PhysicsActor::PhysicsActorFlags operator&(PhysicsActor::PhysicsActorFlags Left, PhysicsActor::PhysicsActorFlags Right);
