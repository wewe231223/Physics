#pragma once

#include "PhysicsLib/Actors/SolverType/PhysicsSolverType.h"

class PhysicsNoConstraintSolver final : public IPhysicsConstraintSolver {
public:
    PhysicsNoConstraintSolver();
    ~PhysicsNoConstraintSolver() override;
    PhysicsNoConstraintSolver(const PhysicsNoConstraintSolver& Other);
    PhysicsNoConstraintSolver& operator=(const PhysicsNoConstraintSolver& Other);
    PhysicsNoConstraintSolver(PhysicsNoConstraintSolver&& Other) noexcept;
    PhysicsNoConstraintSolver& operator=(PhysicsNoConstraintSolver&& Other) noexcept;

public:
    void Solve(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const override;
};
