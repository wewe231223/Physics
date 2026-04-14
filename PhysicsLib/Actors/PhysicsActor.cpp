#include <algorithm>
#include <utility>

#include "PhysicsLib/Actors/PhysicsActor.h"

#undef max
#undef min 

PhysicsActor::PhysicsActor()
    : mName{ "PhysicsActor" },
      mIsActive{ true },
      mMass{ 1.0F },
      mInverseMass{ 1.0F },
      mFriction{ 0.6F },
      mLocalInertiaTensor{ DirectX::SimpleMath::Matrix::Identity },
      mLocalInverseInertiaTensor{ DirectX::SimpleMath::Matrix::Identity },
      mLinearMomentum{},
      mAngularMomentum{},
      mFlags{ PhysicsActorFlags::None },
      mActorType{ PhysicsActorType::Dynamic } {
}

PhysicsActor::~PhysicsActor() {
}

PhysicsActor::PhysicsActor(const PhysicsActor& Other)
    : mName{ Other.mName },
      mIsActive{ Other.mIsActive },
      mMass{ Other.mMass },
      mInverseMass{ Other.mInverseMass },
      mFriction{ Other.mFriction },
      mLocalInertiaTensor{ Other.mLocalInertiaTensor },
      mLocalInverseInertiaTensor{ Other.mLocalInverseInertiaTensor },
      mLinearMomentum{ Other.mLinearMomentum },
      mAngularMomentum{ Other.mAngularMomentum },
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
    mInverseMass = Other.mInverseMass;
    mFriction = Other.mFriction;
    mLocalInertiaTensor = Other.mLocalInertiaTensor;
    mLocalInverseInertiaTensor = Other.mLocalInverseInertiaTensor;
    mLinearMomentum = Other.mLinearMomentum;
    mAngularMomentum = Other.mAngularMomentum;
    mFlags = Other.mFlags;
    mActorType = Other.mActorType;

    return *this;
}

PhysicsActor::PhysicsActor(PhysicsActor&& Other) noexcept
    : mName{ std::move(Other.mName) },
      mIsActive{ Other.mIsActive },
      mMass{ Other.mMass },
      mInverseMass{ Other.mInverseMass },
      mFriction{ Other.mFriction },
      mLocalInertiaTensor{ Other.mLocalInertiaTensor },
      mLocalInverseInertiaTensor{ Other.mLocalInverseInertiaTensor },
      mLinearMomentum{ Other.mLinearMomentum },
      mAngularMomentum{ Other.mAngularMomentum },
      mFlags{ Other.mFlags },
      mActorType{ Other.mActorType } {
    Other.mName = "";
    Other.mIsActive = false;
    Other.mMass = 0.0F;
    Other.mInverseMass = 0.0F;
    Other.mFriction = 0.6F;
    Other.mLocalInertiaTensor = DirectX::SimpleMath::Matrix::Identity;
    Other.mLocalInverseInertiaTensor = DirectX::SimpleMath::Matrix::Identity;
    Other.mLinearMomentum = DirectX::SimpleMath::Vector3{};
    Other.mAngularMomentum = DirectX::SimpleMath::Vector3{};
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
    mInverseMass = Other.mInverseMass;
    mFriction = Other.mFriction;
    mLocalInertiaTensor = Other.mLocalInertiaTensor;
    mLocalInverseInertiaTensor = Other.mLocalInverseInertiaTensor;
    mLinearMomentum = Other.mLinearMomentum;
    mAngularMomentum = Other.mAngularMomentum;
    mFlags = Other.mFlags;
    mActorType = Other.mActorType;

    Other.mName = "";
    Other.mIsActive = false;
    Other.mMass = 0.0F;
    Other.mInverseMass = 0.0F;
    Other.mFriction = 0.6F;
    Other.mLocalInertiaTensor = DirectX::SimpleMath::Matrix::Identity;
    Other.mLocalInverseInertiaTensor = DirectX::SimpleMath::Matrix::Identity;
    Other.mLinearMomentum = DirectX::SimpleMath::Vector3{};
    Other.mAngularMomentum = DirectX::SimpleMath::Vector3{};
    Other.mFlags = PhysicsActorFlags::None;
    Other.mActorType = PhysicsActorType::Dynamic;

    return *this;
}

PhysicsActor::PhysicsActor(std::string Name)
    : mName{ std::move(Name) },
      mIsActive{ true },
      mMass{ 1.0F },
      mInverseMass{ 1.0F },
      mFriction{ 0.6F },
      mLocalInertiaTensor{ DirectX::SimpleMath::Matrix::Identity },
      mLocalInverseInertiaTensor{ DirectX::SimpleMath::Matrix::Identity },
      mLinearMomentum{},
      mAngularMomentum{},
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
    mMass = std::max(0.0F, Mass);
    mInverseMass = mMass > 0.0F ? (1.0F / mMass) : 0.0F;
}

float PhysicsActor::GetMass() const {
    return mMass;
}

void PhysicsActor::SetInverseMass(float InverseMass) {
    mInverseMass = std::max(0.0F, InverseMass);
    mMass = mInverseMass > 0.0F ? (1.0F / mInverseMass) : 0.0F;
}

float PhysicsActor::GetInverseMass() const {
    return mInverseMass;
}

void PhysicsActor::SetFriction(float Friction) {
    mFriction = std::max(0.0F, Friction);
}

float PhysicsActor::GetFriction() const {
    return mFriction;
}

void PhysicsActor::SetLocalInertiaTensor(const DirectX::SimpleMath::Matrix& LocalInertiaTensor) {
    mLocalInertiaTensor = LocalInertiaTensor;
}

const DirectX::SimpleMath::Matrix& PhysicsActor::GetLocalInertiaTensor() const {
    return mLocalInertiaTensor;
}

void PhysicsActor::SetLocalInverseInertiaTensor(const DirectX::SimpleMath::Matrix& LocalInverseInertiaTensor) {
    mLocalInverseInertiaTensor = LocalInverseInertiaTensor;
}

const DirectX::SimpleMath::Matrix& PhysicsActor::GetLocalInverseInertiaTensor() const {
    return mLocalInverseInertiaTensor;
}

void PhysicsActor::SetLinearMomentum(const DirectX::SimpleMath::Vector3& LinearMomentum) {
    mLinearMomentum = LinearMomentum;
}

const DirectX::SimpleMath::Vector3& PhysicsActor::GetLinearMomentum() const {
    return mLinearMomentum;
}

void PhysicsActor::SetAngularMomentum(const DirectX::SimpleMath::Vector3& AngularMomentum) {
    mAngularMomentum = AngularMomentum;
}

const DirectX::SimpleMath::Vector3& PhysicsActor::GetAngularMomentum() const {
    return mAngularMomentum;
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

