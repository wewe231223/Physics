#include "PhysicsDynamicIntegrationLogic.h"

#include <algorithm>
#include <utility>

#include "../Mediator/IPhysicsWorldMediator.h"

#undef min 
#undef max 

void PhysicsDynamicIntegrationLogic::IntegrateActor(IPhysicsWorldMediator& WorldMediator, PhysicsDynamicActor& DynamicActor, float DeltaTime) const {
    if (!DynamicActor.GetIsActive()) {
        return;
    }

    float ActorInverseMass{ DynamicActor.GetInverseMass() };
    if (ActorInverseMass <= 0.0F) {
        return;
    }

    if (DynamicActor.GetIsSleeping()) {
        return;
    }

    float ActorMass{ DynamicActor.GetMass() };
    DirectX::SimpleMath::Vector3 TotalAcceleration{ WorldMediator.GetGravity() + DynamicActor.GetAcceleration() };
    DirectX::SimpleMath::Vector3 TotalForce{ (TotalAcceleration * ActorMass) + DynamicActor.GetAccumulatedForce() };
    DirectX::SimpleMath::Vector3 NextLinearMomentum{ DynamicActor.GetLinearMomentum() + (TotalForce * DeltaTime) };
    DirectX::SimpleMath::Vector3 NextVelocity{ NextLinearMomentum * ActorInverseMass };
    float DampingFactor{ std::max(0.0F, 1.0F - (DynamicActor.GetLinearDamping() * DeltaTime)) };
    NextVelocity *= DampingFactor;
    NextLinearMomentum = NextVelocity * ActorMass;
    DirectX::SimpleMath::Vector3 NextAngularMomentum{ DynamicActor.GetAngularMomentum() };
    float AngularDampingFactor{ std::max(0.0F, 1.0F - (DynamicActor.GetAngularDamping() * DeltaTime)) };
    NextAngularMomentum *= AngularDampingFactor;

    DirectX::SimpleMath::Vector3 NextPosition{ DynamicActor.GetPosition() + (NextVelocity * DeltaTime) };
    DynamicActor.SetPosition(NextPosition);
    DynamicActor.SetVelocity(NextVelocity);
    DynamicActor.SetAngularMomentum(NextAngularMomentum);

    DynamicActor.SetLinearMomentum(DynamicActor.GetVelocity() * ActorMass);
    DynamicActor.ClearAccumulatedForce();
    DynamicActor.UpdateSleepState();
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
