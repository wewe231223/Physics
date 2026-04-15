#pragma once

#include "PhysicsLib/Actors/SolverType/PhysicsSolverType.h"

class PhysicsStaticCollisionSolver final : public IPhysicsCollisionSolver {
public:
    PhysicsStaticCollisionSolver();
    ~PhysicsStaticCollisionSolver() override;
    PhysicsStaticCollisionSolver(const PhysicsStaticCollisionSolver& Other);
    PhysicsStaticCollisionSolver& operator=(const PhysicsStaticCollisionSolver& Other);
    PhysicsStaticCollisionSolver(PhysicsStaticCollisionSolver&& Other) noexcept;
    PhysicsStaticCollisionSolver& operator=(PhysicsStaticCollisionSolver&& Other) noexcept;

public:
    bool ResolveCollision(PhysicsActorBase& SelfActor, PhysicsActorBase& OtherActor, float DeltaTime) const override;
    bool ResolveDynamicCollision(const PhysicsActorBase& SelfActor, PhysicsActorBase& DynamicActor, float DeltaTime) const override;
};
