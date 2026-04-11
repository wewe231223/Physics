#include "PhysicsWorld.h"

#include <utility>

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

        if (SourceActor->GetActorType() == PhysicsActor::PhysicsActorType::Terrain) {
            const PhysicsTerrainActor* TerrainActor{ static_cast<const PhysicsTerrainActor*>(SourceActor) };
            mActors.push_back(std::make_unique<PhysicsTerrainActor>(*TerrainActor));
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

        if (SourceActor->GetActorType() == PhysicsActor::PhysicsActorType::Terrain) {
            const PhysicsTerrainActor* TerrainActor{ static_cast<const PhysicsTerrainActor*>(SourceActor) };
            mActors.push_back(std::make_unique<PhysicsTerrainActor>(*TerrainActor));
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
        IntegrateActor(*DynamicActor, mSettings.FixedTimeStep);
    }
}

void PhysicsWorld::Update(float DeltaTime) {
    mAccumulator += DeltaTime;

    while (mAccumulator >= mSettings.FixedTimeStep) {
        StepSimulation();
        mAccumulator -= mSettings.FixedTimeStep;
    }
}

void PhysicsWorld::IntegrateActor(PhysicsDynamicActor& Actor, float DeltaTime) const {
    if (!Actor.GetIsActive()) {
        return;
    }

    if (Actor.HasFlag(PhysicsActor::PhysicsActorFlags::Static)) {
        return;
    }

    if (Actor.GetMass() <= 0.0F) {
        return;
    }

    DirectX::SimpleMath::Vector3 NextVelocity{ Actor.GetVelocity() + (mSettings.Gravity * DeltaTime) };
    Actor.SetVelocity(NextVelocity);

    DirectX::SimpleMath::Vector3 NextPosition{ Actor.GetPosition() + (NextVelocity * DeltaTime) };

    DirectX::SimpleMath::Vector3 HitPoint{};
    bool HasHit{ FindTerrainHitPoint(Actor, HitPoint) };
    if (HasHit && NextPosition.y <= HitPoint.y) {
        NextPosition.y = HitPoint.y;
        NextVelocity.y = 0.0F;
        Actor.SetVelocity(NextVelocity);
    }

    Actor.SetPosition(NextPosition);
}

bool PhysicsWorld::FindTerrainHitPoint(const PhysicsDynamicActor& Actor, DirectX::SimpleMath::Vector3& HitPoint) const {
    DirectX::SimpleMath::Vector3 ActorPosition{ Actor.GetPosition() };

    std::size_t ActorCount{ mActors.size() };
    for (std::size_t ActorIndex{ 0U }; ActorIndex < ActorCount; ++ActorIndex) {
        const PhysicsActor* CurrentActor{ mActors[ActorIndex].get() };
        if (CurrentActor == nullptr) {
            continue;
        }

        if (CurrentActor->GetActorType() != PhysicsActor::PhysicsActorType::Terrain) {
            continue;
        }

        const PhysicsTerrainActor* TerrainActor{ static_cast<const PhysicsTerrainActor*>(CurrentActor) };
        DirectX::SimpleMath::Vector3 TerrainScale{ TerrainActor->GetScale() };
        DirectX::SimpleMath::Vector3 TerrainPosition{ TerrainActor->GetPosition() };

        float ScaledHalfExtentX{ TerrainActor->GetHalfExtentX() * TerrainScale.x };
        float ScaledHalfExtentZ{ TerrainActor->GetHalfExtentZ() * TerrainScale.z };

        float MinX{ TerrainPosition.x - ScaledHalfExtentX };
        float MaxX{ TerrainPosition.x + ScaledHalfExtentX };
        float MinZ{ TerrainPosition.z - ScaledHalfExtentZ };
        float MaxZ{ TerrainPosition.z + ScaledHalfExtentZ };

        bool IsInXRange{ ActorPosition.x >= MinX && ActorPosition.x <= MaxX };
        bool IsInZRange{ ActorPosition.z >= MinZ && ActorPosition.z <= MaxZ };

        if (!IsInXRange || !IsInZRange) {
            continue;
        }

        float TerrainTopY{ TerrainPosition.y };
        HitPoint = DirectX::SimpleMath::Vector3{ ActorPosition.x, TerrainTopY, ActorPosition.z };
        return true;
    }

    return false;
}
