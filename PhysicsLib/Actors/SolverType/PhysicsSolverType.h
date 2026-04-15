#pragma once

#include <concepts>

class IPhysicsWorldMediator;
class PhysicsActorBase;

class IPhysicsCollisionSolver {
public:
    IPhysicsCollisionSolver();
    virtual ~IPhysicsCollisionSolver();
    IPhysicsCollisionSolver(const IPhysicsCollisionSolver& Other);
    IPhysicsCollisionSolver& operator=(const IPhysicsCollisionSolver& Other);
    IPhysicsCollisionSolver(IPhysicsCollisionSolver&& Other) noexcept;
    IPhysicsCollisionSolver& operator=(IPhysicsCollisionSolver&& Other) noexcept;

public:
    virtual bool ResolveCollision(PhysicsActorBase& SelfActor, PhysicsActorBase& OtherActor, float DeltaTime) const = 0;
    virtual bool ResolveDynamicCollision(const PhysicsActorBase& SelfActor, PhysicsActorBase& DynamicActor, float DeltaTime) const = 0;
};

class IPhysicsIntegrater {
public:
    IPhysicsIntegrater();
    virtual ~IPhysicsIntegrater();
    IPhysicsIntegrater(const IPhysicsIntegrater& Other);
    IPhysicsIntegrater& operator=(const IPhysicsIntegrater& Other);
    IPhysicsIntegrater(IPhysicsIntegrater&& Other) noexcept;
    IPhysicsIntegrater& operator=(IPhysicsIntegrater&& Other) noexcept;

public:
    virtual void Integrate(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const = 0;
};

class IPhysicsConstraintSolver {
public:
    IPhysicsConstraintSolver();
    virtual ~IPhysicsConstraintSolver();
    IPhysicsConstraintSolver(const IPhysicsConstraintSolver& Other);
    IPhysicsConstraintSolver& operator=(const IPhysicsConstraintSolver& Other);
    IPhysicsConstraintSolver(IPhysicsConstraintSolver&& Other) noexcept;
    IPhysicsConstraintSolver& operator=(IPhysicsConstraintSolver&& Other) noexcept;

public:
    virtual void Solve(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const = 0;
};

template <typename SolverType>
concept PhysicsCollisionSolverPolicy = std::derived_from<SolverType, IPhysicsCollisionSolver> && std::default_initializable<SolverType>;

template <typename IntegraterType>
concept PhysicsIntegraterPolicy = std::derived_from<IntegraterType, IPhysicsIntegrater> && std::default_initializable<IntegraterType>;

template <typename ConstraintSolverType>
concept PhysicsConstraintSolverPolicy = std::derived_from<ConstraintSolverType, IPhysicsConstraintSolver> && std::default_initializable<ConstraintSolverType>;
