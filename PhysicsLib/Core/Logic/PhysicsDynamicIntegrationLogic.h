#pragma once

#include "IPhysicsSimulationLogic.h"

class PhysicsDynamicActor;
class IPhysicsWorldMediator;

class PhysicsDynamicIntegrationLogic final : public IPhysicsSimulationLogic {
public:
    PhysicsDynamicIntegrationLogic();
    ~PhysicsDynamicIntegrationLogic() override;
    PhysicsDynamicIntegrationLogic(const PhysicsDynamicIntegrationLogic& Other);
    PhysicsDynamicIntegrationLogic& operator=(const PhysicsDynamicIntegrationLogic& Other);
    PhysicsDynamicIntegrationLogic(PhysicsDynamicIntegrationLogic&& Other) noexcept;
    PhysicsDynamicIntegrationLogic& operator=(PhysicsDynamicIntegrationLogic&& Other) noexcept;

public:
    void Execute(IPhysicsWorldMediator& WorldMediator, float DeltaTime) override;

private:
    void IntegrateActor(IPhysicsWorldMediator& WorldMediator, PhysicsDynamicActor& DynamicActor, float DeltaTime) const;
};
