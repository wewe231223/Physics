#include <utility>

#include "PhysicsActor.h"

PhysicsActor::PhysicsActor()
    : mName{ "PhysicsActor" },
      mIsActive{ true },
      mMass{ 1.0F },
      mFlags{ PhysicsActorFlags::None },
      mActorType{ PhysicsActorType::Dynamic } {
}

PhysicsActor::~PhysicsActor() {
}

PhysicsActor::PhysicsActor(const PhysicsActor& Other)
    : mName{ Other.mName },
      mIsActive{ Other.mIsActive },
      mMass{ Other.mMass },
      mFlags{ Other.mFlags },
      mActorType{ Other.mActorType } {
}

PhysicsActor& PhysicsActor::operator=(const PhysicsActor& Other) {
    if (this == &Other) {
        return *this;
    }

    mName = Other.mName;
    mIsActive = Other.mIsActive;
    mMass = Other.mMass;
    mFlags = Other.mFlags;
    mActorType = Other.mActorType;

    return *this;
}

PhysicsActor::PhysicsActor(PhysicsActor&& Other) noexcept
    : mName{ std::move(Other.mName) },
      mIsActive{ Other.mIsActive },
      mMass{ Other.mMass },
      mFlags{ Other.mFlags },
      mActorType{ Other.mActorType } {
    Other.mName = "";
    Other.mIsActive = false;
    Other.mMass = 0.0F;
    Other.mFlags = PhysicsActorFlags::None;
    Other.mActorType = PhysicsActorType::Dynamic;
}

PhysicsActor& PhysicsActor::operator=(PhysicsActor&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mName = std::move(Other.mName);
    mIsActive = Other.mIsActive;
    mMass = Other.mMass;
    mFlags = Other.mFlags;
    mActorType = Other.mActorType;

    Other.mName = "";
    Other.mIsActive = false;
    Other.mMass = 0.0F;
    Other.mFlags = PhysicsActorFlags::None;
    Other.mActorType = PhysicsActorType::Dynamic;

    return *this;
}

PhysicsActor::PhysicsActor(std::string Name)
    : mName{ std::move(Name) },
      mIsActive{ true },
      mMass{ 1.0F },
      mFlags{ PhysicsActorFlags::None },
      mActorType{ PhysicsActorType::Dynamic } {
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

void PhysicsActor::SetActorType(PhysicsActorType ActorType) {
    mActorType = ActorType;
}

PhysicsActor::PhysicsActorType PhysicsActor::GetActorType() const {
    return mActorType;
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
