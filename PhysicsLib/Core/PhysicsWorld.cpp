#include "PhysicsWorld.h"

#include <algorithm>
#include <cmath>
#include <utility>

#undef min 
#undef max 

namespace {
    struct AxisAlignedBounds {
        DirectX::SimpleMath::Vector3 Minimum;
        DirectX::SimpleMath::Vector3 Maximum;
    };

    AxisAlignedBounds MakeAxisAlignedBounds(const DirectX::BoundingOrientedBox& BoundingBox) {
        DirectX::XMFLOAT3 Corners[8]{};
        BoundingBox.GetCorners(Corners);

        AxisAlignedBounds Bounds{};
        Bounds.Minimum = DirectX::SimpleMath::Vector3{ Corners[0].x, Corners[0].y, Corners[0].z };
        Bounds.Maximum = Bounds.Minimum;

        for (std::size_t CornerIndex{ 1U }; CornerIndex < 8U; ++CornerIndex) {
            Bounds.Minimum.x = std::min(Bounds.Minimum.x, Corners[CornerIndex].x);
            Bounds.Minimum.y = std::min(Bounds.Minimum.y, Corners[CornerIndex].y);
            Bounds.Minimum.z = std::min(Bounds.Minimum.z, Corners[CornerIndex].z);

            Bounds.Maximum.x = std::max(Bounds.Maximum.x, Corners[CornerIndex].x);
            Bounds.Maximum.y = std::max(Bounds.Maximum.y, Corners[CornerIndex].y);
            Bounds.Maximum.z = std::max(Bounds.Maximum.z, Corners[CornerIndex].z);
        }

        return Bounds;
    }

    DirectX::SimpleMath::Vector3 CalculateBoundsCenter(const AxisAlignedBounds& Bounds) {
        DirectX::SimpleMath::Vector3 Center{ (Bounds.Minimum + Bounds.Maximum) * 0.5F };
        return Center;
    }

    DirectX::SimpleMath::Vector3 CalculateBoundsHalfExtent(const AxisAlignedBounds& Bounds) {
        DirectX::SimpleMath::Vector3 HalfExtent{ (Bounds.Maximum - Bounds.Minimum) * 0.5F };
        return HalfExtent;
    }
}

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

    ResolveDynamicCollisions();
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

    float ActorInverseMass{ Actor.GetInverseMass() };
    if (ActorInverseMass <= 0.0F) {
        return;
    }

    if (Actor.GetIsSleeping()) {
        return;
    }

    float ActorMass{ Actor.GetMass() };
    DirectX::SimpleMath::Vector3 TotalAcceleration{ mSettings.Gravity + Actor.GetAcceleration() };
    DirectX::SimpleMath::Vector3 AppliedForce{ TotalAcceleration * ActorMass };
    DirectX::SimpleMath::Vector3 NextLinearMomentum{ Actor.GetLinearMomentum() + (AppliedForce * DeltaTime) };
    DirectX::SimpleMath::Vector3 NextVelocity{ NextLinearMomentum * ActorInverseMass };
    float DampingFactor{ std::max(0.0F, 1.0F - (Actor.GetLinearDamping() * DeltaTime)) };
    NextVelocity *= DampingFactor;
    NextLinearMomentum = NextVelocity * ActorMass;
    DirectX::SimpleMath::Vector3 NextAngularMomentum{ Actor.GetAngularMomentum() };
    float AngularDampingFactor{ std::max(0.0F, 1.0F - (Actor.GetAngularDamping() * DeltaTime)) };
    NextAngularMomentum *= AngularDampingFactor;

    DirectX::SimpleMath::Vector3 NextPosition{ Actor.GetPosition() + (NextVelocity * DeltaTime) };
    DirectX::BoundingOrientedBox PredictedWorldBoundingBox{};
    DirectX::SimpleMath::Matrix ScalingMatrix{ DirectX::SimpleMath::Matrix::CreateScale(Actor.GetScale()) };
    DirectX::SimpleMath::Matrix RotationMatrix{ DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(Actor.GetRotation().y, Actor.GetRotation().x, Actor.GetRotation().z) };
    DirectX::SimpleMath::Matrix TranslationMatrix{ DirectX::SimpleMath::Matrix::CreateTranslation(NextPosition) };
    DirectX::SimpleMath::Matrix WorldMatrix{ ScalingMatrix * RotationMatrix * TranslationMatrix };
    Actor.GetLocalBoundingBox().Transform(PredictedWorldBoundingBox, WorldMatrix);

    DirectX::SimpleMath::Vector3 CorrectedPosition{ NextPosition };
    DirectX::SimpleMath::Vector3 CorrectedVelocity{ NextVelocity };
    ResolveStaticCollisions(PredictedWorldBoundingBox, ActorInverseMass, Actor.GetFriction(), Actor.GetRestitution(), CorrectedPosition, CorrectedVelocity);

    Actor.SetVelocity(CorrectedVelocity);
    Actor.SetLinearMomentum(CorrectedVelocity * ActorMass);
    Actor.SetAngularMomentum(NextAngularMomentum);
    Actor.SetPosition(CorrectedPosition);
    Actor.UpdateSleepState();
}

bool PhysicsWorld::ResolveStaticCollisions(const DirectX::BoundingOrientedBox& PredictedWorldBoundingBox, float DynamicInverseMass, float DynamicFriction, float DynamicRestitution, DirectX::SimpleMath::Vector3& CorrectedPosition, DirectX::SimpleMath::Vector3& CorrectedVelocity) const {
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
        bool CurrentCollision{ StaticActor->ResolveDynamicCollision(PredictedWorldBoundingBox, DynamicInverseMass, DynamicFriction, DynamicRestitution, CorrectedPosition, CorrectedVelocity) };
        HasCollision = HasCollision || CurrentCollision;
    }

    return HasCollision;
}

void PhysicsWorld::ResolveDynamicCollisions() const {
    std::size_t ActorCount{ mActors.size() };
    for (std::size_t FirstActorIndex{ 0U }; FirstActorIndex < ActorCount; ++FirstActorIndex) {
        PhysicsActor* FirstActorBase{ mActors[FirstActorIndex].get() };
        if (FirstActorBase == nullptr || FirstActorBase->GetActorType() != PhysicsActor::PhysicsActorType::Dynamic) {
            continue;
        }

        PhysicsDynamicActor* FirstActor{ static_cast<PhysicsDynamicActor*>(FirstActorBase) };
        if (!FirstActor->GetIsActive() || FirstActor->GetInverseMass() <= 0.0F) {
            continue;
        }

        for (std::size_t SecondActorIndex{ FirstActorIndex + 1U }; SecondActorIndex < ActorCount; ++SecondActorIndex) {
            PhysicsActor* SecondActorBase{ mActors[SecondActorIndex].get() };
            if (SecondActorBase == nullptr || SecondActorBase->GetActorType() != PhysicsActor::PhysicsActorType::Dynamic) {
                continue;
            }

            PhysicsDynamicActor* SecondActor{ static_cast<PhysicsDynamicActor*>(SecondActorBase) };
            if (!SecondActor->GetIsActive() || SecondActor->GetInverseMass() <= 0.0F) {
                continue;
            }

            bool HasCollision{ ResolveDynamicCollisionPair(*FirstActor, *SecondActor) };
            if (!HasCollision) {
                continue;
            }

            FirstActor->UpdateSleepState();
            SecondActor->UpdateSleepState();
        }
    }
}

bool PhysicsWorld::ResolveDynamicCollisionPair(PhysicsDynamicActor& FirstActor, PhysicsDynamicActor& SecondActor) const {
    DirectX::BoundingOrientedBox FirstBounds{ FirstActor.GetWorldBoundingBox() };
    DirectX::BoundingOrientedBox SecondBounds{ SecondActor.GetWorldBoundingBox() };
    bool IsIntersecting{ FirstBounds.Intersects(SecondBounds) };
    if (!IsIntersecting) {
        return false;
    }

    AxisAlignedBounds FirstAabb{ MakeAxisAlignedBounds(FirstBounds) };
    AxisAlignedBounds SecondAabb{ MakeAxisAlignedBounds(SecondBounds) };
    DirectX::SimpleMath::Vector3 FirstCenter{ CalculateBoundsCenter(FirstAabb) };
    DirectX::SimpleMath::Vector3 SecondCenter{ CalculateBoundsCenter(SecondAabb) };
    DirectX::SimpleMath::Vector3 FirstHalfExtent{ CalculateBoundsHalfExtent(FirstAabb) };
    DirectX::SimpleMath::Vector3 SecondHalfExtent{ CalculateBoundsHalfExtent(SecondAabb) };
    DirectX::SimpleMath::Vector3 DeltaCenter{ SecondCenter - FirstCenter };

    float OverlapX{ (FirstHalfExtent.x + SecondHalfExtent.x) - std::abs(DeltaCenter.x) };
    float OverlapY{ (FirstHalfExtent.y + SecondHalfExtent.y) - std::abs(DeltaCenter.y) };
    float OverlapZ{ (FirstHalfExtent.z + SecondHalfExtent.z) - std::abs(DeltaCenter.z) };
    if (OverlapX <= 0.0F || OverlapY <= 0.0F || OverlapZ <= 0.0F) {
        return false;
    }

    float PenetrationDepth{ OverlapX };
    DirectX::SimpleMath::Vector3 CollisionNormal{ DeltaCenter.x >= 0.0F ? 1.0F : -1.0F, 0.0F, 0.0F };

    if (OverlapY < PenetrationDepth) {
        PenetrationDepth = OverlapY;
        CollisionNormal = DirectX::SimpleMath::Vector3{ 0.0F, DeltaCenter.y >= 0.0F ? 1.0F : -1.0F, 0.0F };
    }

    if (OverlapZ < PenetrationDepth) {
        PenetrationDepth = OverlapZ;
        CollisionNormal = DirectX::SimpleMath::Vector3{ 0.0F, 0.0F, DeltaCenter.z >= 0.0F ? 1.0F : -1.0F };
    }

    float FirstInverseMass{ FirstActor.GetInverseMass() };
    float SecondInverseMass{ SecondActor.GetInverseMass() };
    float CombinedInverseMass{ FirstInverseMass + SecondInverseMass };
    if (CombinedInverseMass <= 0.0F) {
        return false;
    }

    float FirstDisplacementFactor{ FirstInverseMass / CombinedInverseMass };
    float SecondDisplacementFactor{ SecondInverseMass / CombinedInverseMass };
    DirectX::SimpleMath::Vector3 SeparationVector{ CollisionNormal * PenetrationDepth };
    DirectX::SimpleMath::Vector3 FirstCorrectedPosition{ FirstActor.GetPosition() - (SeparationVector * FirstDisplacementFactor) };
    DirectX::SimpleMath::Vector3 SecondCorrectedPosition{ SecondActor.GetPosition() + (SeparationVector * SecondDisplacementFactor) };
    FirstActor.SetPosition(FirstCorrectedPosition);
    SecondActor.SetPosition(SecondCorrectedPosition);

    DirectX::SimpleMath::Vector3 FirstVelocity{ FirstActor.GetVelocity() };
    DirectX::SimpleMath::Vector3 SecondVelocity{ SecondActor.GetVelocity() };
    DirectX::SimpleMath::Vector3 NormalRelativeVelocity{ SecondVelocity - FirstVelocity };
    float RelativeNormalVelocity{ NormalRelativeVelocity.Dot(CollisionNormal) };
    if (RelativeNormalVelocity >= 0.0F) {
        return true;
    }

    float EffectiveRestitution{ std::min(FirstActor.GetRestitution(), SecondActor.GetRestitution()) };
    float ImpulseMagnitude{ -(1.0F + EffectiveRestitution) * RelativeNormalVelocity / CombinedInverseMass };
    DirectX::SimpleMath::Vector3 Impulse{ CollisionNormal * ImpulseMagnitude };
    FirstVelocity -= Impulse * FirstInverseMass;
    SecondVelocity += Impulse * SecondInverseMass;

    float EffectiveFriction{ std::sqrt(std::max(0.0F, FirstActor.GetFriction() * SecondActor.GetFriction())) };
    DirectX::SimpleMath::Vector3 RelativeVelocity{ SecondVelocity - FirstVelocity };
    float RelativeVelocityAlongNormal{ RelativeVelocity.Dot(CollisionNormal) };
    DirectX::SimpleMath::Vector3 TangentialVelocity{ RelativeVelocity - (CollisionNormal * RelativeVelocityAlongNormal) };
    float TangentialVelocityLength{ TangentialVelocity.Length() };
    if (TangentialVelocityLength > 0.0001F) {
        DirectX::SimpleMath::Vector3 Tangent{ TangentialVelocity / TangentialVelocityLength };
        float FrictionImpulseMagnitude{ -RelativeVelocity.Dot(Tangent) / CombinedInverseMass };
        float MaximumFrictionImpulse{ ImpulseMagnitude * EffectiveFriction };
        FrictionImpulseMagnitude = std::clamp(FrictionImpulseMagnitude, -MaximumFrictionImpulse, MaximumFrictionImpulse);
        DirectX::SimpleMath::Vector3 FrictionImpulse{ Tangent * FrictionImpulseMagnitude };
        FirstVelocity -= FrictionImpulse * FirstInverseMass;
        SecondVelocity += FrictionImpulse * SecondInverseMass;
    }

    FirstActor.SetVelocity(FirstVelocity);
    SecondActor.SetVelocity(SecondVelocity);
    FirstActor.SetLinearMomentum(FirstVelocity * FirstActor.GetMass());
    SecondActor.SetLinearMomentum(SecondVelocity * SecondActor.GetMass());

    return true;
}
