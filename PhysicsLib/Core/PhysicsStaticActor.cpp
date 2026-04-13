#include <utility>

#include "PhysicsStaticActor.h"

PhysicsStaticActor::PhysicsStaticActor()
    : PhysicsActor{} {
    SetActorType(PhysicsActorType::Static);
    SetFlags(GetFlags() | PhysicsActorFlags::Static);
    SetMass(0.0F);
}

PhysicsStaticActor::~PhysicsStaticActor() {
}

PhysicsStaticActor::PhysicsStaticActor(const PhysicsStaticActor& Other)
    : PhysicsActor{ Other } {
}

PhysicsStaticActor& PhysicsStaticActor::operator=(const PhysicsStaticActor& Other) {
    if (this == &Other) {
        return *this;
    }

    PhysicsActor::operator=(Other);

    return *this;
}

PhysicsStaticActor::PhysicsStaticActor(PhysicsStaticActor&& Other) noexcept
    : PhysicsActor{ std::move(Other) } {
}

PhysicsStaticActor& PhysicsStaticActor::operator=(PhysicsStaticActor&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    PhysicsActor::operator=(std::move(Other));

    return *this;
}

bool PhysicsStaticActor::ResolveDynamicCollision(const DirectX::BoundingOrientedBox& PredictedWorldBoundingBox, float DynamicInverseMass, float DynamicFriction, float DynamicRestitution, DirectX::SimpleMath::Vector3& CorrectedPosition, DirectX::SimpleMath::Vector3& CorrectedVelocity) const {
    (void)PredictedWorldBoundingBox;
    (void)DynamicInverseMass;
    (void)DynamicFriction;
    (void)DynamicRestitution;
    (void)CorrectedPosition;
    (void)CorrectedVelocity;
    return false;
}
