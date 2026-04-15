#pragma once

#include <cstddef>

#include "PhysicsLib/Actors/SolverType/PhysicsSolverType.h"

class PhysicsDynamicCollisionSolver final : public IPhysicsCollisionSolver {
public:
    PhysicsDynamicCollisionSolver();
    ~PhysicsDynamicCollisionSolver() override;
    PhysicsDynamicCollisionSolver(const PhysicsDynamicCollisionSolver& Other);
    PhysicsDynamicCollisionSolver& operator=(const PhysicsDynamicCollisionSolver& Other);
    PhysicsDynamicCollisionSolver(PhysicsDynamicCollisionSolver&& Other) noexcept;
    PhysicsDynamicCollisionSolver& operator=(PhysicsDynamicCollisionSolver&& Other) noexcept;

public:
    bool ResolveCollision(PhysicsActorBase& SelfActor, PhysicsActorBase& OtherActor, float DeltaTime) const override;
    bool ResolveDynamicCollision(const PhysicsActorBase& SelfActor, PhysicsActorBase& DynamicActor, float DeltaTime) const override;
    static void BeginFrame(std::size_t PairCandidateCount);
    static void EndFrame();
};
