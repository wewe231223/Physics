#include <utility>

#include "PhysicsLib/Actors/CollisionSolver/PhysicsKinematicCollisionSolver.h"
#include "PhysicsLib/Actors/PhysicsActorBase.h"

PhysicsKinematicCollisionSolver::PhysicsKinematicCollisionSolver() {
}

PhysicsKinematicCollisionSolver::~PhysicsKinematicCollisionSolver() {
}

PhysicsKinematicCollisionSolver::PhysicsKinematicCollisionSolver(const PhysicsKinematicCollisionSolver& Other)
    : IPhysicsCollisionSolver{ Other } {
}

PhysicsKinematicCollisionSolver& PhysicsKinematicCollisionSolver::operator=(const PhysicsKinematicCollisionSolver& Other) {
    if (this == &Other) {
        return *this;
    }

    IPhysicsCollisionSolver::operator=(Other);
    return *this;
}

PhysicsKinematicCollisionSolver::PhysicsKinematicCollisionSolver(PhysicsKinematicCollisionSolver&& Other) noexcept
    : IPhysicsCollisionSolver{ std::move(Other) } {
}

PhysicsKinematicCollisionSolver& PhysicsKinematicCollisionSolver::operator=(PhysicsKinematicCollisionSolver&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    IPhysicsCollisionSolver::operator=(std::move(Other));
    return *this;
}

bool PhysicsKinematicCollisionSolver::ResolveCollision(PhysicsActorBase& SelfActor, PhysicsActorBase& OtherActor, float DeltaTime) const {
    (void)SelfActor;
    (void)OtherActor;
    (void)DeltaTime;
    return false;
}

bool PhysicsKinematicCollisionSolver::ResolveDynamicCollision(const PhysicsActorBase& SelfActor, PhysicsActorBase& DynamicActor, float DeltaTime) const {
    (void)SelfActor;
    (void)DynamicActor;
    (void)DeltaTime;
    return false;
}
