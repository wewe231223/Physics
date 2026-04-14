#pragma once

#include "IPhysicsSimulationLogic.h"
#include "../Types/PhysicsSimulationTypes.h"

class PhysicsDynamicActor;
class IPhysicsWorldMediator;

class PhysicsDynamicCollisionLogic final : public IPhysicsSimulationLogic {
public:
    PhysicsDynamicCollisionLogic();
    ~PhysicsDynamicCollisionLogic() override;
    PhysicsDynamicCollisionLogic(const PhysicsDynamicCollisionLogic& Other);
    PhysicsDynamicCollisionLogic& operator=(const PhysicsDynamicCollisionLogic& Other);
    PhysicsDynamicCollisionLogic(PhysicsDynamicCollisionLogic&& Other) noexcept;
    PhysicsDynamicCollisionLogic& operator=(PhysicsDynamicCollisionLogic&& Other) noexcept;

public:
    void Execute(IPhysicsWorldMediator& WorldMediator, float DeltaTime) override;

private:
    bool ResolveCollisionPair(IPhysicsWorldMediator& WorldMediator, PhysicsDynamicActor& FirstActor, PhysicsDynamicActor& SecondActor) const;
};
