#pragma once

#include <cstdint>

#include "PhysicsLib/Actors/PhysicsDynamicActor.h"
#include "PhysicsLib/Simulation/Types/RigidBody.h"

class PhysicsActorBase;

enum class PhysicsSimulationEventType : std::uint32_t {
    StaticCollisionResolved = 0U,
    DynamicCollisionResolved = 1U
};

struct PhysicsDynamicCollisionPairCandidate {
    PhysicsDynamicActor* mFirstActor;
    PhysicsDynamicActor* mSecondActor;
};

struct PhysicsSimulationEvent {
    PhysicsSimulationEventType mEventType;
    const PhysicsActorBase* mFirstActor;
    const PhysicsActorBase* mSecondActor;
};
