#include "PhysicsDynamicCollisionLogic.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "PhysicsLib/Simulation/Mediator/IPhysicsWorldMediator.h"
#include "PhysicsLib/Actors/PhysicsStaticActor.h"

#undef max 
#undef min 

namespace {
    struct AxisAlignedBounds {
        DirectX::SimpleMath::Vector3 mMinimum;
        DirectX::SimpleMath::Vector3 mMaximum;
    };

    constexpr std::size_t DynamicCollisionSolverMinimumIterationCount{ 1U };
    constexpr std::size_t DynamicCollisionSolverMaximumIterationCount{ 4U };
    constexpr std::size_t DynamicCollisionPairsPerAdditionalIteration{ 24U };
    constexpr float DynamicCollisionSeparationBias{ 0.001F };

    AxisAlignedBounds MakeAxisAlignedBounds(const DirectX::BoundingOrientedBox& BoundingBox) {
        DirectX::XMFLOAT3 Corners[8]{};
        BoundingBox.GetCorners(Corners);

        AxisAlignedBounds Bounds{};
        Bounds.mMinimum = DirectX::SimpleMath::Vector3{ Corners[0].x, Corners[0].y, Corners[0].z };
        Bounds.mMaximum = Bounds.mMinimum;

        for (std::size_t CornerIndex{ 1U }; CornerIndex < 8U; ++CornerIndex) {
            Bounds.mMinimum.x = std::min(Bounds.mMinimum.x, Corners[CornerIndex].x);
            Bounds.mMinimum.y = std::min(Bounds.mMinimum.y, Corners[CornerIndex].y);
            Bounds.mMinimum.z = std::min(Bounds.mMinimum.z, Corners[CornerIndex].z);

            Bounds.mMaximum.x = std::max(Bounds.mMaximum.x, Corners[CornerIndex].x);
            Bounds.mMaximum.y = std::max(Bounds.mMaximum.y, Corners[CornerIndex].y);
            Bounds.mMaximum.z = std::max(Bounds.mMaximum.z, Corners[CornerIndex].z);
        }

        return Bounds;
    }

    DirectX::SimpleMath::Vector3 CalculateBoundsCenter(const AxisAlignedBounds& Bounds) {
        DirectX::SimpleMath::Vector3 Center{ (Bounds.mMinimum + Bounds.mMaximum) * 0.5F };
        return Center;
    }

    DirectX::SimpleMath::Vector3 CalculateBoundsHalfExtent(const AxisAlignedBounds& Bounds) {
        DirectX::SimpleMath::Vector3 HalfExtent{ (Bounds.mMaximum - Bounds.mMinimum) * 0.5F };
        return HalfExtent;
    }
}

PhysicsDynamicCollisionLogic::PhysicsDynamicCollisionLogic() {
}

PhysicsDynamicCollisionLogic::~PhysicsDynamicCollisionLogic() {
}

PhysicsDynamicCollisionLogic::PhysicsDynamicCollisionLogic(const PhysicsDynamicCollisionLogic& Other)
    : IPhysicsSimulationLogic{ Other } {
}

PhysicsDynamicCollisionLogic& PhysicsDynamicCollisionLogic::operator=(const PhysicsDynamicCollisionLogic& Other) {
    if (this == &Other) {
        return *this;
    }

    IPhysicsSimulationLogic::operator=(Other);

    return *this;
}

PhysicsDynamicCollisionLogic::PhysicsDynamicCollisionLogic(PhysicsDynamicCollisionLogic&& Other) noexcept
    : IPhysicsSimulationLogic{ std::move(Other) } {
}

PhysicsDynamicCollisionLogic& PhysicsDynamicCollisionLogic::operator=(PhysicsDynamicCollisionLogic&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    IPhysicsSimulationLogic::operator=(std::move(Other));

    return *this;
}

void PhysicsDynamicCollisionLogic::Execute(IPhysicsWorldMediator& WorldMediator, float DeltaTime) {
    (void)DeltaTime;

    std::vector<PhysicsDynamicCollisionPairCandidate> PairCandidates{ WorldMediator.GetSpatialQuery().QueryDynamicCollisionPairs(WorldMediator.GetActorRepository()) };
    std::vector<PhysicsDynamicActor*> DynamicActors{ WorldMediator.GetActorRepository().CollectDynamicActors() };
    std::vector<const PhysicsStaticActor*> StaticActors{ WorldMediator.GetActorRepository().CollectStaticActors() };

    ResolveDynamicCollisions(WorldMediator, PairCandidates);
    ResolveStaticCollisions(WorldMediator, DynamicActors, StaticActors);
}

void PhysicsDynamicCollisionLogic::ResolveDynamicCollisions(IPhysicsWorldMediator& WorldMediator, const std::vector<PhysicsDynamicCollisionPairCandidate>& PairCandidates) const {
    std::size_t PairCandidateCount{ PairCandidates.size() };
    if (PairCandidateCount == 0U) {
        return;
    }

    std::size_t AdditionalIterationCount{ PairCandidateCount / DynamicCollisionPairsPerAdditionalIteration };
    std::size_t SolverIterationCount{ DynamicCollisionSolverMinimumIterationCount + AdditionalIterationCount };
    if (SolverIterationCount > DynamicCollisionSolverMaximumIterationCount) {
        SolverIterationCount = DynamicCollisionSolverMaximumIterationCount;
    }

    for (std::size_t IterationIndex{ 0U }; IterationIndex < SolverIterationCount; ++IterationIndex) {
        bool HasAnyCollision{};

        for (std::size_t PairIndex{ 0U }; PairIndex < PairCandidateCount; ++PairIndex) {
            PhysicsDynamicActor* FirstActor{ PairCandidates[PairIndex].mFirstActor };
            PhysicsDynamicActor* SecondActor{ PairCandidates[PairIndex].mSecondActor };
            if (FirstActor == nullptr || SecondActor == nullptr) {
                continue;
            }

            if (!FirstActor->GetIsActive() || FirstActor->GetInverseMass() <= 0.0F) {
                continue;
            }

            if (!SecondActor->GetIsActive() || SecondActor->GetInverseMass() <= 0.0F) {
                continue;
            }

            bool HasCollision{ ResolveCollisionPair(WorldMediator, *FirstActor, *SecondActor) };
            if (!HasCollision) {
                continue;
            }

            HasAnyCollision = true;
            FirstActor->UpdateSleepState();
            SecondActor->UpdateSleepState();
        }

        if (!HasAnyCollision) {
            break;
        }
    }
}

void PhysicsDynamicCollisionLogic::ResolveStaticCollisions(IPhysicsWorldMediator& WorldMediator, const std::vector<PhysicsDynamicActor*>& DynamicActors, const std::vector<const PhysicsStaticActor*>& StaticActors) const {
    std::size_t DynamicActorCount{ DynamicActors.size() };
    if (DynamicActorCount == 0U) {
        return;
    }

    std::size_t StaticActorCount{ StaticActors.size() };
    if (StaticActorCount == 0U) {
        return;
    }

    for (std::size_t DynamicActorIndex{ 0U }; DynamicActorIndex < DynamicActorCount; ++DynamicActorIndex) {
        PhysicsDynamicActor* DynamicActor{ DynamicActors[DynamicActorIndex] };
        if (DynamicActor == nullptr) {
            continue;
        }

        if (!DynamicActor->GetIsActive() || DynamicActor->GetInverseMass() <= 0.0F) {
            continue;
        }

        for (std::size_t StaticActorIndex{ 0U }; StaticActorIndex < StaticActorCount; ++StaticActorIndex) {
            const PhysicsStaticActor* StaticActor{ StaticActors[StaticActorIndex] };
            if (StaticActor == nullptr) {
                continue;
            }

            bool HasCollision{ StaticActor->ResolveDynamicCollision(*DynamicActor) };
            if (!HasCollision) {
                continue;
            }

            WorldMediator.PublishEvent(PhysicsSimulationEventType::StaticCollisionResolved, DynamicActor, StaticActor);
            DynamicActor->UpdateSleepState();
        }
    }
}

bool PhysicsDynamicCollisionLogic::ResolveCollisionPair(IPhysicsWorldMediator& WorldMediator, PhysicsDynamicActor& FirstActor, PhysicsDynamicActor& SecondActor) const {
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
    float CorrectedPenetrationDepth{ PenetrationDepth + DynamicCollisionSeparationBias };
    DirectX::SimpleMath::Vector3 SeparationVector{ CollisionNormal * CorrectedPenetrationDepth };
    DirectX::SimpleMath::Vector3 FirstCorrectedPosition{ FirstActor.GetPosition() - (SeparationVector * FirstDisplacementFactor) };
    DirectX::SimpleMath::Vector3 SecondCorrectedPosition{ SecondActor.GetPosition() + (SeparationVector * SecondDisplacementFactor) };
    FirstActor.SetPosition(FirstCorrectedPosition);
    SecondActor.SetPosition(SecondCorrectedPosition);

    DirectX::SimpleMath::Vector3 FirstVelocity{ FirstActor.GetVelocity() };
    DirectX::SimpleMath::Vector3 SecondVelocity{ SecondActor.GetVelocity() };
    DirectX::SimpleMath::Vector3 NormalRelativeVelocity{ SecondVelocity - FirstVelocity };
    float RelativeNormalVelocity{ NormalRelativeVelocity.Dot(CollisionNormal) };
    if (RelativeNormalVelocity >= 0.0F) {
        WorldMediator.PublishEvent(PhysicsSimulationEventType::DynamicCollisionResolved, &FirstActor, &SecondActor);
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

    WorldMediator.PublishEvent(PhysicsSimulationEventType::DynamicCollisionResolved, &FirstActor, &SecondActor);

    return true;
}


