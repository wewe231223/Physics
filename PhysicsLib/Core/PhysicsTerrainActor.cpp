#include <utility>

#include "PhysicsTerrainActor.h"

PhysicsTerrainActor::PhysicsTerrainActor()
    : PhysicsActor{},
      mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F },
      mHalfExtentX{ 0.5F },
      mHalfExtentZ{ 0.5F } {
    SetActorType(PhysicsActorType::Terrain);
    SetFlags(GetFlags() | PhysicsActorFlags::Static);
    SetMass(0.0F);
}

PhysicsTerrainActor::~PhysicsTerrainActor() {
}

PhysicsTerrainActor::PhysicsTerrainActor(const PhysicsTerrainActor& Other)
    : PhysicsActor{ Other },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale },
      mHalfExtentX{ Other.mHalfExtentX },
      mHalfExtentZ{ Other.mHalfExtentZ } {
}

PhysicsTerrainActor& PhysicsTerrainActor::operator=(const PhysicsTerrainActor& Other) {
    if (this == &Other) {
        return *this;
    }

    PhysicsActor::operator=(Other);
    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;
    mHalfExtentX = Other.mHalfExtentX;
    mHalfExtentZ = Other.mHalfExtentZ;

    return *this;
}

PhysicsTerrainActor::PhysicsTerrainActor(PhysicsTerrainActor&& Other) noexcept
    : PhysicsActor{ std::move(Other) },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale },
      mHalfExtentX{ Other.mHalfExtentX },
      mHalfExtentZ{ Other.mHalfExtentZ } {
    Other.mPosition = DirectX::SimpleMath::Vector3{};
    Other.mRotation = DirectX::SimpleMath::Vector3{};
    Other.mScale = DirectX::SimpleMath::Vector3{};
    Other.mHalfExtentX = 0.0F;
    Other.mHalfExtentZ = 0.0F;
}

PhysicsTerrainActor& PhysicsTerrainActor::operator=(PhysicsTerrainActor&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    PhysicsActor::operator=(std::move(Other));
    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;
    mHalfExtentX = Other.mHalfExtentX;
    mHalfExtentZ = Other.mHalfExtentZ;

    Other.mPosition = DirectX::SimpleMath::Vector3{};
    Other.mRotation = DirectX::SimpleMath::Vector3{};
    Other.mScale = DirectX::SimpleMath::Vector3{};
    Other.mHalfExtentX = 0.0F;
    Other.mHalfExtentZ = 0.0F;

    return *this;
}

PhysicsTerrainActor::PhysicsTerrainActor(std::string Name)
    : PhysicsActor{ std::move(Name) },
      mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F },
      mHalfExtentX{ 0.5F },
      mHalfExtentZ{ 0.5F } {
    SetActorType(PhysicsActorType::Terrain);
    SetFlags(GetFlags() | PhysicsActorFlags::Static);
    SetMass(0.0F);
}

PhysicsTerrainActor::PhysicsTerrainActor(const ActorDesc& Desc)
    : PhysicsActor{ Desc.Name },
      mPosition{ Desc.Position },
      mRotation{ Desc.Rotation },
      mScale{ Desc.Scale },
      mHalfExtentX{ Desc.HalfExtentX },
      mHalfExtentZ{ Desc.HalfExtentZ } {
    SetIsActive(Desc.IsActive);
    SetMass(0.0F);
    SetFlags(Desc.Flags | PhysicsActorFlags::Static);
    SetActorType(PhysicsActorType::Terrain);
}

void PhysicsTerrainActor::SetPosition(const DirectX::SimpleMath::Vector3& Position) {
    mPosition = Position;
}

const DirectX::SimpleMath::Vector3& PhysicsTerrainActor::GetPosition() const {
    return mPosition;
}

void PhysicsTerrainActor::SetRotation(const DirectX::SimpleMath::Vector3& Rotation) {
    mRotation = Rotation;
}

const DirectX::SimpleMath::Vector3& PhysicsTerrainActor::GetRotation() const {
    return mRotation;
}

void PhysicsTerrainActor::SetScale(const DirectX::SimpleMath::Vector3& Scale) {
    mScale = Scale;
}

const DirectX::SimpleMath::Vector3& PhysicsTerrainActor::GetScale() const {
    return mScale;
}

void PhysicsTerrainActor::SetHalfExtentX(float HalfExtentX) {
    mHalfExtentX = HalfExtentX;
}

float PhysicsTerrainActor::GetHalfExtentX() const {
    return mHalfExtentX;
}

void PhysicsTerrainActor::SetHalfExtentZ(float HalfExtentZ) {
    mHalfExtentZ = HalfExtentZ;
}

float PhysicsTerrainActor::GetHalfExtentZ() const {
    return mHalfExtentZ;
}
