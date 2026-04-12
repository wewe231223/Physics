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

bool PhysicsStaticActor::ResolveDynamicCollision(const DirectX::BoundingOrientedBox& PredictedWorldBoundingBox, DirectX::SimpleMath::Vector3& CorrectedPosition, DirectX::SimpleMath::Vector3& CorrectedVelocity) const {
    (void)PredictedWorldBoundingBox;
    (void)CorrectedPosition;
    (void)CorrectedVelocity;
    return false;
}
