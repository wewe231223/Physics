#pragma once

#include <vector>

#include "IPhysicsSimulationLogic.h"
#include "PhysicsLib/Actors/PhysicsDynamicActor.h"
#include "PhysicsLib/Actors/PhysicsStaticActor.h"
#include "PhysicsLib/Simulation/Types/PhysicsSimulationTypes.h"

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
    void ResolveDynamicCollisions(IPhysicsWorldMediator& WorldMediator, const std::vector<PhysicsDynamicCollisionPairCandidate>& PairCandidates, float DeltaTime) const;
    void ResolveStaticCollisions(IPhysicsWorldMediator& WorldMediator, const std::vector<PhysicsDynamicActor*>& DynamicActors, const std::vector<const PhysicsStaticActor*>& StaticActors, float DeltaTime) const;
    bool ResolveCollisionPair(IPhysicsWorldMediator& WorldMediator, PhysicsDynamicActor& FirstActor, PhysicsDynamicActor& SecondActor, float DeltaTime) const;
};
