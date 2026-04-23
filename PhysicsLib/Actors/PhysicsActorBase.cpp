#include <algorithm>
#include <cmath>
#include <cstdint>
#include <utility>

#include "PhysicsLib/Actors/PhysicsActorBase.h"

#undef max
#undef min

namespace {
constexpr float InertiaTensorEpsilon{ 0.000001F };

DirectX::BoundingOrientedBox MakeDefaultBoundingOrientedBox() {
    DirectX::BoundingOrientedBox BoundingBoxValue{};
    BoundingBoxValue.Center = DirectX::XMFLOAT3{ 0.0F, 0.0F, 0.0F };
    BoundingBoxValue.Extents = DirectX::XMFLOAT3{ 0.5F, 0.5F, 0.5F };
    BoundingBoxValue.Orientation = DirectX::XMFLOAT4{ 0.0F, 0.0F, 0.0F, 1.0F };
    return BoundingBoxValue;
}

DirectX::SimpleMath::Matrix MakeTensorMatrix(float X, float Y, float Z) {
    DirectX::SimpleMath::Matrix TensorMatrix{ X, 0.0F, 0.0F, 0.0F, 0.0F, Y, 0.0F, 0.0F, 0.0F, 0.0F, Z, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F };
    return TensorMatrix;
}

DirectX::SimpleMath::Vector3 GetScaledHalfExtents(const DirectX::BoundingOrientedBox& BoundingBox, const DirectX::SimpleMath::Vector3& Scale) {
    DirectX::SimpleMath::Vector3 HalfExtents{ std::abs(BoundingBox.Extents.x * Scale.x), std::abs(BoundingBox.Extents.y * Scale.y), std::abs(BoundingBox.Extents.z * Scale.z) };
    return HalfExtents;
}

DirectX::SimpleMath::Matrix CalculateBoxLocalInertiaTensor(float Mass, const DirectX::SimpleMath::Vector3& HalfExtents) {
    float Width{ HalfExtents.x * 2.0F };
    float Height{ HalfExtents.y * 2.0F };
    float Depth{ HalfExtents.z * 2.0F };
    float Factor{ Mass / 12.0F };
    float InertiaX{ Factor * ((Height * Height) + (Depth * Depth)) };
    float InertiaY{ Factor * ((Width * Width) + (Depth * Depth)) };
    float InertiaZ{ Factor * ((Width * Width) + (Height * Height)) };
    DirectX::SimpleMath::Matrix TensorMatrix{ MakeTensorMatrix(InertiaX, InertiaY, InertiaZ) };
    return TensorMatrix;
}

DirectX::SimpleMath::Matrix CalculateSphereLocalInertiaTensor(float Mass, const DirectX::SimpleMath::Vector3& HalfExtents) {
    float Radius{ std::max(std::max(HalfExtents.x, HalfExtents.y), HalfExtents.z) };
    float Inertia{ 0.4F * Mass * Radius * Radius };
    DirectX::SimpleMath::Matrix TensorMatrix{ MakeTensorMatrix(Inertia, Inertia, Inertia) };
    return TensorMatrix;
}

DirectX::SimpleMath::Matrix CalculateCylinderLocalInertiaTensor(float Mass, const DirectX::SimpleMath::Vector3& HalfExtents) {
    float Radius{ std::max(HalfExtents.x, HalfExtents.z) };
    float Height{ HalfExtents.y * 2.0F };
    float RadiusSquared{ Radius * Radius };
    float HeightSquared{ Height * Height };
    float InertiaX{ (Mass / 12.0F) * ((3.0F * RadiusSquared) + HeightSquared) };
    float InertiaY{ 0.5F * Mass * RadiusSquared };
    float InertiaZ{ InertiaX };
    DirectX::SimpleMath::Matrix TensorMatrix{ MakeTensorMatrix(InertiaX, InertiaY, InertiaZ) };
    return TensorMatrix;
}

DirectX::SimpleMath::Matrix CalculateLocalInertiaTensor(PhysicsActorBase::PhysicsInertiaShapeType ShapeType, float Mass, const DirectX::BoundingOrientedBox& BoundingBox, const DirectX::SimpleMath::Vector3& Scale) {
    if (Mass <= 0.0F) {
        return MakeTensorMatrix(0.0F, 0.0F, 0.0F);
    }

    DirectX::SimpleMath::Vector3 HalfExtents{ GetScaledHalfExtents(BoundingBox, Scale) };
    if (ShapeType == PhysicsActorBase::PhysicsInertiaShapeType::Sphere) {
        return CalculateSphereLocalInertiaTensor(Mass, HalfExtents);
    }

    if (ShapeType == PhysicsActorBase::PhysicsInertiaShapeType::Cylinder) {
        return CalculateCylinderLocalInertiaTensor(Mass, HalfExtents);
    }

    return CalculateBoxLocalInertiaTensor(Mass, HalfExtents);
}

DirectX::SimpleMath::Matrix CalculateLocalInverseInertiaTensor(const DirectX::SimpleMath::Matrix& LocalInertiaTensor) {
    float InverseX{ LocalInertiaTensor._11 > InertiaTensorEpsilon ? (1.0F / LocalInertiaTensor._11) : 0.0F };
    float InverseY{ LocalInertiaTensor._22 > InertiaTensorEpsilon ? (1.0F / LocalInertiaTensor._22) : 0.0F };
    float InverseZ{ LocalInertiaTensor._33 > InertiaTensorEpsilon ? (1.0F / LocalInertiaTensor._33) : 0.0F };
    DirectX::SimpleMath::Matrix TensorMatrix{ MakeTensorMatrix(InverseX, InverseY, InverseZ) };
    return TensorMatrix;
}

DirectX::SimpleMath::Matrix CalculateWorldTensor(const DirectX::SimpleMath::Matrix& LocalTensor, const DirectX::SimpleMath::Quaternion& Orientation) {
    DirectX::SimpleMath::Quaternion NormalizedOrientation{ Orientation };
    if (NormalizedOrientation.LengthSquared() <= 0.0F) {
        NormalizedOrientation = DirectX::SimpleMath::Quaternion{ 0.0F, 0.0F, 0.0F, 1.0F };
    } else {
        NormalizedOrientation.Normalize();
    }

    DirectX::SimpleMath::Matrix RotationMatrix{ DirectX::SimpleMath::Matrix::CreateFromQuaternion(NormalizedOrientation) };
    DirectX::SimpleMath::Matrix WorldTensor{ RotationMatrix.Transpose() * LocalTensor * RotationMatrix };
    return WorldTensor;
}
}

PhysicsActorBase::PhysicsActorBase()
    : mName{ "PhysicsActor" },
      mIsActive{ true },
      mIsSleeping{},
      mSleepThreshold{ 0.05F },
      mBoundingBoxFatMargin{ 0.1F },
      mRigidBody{},
      mFlags{ PhysicsActorFlags::None },
      mActorType{ PhysicsActorType::Dynamic },
      mInertiaShapeType{ PhysicsInertiaShapeType::Box },
      mLocalBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mWorldBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mFatWorldBoundingBox{ MakeDefaultBoundingOrientedBox() } {
    RecalculateLocalInertiaTensor();
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
    mRigidBody.mPosition = Desc.Position;
    mRigidBody.mOrientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(Desc.Rotation.y, Desc.Rotation.x, Desc.Rotation.z);
    NormalizeRigidBodyOrientation();
    mRigidBody.mScale = Desc.Scale;
    mRigidBody.mVelocity = Desc.Velocity;
    mRigidBody.mAcceleration = Desc.Acceleration;
    mRigidBody.mRestitution = Desc.Restitution;
    mRigidBody.mLinearDamping = Desc.LinearDamping;
    mRigidBody.mAngularDamping = Desc.AngularDamping;
    mInertiaShapeType = Desc.mInertiaShapeType;
    mSleepThreshold = Desc.SleepThreshold;
    mBoundingBoxFatMargin = Desc.BoundingBoxFatMargin;

    SetIsActive(Desc.IsActive);
    SetFlags(Desc.Flags);
    SetActorType(Desc.ActorType);
    SetLocalBoundingBox(Desc.LocalBoundingBox);
    SetInertiaShapeType(Desc.mInertiaShapeType);
    SetMass(Desc.Mass);
    SetFriction(Desc.Friction);
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
    mRigidBody.mMass = std::max(0.0F, Mass);
    mRigidBody.mInverseMass = mRigidBody.mMass > 0.0F ? (1.0F / mRigidBody.mMass) : 0.0F;
    RecalculateLocalInertiaTensor();
}

float PhysicsActorBase::GetMass() const {
    return mRigidBody.mMass;
}

void PhysicsActorBase::SetInverseMass(float InverseMass) {
    mRigidBody.mInverseMass = std::max(0.0F, InverseMass);
    mRigidBody.mMass = mRigidBody.mInverseMass > 0.0F ? (1.0F / mRigidBody.mInverseMass) : 0.0F;
    RecalculateLocalInertiaTensor();
}

float PhysicsActorBase::GetInverseMass() const {
    return mRigidBody.mInverseMass;
}

void PhysicsActorBase::SetFriction(float Friction) {
    mRigidBody.mFriction = std::max(0.0F, Friction);
}

float PhysicsActorBase::GetFriction() const {
    return mRigidBody.mFriction;
}

void PhysicsActorBase::SetLocalInertiaTensor(const DirectX::SimpleMath::Matrix& LocalInertiaTensor) {
    mRigidBody.mLocalInertiaTensor = LocalInertiaTensor;
    if (mRigidBody.mInverseMass <= 0.0F) {
        mRigidBody.mLocalInverseInertiaTensor = MakeTensorMatrix(0.0F, 0.0F, 0.0F);
    } else {
        mRigidBody.mLocalInverseInertiaTensor = CalculateLocalInverseInertiaTensor(mRigidBody.mLocalInertiaTensor);
    }

    UpdateInverseInertiaTensorWorld();
}

const DirectX::SimpleMath::Matrix& PhysicsActorBase::GetLocalInertiaTensor() const {
    return mRigidBody.mLocalInertiaTensor;
}

void PhysicsActorBase::SetLocalInverseInertiaTensor(const DirectX::SimpleMath::Matrix& LocalInverseInertiaTensor) {
    mRigidBody.mLocalInverseInertiaTensor = mRigidBody.mInverseMass > 0.0F ? LocalInverseInertiaTensor : MakeTensorMatrix(0.0F, 0.0F, 0.0F);
    UpdateInverseInertiaTensorWorld();
}

const DirectX::SimpleMath::Matrix& PhysicsActorBase::GetLocalInverseInertiaTensor() const {
    return mRigidBody.mLocalInverseInertiaTensor;
}

void PhysicsActorBase::SetLinearMomentum(const DirectX::SimpleMath::Vector3& LinearMomentum) {
    mRigidBody.mLinearMomentum = LinearMomentum;
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetLinearMomentum() const {
    return mRigidBody.mLinearMomentum;
}

void PhysicsActorBase::SetAngularMomentum(const DirectX::SimpleMath::Vector3& AngularMomentum) {
    mRigidBody.mAngularMomentum = AngularMomentum;
    mRigidBody.mAngularVelocity = DirectX::SimpleMath::Vector3::TransformNormal(mRigidBody.mAngularMomentum, mRigidBody.mInverseInertiaTensorWorld);
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetAngularMomentum() const {
    return mRigidBody.mAngularMomentum;
}

void PhysicsActorBase::SetRigidBody(const RigidBody& RigidBodyState) {
    mRigidBody = RigidBodyState;
    mRigidBody.mMass = std::max(0.0F, mRigidBody.mMass);
    if (mRigidBody.mMass <= 0.0F) {
        mRigidBody.mInverseMass = 0.0F;
        mRigidBody.mLocalInertiaTensor = MakeTensorMatrix(0.0F, 0.0F, 0.0F);
        mRigidBody.mLocalInverseInertiaTensor = MakeTensorMatrix(0.0F, 0.0F, 0.0F);
    }

    NormalizeRigidBodyOrientation();
    UpdateInverseInertiaTensorWorld();
    UpdateWorldBoundingBox();
}

const RigidBody& PhysicsActorBase::GetRigidBody() const {
    return mRigidBody;
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

void PhysicsActorBase::SetInertiaShapeType(PhysicsInertiaShapeType InertiaShapeType) {
    mInertiaShapeType = InertiaShapeType;
    RecalculateLocalInertiaTensor();
}

PhysicsActorBase::PhysicsInertiaShapeType PhysicsActorBase::GetInertiaShapeType() const {
    return mInertiaShapeType;
}

void PhysicsActorBase::SetLocalBoundingBox(const DirectX::BoundingOrientedBox& LocalBoundingBox) {
    mLocalBoundingBox = LocalBoundingBox;
    RecalculateLocalInertiaTensor();
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
    mRigidBody.mPosition = Position;
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetPosition() const {
    return mRigidBody.mPosition;
}

void PhysicsActorBase::SetRotation(const DirectX::SimpleMath::Vector3& Rotation) {
    mRigidBody.mOrientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(Rotation.y, Rotation.x, Rotation.z);
    NormalizeRigidBodyOrientation();
    UpdateWorldBoundingBox();
}

DirectX::SimpleMath::Vector3 PhysicsActorBase::GetRotation() const {
    DirectX::SimpleMath::Quaternion NormalizedOrientation{ mRigidBody.mOrientation };
    if (NormalizedOrientation.LengthSquared() <= 0.0F) {
        NormalizedOrientation = DirectX::SimpleMath::Quaternion{ 0.0F, 0.0F, 0.0F, 1.0F };
    } else {
        NormalizedOrientation.Normalize();
    }

    DirectX::SimpleMath::Vector3 Rotation{ NormalizedOrientation.ToEuler() };
    return Rotation;
}

void PhysicsActorBase::SetOrientation(const DirectX::SimpleMath::Quaternion& Orientation) {
    mRigidBody.mOrientation = Orientation;
    NormalizeRigidBodyOrientation();
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Quaternion& PhysicsActorBase::GetOrientation() const {
    return mRigidBody.mOrientation;
}

void PhysicsActorBase::SetScale(const DirectX::SimpleMath::Vector3& Scale) {
    mRigidBody.mScale = Scale;
    RecalculateLocalInertiaTensor();
    UpdateWorldBoundingBox();
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetScale() const {
    return mRigidBody.mScale;
}

void PhysicsActorBase::SetVelocity(const DirectX::SimpleMath::Vector3& Velocity) {
    mRigidBody.mVelocity = Velocity;
    SetLinearMomentum(mRigidBody.mVelocity * GetMass());
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetVelocity() const {
    return mRigidBody.mVelocity;
}

void PhysicsActorBase::SetAcceleration(const DirectX::SimpleMath::Vector3& Acceleration) {
    mRigidBody.mAcceleration = Acceleration;
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetAcceleration() const {
    return mRigidBody.mAcceleration;
}

void PhysicsActorBase::AddForce(const DirectX::SimpleMath::Vector3& Force) {
    mRigidBody.mAccumulatedForce += Force;
    SetIsSleeping(false);
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetAccumulatedForce() const {
    return mRigidBody.mAccumulatedForce;
}

void PhysicsActorBase::ClearAccumulatedForce() {
    mRigidBody.mAccumulatedForce = DirectX::SimpleMath::Vector3{};
}

void PhysicsActorBase::AddImpulse(const DirectX::SimpleMath::Vector3& Impulse) {
    DirectX::SimpleMath::Vector3 NextLinearMomentum{ GetLinearMomentum() + Impulse };
    SetLinearMomentum(NextLinearMomentum);
    float InverseMass{ GetInverseMass() };
    mRigidBody.mVelocity = InverseMass > 0.0F ? NextLinearMomentum * InverseMass : DirectX::SimpleMath::Vector3{};
    SetIsSleeping(false);
}

void PhysicsActorBase::SetRestitution(float Restitution) {
    mRigidBody.mRestitution = std::clamp(Restitution, 0.0F, 1.0F);
}

float PhysicsActorBase::GetRestitution() const {
    return mRigidBody.mRestitution;
}

void PhysicsActorBase::SetLinearDamping(float LinearDamping) {
    mRigidBody.mLinearDamping = std::max(LinearDamping, 0.0F);
}

float PhysicsActorBase::GetLinearDamping() const {
    return mRigidBody.mLinearDamping;
}

void PhysicsActorBase::SetAngularDamping(float AngularDamping) {
    mRigidBody.mAngularDamping = std::max(AngularDamping, 0.0F);
}

float PhysicsActorBase::GetAngularDamping() const {
    return mRigidBody.mAngularDamping;
}

void PhysicsActorBase::SetAngularVelocity(const DirectX::SimpleMath::Vector3& AngularVelocity) {
    if (mRigidBody.mInverseMass <= 0.0F || mRigidBody.mMass <= 0.0F) {
        mRigidBody.mAngularVelocity = DirectX::SimpleMath::Vector3{};
        mRigidBody.mAngularMomentum = DirectX::SimpleMath::Vector3{};
        return;
    }

    mRigidBody.mAngularVelocity = AngularVelocity;
    DirectX::SimpleMath::Matrix WorldInertiaTensor{ CalculateWorldTensor(mRigidBody.mLocalInertiaTensor, mRigidBody.mOrientation) };
    mRigidBody.mAngularMomentum = DirectX::SimpleMath::Vector3::TransformNormal(mRigidBody.mAngularVelocity, WorldInertiaTensor);
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetAngularVelocity() const {
    return mRigidBody.mAngularVelocity;
}

void PhysicsActorBase::AddTorque(const DirectX::SimpleMath::Vector3& Torque) {
    mRigidBody.mTorque += Torque;
    SetIsSleeping(false);
}

const DirectX::SimpleMath::Vector3& PhysicsActorBase::GetTorque() const {
    return mRigidBody.mTorque;
}

void PhysicsActorBase::ClearTorque() {
    mRigidBody.mTorque = DirectX::SimpleMath::Vector3{};
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
        mRigidBody.mVelocity = DirectX::SimpleMath::Vector3{};
        mRigidBody.mAcceleration = DirectX::SimpleMath::Vector3{};
        mRigidBody.mAccumulatedForce = DirectX::SimpleMath::Vector3{};
        mRigidBody.mAngularVelocity = DirectX::SimpleMath::Vector3{};
        mRigidBody.mTorque = DirectX::SimpleMath::Vector3{};
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

    DirectX::SimpleMath::Vector3 NextVelocity{ (TargetPosition - mRigidBody.mPosition) / DeltaTime };
    SetVelocity(NextVelocity);
    SetPosition(TargetPosition);
}

void PhysicsActorBase::UpdateSleepState() {
    float VelocityLengthSquared{ mRigidBody.mVelocity.LengthSquared() };
    float AngularVelocityLengthSquared{ mRigidBody.mAngularVelocity.LengthSquared() };
    float AccelerationLengthSquared{ mRigidBody.mAcceleration.LengthSquared() };
    float ThresholdSquared{ mSleepThreshold * mSleepThreshold };
    bool ShouldSleep{ VelocityLengthSquared <= ThresholdSquared && AngularVelocityLengthSquared <= ThresholdSquared && AccelerationLengthSquared <= ThresholdSquared };
    SetIsSleeping(ShouldSleep);
}

void PhysicsActorBase::UpdateWorldBoundingBox() {
    NormalizeRigidBodyOrientation();
    DirectX::SimpleMath::Matrix ScalingMatrix{ DirectX::SimpleMath::Matrix::CreateScale(mRigidBody.mScale) };
    DirectX::SimpleMath::Matrix RotationMatrix{ DirectX::SimpleMath::Matrix::CreateFromQuaternion(mRigidBody.mOrientation) };
    DirectX::SimpleMath::Matrix TranslationMatrix{ DirectX::SimpleMath::Matrix::CreateTranslation(mRigidBody.mPosition) };
    DirectX::SimpleMath::Matrix WorldMatrix{ ScalingMatrix * RotationMatrix * TranslationMatrix };
    mLocalBoundingBox.Transform(mWorldBoundingBox, WorldMatrix);
    UpdateInverseInertiaTensorWorld();
    UpdateFatWorldBoundingBox();
}

void PhysicsActorBase::RecalculateLocalInertiaTensor() {
    if (mRigidBody.mInverseMass <= 0.0F || mRigidBody.mMass <= 0.0F) {
        mRigidBody.mLocalInertiaTensor = MakeTensorMatrix(0.0F, 0.0F, 0.0F);
        mRigidBody.mLocalInverseInertiaTensor = MakeTensorMatrix(0.0F, 0.0F, 0.0F);
        UpdateInverseInertiaTensorWorld();
        return;
    }

    mRigidBody.mLocalInertiaTensor = CalculateLocalInertiaTensor(mInertiaShapeType, mRigidBody.mMass, mLocalBoundingBox, mRigidBody.mScale);
    mRigidBody.mLocalInverseInertiaTensor = CalculateLocalInverseInertiaTensor(mRigidBody.mLocalInertiaTensor);
    UpdateInverseInertiaTensorWorld();
}

void PhysicsActorBase::UpdateInverseInertiaTensorWorld() {
    if (mRigidBody.mInverseMass <= 0.0F || mRigidBody.mMass <= 0.0F) {
        mRigidBody.mInverseInertiaTensorWorld = MakeTensorMatrix(0.0F, 0.0F, 0.0F);
        mRigidBody.mAngularVelocity = DirectX::SimpleMath::Vector3{};
        return;
    }

    mRigidBody.mInverseInertiaTensorWorld = CalculateWorldTensor(mRigidBody.mLocalInverseInertiaTensor, mRigidBody.mOrientation);
    mRigidBody.mAngularVelocity = DirectX::SimpleMath::Vector3::TransformNormal(mRigidBody.mAngularMomentum, mRigidBody.mInverseInertiaTensorWorld);
}

const DirectX::SimpleMath::Matrix& PhysicsActorBase::GetInverseInertiaTensorWorld() const {
    return mRigidBody.mInverseInertiaTensorWorld;
}

void PhysicsActorBase::NormalizeRigidBodyOrientation() {
    if (mRigidBody.mOrientation.LengthSquared() <= 0.0F) {
        mRigidBody.mOrientation = DirectX::SimpleMath::Quaternion{ 0.0F, 0.0F, 0.0F, 1.0F };
        return;
    }

    mRigidBody.mOrientation.Normalize();
}

void PhysicsActorBase::UpdateFatWorldBoundingBox() {
    mFatWorldBoundingBox = mWorldBoundingBox;
    float VelocityBasedMargin{ std::sqrt(mRigidBody.mVelocity.LengthSquared()) * 0.05F };
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
