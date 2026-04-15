#pragma once

#include "PhysicsLib/Actors/SolverType/PhysicsSolverType.h"

class PhysicsStaticIntegrater final : public IPhysicsIntegrater {
public:
    PhysicsStaticIntegrater();
    ~PhysicsStaticIntegrater() override;
    PhysicsStaticIntegrater(const PhysicsStaticIntegrater& Other);
    PhysicsStaticIntegrater& operator=(const PhysicsStaticIntegrater& Other);
    PhysicsStaticIntegrater(PhysicsStaticIntegrater&& Other) noexcept;
    PhysicsStaticIntegrater& operator=(PhysicsStaticIntegrater&& Other) noexcept;

public:
    void Integrate(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const override;
};
