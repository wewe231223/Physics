#include <utility>

#include "PhysicsDynamicActor.h"

namespace {
DirectX::BoundingOrientedBox MakeDefaultBoundingOrientedBox() {
    DirectX::BoundingOrientedBox BoundingBoxValue{};
    BoundingBoxValue.Center = DirectX::XMFLOAT3{ 0.0F, 0.0F, 0.0F };
    BoundingBoxValue.Extents = DirectX::XMFLOAT3{ 0.5F, 0.5F, 0.5F };
    BoundingBoxValue.Orientation = DirectX::XMFLOAT4{ 0.0F, 0.0F, 0.0F, 1.0F };
    return BoundingBoxValue;
}

DirectX::BoundingOrientedBox MakeEmptyBoundingOrientedBox() {
    DirectX::BoundingOrientedBox BoundingBoxValue{};
    BoundingBoxValue.Center = DirectX::XMFLOAT3{ 0.0F, 0.0F, 0.0F };
    BoundingBoxValue.Extents = DirectX::XMFLOAT3{ 0.0F, 0.0F, 0.0F };
    BoundingBoxValue.Orientation = DirectX::XMFLOAT4{ 0.0F, 0.0F, 0.0F, 1.0F };
    return BoundingBoxValue;
}
}

PhysicsDynamicActor::PhysicsDynamicActor()
    : PhysicsActor{},
      mBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F },
      mVelocity{} {
    SetActorType(PhysicsActorType::Dynamic);
}

PhysicsDynamicActor::~PhysicsDynamicActor() {
}

PhysicsDynamicActor::PhysicsDynamicActor(const PhysicsDynamicActor& Other)
    : PhysicsActor{ Other },
      mBoundingBox{ Other.mBoundingBox },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale },
      mVelocity{ Other.mVelocity } {
}

PhysicsDynamicActor& PhysicsDynamicActor::operator=(const PhysicsDynamicActor& Other) {
    if (this == &Other) {
        return *this;
    }

    PhysicsActor::operator=(Other);
    mBoundingBox = Other.mBoundingBox;
    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;
    mVelocity = Other.mVelocity;

    return *this;
}

PhysicsDynamicActor::PhysicsDynamicActor(PhysicsDynamicActor&& Other) noexcept
    : PhysicsActor{ std::move(Other) },
      mBoundingBox{ Other.mBoundingBox },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale },
      mVelocity{ Other.mVelocity } {
    Other.mBoundingBox = MakeEmptyBoundingOrientedBox();
    Other.mPosition = DirectX::SimpleMath::Vector3{};
    Other.mRotation = DirectX::SimpleMath::Vector3{};
    Other.mScale = DirectX::SimpleMath::Vector3{};
    Other.mVelocity = DirectX::SimpleMath::Vector3{};
}

PhysicsDynamicActor& PhysicsDynamicActor::operator=(PhysicsDynamicActor&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    PhysicsActor::operator=(std::move(Other));
    mBoundingBox = Other.mBoundingBox;
    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;
    mVelocity = Other.mVelocity;

    Other.mBoundingBox = MakeEmptyBoundingOrientedBox();
    Other.mPosition = DirectX::SimpleMath::Vector3{};
    Other.mRotation = DirectX::SimpleMath::Vector3{};
    Other.mScale = DirectX::SimpleMath::Vector3{};
    Other.mVelocity = DirectX::SimpleMath::Vector3{};

    return *this;
}

PhysicsDynamicActor::PhysicsDynamicActor(std::string Name)
    : PhysicsActor{ std::move(Name) },
      mBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F },
      mVelocity{} {
    SetActorType(PhysicsActorType::Dynamic);
}

PhysicsDynamicActor::PhysicsDynamicActor(const ActorDesc& Desc)
    : PhysicsActor{ Desc.Name },
      mBoundingBox{ Desc.BoundingBoxValue },
      mPosition{ Desc.Position },
      mRotation{ Desc.Rotation },
      mScale{ Desc.Scale },
      mVelocity{ Desc.Velocity } {
    SetIsActive(Desc.IsActive);
    SetMass(Desc.Mass);
    SetFlags(Desc.Flags);
    SetActorType(PhysicsActorType::Dynamic);
}

void PhysicsDynamicActor::SetBoundingBox(const DirectX::BoundingOrientedBox& BoundingBoxValue) {
    mBoundingBox = BoundingBoxValue;
}

const DirectX::BoundingOrientedBox& PhysicsDynamicActor::GetBoundingBox() const {
    return mBoundingBox;
}

void PhysicsDynamicActor::SetPosition(const DirectX::SimpleMath::Vector3& Position) {
    mPosition = Position;
}

const DirectX::SimpleMath::Vector3& PhysicsDynamicActor::GetPosition() const {
    return mPosition;
}

void PhysicsDynamicActor::SetRotation(const DirectX::SimpleMath::Vector3& Rotation) {
    mRotation = Rotation;
}

const DirectX::SimpleMath::Vector3& PhysicsDynamicActor::GetRotation() const {
    return mRotation;
}

void PhysicsDynamicActor::SetScale(const DirectX::SimpleMath::Vector3& Scale) {
    mScale = Scale;
}

const DirectX::SimpleMath::Vector3& PhysicsDynamicActor::GetScale() const {
    return mScale;
}

void PhysicsDynamicActor::SetVelocity(const DirectX::SimpleMath::Vector3& Velocity) {
    mVelocity = Velocity;
}

const DirectX::SimpleMath::Vector3& PhysicsDynamicActor::GetVelocity() const {
    return mVelocity;
}
