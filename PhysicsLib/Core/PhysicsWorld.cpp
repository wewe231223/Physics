#include "PhysicsWorld.h"

#include <algorithm>
#include <cmath>
#include <utility>

#undef min 
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
    DirectX::SimpleMath::Vector3 NextPosition{ Actor.GetPosition() + (NextVelocity * DeltaTime) };
    DirectX::BoundingOrientedBox PredictedWorldBoundingBox{};
    DirectX::SimpleMath::Matrix ScalingMatrix{ DirectX::SimpleMath::Matrix::CreateScale(Actor.GetScale()) };
    DirectX::SimpleMath::Matrix RotationMatrix{ DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(Actor.GetRotation().y, Actor.GetRotation().x, Actor.GetRotation().z) };
    DirectX::SimpleMath::Matrix TranslationMatrix{ DirectX::SimpleMath::Matrix::CreateTranslation(NextPosition) };
    DirectX::SimpleMath::Matrix WorldMatrix{ ScalingMatrix * RotationMatrix * TranslationMatrix };
    Actor.GetLocalBoundingBox().Transform(PredictedWorldBoundingBox, WorldMatrix);

    DirectX::SimpleMath::Vector3 CorrectedPosition{ NextPosition };
    DirectX::SimpleMath::Vector3 CorrectedVelocity{ NextVelocity };
    ResolveTerrainCollision(PredictedWorldBoundingBox, CorrectedPosition, CorrectedVelocity);

    Actor.SetVelocity(CorrectedVelocity);
    Actor.SetPosition(CorrectedPosition);
}

bool PhysicsWorld::ResolveTerrainCollision(const DirectX::BoundingOrientedBox& PredictedWorldBoundingBox, DirectX::SimpleMath::Vector3& CorrectedPosition, DirectX::SimpleMath::Vector3& CorrectedVelocity) const {
    DirectX::XMFLOAT3 DynamicCorners[8]{};
    PredictedWorldBoundingBox.GetCorners(DynamicCorners);
    float DynamicMinimumX{ DynamicCorners[0].x };
    float DynamicMaximumX{ DynamicCorners[0].x };
    float DynamicMinimumY{ DynamicCorners[0].y };
    float DynamicMinimumZ{ DynamicCorners[0].z };
    float DynamicMaximumZ{ DynamicCorners[0].z };
    float DynamicCenterX{ DynamicCorners[0].x };
    float DynamicCenterZ{ DynamicCorners[0].z };
    for (std::size_t CornerIndex{ 1U }; CornerIndex < 8U; ++CornerIndex) {
        DynamicMinimumX = std::min(DynamicMinimumX, DynamicCorners[CornerIndex].x);
        DynamicMaximumX = std::max(DynamicMaximumX, DynamicCorners[CornerIndex].x);
        DynamicMinimumY = std::min(DynamicMinimumY, DynamicCorners[CornerIndex].y);
        DynamicMinimumZ = std::min(DynamicMinimumZ, DynamicCorners[CornerIndex].z);
        DynamicMaximumZ = std::max(DynamicMaximumZ, DynamicCorners[CornerIndex].z);
        DynamicCenterX += DynamicCorners[CornerIndex].x;
        DynamicCenterZ += DynamicCorners[CornerIndex].z;
    }
    DynamicCenterX /= 8.0F;
    DynamicCenterZ /= 8.0F;

    bool HasCollision{};
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
        PhysicsTerrainActor::ActorDesc TerrainDesc{ TerrainActor->GetActorDesc() };
        DirectX::SimpleMath::Vector3 TerrainPosition{ TerrainDesc.Position };
        DirectX::SimpleMath::Vector3 TerrainScale{ TerrainDesc.Scale };
        float TerrainHalfExtentX{ TerrainDesc.HalfExtentX * std::abs(TerrainScale.x) };
        float TerrainHalfExtentZ{ TerrainDesc.HalfExtentZ * std::abs(TerrainScale.z) };
        std::uint32_t TerrainHeightFieldWidth{ TerrainDesc.HeightFieldWidth };
        std::uint32_t TerrainHeightFieldHeight{ TerrainDesc.HeightFieldHeight };
        float TerrainHeightFieldCellSpacing{ TerrainDesc.HeightFieldCellSpacing };
        if (TerrainHeightFieldWidth > 1U && TerrainHeightFieldHeight > 1U && TerrainHeightFieldCellSpacing > 0.0F) {
            float HeightFieldHalfExtentX{ (static_cast<float>(TerrainHeightFieldWidth - 1U) * TerrainHeightFieldCellSpacing * 0.5F) * std::abs(TerrainScale.x) };
            float HeightFieldHalfExtentZ{ (static_cast<float>(TerrainHeightFieldHeight - 1U) * TerrainHeightFieldCellSpacing * 0.5F) * std::abs(TerrainScale.z) };
            TerrainHalfExtentX = std::max(TerrainHalfExtentX, HeightFieldHalfExtentX);
            TerrainHalfExtentZ = std::max(TerrainHalfExtentZ, HeightFieldHalfExtentZ);
        }
        float TerrainMinimumX{ TerrainPosition.x - TerrainHalfExtentX };
        float TerrainMaximumX{ TerrainPosition.x + TerrainHalfExtentX };
        float TerrainMinimumZ{ TerrainPosition.z - TerrainHalfExtentZ };
        float TerrainMaximumZ{ TerrainPosition.z + TerrainHalfExtentZ };
        bool IsOverlappingX{ DynamicMaximumX >= TerrainMinimumX && DynamicMinimumX <= TerrainMaximumX };
        bool IsOverlappingZ{ DynamicMaximumZ >= TerrainMinimumZ && DynamicMinimumZ <= TerrainMaximumZ };
        if (!IsOverlappingX || !IsOverlappingZ) {
            continue;
        }

        float CollisionSampleX[5]{ DynamicMinimumX, DynamicMaximumX, DynamicMinimumX, DynamicMaximumX, DynamicCenterX };
        float CollisionSampleZ[5]{ DynamicMinimumZ, DynamicMinimumZ, DynamicMaximumZ, DynamicMaximumZ, DynamicCenterZ };
        float TerrainTopY{ TerrainPosition.y };
        bool HasValidSurfaceHeight{};
        for (std::size_t SampleIndex{ 0U }; SampleIndex < 5U; ++SampleIndex) {
            float SampledSurfaceHeight{};
            bool HasSurfaceHeight{ TerrainActor->TryGetSurfaceHeightAtWorldPosition(CollisionSampleX[SampleIndex], CollisionSampleZ[SampleIndex], SampledSurfaceHeight) };
            if (!HasSurfaceHeight) {
                continue;
            }

            TerrainTopY = std::max(TerrainTopY, SampledSurfaceHeight);
            HasValidSurfaceHeight = true;
        }

        if (!HasValidSurfaceHeight) {
            continue;
        }

        if (DynamicMinimumY <= TerrainTopY) {
            float PenetrationDepth{ TerrainTopY - DynamicMinimumY };
            CorrectedPosition.y += PenetrationDepth;
            CorrectedVelocity.y = 0.0F;
            HasCollision = true;
        }
    }

    return HasCollision;
}
