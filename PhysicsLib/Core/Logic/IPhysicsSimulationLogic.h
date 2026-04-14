#pragma once

class IPhysicsWorldMediator;

class IPhysicsSimulationLogic {
public:
    IPhysicsSimulationLogic();
    virtual ~IPhysicsSimulationLogic();
    IPhysicsSimulationLogic(const IPhysicsSimulationLogic& Other);
    IPhysicsSimulationLogic& operator=(const IPhysicsSimulationLogic& Other);
    IPhysicsSimulationLogic(IPhysicsSimulationLogic&& Other) noexcept;
    IPhysicsSimulationLogic& operator=(IPhysicsSimulationLogic&& Other) noexcept;

public:
    virtual void Execute(IPhysicsWorldMediator& WorldMediator, float DeltaTime) = 0;
};
