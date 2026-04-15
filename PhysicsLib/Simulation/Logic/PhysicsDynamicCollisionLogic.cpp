#include "PhysicsDynamicCollisionLogic.h"

#include <cstddef>
#include <utility>

#include "PhysicsLib/Actors/PhysicsDynamicActor.h"
#include "PhysicsLib/Actors/PhysicsStaticActor.h"
#include "PhysicsLib/Simulation/Mediator/IPhysicsWorldMediator.h"

constexpr std::size_t DynamicCollisionSolverMinimumIterationCount{ 1U };
constexpr std::size_t DynamicCollisionSolverMaximumIterationCount{ 4U };
constexpr std::size_t DynamicCollisionPairsPerAdditionalIteration{ 24U };

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
    std::vector<PhysicsDynamicCollisionPairCandidate> PairCandidates{ WorldMediator.GetSpatialQuery().QueryDynamicCollisionPairs(WorldMediator.GetActorRepository()) };
    std::vector<PhysicsDynamicActor*> DynamicActors{ WorldMediator.GetActorRepository().CollectDynamicActors() };
    std::vector<const PhysicsStaticActor*> StaticActors{ WorldMediator.GetActorRepository().CollectStaticActors() };

    ResolveDynamicCollisions(WorldMediator, PairCandidates, DeltaTime);
    ResolveStaticCollisions(WorldMediator, DynamicActors, StaticActors, DeltaTime);
}

void PhysicsDynamicCollisionLogic::ResolveDynamicCollisions(IPhysicsWorldMediator& WorldMediator, const std::vector<PhysicsDynamicCollisionPairCandidate>& PairCandidates, float DeltaTime) const {
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

            bool HasCollision{ ResolveCollisionPair(WorldMediator, *FirstActor, *SecondActor, DeltaTime) };
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

void PhysicsDynamicCollisionLogic::ResolveStaticCollisions(IPhysicsWorldMediator& WorldMediator, const std::vector<PhysicsDynamicActor*>& DynamicActors, const std::vector<const PhysicsStaticActor*>& StaticActors, float DeltaTime) const {
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

            bool HasCollision{ StaticActor->ResolveDynamicCollision(*DynamicActor, DeltaTime) };
            if (!HasCollision) {
                continue;
            }

            WorldMediator.PublishEvent(PhysicsSimulationEventType::StaticCollisionResolved, DynamicActor, StaticActor);
            DynamicActor->UpdateSleepState();
        }
    }
}

bool PhysicsDynamicCollisionLogic::ResolveCollisionPair(IPhysicsWorldMediator& WorldMediator, PhysicsDynamicActor& FirstActor, PhysicsDynamicActor& SecondActor, float DeltaTime) const {
    bool HasCollision{ FirstActor.ResolveActorCollision(SecondActor, DeltaTime) };
    if (HasCollision) {
        WorldMediator.PublishEvent(PhysicsSimulationEventType::DynamicCollisionResolved, &FirstActor, &SecondActor);
    }

    return HasCollision;
}
