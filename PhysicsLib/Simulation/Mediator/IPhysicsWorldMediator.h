#pragma once

#include <vector>

#include <SimpleMath/SimpleMath.h>

#include "PhysicsLib/Simulation/Repository/IPhysicsActorRepository.h"
#include "PhysicsLib/Simulation/SpatialQuery/IPhysicsSpatialQuery.h"
#include "PhysicsLib/Simulation/Types/PhysicsSimulationTypes.h"

class IPhysicsWorldMediator {
public:
    IPhysicsWorldMediator();
    virtual ~IPhysicsWorldMediator();
    IPhysicsWorldMediator(const IPhysicsWorldMediator& Other);
    IPhysicsWorldMediator& operator=(const IPhysicsWorldMediator& Other);
    IPhysicsWorldMediator(IPhysicsWorldMediator&& Other) noexcept;
    IPhysicsWorldMediator& operator=(IPhysicsWorldMediator&& Other) noexcept;

public:
    virtual const DirectX::SimpleMath::Vector3& GetGravity() const = 0;

    virtual IPhysicsActorRepository& GetActorRepository() = 0;
    virtual const IPhysicsActorRepository& GetActorRepository() const = 0;

    virtual IPhysicsSpatialQuery& GetSpatialQuery() = 0;
    virtual const IPhysicsSpatialQuery& GetSpatialQuery() const = 0;

    virtual void PublishEvent(PhysicsSimulationEventType EventType, const PhysicsActor* FirstActor, const PhysicsActor* SecondActor) = 0;
    virtual void ClearPublishedEvents() = 0;
    virtual const std::vector<PhysicsSimulationEvent>& GetPublishedEvents() const = 0;
};


