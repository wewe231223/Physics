#include "PhysicsWorld.h"

#include <algorithm>
#include <cmath>
#include <utility>

#undef max 

PhysicsWorld::PhysicsWorld()
    : mSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{ 0.0F, -9.8F, 0.0F } },
      mAccumulator{},
      mActors{} {
}

PhysicsWorld::~PhysicsWorld() {
}

PhysicsWorld::PhysicsWorld(const PhysicsWorld& Other)
    : mSettings{ Other.mSettings },
      mAccumulator{ Other.mAccumulator },
      mActors{} {
    std::size_t ActorCount{ Other.mActors.size() };
    for (std::size_t ActorIndex{ 0U }; ActorIndex < ActorCount; ++ActorIndex) {
        const PhysicsActor* SourceActor{ Other.mActors[ActorIndex].get() };
        if (SourceActor == nullptr) {
            continue;
        }

        if (SourceActor->GetActorType() == PhysicsActor::PhysicsActorType::Static) {
            const PhysicsTerrainActor* TerrainActor{ dynamic_cast<const PhysicsTerrainActor*>(SourceActor) };
            if (TerrainActor != nullptr) {
                mActors.push_back(std::make_unique<PhysicsTerrainActor>(*TerrainActor));
            }
            continue;
        }

        if (SourceActor->GetActorType() == PhysicsActor::PhysicsActorType::Kinematic) {
            const PhysicsKinematicActor* KinematicActor{ static_cast<const PhysicsKinematicActor*>(SourceActor) };
            mActors.push_back(std::make_unique<PhysicsKinematicActor>(*KinematicActor));
            continue;
        }

        const PhysicsDynamicActor* DynamicActor{ static_cast<const PhysicsDynamicActor*>(SourceActor) };
        mActors.push_back(std::make_unique<PhysicsDynamicActor>(*DynamicActor));
    }
}

PhysicsWorld& PhysicsWorld::operator=(const PhysicsWorld& Other) {
    if (this == &Other) {
        return *this;
    }

    mSettings = Other.mSettings;
    mAccumulator = Other.mAccumulator;
    mActors.clear();

    std::size_t ActorCount{ Other.mActors.size() };
    for (std::size_t ActorIndex{ 0U }; ActorIndex < ActorCount; ++ActorIndex) {
        const PhysicsActor* SourceActor{ Other.mActors[ActorIndex].get() };
        if (SourceActor == nullptr) {
            continue;
        }

        if (SourceActor->GetActorType() == PhysicsActor::PhysicsActorType::Static) {
            const PhysicsTerrainActor* TerrainActor{ dynamic_cast<const PhysicsTerrainActor*>(SourceActor) };
            if (TerrainActor != nullptr) {
                mActors.push_back(std::make_unique<PhysicsTerrainActor>(*TerrainActor));
            }
            continue;
        }

        if (SourceActor->GetActorType() == PhysicsActor::PhysicsActorType::Kinematic) {
            const PhysicsKinematicActor* KinematicActor{ static_cast<const PhysicsKinematicActor*>(SourceActor) };
            mActors.push_back(std::make_unique<PhysicsKinematicActor>(*KinematicActor));
            continue;
        }

        const PhysicsDynamicActor* DynamicActor{ static_cast<const PhysicsDynamicActor*>(SourceActor) };
        mActors.push_back(std::make_unique<PhysicsDynamicActor>(*DynamicActor));
    }

    return *this;
}

PhysicsWorld::PhysicsWorld(PhysicsWorld&& Other) noexcept
    : mSettings{ Other.mSettings },
      mAccumulator{ Other.mAccumulator },
      mActors{ std::move(Other.mActors) } {
    Other.mSettings = WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{} };
    Other.mAccumulator = 0.0F;
}

PhysicsWorld& PhysicsWorld::operator=(PhysicsWorld&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mSettings = Other.mSettings;
    mAccumulator = Other.mAccumulator;
    mActors = std::move(Other.mActors);

    Other.mSettings = WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{} };
    Other.mAccumulator = 0.0F;

    return *this;
}

PhysicsWorld::PhysicsWorld(const WorldSettings& Settings)
    : mSettings{ Settings },
      mAccumulator{},
      mActors{} {
}

void PhysicsWorld::Initialize(const WorldSettings& Settings) {
    mSettings = Settings;
    mAccumulator = 0.0F;
}

PhysicsDynamicActor* PhysicsWorld::CreateDynamicActor(const PhysicsDynamicActor::ActorDesc& Desc) {
    std::unique_ptr<PhysicsActor> NewActor{ std::make_unique<PhysicsDynamicActor>(Desc) };
    PhysicsDynamicActor* CreatedActor{ static_cast<PhysicsDynamicActor*>(NewActor.get()) };
    mActors.push_back(std::move(NewActor));
    return CreatedActor;
}

PhysicsKinematicActor* PhysicsWorld::CreateKinematicActor(const PhysicsKinematicActor::ActorDesc& Desc) {
    std::unique_ptr<PhysicsActor> NewActor{ std::make_unique<PhysicsKinematicActor>(Desc) };
    PhysicsKinematicActor* CreatedActor{ static_cast<PhysicsKinematicActor*>(NewActor.get()) };
    mActors.push_back(std::move(NewActor));
    return CreatedActor;
}

PhysicsTerrainActor* PhysicsWorld::CreateTerrainActor(const PhysicsTerrainActor::ActorDesc& Desc) {
    std::unique_ptr<PhysicsActor> NewActor{ std::make_unique<PhysicsTerrainActor>(Desc) };
    PhysicsTerrainActor* CreatedActor{ static_cast<PhysicsTerrainActor*>(NewActor.get()) };
    mActors.push_back(std::move(NewActor));
    return CreatedActor;
}

void PhysicsWorld::AddActor(std::unique_ptr<PhysicsActor> Actor) {
    mActors.push_back(std::move(Actor));
}

void PhysicsWorld::ClearActors() {
    mActors.clear();
}

PhysicsActor* PhysicsWorld::GetActor(std::size_t Index) {
    if (Index >= mActors.size()) {
        return nullptr;
    }

    return mActors[Index].get();
}

const PhysicsActor* PhysicsWorld::GetActor(std::size_t Index) const {
    if (Index >= mActors.size()) {
        return nullptr;
    }

    return mActors[Index].get();
}

std::size_t PhysicsWorld::GetActorCount() const {
    std::size_t ActorCount{ mActors.size() };
    return ActorCount;
}

const PhysicsWorld::WorldSettings& PhysicsWorld::GetSettings() const {
    return mSettings;
}

float PhysicsWorld::GetAccumulator() const {
    return mAccumulator;
}

void PhysicsWorld::StepSimulation() {
    std::size_t ActorCount{ mActors.size() };
    for (std::size_t ActorIndex{ 0U }; ActorIndex < ActorCount; ++ActorIndex) {
        PhysicsActor* CurrentActor{ mActors[ActorIndex].get() };
        if (CurrentActor == nullptr) {
            continue;
        }

        if (CurrentActor->GetActorType() != PhysicsActor::PhysicsActorType::Dynamic) {
            continue;
        }

        PhysicsDynamicActor* DynamicActor{ static_cast<PhysicsDynamicActor*>(CurrentActor) };
        IntegrateDynamicActor(*DynamicActor, mSettings.FixedTimeStep);
    }
}

void PhysicsWorld::Update(float DeltaTime) {
    mAccumulator += DeltaTime;

    while (mAccumulator >= mSettings.FixedTimeStep) {
        StepSimulation();
        mAccumulator -= mSettings.FixedTimeStep;
    }
}

void PhysicsWorld::IntegrateDynamicActor(PhysicsDynamicActor& Actor, float DeltaTime) const {
    if (!Actor.GetIsActive()) {
        return;
    }

    if (Actor.GetMass() <= 0.0F) {
        return;
    }

    if (Actor.GetIsSleeping()) {
        return;
    }

    DirectX::SimpleMath::Vector3 TotalAcceleration{ mSettings.Gravity + Actor.GetAcceleration() };
    DirectX::SimpleMath::Vector3 NextVelocity{ Actor.GetVelocity() + (TotalAcceleration * DeltaTime) };
    float DampingFactor{ std::max(0.0F, 1.0F - (Actor.GetLinearDamping() * DeltaTime)) };
    NextVelocity *= DampingFactor;

    DirectX::SimpleMath::Vector3 NextPosition{ Actor.GetPosition() + (NextVelocity * DeltaTime) };
    DirectX::BoundingOrientedBox PredictedWorldBoundingBox{};
    DirectX::SimpleMath::Matrix ScalingMatrix{ DirectX::SimpleMath::Matrix::CreateScale(Actor.GetScale()) };
    DirectX::SimpleMath::Matrix RotationMatrix{ DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(Actor.GetRotation().y, Actor.GetRotation().x, Actor.GetRotation().z) };
    DirectX::SimpleMath::Matrix TranslationMatrix{ DirectX::SimpleMath::Matrix::CreateTranslation(NextPosition) };
    DirectX::SimpleMath::Matrix WorldMatrix{ ScalingMatrix * RotationMatrix * TranslationMatrix };
    Actor.GetLocalBoundingBox().Transform(PredictedWorldBoundingBox, WorldMatrix);

    DirectX::SimpleMath::Vector3 CorrectedPosition{ NextPosition };
    DirectX::SimpleMath::Vector3 CorrectedVelocity{ NextVelocity };
    ResolveStaticCollisions(PredictedWorldBoundingBox, CorrectedPosition, CorrectedVelocity);

    Actor.SetVelocity(CorrectedVelocity);
    Actor.SetPosition(CorrectedPosition);
    Actor.UpdateSleepState();
}

bool PhysicsWorld::ResolveStaticCollisions(const DirectX::BoundingOrientedBox& PredictedWorldBoundingBox, DirectX::SimpleMath::Vector3& CorrectedPosition, DirectX::SimpleMath::Vector3& CorrectedVelocity) const {
    bool HasCollision{};
    std::size_t ActorCount{ mActors.size() };

    for (std::size_t ActorIndex{ 0U }; ActorIndex < ActorCount; ++ActorIndex) {
        const PhysicsActor* CurrentActor{ mActors[ActorIndex].get() };
        if (CurrentActor == nullptr) {
            continue;
        }

        if (CurrentActor->GetActorType() != PhysicsActor::PhysicsActorType::Static) {
            continue;
        }

        const PhysicsStaticActor* StaticActor{ static_cast<const PhysicsStaticActor*>(CurrentActor) };
        bool CurrentCollision{ StaticActor->ResolveDynamicCollision(PredictedWorldBoundingBox, CorrectedPosition, CorrectedVelocity) };
        HasCollision = HasCollision || CurrentCollision;
    }

    return HasCollision;
}
