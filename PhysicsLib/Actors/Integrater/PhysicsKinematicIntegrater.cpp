#include <algorithm>
#include <utility>

#include "PhysicsLib/Actors/Integrater/PhysicsKinematicIntegrater.h"
#include "PhysicsLib/Actors/PhysicsActorBase.h"
#include "PhysicsLib/Simulation/Mediator/IPhysicsWorldMediator.h"

#undef max
#undef min

PhysicsKinematicIntegrater::PhysicsKinematicIntegrater() {
}

PhysicsKinematicIntegrater::~PhysicsKinematicIntegrater() {
}

PhysicsKinematicIntegrater::PhysicsKinematicIntegrater(const PhysicsKinematicIntegrater& Other)
    : IPhysicsIntegrater{ Other } {
}

PhysicsKinematicIntegrater& PhysicsKinematicIntegrater::operator=(const PhysicsKinematicIntegrater& Other) {
    if (this == &Other) {
        return *this;
    }

    IPhysicsIntegrater::operator=(Other);
    return *this;
}

PhysicsKinematicIntegrater::PhysicsKinematicIntegrater(PhysicsKinematicIntegrater&& Other) noexcept
    : IPhysicsIntegrater{ std::move(Other) } {
}

PhysicsKinematicIntegrater& PhysicsKinematicIntegrater::operator=(PhysicsKinematicIntegrater&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    IPhysicsIntegrater::operator=(std::move(Other));
    return *this;
}

void PhysicsKinematicIntegrater::Integrate(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const {
    (void)WorldMediator;

    if (Actor.GetActorType() != PhysicsActorBase::PhysicsActorType::Kinematic) {
        return;
    }

    if (!Actor.GetIsActive() || DeltaTime <= 0.0F) {
        return;
    }

    DirectX::SimpleMath::Vector3 NextVelocity{ Actor.GetVelocity() };
    float DampingFactor{ std::max(0.0F, 1.0F - (Actor.GetLinearDamping() * DeltaTime)) };
    NextVelocity *= DampingFactor;
    DirectX::SimpleMath::Vector3 NextPosition{ Actor.GetPosition() + (NextVelocity * DeltaTime) };
    Actor.SetVelocity(NextVelocity);
    Actor.SetPosition(NextPosition);
    Actor.ClearAccumulatedForce();
}
