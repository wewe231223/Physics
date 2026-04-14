#pragma once

#include <SimpleMath/SimpleMath.h>
#include <DirectXCollision.h>

#include <string>

#include "PhysicsLib/Actors/PhysicsActor.h"

class PhysicsKinematicActor final : public PhysicsActor {
public:
    struct ActorDesc {
        std::string Name;
        bool IsActive;
        float Mass;
        PhysicsActorFlags Flags;
        DirectX::BoundingOrientedBox LocalBoundingBox;
        DirectX::SimpleMath::Vector3 Position;
        DirectX::SimpleMath::Vector3 Rotation;
        DirectX::SimpleMath::Vector3 Scale;
        DirectX::SimpleMath::Vector3 Velocity;
        float Friction;
        float Restitution;
        float LinearDamping;
        float AngularDamping;
        bool IsSleeping;
        float SleepThreshold;
        float BoundingBoxFatMargin;
    };

public:
    PhysicsKinematicActor();
    ~PhysicsKinematicActor() override;
    PhysicsKinematicActor(const PhysicsKinematicActor& Other);
    PhysicsKinematicActor& operator=(const PhysicsKinematicActor& Other);
    PhysicsKinematicActor(PhysicsKinematicActor&& Other) noexcept;
    PhysicsKinematicActor& operator=(PhysicsKinematicActor&& Other) noexcept;

    explicit PhysicsKinematicActor(std::string Name);
    explicit PhysicsKinematicActor(const ActorDesc& Desc);

public:
    void SetLocalBoundingBox(const DirectX::BoundingOrientedBox& LocalBoundingBox);
    const DirectX::BoundingOrientedBox& GetLocalBoundingBox() const;
    const DirectX::BoundingOrientedBox& GetWorldBoundingBox() const;
    const DirectX::BoundingOrientedBox& GetFatWorldBoundingBox() const;

    void SetPosition(const DirectX::SimpleMath::Vector3& Position);
    const DirectX::SimpleMath::Vector3& GetPosition() const;

    void SetRotation(const DirectX::SimpleMath::Vector3& Rotation);
    const DirectX::SimpleMath::Vector3& GetRotation() const;

    void SetScale(const DirectX::SimpleMath::Vector3& Scale);
    const DirectX::SimpleMath::Vector3& GetScale() const;

    void SetVelocity(const DirectX::SimpleMath::Vector3& Velocity);
    const DirectX::SimpleMath::Vector3& GetVelocity() const;

    void SetRestitution(float Restitution);
    float GetRestitution() const;

    void SetLinearDamping(float LinearDamping);
    float GetLinearDamping() const;

    void SetAngularDamping(float AngularDamping);
    float GetAngularDamping() const;

    void SetSleepThreshold(float SleepThreshold);
    float GetSleepThreshold() const;

    void SetBoundingBoxFatMargin(float BoundingBoxFatMargin);
    float GetBoundingBoxFatMargin() const;

    void MoveToTarget(const DirectX::SimpleMath::Vector3& TargetPosition, float DeltaTime);
    void UpdateWorldBoundingBox();

private:
    void UpdateFatWorldBoundingBox();

private:
    DirectX::BoundingOrientedBox mLocalBoundingBox;
    DirectX::BoundingOrientedBox mWorldBoundingBox;
    DirectX::BoundingOrientedBox mFatWorldBoundingBox;
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector3 mRotation;
    DirectX::SimpleMath::Vector3 mScale;
    DirectX::SimpleMath::Vector3 mVelocity;
    float mRestitution;
    float mLinearDamping;
    float mAngularDamping;
    bool mIsSleeping;
    float mSleepThreshold;
    float mBoundingBoxFatMargin;
};

