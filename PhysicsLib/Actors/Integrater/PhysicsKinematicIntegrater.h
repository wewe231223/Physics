#pragma once

#include "PhysicsLib/Actors/SolverType/PhysicsSolverType.h"

class PhysicsKinematicIntegrater final : public IPhysicsIntegrater {
public:
    PhysicsKinematicIntegrater();
    ~PhysicsKinematicIntegrater() override;
    PhysicsKinematicIntegrater(const PhysicsKinematicIntegrater& Other);
    PhysicsKinematicIntegrater& operator=(const PhysicsKinematicIntegrater& Other);
    PhysicsKinematicIntegrater(PhysicsKinematicIntegrater&& Other) noexcept;
    PhysicsKinematicIntegrater& operator=(PhysicsKinematicIntegrater&& Other) noexcept;

public:
    void Integrate(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const override;
};
