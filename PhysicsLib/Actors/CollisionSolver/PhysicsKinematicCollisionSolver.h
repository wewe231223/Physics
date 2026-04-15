#pragma once

#include "PhysicsLib/Actors/SolverType/PhysicsSolverType.h"

class PhysicsKinematicCollisionSolver final : public IPhysicsCollisionSolver {
public:
    PhysicsKinematicCollisionSolver();
    ~PhysicsKinematicCollisionSolver() override;
    PhysicsKinematicCollisionSolver(const PhysicsKinematicCollisionSolver& Other);
    PhysicsKinematicCollisionSolver& operator=(const PhysicsKinematicCollisionSolver& Other);
    PhysicsKinematicCollisionSolver(PhysicsKinematicCollisionSolver&& Other) noexcept;
    PhysicsKinematicCollisionSolver& operator=(PhysicsKinematicCollisionSolver&& Other) noexcept;

public:
    bool ResolveCollision(PhysicsActorBase& SelfActor, PhysicsActorBase& OtherActor, float DeltaTime) const override;
    bool ResolveDynamicCollision(const PhysicsActorBase& SelfActor, PhysicsActorBase& DynamicActor, float DeltaTime) const override;
};
