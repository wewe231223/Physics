#include "PhysicsDynamicIntegrationLogic.h"

#include <cstddef>
#include <utility>
#include <vector>

#include "PhysicsLib/Actors/PhysicsDynamicActor.h"
#include "PhysicsLib/Simulation/Mediator/IPhysicsWorldMediator.h"

void PhysicsDynamicIntegrationLogic::IntegrateActor(IPhysicsWorldMediator& WorldMediator, PhysicsDynamicActor& DynamicActor, float DeltaTime) const {
    DynamicActor.Integrate(WorldMediator, DeltaTime);
    DynamicActor.SolveConstraints(WorldMediator, DeltaTime);
}

PhysicsDynamicIntegrationLogic::PhysicsDynamicIntegrationLogic() {
}

PhysicsDynamicIntegrationLogic::~PhysicsDynamicIntegrationLogic() {
}

PhysicsDynamicIntegrationLogic::PhysicsDynamicIntegrationLogic(const PhysicsDynamicIntegrationLogic& Other)
    : IPhysicsSimulationLogic{ Other } {
}

PhysicsDynamicIntegrationLogic& PhysicsDynamicIntegrationLogic::operator=(const PhysicsDynamicIntegrationLogic& Other) {
    if (this == &Other) {
        return *this;
    }

    IPhysicsSimulationLogic::operator=(Other);

    return *this;
}

PhysicsDynamicIntegrationLogic::PhysicsDynamicIntegrationLogic(PhysicsDynamicIntegrationLogic&& Other) noexcept
    : IPhysicsSimulationLogic{ std::move(Other) } {
}

PhysicsDynamicIntegrationLogic& PhysicsDynamicIntegrationLogic::operator=(PhysicsDynamicIntegrationLogic&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    IPhysicsSimulationLogic::operator=(std::move(Other));

    return *this;
}

void PhysicsDynamicIntegrationLogic::Execute(IPhysicsWorldMediator& WorldMediator, float DeltaTime) {
    std::vector<PhysicsDynamicActor*> DynamicActors{ WorldMediator.GetActorRepository().CollectDynamicActors() };
    std::size_t DynamicActorCount{ DynamicActors.size() };

    for (std::size_t ActorIndex{ 0U }; ActorIndex < DynamicActorCount; ++ActorIndex) {
        PhysicsDynamicActor* DynamicActor{ DynamicActors[ActorIndex] };
        if (DynamicActor == nullptr) {
            continue;
        }

        IntegrateActor(WorldMediator, *DynamicActor, DeltaTime);
    }
}
