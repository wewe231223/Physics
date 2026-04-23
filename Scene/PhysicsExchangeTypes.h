#pragma once

#include "PhysicsLib/Actors/PhysicsActor.h"

#include <DirectXCollision.h>
#include <SimpleMath/SimpleMath.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#undef max 
#undef min 

using ActorId = std::uint32_t;

constexpr ActorId InvalidActorId{ std::numeric_limits<ActorId>::max() };

enum class PhysicsCommandType : std::uint32_t {
    ResetScene = 0U,
    AddImpulse = 1U,
    SetKinematicVelocity = 2U
};

struct PhysicsResetSceneCommand final {
    std::size_t mSceneIndex{};
    std::uint32_t mWorldVersion{};
};

struct PhysicsAddImpulseCommand final {
    ActorId mActorId{ InvalidActorId };
    DirectX::SimpleMath::Vector3 mImpulse{};
};

struct PhysicsSetKinematicVelocityCommand final {
    ActorId mActorId{ InvalidActorId };
    DirectX::SimpleMath::Vector3 mVelocity{};
};

struct PhysicsCommand final {
    PhysicsCommandType mType{ PhysicsCommandType::ResetScene };
    PhysicsResetSceneCommand mResetScene{};
    PhysicsAddImpulseCommand mAddImpulse{};
    PhysicsSetKinematicVelocityCommand mSetKinematicVelocity{};
};

struct PhysicsActorSnapshot final {
    ActorId mActorId{ InvalidActorId };
    PhysicsActorBase::PhysicsActorType mActorType{ PhysicsActorBase::PhysicsActorType::Dynamic };
    bool mIsActive{};
    DirectX::SimpleMath::Vector3 mPosition{};
    DirectX::SimpleMath::Quaternion mOrientation{ 0.0F, 0.0F, 0.0F, 1.0F };
    DirectX::SimpleMath::Vector3 mScale{ 1.0F, 1.0F, 1.0F };
    DirectX::BoundingOrientedBox mWorldBoundingBox{};
};

struct PhysicsSnapshot final {
    std::uint32_t mWorldVersion{};
    std::size_t mSceneIndex{};
    std::size_t mActorCount{};
    std::size_t mLastUpdateStepCount{};
    double mLastUpdateStepElapsedMilliseconds{};
    double mLastStepElapsedMilliseconds{};
    std::vector<PhysicsActorSnapshot> mActors{};
};


