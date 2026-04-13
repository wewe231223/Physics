#pragma once

#include <SimpleMath/SimpleMath.h>
#include <DirectXCollision.h>

#include "PhysicsActor.h"

class PhysicsStaticActor : public PhysicsActor {
public:
    PhysicsStaticActor();
    ~PhysicsStaticActor() override;
    PhysicsStaticActor(const PhysicsStaticActor& Other);
    PhysicsStaticActor& operator=(const PhysicsStaticActor& Other);
    PhysicsStaticActor(PhysicsStaticActor&& Other) noexcept;
    PhysicsStaticActor& operator=(PhysicsStaticActor&& Other) noexcept;

public:
    virtual bool ResolveDynamicCollision(const DirectX::BoundingOrientedBox& PredictedWorldBoundingBox, float DynamicInverseMass, float DynamicFriction, float DynamicRestitution, DirectX::SimpleMath::Vector3& CorrectedPosition, DirectX::SimpleMath::Vector3& CorrectedVelocity) const;
};
