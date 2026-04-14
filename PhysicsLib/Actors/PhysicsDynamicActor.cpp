#include <algorithm>
#include <cmath>
#include <utility>

#include "PhysicsLib/Actors/PhysicsDynamicActor.h"

#undef max 

namespace {
    DirectX::BoundingOrientedBox MakeDefaultBoundingOrientedBox() {
        DirectX::BoundingOrientedBox BoundingBoxValue{};
        BoundingBoxValue.Center = DirectX::XMFLOAT3{ 0.0F, 0.0F, 0.0F };
        BoundingBoxValue.Extents = DirectX::XMFLOAT3{ 0.5F, 0.5F, 0.5F };
        BoundingBoxValue.Orientation = DirectX::XMFLOAT4{ 0.0F, 0.0F, 0.0F, 1.0F };
        return BoundingBoxValue;
    }
}

PhysicsDynamicActor::PhysicsDynamicActor()
    : PhysicsActor{},
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
    SetActorType(PhysicsActorType::Dynamic);
    SetLinearMomentum(mVelocity * GetMass());
    SetAngularMomentum(DirectX::SimpleMath::Vector3{});
    UpdateWorldBoundingBox();
}

PhysicsDynamicActor::~PhysicsDynamicActor() {
}

PhysicsDynamicActor::PhysicsDynamicActor(const PhysicsDynamicActor& Other)
    : PhysicsActor{ Other },
      mLocalBoundingBox{ Other.mLocalBoundingBox },
      mWorldBoundingBox{ Other.mWorldBoundingBox },
      mFatWorldBoundingBox{ Other.mFatWorldBoundingBox },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale },
      mVelocity{ Other.mVelocity },
      mAcceleration{ Other.mAcceleration },
      mAccumulatedForce{ Other.mAccumulatedForce },
      mRestitution{ Other.mRestitution },
      mLinearDamping{ Other.mLinearDamping },
      mAngularDamping{ Other.mAngularDamping },
      mIsSleeping{ Other.mIsSleeping },
      mSleepThreshold{ Other.mSleepThreshold },
      mBoundingBoxFatMargin{ Other.mBoundingBoxFatMargin } {
}

PhysicsDynamicActor& PhysicsDynamicActor::operator=(const PhysicsDynamicActor& Other) {
    if (this == &Other) {
        return *this;
    }

    PhysicsActor::operator=(Other);
    mLocalBoundingBox = Other.mLocalBoundingBox;
    mWorldBoundingBox = Other.mWorldBoundingBox;
    mFatWorldBoundingBox = Other.mFatWorldBoundingBox;
    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;
    mVelocity = Other.mVelocity;
    mAcceleration = Other.mAcceleration;
    mAccumulatedForce = Other.mAccumulatedForce;
    mRestitution = Other.mRestitution;
    mLinearDamping = Other.mLinearDamping;
    mAngularDamping = Other.mAngularDamping;
    mIsSleeping = Other.mIsSleeping;
    mSleepThreshold = Other.mSleepThreshold;
    mBoundingBoxFatMargin = Other.mBoundingBoxFatMargin;

    return *this;
}

PhysicsDynamicActor::PhysicsDynamicActor(PhysicsDynamicActor&& Other) noexcept
    : PhysicsActor{ std::move(Other) },
      mLocalBoundingBox{ Other.mLocalBoundingBox },
      mWorldBoundingBox{ Other.mWorldBoundingBox },
      mFatWorldBoundingBox{ Other.mFatWorldBoundingBox },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale },
      mVelocity{ Other.mVelocity },
      mAcceleration{ Other.mAcceleration },
      mAccumulatedForce{ Other.mAccumulatedForce },
      mRestitution{ Other.mRestitution },
      mLinearDamping{ Other.mLinearDamping },
      mAngularDamping{ Other.mAngularDamping },
      mIsSleeping{ Other.mIsSleeping },
      mSleepThreshold{ Other.mSleepThreshold },
      mBoundingBoxFatMargin{ Other.mBoundingBoxFatMargin } {
}

PhysicsDynamicActor& PhysicsDynamicActor::operator=(PhysicsDynamicActor&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    PhysicsActor::operator=(std::move(Other));
    mLocalBoundingBox = Other.mLocalBoundingBox;
    mWorldBoundingBox = Other.mWorldBoundingBox;
    mFatWorldBoundingBox = Other.mFatWorldBoundingBox;
    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;
    mVelocity = Other.mVelocity;
    mAcceleration = Other.mAcceleration;
    mAccumulatedForce = Other.mAccumulatedForce;
    mRestitution = Other.mRestitution;
    mLinearDamping = Other.mLinearDamping;
    mAngularDamping = Other.mAngularDamping;
    mIsSleeping = Other.mIsSleeping;
    mSleepThreshold = Other.mSleepThreshold;
    mBoundingBoxFatMargin = Other.mBoundingBoxFatMargin;

    return *this;
}

PhysicsDynamicActor::PhysicsDynamicActor(std::string Name)
    : PhysicsActor{ std::move(Name) },
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
    SetActorType(PhysicsActorType::Dynamic);
    SetLinearMomentum(mVelocity * GetMass());
    SetAngularMomentum(DirectX::SimpleMath::Vector3{});
    UpdateWorldBoundingBox();
}

PhysicsDynamicActor::PhysicsDynamicActor(const ActorDesc& Desc)
    : PhysicsActor{ Desc.Name },
      mLocalBoundingBox{ Desc.LocalBoundingBox },
      mWorldBoundingBox{ Desc.LocalBoundingBox },
      mFatWorldBoundingBox{ Desc.LocalBoundingBox },
      mPosition{ Desc.Position },
      mRotation{ Desc.Rotation },
      mScale{ Desc.Scale },
      mVelocity{ Desc.Velocity },
      mAcceleration{ Desc.Acceleration },
      mAccumulatedForce{},
      mRestitution{ Desc.Restitution },
      mLinearDamping{ Desc.LinearDamping },
      mAngularDamping{ Desc.AngularDamping },
      mIsSleeping{ Desc.IsSleeping },
      mSleepThreshold{ Desc.SleepThreshold },
      mBoundingBoxFatMargin{ Desc.BoundingBoxFatMargin } {
    SetIsActive(Desc.IsActive);
    SetMass(Desc.Mass);
    SetFriction(Desc.Friction);
    SetFlags(Desc.Flags);
    SetActorType(PhysicsActorType::Dynamic);
    SetLinearMomentum(mVelocity * GetMass());
    SetAngularMomentum(DirectX::SimpleMath::Vector3{});
    UpdateWorldBoundingBox();
}

void PhysicsDynamicActor::SetLocalBoundingBox(const DirectX::BoundingOrientedBox& LocalBoundingBox) {
    mLocalBoundingBox = LocalBoundingBox;
    UpdateWorldBoundingBox();
}

const DirectX::BoundingOrientedBox& PhysicsDynamicActor::GetLocalBoundingBox() const {
    return mLocalBoundingBox;
}

const DirectX::BoundingOrientedBox& PhysicsDynamicActor::GetWorldBoundingBox() const {
    return mWorldBoundingBox;
}

const DirectX::BoundingOrientedBox& PhysicsDynamicActor::GetFatWorldBoundingBox() const {
    return mFatWorldBoundingBox;
}

void PhysicsDynamicActor::SetPosition(const DirectX::SimpleMath::Vector3& Position) {
    mPosition = Position;
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsDynamicActor::GetPosition() const {
    return mPosition;
}

void PhysicsDynamicActor::SetRotation(const DirectX::SimpleMath::Vector3& Rotation) {
    mRotation = Rotation;
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsDynamicActor::GetRotation() const {
    return mRotation;
}

void PhysicsDynamicActor::SetScale(const DirectX::SimpleMath::Vector3& Scale) {
    mScale = Scale;
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsDynamicActor::GetScale() const {
    return mScale;
}

void PhysicsDynamicActor::SetVelocity(const DirectX::SimpleMath::Vector3& Velocity) {
    mVelocity = Velocity;
    SetLinearMomentum(mVelocity * GetMass());
}

const DirectX::SimpleMath::Vector3& PhysicsDynamicActor::GetVelocity() const {
    return mVelocity;
}

void PhysicsDynamicActor::SetAcceleration(const DirectX::SimpleMath::Vector3& Acceleration) {
    mAcceleration = Acceleration;
}

const DirectX::SimpleMath::Vector3& PhysicsDynamicActor::GetAcceleration() const {
    return mAcceleration;
}

void PhysicsDynamicActor::AddForce(const DirectX::SimpleMath::Vector3& Force) {
    mAccumulatedForce += Force;
    SetIsSleeping(false);
}

const DirectX::SimpleMath::Vector3& PhysicsDynamicActor::GetAccumulatedForce() const {
    return mAccumulatedForce;
}

void PhysicsDynamicActor::ClearAccumulatedForce() {
    mAccumulatedForce = DirectX::SimpleMath::Vector3{};
}

void PhysicsDynamicActor::AddImpulse(const DirectX::SimpleMath::Vector3& Impulse) {
    DirectX::SimpleMath::Vector3 NextLinearMomentum{ GetLinearMomentum() + Impulse };
    SetLinearMomentum(NextLinearMomentum);
    float InverseMass{ GetInverseMass() };
    mVelocity = InverseMass > 0.0F ? NextLinearMomentum * InverseMass : DirectX::SimpleMath::Vector3{};
    SetIsSleeping(false);
}

void PhysicsDynamicActor::SetRestitution(float Restitution) {
    mRestitution = std::clamp(Restitution, 0.0F, 1.0F);
}

float PhysicsDynamicActor::GetRestitution() const {
    return mRestitution;
}

void PhysicsDynamicActor::SetLinearDamping(float LinearDamping) {
    mLinearDamping = std::max(LinearDamping, 0.0F);
}

float PhysicsDynamicActor::GetLinearDamping() const {
    return mLinearDamping;
}

void PhysicsDynamicActor::SetAngularDamping(float AngularDamping) {
    mAngularDamping = std::max(AngularDamping, 0.0F);
}

float PhysicsDynamicActor::GetAngularDamping() const {
    return mAngularDamping;
}

void PhysicsDynamicActor::SetSleepThreshold(float SleepThreshold) {
    mSleepThreshold = std::max(SleepThreshold, 0.0F);
}

float PhysicsDynamicActor::GetSleepThreshold() const {
    return mSleepThreshold;
}

void PhysicsDynamicActor::SetBoundingBoxFatMargin(float BoundingBoxFatMargin) {
    mBoundingBoxFatMargin = std::max(BoundingBoxFatMargin, 0.0F);
    UpdateFatWorldBoundingBox();
}

float PhysicsDynamicActor::GetBoundingBoxFatMargin() const {
    return mBoundingBoxFatMargin;
}

void PhysicsDynamicActor::SetIsSleeping(bool IsSleeping) {
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

bool PhysicsDynamicActor::GetIsSleeping() const {
    return mIsSleeping;
}

void PhysicsDynamicActor::UpdateSleepState() {
    float VelocityLengthSquared{ mVelocity.LengthSquared() };
    float AccelerationLengthSquared{ mAcceleration.LengthSquared() };
    DirectX::SimpleMath::Vector3 AngularVelocity{ DirectX::SimpleMath::Vector3::Transform(GetAngularMomentum(), GetLocalInverseInertiaTensor()) };
    float AngularVelocityLengthSquared{ AngularVelocity.LengthSquared() };
    float ThresholdSquared{ mSleepThreshold * mSleepThreshold };
    bool ShouldSleep{ VelocityLengthSquared <= ThresholdSquared && AccelerationLengthSquared <= ThresholdSquared && AngularVelocityLengthSquared <= ThresholdSquared };
    SetIsSleeping(ShouldSleep);
}

void PhysicsDynamicActor::UpdateWorldBoundingBox() {
    DirectX::SimpleMath::Matrix ScalingMatrix{ DirectX::SimpleMath::Matrix::CreateScale(mScale) };
    DirectX::SimpleMath::Matrix RotationMatrix{ DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(mRotation.y, mRotation.x, mRotation.z) };
    DirectX::SimpleMath::Matrix TranslationMatrix{ DirectX::SimpleMath::Matrix::CreateTranslation(mPosition) };
    DirectX::SimpleMath::Matrix WorldMatrix{ ScalingMatrix * RotationMatrix * TranslationMatrix };
    mLocalBoundingBox.Transform(mWorldBoundingBox, WorldMatrix);
    UpdateFatWorldBoundingBox();
}

void PhysicsDynamicActor::UpdateFatWorldBoundingBox() {
    mFatWorldBoundingBox = mWorldBoundingBox;
    float VelocityBasedMargin{ std::sqrt(mVelocity.LengthSquared()) * 0.05F };
    float FinalMargin{ mBoundingBoxFatMargin + VelocityBasedMargin };
    mFatWorldBoundingBox.Extents.x += FinalMargin;
    mFatWorldBoundingBox.Extents.y += FinalMargin;
    mFatWorldBoundingBox.Extents.z += FinalMargin;
}

