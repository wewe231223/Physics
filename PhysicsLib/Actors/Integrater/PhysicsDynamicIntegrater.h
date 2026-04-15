#pragma once

#include "PhysicsLib/Actors/SolverType/PhysicsSolverType.h"

class PhysicsDynamicIntegrater final : public IPhysicsIntegrater {
public:
    PhysicsDynamicIntegrater();
    ~PhysicsDynamicIntegrater() override;
    PhysicsDynamicIntegrater(const PhysicsDynamicIntegrater& Other);
    PhysicsDynamicIntegrater& operator=(const PhysicsDynamicIntegrater& Other);
    PhysicsDynamicIntegrater(PhysicsDynamicIntegrater&& Other) noexcept;
    PhysicsDynamicIntegrater& operator=(PhysicsDynamicIntegrater&& Other) noexcept;

public:
    void Integrate(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const override;
};
