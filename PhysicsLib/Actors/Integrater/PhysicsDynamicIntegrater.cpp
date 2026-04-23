#include <algorithm>
#include <utility>

#include "PhysicsLib/Actors/Integrater/PhysicsDynamicIntegrater.h"
#include "PhysicsLib/Actors/PhysicsActorBase.h"
#include "PhysicsLib/Simulation/Mediator/IPhysicsWorldMediator.h"

#undef max
#undef min

namespace {
constexpr float AngularVelocityEpsilon{ 0.000001F };

DirectX::SimpleMath::Quaternion IntegrateOrientation(const DirectX::SimpleMath::Quaternion& Orientation, const DirectX::SimpleMath::Vector3& AngularVelocity, float DeltaTime) {
    DirectX::SimpleMath::Quaternion AngularVelocityQuaternion{ AngularVelocity.x, AngularVelocity.y, AngularVelocity.z, 0.0F };
    DirectX::SimpleMath::Quaternion OrientationDelta{ AngularVelocityQuaternion * Orientation };
    float Scale{ 0.5F * DeltaTime };
    DirectX::SimpleMath::Quaternion NextOrientation{ Orientation.x + (OrientationDelta.x * Scale), Orientation.y + (OrientationDelta.y * Scale), Orientation.z + (OrientationDelta.z * Scale), Orientation.w + (OrientationDelta.w * Scale) };
    if (NextOrientation.LengthSquared() <= 0.0F) {
        NextOrientation = DirectX::SimpleMath::Quaternion{ 0.0F, 0.0F, 0.0F, 1.0F };
    } else {
        NextOrientation.Normalize();
    }

    return NextOrientation;
}
}

PhysicsDynamicIntegrater::PhysicsDynamicIntegrater() {
}

PhysicsDynamicIntegrater::~PhysicsDynamicIntegrater() {
}

PhysicsDynamicIntegrater::PhysicsDynamicIntegrater(const PhysicsDynamicIntegrater& Other)
    : IPhysicsIntegrater{ Other } {
}

PhysicsDynamicIntegrater& PhysicsDynamicIntegrater::operator=(const PhysicsDynamicIntegrater& Other) {
    if (this == &Other) {
        return *this;
    }

    IPhysicsIntegrater::operator=(Other);
    return *this;
}

PhysicsDynamicIntegrater::PhysicsDynamicIntegrater(PhysicsDynamicIntegrater&& Other) noexcept
    : IPhysicsIntegrater{ std::move(Other) } {
}

PhysicsDynamicIntegrater& PhysicsDynamicIntegrater::operator=(PhysicsDynamicIntegrater&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    IPhysicsIntegrater::operator=(std::move(Other));
    return *this;
}

void PhysicsDynamicIntegrater::Integrate(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const {
    if (Actor.GetActorType() != PhysicsActorBase::PhysicsActorType::Dynamic) {
        return;
    }

    if (!Actor.GetIsActive() || Actor.GetInverseMass() <= 0.0F || Actor.GetIsSleeping()) {
        return;
    }

    float ActorMass{ Actor.GetMass() };
    float ActorInverseMass{ Actor.GetInverseMass() };
    DirectX::SimpleMath::Vector3 TotalAcceleration{ WorldMediator.GetGravity() + Actor.GetAcceleration() };
    DirectX::SimpleMath::Vector3 TotalForce{ (TotalAcceleration * ActorMass) + Actor.GetAccumulatedForce() };
    DirectX::SimpleMath::Vector3 NextLinearMomentum{ Actor.GetLinearMomentum() + (TotalForce * DeltaTime) };
    DirectX::SimpleMath::Vector3 NextVelocity{ NextLinearMomentum * ActorInverseMass };
    float DampingFactor{ std::max(0.0F, 1.0F - (Actor.GetLinearDamping() * DeltaTime)) };
    NextVelocity *= DampingFactor;
    NextLinearMomentum = NextVelocity * ActorMass;

    DirectX::SimpleMath::Vector3 NextPosition{ Actor.GetPosition() + (NextVelocity * DeltaTime) };
    Actor.SetPosition(NextPosition);
    Actor.SetVelocity(NextVelocity);
    Actor.SetLinearMomentum(NextLinearMomentum);

    Actor.UpdateInverseInertiaTensorWorld();
    DirectX::SimpleMath::Vector3 NextAngularMomentum{ Actor.GetAngularMomentum() + (Actor.GetTorque() * DeltaTime) };
    float AngularDampingFactor{ std::max(0.0F, 1.0F - (Actor.GetAngularDamping() * DeltaTime)) };
    NextAngularMomentum *= AngularDampingFactor;
    Actor.SetAngularMomentum(NextAngularMomentum);
    DirectX::SimpleMath::Vector3 NextAngularVelocity{ Actor.GetAngularVelocity() };
    if (NextAngularVelocity.LengthSquared() > (AngularVelocityEpsilon * AngularVelocityEpsilon)) {
        DirectX::SimpleMath::Quaternion NextOrientation{ IntegrateOrientation(Actor.GetOrientation(), NextAngularVelocity, DeltaTime) };
        Actor.SetOrientation(NextOrientation);
    }

    Actor.ClearAccumulatedForce();
    Actor.ClearTorque();
    Actor.UpdateSleepState();
}
