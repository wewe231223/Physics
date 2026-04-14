#pragma once

#include <cstdint>

class PhysicsActor;
class PhysicsDynamicActor;

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
    const PhysicsActor* mFirstActor;
    const PhysicsActor* mSecondActor;
};
