#include <algorithm>
#include <cmath>
#include <utility>

#include "PhysicsLib/Actors/PhysicsKinematicActor.h"

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

PhysicsKinematicActor::PhysicsKinematicActor()
    : PhysicsActor{},
      mLocalBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mWorldBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mFatWorldBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F },
      mVelocity{},
      mRestitution{ 0.1F },
      mLinearDamping{ 0.03F },
      mAngularDamping{ 0.03F },
      mIsSleeping{},
      mSleepThreshold{ 0.0F },
      mBoundingBoxFatMargin{ 0.1F } {
    SetActorType(PhysicsActorType::Kinematic);
    SetFlags(GetFlags() | PhysicsActorFlags::Kinematic);
    SetLinearMomentum(mVelocity * GetMass());
    UpdateWorldBoundingBox();
}

PhysicsKinematicActor::~PhysicsKinematicActor() {
}

PhysicsKinematicActor::PhysicsKinematicActor(const PhysicsKinematicActor& Other)
    : PhysicsActor{ Other },
      mLocalBoundingBox{ Other.mLocalBoundingBox },
      mWorldBoundingBox{ Other.mWorldBoundingBox },
      mFatWorldBoundingBox{ Other.mFatWorldBoundingBox },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale },
      mVelocity{ Other.mVelocity },
      mRestitution{ Other.mRestitution },
      mLinearDamping{ Other.mLinearDamping },
      mAngularDamping{ Other.mAngularDamping },
      mIsSleeping{ Other.mIsSleeping },
      mSleepThreshold{ Other.mSleepThreshold },
      mBoundingBoxFatMargin{ Other.mBoundingBoxFatMargin } {
}

PhysicsKinematicActor& PhysicsKinematicActor::operator=(const PhysicsKinematicActor& Other) {
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
    mRestitution = Other.mRestitution;
    mLinearDamping = Other.mLinearDamping;
    mAngularDamping = Other.mAngularDamping;
    mIsSleeping = Other.mIsSleeping;
    mSleepThreshold = Other.mSleepThreshold;
    mBoundingBoxFatMargin = Other.mBoundingBoxFatMargin;

    return *this;
}

PhysicsKinematicActor::PhysicsKinematicActor(PhysicsKinematicActor&& Other) noexcept
    : PhysicsActor{ std::move(Other) },
      mLocalBoundingBox{ Other.mLocalBoundingBox },
      mWorldBoundingBox{ Other.mWorldBoundingBox },
      mFatWorldBoundingBox{ Other.mFatWorldBoundingBox },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale },
      mVelocity{ Other.mVelocity },
      mRestitution{ Other.mRestitution },
      mLinearDamping{ Other.mLinearDamping },
      mAngularDamping{ Other.mAngularDamping },
      mIsSleeping{ Other.mIsSleeping },
      mSleepThreshold{ Other.mSleepThreshold },
      mBoundingBoxFatMargin{ Other.mBoundingBoxFatMargin } {
}

PhysicsKinematicActor& PhysicsKinematicActor::operator=(PhysicsKinematicActor&& Other) noexcept {
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
    mRestitution = Other.mRestitution;
    mLinearDamping = Other.mLinearDamping;
    mAngularDamping = Other.mAngularDamping;
    mIsSleeping = Other.mIsSleeping;
    mSleepThreshold = Other.mSleepThreshold;
    mBoundingBoxFatMargin = Other.mBoundingBoxFatMargin;

    return *this;
}

PhysicsKinematicActor::PhysicsKinematicActor(std::string Name)
    : PhysicsActor{ std::move(Name) },
      mLocalBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mWorldBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mFatWorldBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F },
      mVelocity{},
      mRestitution{ 0.1F },
      mLinearDamping{ 0.03F },
      mAngularDamping{ 0.03F },
      mIsSleeping{},
      mSleepThreshold{ 0.0F },
      mBoundingBoxFatMargin{ 0.1F } {
    SetActorType(PhysicsActorType::Kinematic);
    SetFlags(GetFlags() | PhysicsActorFlags::Kinematic);
    SetLinearMomentum(mVelocity * GetMass());
    UpdateWorldBoundingBox();
}

PhysicsKinematicActor::PhysicsKinematicActor(const ActorDesc& Desc)
    : PhysicsActor{ Desc.Name },
      mLocalBoundingBox{ Desc.LocalBoundingBox },
      mWorldBoundingBox{ Desc.LocalBoundingBox },
      mFatWorldBoundingBox{ Desc.LocalBoundingBox },
      mPosition{ Desc.Position },
      mRotation{ Desc.Rotation },
      mScale{ Desc.Scale },
      mVelocity{ Desc.Velocity },
      mRestitution{ Desc.Restitution },
      mLinearDamping{ Desc.LinearDamping },
      mAngularDamping{ Desc.AngularDamping },
      mIsSleeping{ Desc.IsSleeping },
      mSleepThreshold{ Desc.SleepThreshold },
      mBoundingBoxFatMargin{ Desc.BoundingBoxFatMargin } {
    SetIsActive(Desc.IsActive);
    SetMass(Desc.Mass);
    SetFriction(Desc.Friction);
    SetFlags(Desc.Flags | PhysicsActorFlags::Kinematic);
    SetActorType(PhysicsActorType::Kinematic);
    SetLinearMomentum(mVelocity * GetMass());
    UpdateWorldBoundingBox();
}

void PhysicsKinematicActor::SetLocalBoundingBox(const DirectX::BoundingOrientedBox& LocalBoundingBox) {
    mLocalBoundingBox = LocalBoundingBox;
    UpdateWorldBoundingBox();
}

const DirectX::BoundingOrientedBox& PhysicsKinematicActor::GetLocalBoundingBox() const {
    return mLocalBoundingBox;
}

const DirectX::BoundingOrientedBox& PhysicsKinematicActor::GetWorldBoundingBox() const {
    return mWorldBoundingBox;
}

const DirectX::BoundingOrientedBox& PhysicsKinematicActor::GetFatWorldBoundingBox() const {
    return mFatWorldBoundingBox;
}

void PhysicsKinematicActor::SetPosition(const DirectX::SimpleMath::Vector3& Position) {
    mPosition = Position;
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsKinematicActor::GetPosition() const {
    return mPosition;
}

void PhysicsKinematicActor::SetRotation(const DirectX::SimpleMath::Vector3& Rotation) {
    mRotation = Rotation;
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsKinematicActor::GetRotation() const {
    return mRotation;
}

void PhysicsKinematicActor::SetScale(const DirectX::SimpleMath::Vector3& Scale) {
    mScale = Scale;
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsKinematicActor::GetScale() const {
    return mScale;
}

void PhysicsKinematicActor::SetVelocity(const DirectX::SimpleMath::Vector3& Velocity) {
    mVelocity = Velocity;
    SetLinearMomentum(mVelocity * GetMass());
}

const DirectX::SimpleMath::Vector3& PhysicsKinematicActor::GetVelocity() const {
    return mVelocity;
}

void PhysicsKinematicActor::SetRestitution(float Restitution) {
    mRestitution = std::clamp(Restitution, 0.0F, 1.0F);
}

float PhysicsKinematicActor::GetRestitution() const {
    return mRestitution;
}

void PhysicsKinematicActor::SetLinearDamping(float LinearDamping) {
    mLinearDamping = std::max(LinearDamping, 0.0F);
}

float PhysicsKinematicActor::GetLinearDamping() const {
    return mLinearDamping;
}

void PhysicsKinematicActor::SetAngularDamping(float AngularDamping) {
    mAngularDamping = std::max(AngularDamping, 0.0F);
}

float PhysicsKinematicActor::GetAngularDamping() const {
    return mAngularDamping;
}

void PhysicsKinematicActor::SetSleepThreshold(float SleepThreshold) {
    mSleepThreshold = std::max(SleepThreshold, 0.0F);
}

float PhysicsKinematicActor::GetSleepThreshold() const {
    return mSleepThreshold;
}

void PhysicsKinematicActor::SetBoundingBoxFatMargin(float BoundingBoxFatMargin) {
    mBoundingBoxFatMargin = std::max(BoundingBoxFatMargin, 0.0F);
    UpdateFatWorldBoundingBox();
}

float PhysicsKinematicActor::GetBoundingBoxFatMargin() const {
    return mBoundingBoxFatMargin;
}

void PhysicsKinematicActor::MoveToTarget(const DirectX::SimpleMath::Vector3& TargetPosition, float DeltaTime) {
    if (DeltaTime <= 0.0F) {
        return;
    }

    mVelocity = (TargetPosition - mPosition) / DeltaTime;
    SetLinearMomentum(mVelocity * GetMass());
    mPosition = TargetPosition;
    UpdateWorldBoundingBox();
}

void PhysicsKinematicActor::UpdateWorldBoundingBox() {
    DirectX::SimpleMath::Matrix ScalingMatrix{ DirectX::SimpleMath::Matrix::CreateScale(mScale) };
    DirectX::SimpleMath::Matrix RotationMatrix{ DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(mRotation.y, mRotation.x, mRotation.z) };
    DirectX::SimpleMath::Matrix TranslationMatrix{ DirectX::SimpleMath::Matrix::CreateTranslation(mPosition) };
    DirectX::SimpleMath::Matrix WorldMatrix{ ScalingMatrix * RotationMatrix * TranslationMatrix };
    mLocalBoundingBox.Transform(mWorldBoundingBox, WorldMatrix);
    UpdateFatWorldBoundingBox();
}

void PhysicsKinematicActor::UpdateFatWorldBoundingBox() {
    mFatWorldBoundingBox = mWorldBoundingBox;
    float VelocityBasedMargin{ std::sqrt(mVelocity.LengthSquared()) * 0.05F };
    float FinalMargin{ mBoundingBoxFatMargin + VelocityBasedMargin };
    mFatWorldBoundingBox.Extents.x += FinalMargin;
    mFatWorldBoundingBox.Extents.y += FinalMargin;
    mFatWorldBoundingBox.Extents.z += FinalMargin;
}

