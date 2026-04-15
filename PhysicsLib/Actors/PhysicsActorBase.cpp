#include <algorithm>
#include <cmath>
#include <cstdint>
#include <utility>

#include "PhysicsLib/Actors/PhysicsActorBase.h"

#undef max
#undef min

namespace {
DirectX::BoundingOrientedBox MakeDefaultBoundingOrientedBox() {
    DirectX::BoundingOrientedBox BoundingBoxValue{};
    BoundingBoxValue.Center = DirectX::XMFLOAT3{ 0.0F, 0.0F, 0.0F };
    BoundingBoxValue.Extents = DirectX::XMFLOAT3{ 0.5F, 0.5F, 0.5F };
    BoundingBoxValue.Orientation = DirectX::XMFLOAT4{ 0.0F, 0.0F, 0.0F, 1.0F };
    return BoundingBoxValue;
}
}

PhysicsActorBase::PhysicsActorBase()
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
      mActorType{ PhysicsActorType::Dynamic },
      mLocalBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mWorldBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mFatWorldBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F },
      mVelocity{},
      mAcceleration{},
      mAccumulatedForce{},
      mRestitution{ 0.1F },
      mLinearDamping{ 0.03F },
      mAngularDamping{ 0.03F },
      mIsSleeping{},
      mSleepThreshold{ 0.05F },
      mBoundingBoxFatMargin{ 0.1F } {
    UpdateWorldBoundingBox();
}

PhysicsActorBase::~PhysicsActorBase() {
}

PhysicsActorBase::PhysicsActorBase(const PhysicsActorBase& Other) = default;

PhysicsActorBase& PhysicsActorBase::operator=(const PhysicsActorBase& Other) = default;

PhysicsActorBase::PhysicsActorBase(PhysicsActorBase&& Other) noexcept = default;

PhysicsActorBase& PhysicsActorBase::operator=(PhysicsActorBase&& Other) noexcept = default;

PhysicsActorBase::PhysicsActorBase(std::string Name)
    : PhysicsActorBase{} {
    mName = std::move(Name);
}

PhysicsActorBase::PhysicsActorBase(const ActorDesc& Desc)
    : PhysicsActorBase{} {
    mName = Desc.Name;
    mPosition = Desc.Position;
    mRotation = Desc.Rotation;
    mScale = Desc.Scale;
    mVelocity = Desc.Velocity;
    mAcceleration = Desc.Acceleration;
    mRestitution = Desc.Restitution;
    mLinearDamping = Desc.LinearDamping;
    mAngularDamping = Desc.AngularDamping;
    mSleepThreshold = Desc.SleepThreshold;
    mBoundingBoxFatMargin = Desc.BoundingBoxFatMargin;

    SetIsActive(Desc.IsActive);
    SetMass(Desc.Mass);
    SetFriction(Desc.Friction);
    SetFlags(Desc.Flags);
    SetActorType(Desc.ActorType);
    SetLocalBoundingBox(Desc.LocalBoundingBox);
    SetVelocity(Desc.Velocity);
    SetAcceleration(Desc.Acceleration);
    UpdateWorldBoundingBox();
    SetIsSleeping(Desc.IsSleeping);
}

void PhysicsActorBase::SetName(std::string Name) {
    mName = std::move(Name);
}

const std::string& PhysicsActorBase::GetName() const {
    return mName;
}

void PhysicsActorBase::SetIsActive(bool IsActive) {
    mIsActive = IsActive;
}

bool PhysicsActorBase::GetIsActive() const {
    return mIsActive;
}

void PhysicsActorBase::SetMass(float Mass) {
    mMass = std::max(0.0F, Mass);
    mInverseMass = mMass > 0.0F ? (1.0F / mMass) : 0.0F;
}

float PhysicsActorBase::GetMass() const {
    return mMass;
}

void PhysicsActorBase::SetInverseMass(float InverseMass) {
    mInverseMass = std::max(0.0F, InverseMass);
    mMass = mInverseMass > 0.0F ? (1.0F / mInverseMass) : 0.0F;
}

float PhysicsActorBase::GetInverseMass() const {
    return mInverseMass;
}

void PhysicsActorBase::SetFriction(float Friction) {
    mFriction = std::max(0.0F, Friction);
}

float PhysicsActorBase::GetFriction() const {
    return mFriction;
}

void PhysicsActorBase::SetLocalInertiaTensor(const DirectX::SimpleMath::Matrix& LocalInertiaTensor) {
    mLocalInertiaTensor = LocalInertiaTensor;
}

const DirectX::SimpleMath::Matrix& PhysicsActorBase::GetLocalInertiaTensor() const {
    return mLocalInertiaTensor;
}

void PhysicsActorBase::SetLocalInverseInertiaTensor(const DirectX::SimpleMath::Matrix& LocalInverseInertiaTensor) {
    mLocalInverseInertiaTensor = LocalInverseInertiaTensor;
}

const DirectX::SimpleMath::Matrix& PhysicsActorBase::GetLocalInverseInertiaTensor() const {
    return mLocalInverseInertiaTensor;
}

void PhysicsActorBase::SetLinearMomentum(const DirectX::SimpleMath::Vector3& LinearMomentum) {
    mLinearMomentum = LinearMomentum;
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetLinearMomentum() const {
    return mLinearMomentum;
}

void PhysicsActorBase::SetAngularMomentum(const DirectX::SimpleMath::Vector3& AngularMomentum) {
    mAngularMomentum = AngularMomentum;
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetAngularMomentum() const {
    return mAngularMomentum;
}

void PhysicsActorBase::SetFlags(PhysicsActorFlags Flags) {
    mFlags = Flags;
}

PhysicsActorBase::PhysicsActorFlags PhysicsActorBase::GetFlags() const {
    return mFlags;
}

bool PhysicsActorBase::HasFlag(PhysicsActorFlags Flag) const {
    PhysicsActorFlags BitAndResult{ mFlags & Flag };
    return BitAndResult != PhysicsActorFlags::None;
}

void PhysicsActorBase::SetActorType(PhysicsActorType ActorType) {
    mActorType = ActorType;
}

PhysicsActorBase::PhysicsActorType PhysicsActorBase::GetActorType() const {
    return mActorType;
}

void PhysicsActorBase::SetLocalBoundingBox(const DirectX::BoundingOrientedBox& LocalBoundingBox) {
    mLocalBoundingBox = LocalBoundingBox;
    UpdateWorldBoundingBox();
}

const DirectX::BoundingOrientedBox& PhysicsActorBase::GetLocalBoundingBox() const {
    return mLocalBoundingBox;
}

const DirectX::BoundingOrientedBox& PhysicsActorBase::GetWorldBoundingBox() const {
    return mWorldBoundingBox;
}

const DirectX::BoundingOrientedBox& PhysicsActorBase::GetFatWorldBoundingBox() const {
    return mFatWorldBoundingBox;
}

void PhysicsActorBase::SetPosition(const DirectX::SimpleMath::Vector3& Position) {
    mPosition = Position;
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetPosition() const {
    return mPosition;
}

void PhysicsActorBase::SetRotation(const DirectX::SimpleMath::Vector3& Rotation) {
    mRotation = Rotation;
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetRotation() const {
    return mRotation;
}

void PhysicsActorBase::SetScale(const DirectX::SimpleMath::Vector3& Scale) {
    mScale = Scale;
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetScale() const {
    return mScale;
}

void PhysicsActorBase::SetVelocity(const DirectX::SimpleMath::Vector3& Velocity) {
    mVelocity = Velocity;
    SetLinearMomentum(mVelocity * GetMass());
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetVelocity() const {
    return mVelocity;
}

void PhysicsActorBase::SetAcceleration(const DirectX::SimpleMath::Vector3& Acceleration) {
    mAcceleration = Acceleration;
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetAcceleration() const {
    return mAcceleration;
}

void PhysicsActorBase::AddForce(const DirectX::SimpleMath::Vector3& Force) {
    mAccumulatedForce += Force;
    SetIsSleeping(false);
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetAccumulatedForce() const {
    return mAccumulatedForce;
}

void PhysicsActorBase::ClearAccumulatedForce() {
    mAccumulatedForce = DirectX::SimpleMath::Vector3{};
}

void PhysicsActorBase::AddImpulse(const DirectX::SimpleMath::Vector3& Impulse) {
    DirectX::SimpleMath::Vector3 NextLinearMomentum{ GetLinearMomentum() + Impulse };
    SetLinearMomentum(NextLinearMomentum);
    float InverseMass{ GetInverseMass() };
    mVelocity = InverseMass > 0.0F ? NextLinearMomentum * InverseMass : DirectX::SimpleMath::Vector3{};
    SetIsSleeping(false);
}

void PhysicsActorBase::SetRestitution(float Restitution) {
    mRestitution = std::clamp(Restitution, 0.0F, 1.0F);
}

float PhysicsActorBase::GetRestitution() const {
    return mRestitution;
}

void PhysicsActorBase::SetLinearDamping(float LinearDamping) {
    mLinearDamping = std::max(LinearDamping, 0.0F);
}

float PhysicsActorBase::GetLinearDamping() const {
    return mLinearDamping;
}

void PhysicsActorBase::SetAngularDamping(float AngularDamping) {
    mAngularDamping = std::max(AngularDamping, 0.0F);
}

float PhysicsActorBase::GetAngularDamping() const {
    return mAngularDamping;
}

void PhysicsActorBase::SetSleepThreshold(float SleepThreshold) {
    mSleepThreshold = std::max(SleepThreshold, 0.0F);
}

float PhysicsActorBase::GetSleepThreshold() const {
    return mSleepThreshold;
}

void PhysicsActorBase::SetBoundingBoxFatMargin(float BoundingBoxFatMargin) {
    mBoundingBoxFatMargin = std::max(BoundingBoxFatMargin, 0.0F);
    UpdateFatWorldBoundingBox();
}

float PhysicsActorBase::GetBoundingBoxFatMargin() const {
    return mBoundingBoxFatMargin;
}

void PhysicsActorBase::SetIsSleeping(bool IsSleeping) {
    mIsSleeping = IsSleeping;
    if (mIsSleeping) {
        SetFlags(GetFlags() | PhysicsActorFlags::Sleeping);
        mVelocity = DirectX::SimpleMath::Vector3{};
        mAcceleration = DirectX::SimpleMath::Vector3{};
        mAccumulatedForce = DirectX::SimpleMath::Vector3{};
        SetLinearMomentum(DirectX::SimpleMath::Vector3{});
        SetAngularMomentum(DirectX::SimpleMath::Vector3{});
        return;
    }

    PhysicsActorFlags UpdatedFlags{ GetFlags() };
    std::uint32_t UpdatedValue{ static_cast<std::uint32_t>(UpdatedFlags) & ~static_cast<std::uint32_t>(PhysicsActorFlags::Sleeping) };
    SetFlags(static_cast<PhysicsActorFlags>(UpdatedValue));
}

bool PhysicsActorBase::GetIsSleeping() const {
    return mIsSleeping;
}

void PhysicsActorBase::MoveToTarget(const DirectX::SimpleMath::Vector3& TargetPosition, float DeltaTime) {
    if (DeltaTime <= 0.0F) {
        return;
    }

    DirectX::SimpleMath::Vector3 NextVelocity{ (TargetPosition - mPosition) / DeltaTime };
    SetVelocity(NextVelocity);
    SetPosition(TargetPosition);
}

void PhysicsActorBase::UpdateSleepState() {
    float VelocityLengthSquared{ mVelocity.LengthSquared() };
    float AccelerationLengthSquared{ mAcceleration.LengthSquared() };
    DirectX::SimpleMath::Vector3 AngularVelocity{ DirectX::SimpleMath::Vector3::Transform(GetAngularMomentum(), GetLocalInverseInertiaTensor()) };
    float AngularVelocityLengthSquared{ AngularVelocity.LengthSquared() };
    float ThresholdSquared{ mSleepThreshold * mSleepThreshold };
    bool ShouldSleep{ VelocityLengthSquared <= ThresholdSquared && AccelerationLengthSquared <= ThresholdSquared && AngularVelocityLengthSquared <= ThresholdSquared };
    SetIsSleeping(ShouldSleep);
}

void PhysicsActorBase::UpdateWorldBoundingBox() {
    DirectX::SimpleMath::Matrix ScalingMatrix{ DirectX::SimpleMath::Matrix::CreateScale(mScale) };
    DirectX::SimpleMath::Matrix RotationMatrix{ DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(mRotation.y, mRotation.x, mRotation.z) };
    DirectX::SimpleMath::Matrix TranslationMatrix{ DirectX::SimpleMath::Matrix::CreateTranslation(mPosition) };
    DirectX::SimpleMath::Matrix WorldMatrix{ ScalingMatrix * RotationMatrix * TranslationMatrix };
    mLocalBoundingBox.Transform(mWorldBoundingBox, WorldMatrix);
    UpdateFatWorldBoundingBox();
}

void PhysicsActorBase::UpdateFatWorldBoundingBox() {
    mFatWorldBoundingBox = mWorldBoundingBox;
    float VelocityBasedMargin{ std::sqrt(mVelocity.LengthSquared()) * 0.05F };
    float FinalMargin{ mBoundingBoxFatMargin + VelocityBasedMargin };
    mFatWorldBoundingBox.Extents.x += FinalMargin;
    mFatWorldBoundingBox.Extents.y += FinalMargin;
    mFatWorldBoundingBox.Extents.z += FinalMargin;
}

PhysicsActorBase::PhysicsActorFlags operator|(PhysicsActorBase::PhysicsActorFlags Left, PhysicsActorBase::PhysicsActorFlags Right) {
    std::uint32_t LeftValue{ static_cast<std::uint32_t>(Left) };
    std::uint32_t RightValue{ static_cast<std::uint32_t>(Right) };
    PhysicsActorBase::PhysicsActorFlags Combined{ static_cast<PhysicsActorBase::PhysicsActorFlags>(LeftValue | RightValue) };
    return Combined;
}

PhysicsActorBase::PhysicsActorFlags operator&(PhysicsActorBase::PhysicsActorFlags Left, PhysicsActorBase::PhysicsActorFlags Right) {
    std::uint32_t LeftValue{ static_cast<std::uint32_t>(Left) };
    std::uint32_t RightValue{ static_cast<std::uint32_t>(Right) };
    PhysicsActorBase::PhysicsActorFlags Intersected{ static_cast<PhysicsActorBase::PhysicsActorFlags>(LeftValue & RightValue) };
    return Intersected;
}
