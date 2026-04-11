#include <utility>
#include "PhysicsActor.h"


PhysicsActor::PhysicsActor()
    : mName{ "PhysicsActor" },
      mIsActive{ true },
      mMass{ 1.0F },
      mFlags{ PhysicsActorFlags::None },
      mBoundingBox{ DirectX::SimpleMath::Vector3{ -0.5F, -0.5F, -0.5F }, DirectX::SimpleMath::Vector3{ 0.5F, 0.5F, 0.5F } },
      mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F } {
}

PhysicsActor::~PhysicsActor() {
}

PhysicsActor::PhysicsActor(const PhysicsActor& Other)
    : mName{ Other.mName },
      mIsActive{ Other.mIsActive },
      mMass{ Other.mMass },
      mFlags{ Other.mFlags },
      mBoundingBox{ Other.mBoundingBox },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale } {
}

PhysicsActor& PhysicsActor::operator=(const PhysicsActor& Other) {
    if (this == &Other) {
        return *this;
    }

    mName = Other.mName;
    mIsActive = Other.mIsActive;
    mMass = Other.mMass;
    mFlags = Other.mFlags;
    mBoundingBox = Other.mBoundingBox;
    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;

    return *this;
}

PhysicsActor::PhysicsActor(PhysicsActor&& Other) noexcept
    : mName{ std::move(Other.mName) },
      mIsActive{ Other.mIsActive },
      mMass{ Other.mMass },
      mFlags{ Other.mFlags },
      mBoundingBox{ Other.mBoundingBox },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale } {
    Other.mName = "";
    Other.mIsActive = false;
    Other.mMass = 0.0F;
    Other.mFlags = PhysicsActorFlags::None;
    Other.mBoundingBox = BoundingBox{ DirectX::SimpleMath::Vector3{}, DirectX::SimpleMath::Vector3{} };
    Other.mPosition = DirectX::SimpleMath::Vector3{};
    Other.mRotation = DirectX::SimpleMath::Vector3{};
    Other.mScale = DirectX::SimpleMath::Vector3{};
}

PhysicsActor& PhysicsActor::operator=(PhysicsActor&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mName = std::move(Other.mName);
    mIsActive = Other.mIsActive;
    mMass = Other.mMass;
    mFlags = Other.mFlags;
    mBoundingBox = Other.mBoundingBox;
    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;

    Other.mName = "";
    Other.mIsActive = false;
    Other.mMass = 0.0F;
    Other.mFlags = PhysicsActorFlags::None;
    Other.mBoundingBox = BoundingBox{ DirectX::SimpleMath::Vector3{}, DirectX::SimpleMath::Vector3{} };
    Other.mPosition = DirectX::SimpleMath::Vector3{};
    Other.mRotation = DirectX::SimpleMath::Vector3{};
    Other.mScale = DirectX::SimpleMath::Vector3{};

    return *this;
}

PhysicsActor::PhysicsActor(std::string Name)
    : mName{ std::move(Name) },
      mIsActive{ true },
      mMass{ 1.0F },
      mFlags{ PhysicsActorFlags::None },
      mBoundingBox{ DirectX::SimpleMath::Vector3{ -0.5F, -0.5F, -0.5F }, DirectX::SimpleMath::Vector3{ 0.5F, 0.5F, 0.5F } },
      mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F } {
}

PhysicsActor::PhysicsActor(const ActorDesc& Desc)
    : mName{ Desc.Name },
      mIsActive{ Desc.IsActive },
      mMass{ Desc.Mass },
      mFlags{ Desc.Flags },
      mBoundingBox{ Desc.BoundingBoxValue },
      mPosition{ Desc.Position },
      mRotation{ Desc.Rotation },
      mScale{ Desc.Scale } {
}

void PhysicsActor::SetName(std::string Name) {
    mName = std::move(Name);
}

const std::string& PhysicsActor::GetName() const {
    return mName;
}

void PhysicsActor::SetIsActive(bool IsActive) {
    mIsActive = IsActive;
}

bool PhysicsActor::GetIsActive() const {
    return mIsActive;
}

void PhysicsActor::SetMass(float Mass) {
    mMass = Mass;
}

float PhysicsActor::GetMass() const {
    return mMass;
}

void PhysicsActor::SetFlags(PhysicsActorFlags Flags) {
    mFlags = Flags;
}

PhysicsActor::PhysicsActorFlags PhysicsActor::GetFlags() const {
    return mFlags;
}

bool PhysicsActor::HasFlag(PhysicsActorFlags Flag) const {
    PhysicsActorFlags BitAndResult{ mFlags & Flag };
    return BitAndResult != PhysicsActorFlags::None;
}

void PhysicsActor::SetBoundingBox(const BoundingBox& Box) {
    mBoundingBox = Box;
}

const PhysicsActor::BoundingBox& PhysicsActor::GetBoundingBox() const {
    return mBoundingBox;
}

void PhysicsActor::SetPosition(const DirectX::SimpleMath::Vector3& Position) {
    mPosition = Position;
}

const DirectX::SimpleMath::Vector3& PhysicsActor::GetPosition() const {
    return mPosition;
}

void PhysicsActor::SetRotation(const DirectX::SimpleMath::Vector3& Rotation) {
    mRotation = Rotation;
}

const DirectX::SimpleMath::Vector3& PhysicsActor::GetRotation() const {
    return mRotation;
}

void PhysicsActor::SetScale(const DirectX::SimpleMath::Vector3& Scale) {
    mScale = Scale;
}

const DirectX::SimpleMath::Vector3& PhysicsActor::GetScale() const {
    return mScale;
}

PhysicsActor::PhysicsActorFlags operator|(PhysicsActor::PhysicsActorFlags Left, PhysicsActor::PhysicsActorFlags Right) {
    std::uint32_t LeftValue{ static_cast<std::uint32_t>(Left) };
    std::uint32_t RightValue{ static_cast<std::uint32_t>(Right) };
    PhysicsActor::PhysicsActorFlags Combined{ static_cast<PhysicsActor::PhysicsActorFlags>(LeftValue | RightValue) };
    return Combined;
}

PhysicsActor::PhysicsActorFlags operator&(PhysicsActor::PhysicsActorFlags Left, PhysicsActor::PhysicsActorFlags Right) {
    std::uint32_t LeftValue{ static_cast<std::uint32_t>(Left) };
    std::uint32_t RightValue{ static_cast<std::uint32_t>(Right) };
    PhysicsActor::PhysicsActorFlags Intersected{ static_cast<PhysicsActor::PhysicsActorFlags>(LeftValue & RightValue) };
    return Intersected;
}
