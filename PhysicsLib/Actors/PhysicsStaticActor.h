#pragma once

#include <SimpleMath/SimpleMath.h>
#include <DirectXCollision.h>

#include "PhysicsLib/Actors/PhysicsActor.h"
#include "PhysicsLib/Actors/PhysicsDynamicActor.h"

class PhysicsStaticActor : public PhysicsActor {
public:
    PhysicsStaticActor();
    ~PhysicsStaticActor() override;
    PhysicsStaticActor(const PhysicsStaticActor& Other);
    PhysicsStaticActor& operator=(const PhysicsStaticActor& Other);
    PhysicsStaticActor(PhysicsStaticActor&& Other) noexcept;
    PhysicsStaticActor& operator=(PhysicsStaticActor&& Other) noexcept;

public:
    virtual bool ResolveDynamicCollision(PhysicsDynamicActor& DynamicActor) const;
};

