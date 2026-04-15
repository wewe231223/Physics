#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>

#include <DirectXCollision.h>
#include <SimpleMath/SimpleMath.h>

#ifdef _DEBUG
#pragma comment(lib, "debug/DirectXTK12.lib")
#else
#pragma comment(lib, "release/DirectXTK12.lib")
#endif

class IPhysicsWorldMediator;
class PhysicsActorBase;

class IPhysicsCollisionSolver {
public:
    IPhysicsCollisionSolver();
    virtual ~IPhysicsCollisionSolver();
    IPhysicsCollisionSolver(const IPhysicsCollisionSolver& Other);
    IPhysicsCollisionSolver& operator=(const IPhysicsCollisionSolver& Other);
    IPhysicsCollisionSolver(IPhysicsCollisionSolver&& Other) noexcept;
    IPhysicsCollisionSolver& operator=(IPhysicsCollisionSolver&& Other) noexcept;

public:
    virtual bool ResolveCollision(PhysicsActorBase& SelfActor, PhysicsActorBase& OtherActor, float DeltaTime) const = 0;
    virtual bool ResolveDynamicCollision(const PhysicsActorBase& SelfActor, PhysicsActorBase& DynamicActor, float DeltaTime) const = 0;
};

class IPhysicsIntegrater {
public:
    IPhysicsIntegrater();
    virtual ~IPhysicsIntegrater();
    IPhysicsIntegrater(const IPhysicsIntegrater& Other);
    IPhysicsIntegrater& operator=(const IPhysicsIntegrater& Other);
    IPhysicsIntegrater(IPhysicsIntegrater&& Other) noexcept;
    IPhysicsIntegrater& operator=(IPhysicsIntegrater&& Other) noexcept;

public:
    virtual void Integrate(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const = 0;
};

class IPhysicsConstraintSolver {
public:
    IPhysicsConstraintSolver();
    virtual ~IPhysicsConstraintSolver();
    IPhysicsConstraintSolver(const IPhysicsConstraintSolver& Other);
    IPhysicsConstraintSolver& operator=(const IPhysicsConstraintSolver& Other);
    IPhysicsConstraintSolver(IPhysicsConstraintSolver&& Other) noexcept;
    IPhysicsConstraintSolver& operator=(IPhysicsConstraintSolver&& Other) noexcept;

public:
    virtual void Solve(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const = 0;
};

class PhysicsNoConstraintSolver final : public IPhysicsConstraintSolver {
public:
    PhysicsNoConstraintSolver();
    ~PhysicsNoConstraintSolver() override;
    PhysicsNoConstraintSolver(const PhysicsNoConstraintSolver& Other);
    PhysicsNoConstraintSolver& operator=(const PhysicsNoConstraintSolver& Other);
    PhysicsNoConstraintSolver(PhysicsNoConstraintSolver&& Other) noexcept;
    PhysicsNoConstraintSolver& operator=(PhysicsNoConstraintSolver&& Other) noexcept;

public:
    void Solve(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const override;
};

class PhysicsDynamicCollisionSolver final : public IPhysicsCollisionSolver {
public:
    PhysicsDynamicCollisionSolver();
    ~PhysicsDynamicCollisionSolver() override;
    PhysicsDynamicCollisionSolver(const PhysicsDynamicCollisionSolver& Other);
    PhysicsDynamicCollisionSolver& operator=(const PhysicsDynamicCollisionSolver& Other);
    PhysicsDynamicCollisionSolver(PhysicsDynamicCollisionSolver&& Other) noexcept;
    PhysicsDynamicCollisionSolver& operator=(PhysicsDynamicCollisionSolver&& Other) noexcept;

public:
    bool ResolveCollision(PhysicsActorBase& SelfActor, PhysicsActorBase& OtherActor, float DeltaTime) const override;
    bool ResolveDynamicCollision(const PhysicsActorBase& SelfActor, PhysicsActorBase& DynamicActor, float DeltaTime) const override;
};

class PhysicsStaticCollisionSolver final : public IPhysicsCollisionSolver {
public:
    PhysicsStaticCollisionSolver();
    ~PhysicsStaticCollisionSolver() override;
    PhysicsStaticCollisionSolver(const PhysicsStaticCollisionSolver& Other);
    PhysicsStaticCollisionSolver& operator=(const PhysicsStaticCollisionSolver& Other);
    PhysicsStaticCollisionSolver(PhysicsStaticCollisionSolver&& Other) noexcept;
    PhysicsStaticCollisionSolver& operator=(PhysicsStaticCollisionSolver&& Other) noexcept;

public:
    bool ResolveCollision(PhysicsActorBase& SelfActor, PhysicsActorBase& OtherActor, float DeltaTime) const override;
    bool ResolveDynamicCollision(const PhysicsActorBase& SelfActor, PhysicsActorBase& DynamicActor, float DeltaTime) const override;
};

class PhysicsKinematicCollisionSolver final : public IPhysicsCollisionSolver {
public:
    PhysicsKinematicCollisionSolver();
    ~PhysicsKinematicCollisionSolver() override;
    PhysicsKinematicCollisionSolver(const PhysicsKinematicCollisionSolver& Other);
    PhysicsKinematicCollisionSolver& operator=(const PhysicsKinematicCollisionSolver& Other);
    PhysicsKinematicCollisionSolver(PhysicsKinematicCollisionSolver&& Other) noexcept;
    PhysicsKinematicCollisionSolver& operator=(PhysicsKinematicCollisionSolver&& Other) noexcept;

public:
    bool ResolveCollision(PhysicsActorBase& SelfActor, PhysicsActorBase& OtherActor, float DeltaTime) const override;
    bool ResolveDynamicCollision(const PhysicsActorBase& SelfActor, PhysicsActorBase& DynamicActor, float DeltaTime) const override;
};

class PhysicsDynamicIntegrater final : public IPhysicsIntegrater {
public:
    PhysicsDynamicIntegrater();
    ~PhysicsDynamicIntegrater() override;
    PhysicsDynamicIntegrater(const PhysicsDynamicIntegrater& Other);
    PhysicsDynamicIntegrater& operator=(const PhysicsDynamicIntegrater& Other);
    PhysicsDynamicIntegrater(PhysicsDynamicIntegrater&& Other) noexcept;
    PhysicsDynamicIntegrater& operator=(PhysicsDynamicIntegrater&& Other) noexcept;

public:
    void Integrate(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const override;
};

class PhysicsStaticIntegrater final : public IPhysicsIntegrater {
public:
    PhysicsStaticIntegrater();
    ~PhysicsStaticIntegrater() override;
    PhysicsStaticIntegrater(const PhysicsStaticIntegrater& Other);
    PhysicsStaticIntegrater& operator=(const PhysicsStaticIntegrater& Other);
    PhysicsStaticIntegrater(PhysicsStaticIntegrater&& Other) noexcept;
    PhysicsStaticIntegrater& operator=(PhysicsStaticIntegrater&& Other) noexcept;

public:
    void Integrate(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const override;
};

class PhysicsKinematicIntegrater final : public IPhysicsIntegrater {
public:
    PhysicsKinematicIntegrater();
    ~PhysicsKinematicIntegrater() override;
    PhysicsKinematicIntegrater(const PhysicsKinematicIntegrater& Other);
    PhysicsKinematicIntegrater& operator=(const PhysicsKinematicIntegrater& Other);
    PhysicsKinematicIntegrater(PhysicsKinematicIntegrater&& Other) noexcept;
    PhysicsKinematicIntegrater& operator=(PhysicsKinematicIntegrater&& Other) noexcept;

public:
    void Integrate(IPhysicsWorldMediator& WorldMediator, PhysicsActorBase& Actor, float DeltaTime) const override;
};

using DynamicSolver = PhysicsDynamicCollisionSolver;
using StaticSolver = PhysicsStaticCollisionSolver;
using KinematicSolver = PhysicsKinematicCollisionSolver;
using DynamicIntegrater = PhysicsDynamicIntegrater;
using StaticIntegrater = PhysicsStaticIntegrater;
using KinematicIntegrater = PhysicsKinematicIntegrater;
using DefaultConstraintSolver = PhysicsNoConstraintSolver;

template <typename SolverType>
concept PhysicsCollisionSolverPolicy = std::derived_from<SolverType, IPhysicsCollisionSolver> && std::default_initializable<SolverType>;

template <typename IntegraterType>
concept PhysicsIntegraterPolicy = std::derived_from<IntegraterType, IPhysicsIntegrater> && std::default_initializable<IntegraterType>;

template <typename ConstraintSolverType>
concept PhysicsConstraintSolverPolicy = std::derived_from<ConstraintSolverType, IPhysicsConstraintSolver> && std::default_initializable<ConstraintSolverType>;

class PhysicsActorBase {
public:
    enum class PhysicsActorFlags : std::uint32_t {
        None = 0U,
        Static = 1U << 0U,
        Kinematic = 1U << 1U,
        Trigger = 1U << 2U,
        Sleeping = 1U << 3U
    };

    enum class PhysicsActorType : std::uint32_t {
        Dynamic = 0U,
        Kinematic = 1U,
        Static = 2U
    };

    struct ActorDesc {
        std::string Name{ "PhysicsActor" };
        bool IsActive{ true };
        float Mass{ 1.0F };
        PhysicsActorFlags Flags{ PhysicsActorFlags::None };
        PhysicsActorType ActorType{ PhysicsActorType::Dynamic };
        DirectX::BoundingOrientedBox LocalBoundingBox{};
        DirectX::SimpleMath::Vector3 Position{};
        DirectX::SimpleMath::Vector3 Rotation{};
        DirectX::SimpleMath::Vector3 Scale{ 1.0F, 1.0F, 1.0F };
        DirectX::SimpleMath::Vector3 Velocity{};
        DirectX::SimpleMath::Vector3 Acceleration{};
        float Friction{ 0.6F };
        float Restitution{ 0.1F };
        float LinearDamping{ 0.03F };
        float AngularDamping{ 0.03F };
        bool IsSleeping{};
        float SleepThreshold{ 0.05F };
        float BoundingBoxFatMargin{ 0.1F };
    };

public:
    PhysicsActorBase();
    virtual ~PhysicsActorBase();
    PhysicsActorBase(const PhysicsActorBase& Other);
    PhysicsActorBase& operator=(const PhysicsActorBase& Other);
    PhysicsActorBase(PhysicsActorBase&& Other) noexcept;
    PhysicsActorBase& operator=(PhysicsActorBase&& Other) noexcept;

    explicit PhysicsActorBase(std::string Name);
    explicit PhysicsActorBase(const ActorDesc& Desc);

public:
    void SetName(std::string Name);
    const std::string& GetName() const;

    void SetIsActive(bool IsActive);
    bool GetIsActive() const;

    void SetMass(float Mass);
    float GetMass() const;
    void SetInverseMass(float InverseMass);
    float GetInverseMass() const;
    void SetFriction(float Friction);
    float GetFriction() const;
    void SetLocalInertiaTensor(const DirectX::SimpleMath::Matrix& LocalInertiaTensor);
    const DirectX::SimpleMath::Matrix& GetLocalInertiaTensor() const;
    void SetLocalInverseInertiaTensor(const DirectX::SimpleMath::Matrix& LocalInverseInertiaTensor);
    const DirectX::SimpleMath::Matrix& GetLocalInverseInertiaTensor() const;
    void SetLinearMomentum(const DirectX::SimpleMath::Vector3& LinearMomentum);
    const DirectX::SimpleMath::Vector3& GetLinearMomentum() const;
    void SetAngularMomentum(const DirectX::SimpleMath::Vector3& AngularMomentum);
    const DirectX::SimpleMath::Vector3& GetAngularMomentum() const;

    void SetFlags(PhysicsActorFlags Flags);
    PhysicsActorFlags GetFlags() const;
    bool HasFlag(PhysicsActorFlags Flag) const;

    void SetActorType(PhysicsActorType ActorType);
    PhysicsActorType GetActorType() const;

    void SetLocalBoundingBox(const DirectX::BoundingOrientedBox& LocalBoundingBox);
    const DirectX::BoundingOrientedBox& GetLocalBoundingBox() const;
    const DirectX::BoundingOrientedBox& GetWorldBoundingBox() const;
    const DirectX::BoundingOrientedBox& GetFatWorldBoundingBox() const;

    void SetPosition(const DirectX::SimpleMath::Vector3& Position);
    const DirectX::SimpleMath::Vector3& GetPosition() const;

    void SetRotation(const DirectX::SimpleMath::Vector3& Rotation);
    const DirectX::SimpleMath::Vector3& GetRotation() const;

    void SetScale(const DirectX::SimpleMath::Vector3& Scale);
    const DirectX::SimpleMath::Vector3& GetScale() const;

    void SetVelocity(const DirectX::SimpleMath::Vector3& Velocity);
    const DirectX::SimpleMath::Vector3& GetVelocity() const;

    void SetAcceleration(const DirectX::SimpleMath::Vector3& Acceleration);
    const DirectX::SimpleMath::Vector3& GetAcceleration() const;
    void AddForce(const DirectX::SimpleMath::Vector3& Force);
    const DirectX::SimpleMath::Vector3& GetAccumulatedForce() const;
    void ClearAccumulatedForce();
    void AddImpulse(const DirectX::SimpleMath::Vector3& Impulse);

    void SetRestitution(float Restitution);
    float GetRestitution() const;

    void SetLinearDamping(float LinearDamping);
    float GetLinearDamping() const;

    void SetAngularDamping(float AngularDamping);
    float GetAngularDamping() const;

    void SetSleepThreshold(float SleepThreshold);
    float GetSleepThreshold() const;

    void SetBoundingBoxFatMargin(float BoundingBoxFatMargin);
    float GetBoundingBoxFatMargin() const;

    void SetIsSleeping(bool IsSleeping);
    bool GetIsSleeping() const;

    void MoveToTarget(const DirectX::SimpleMath::Vector3& TargetPosition, float DeltaTime);
    void UpdateSleepState();
    void UpdateWorldBoundingBox();

public:
    virtual bool ResolveDynamicCollision(PhysicsActorBase& DynamicActor, float DeltaTime) const = 0;
    virtual bool ResolveActorCollision(PhysicsActorBase& OtherActor, float DeltaTime) = 0;
    virtual void Integrate(IPhysicsWorldMediator& WorldMediator, float DeltaTime) = 0;
    virtual void SolveConstraints(IPhysicsWorldMediator& WorldMediator, float DeltaTime) = 0;
    virtual std::unique_ptr<PhysicsActorBase> Clone() const = 0;

private:
    void UpdateFatWorldBoundingBox();

private:
    std::string mName;
    bool mIsActive;
    float mMass;
    float mInverseMass;
    float mFriction;
    DirectX::SimpleMath::Matrix mLocalInertiaTensor;
    DirectX::SimpleMath::Matrix mLocalInverseInertiaTensor;
    DirectX::SimpleMath::Vector3 mLinearMomentum;
    DirectX::SimpleMath::Vector3 mAngularMomentum;
    PhysicsActorFlags mFlags;
    PhysicsActorType mActorType;
    DirectX::BoundingOrientedBox mLocalBoundingBox;
    DirectX::BoundingOrientedBox mWorldBoundingBox;
    DirectX::BoundingOrientedBox mFatWorldBoundingBox;
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector3 mRotation;
    DirectX::SimpleMath::Vector3 mScale;
    DirectX::SimpleMath::Vector3 mVelocity;
    DirectX::SimpleMath::Vector3 mAcceleration;
    DirectX::SimpleMath::Vector3 mAccumulatedForce;
    float mRestitution;
    float mLinearDamping;
    float mAngularDamping;
    bool mIsSleeping;
    float mSleepThreshold;
    float mBoundingBoxFatMargin;
};

template <PhysicsCollisionSolverPolicy CollisionSolverType, PhysicsIntegraterPolicy IntegraterType, PhysicsConstraintSolverPolicy ConstraintSolverType = DefaultConstraintSolver, PhysicsActorBase::PhysicsActorType ActorTypeValue = PhysicsActorBase::PhysicsActorType::Dynamic>
class PhysicsActor : public PhysicsActorBase {
public:
    using CollisionSolver = CollisionSolverType;
    using Integrater = IntegraterType;
    using ConstraintSolver = ConstraintSolverType;
    using ActorDesc = PhysicsActorBase::ActorDesc;

public:
    PhysicsActor();
    ~PhysicsActor() override = default;
    PhysicsActor(const PhysicsActor& Other) = default;
    PhysicsActor& operator=(const PhysicsActor& Other) = default;
    PhysicsActor(PhysicsActor&& Other) noexcept = default;
    PhysicsActor& operator=(PhysicsActor&& Other) noexcept = default;

    explicit PhysicsActor(std::string Name);
    explicit PhysicsActor(const ActorDesc& Desc);

public:
    bool ResolveDynamicCollision(PhysicsActorBase& DynamicActor, float DeltaTime) const override;
    bool ResolveActorCollision(PhysicsActorBase& OtherActor, float DeltaTime) override;
    void Integrate(IPhysicsWorldMediator& WorldMediator, float DeltaTime) override;
    void SolveConstraints(IPhysicsWorldMediator& WorldMediator, float DeltaTime) override;
    std::unique_ptr<PhysicsActorBase> Clone() const override;

private:
    void ApplyActorTypeDefaults();

private:
    CollisionSolverType mCollisionSolver;
    IntegraterType mIntegrater;
    ConstraintSolverType mConstraintSolver;
};

template <PhysicsCollisionSolverPolicy CollisionSolverType, PhysicsIntegraterPolicy IntegraterType, PhysicsConstraintSolverPolicy ConstraintSolverType, PhysicsActorBase::PhysicsActorType ActorTypeValue>
PhysicsActor<CollisionSolverType, IntegraterType, ConstraintSolverType, ActorTypeValue>::PhysicsActor()
    : PhysicsActorBase{},
      mCollisionSolver{},
      mIntegrater{},
      mConstraintSolver{} {
    ApplyActorTypeDefaults();
}

template <PhysicsCollisionSolverPolicy CollisionSolverType, PhysicsIntegraterPolicy IntegraterType, PhysicsConstraintSolverPolicy ConstraintSolverType, PhysicsActorBase::PhysicsActorType ActorTypeValue>
PhysicsActor<CollisionSolverType, IntegraterType, ConstraintSolverType, ActorTypeValue>::PhysicsActor(std::string Name)
    : PhysicsActorBase{ std::move(Name) },
      mCollisionSolver{},
      mIntegrater{},
      mConstraintSolver{} {
    ApplyActorTypeDefaults();
}

template <PhysicsCollisionSolverPolicy CollisionSolverType, PhysicsIntegraterPolicy IntegraterType, PhysicsConstraintSolverPolicy ConstraintSolverType, PhysicsActorBase::PhysicsActorType ActorTypeValue>
PhysicsActor<CollisionSolverType, IntegraterType, ConstraintSolverType, ActorTypeValue>::PhysicsActor(const ActorDesc& Desc)
    : PhysicsActorBase{ Desc },
      mCollisionSolver{},
      mIntegrater{},
      mConstraintSolver{} {
    ApplyActorTypeDefaults();
}

template <PhysicsCollisionSolverPolicy CollisionSolverType, PhysicsIntegraterPolicy IntegraterType, PhysicsConstraintSolverPolicy ConstraintSolverType, PhysicsActorBase::PhysicsActorType ActorTypeValue>
bool PhysicsActor<CollisionSolverType, IntegraterType, ConstraintSolverType, ActorTypeValue>::ResolveDynamicCollision(PhysicsActorBase& DynamicActor, float DeltaTime) const {
    bool HasResolved{ mCollisionSolver.ResolveDynamicCollision(*this, DynamicActor, DeltaTime) };
    return HasResolved;
}

template <PhysicsCollisionSolverPolicy CollisionSolverType, PhysicsIntegraterPolicy IntegraterType, PhysicsConstraintSolverPolicy ConstraintSolverType, PhysicsActorBase::PhysicsActorType ActorTypeValue>
bool PhysicsActor<CollisionSolverType, IntegraterType, ConstraintSolverType, ActorTypeValue>::ResolveActorCollision(PhysicsActorBase& OtherActor, float DeltaTime) {
    bool HasResolved{ mCollisionSolver.ResolveCollision(*this, OtherActor, DeltaTime) };
    return HasResolved;
}

template <PhysicsCollisionSolverPolicy CollisionSolverType, PhysicsIntegraterPolicy IntegraterType, PhysicsConstraintSolverPolicy ConstraintSolverType, PhysicsActorBase::PhysicsActorType ActorTypeValue>
void PhysicsActor<CollisionSolverType, IntegraterType, ConstraintSolverType, ActorTypeValue>::Integrate(IPhysicsWorldMediator& WorldMediator, float DeltaTime) {
    mIntegrater.Integrate(WorldMediator, *this, DeltaTime);
}

template <PhysicsCollisionSolverPolicy CollisionSolverType, PhysicsIntegraterPolicy IntegraterType, PhysicsConstraintSolverPolicy ConstraintSolverType, PhysicsActorBase::PhysicsActorType ActorTypeValue>
void PhysicsActor<CollisionSolverType, IntegraterType, ConstraintSolverType, ActorTypeValue>::SolveConstraints(IPhysicsWorldMediator& WorldMediator, float DeltaTime) {
    mConstraintSolver.Solve(WorldMediator, *this, DeltaTime);
}

template <PhysicsCollisionSolverPolicy CollisionSolverType, PhysicsIntegraterPolicy IntegraterType, PhysicsConstraintSolverPolicy ConstraintSolverType, PhysicsActorBase::PhysicsActorType ActorTypeValue>
std::unique_ptr<PhysicsActorBase> PhysicsActor<CollisionSolverType, IntegraterType, ConstraintSolverType, ActorTypeValue>::Clone() const {
    std::unique_ptr<PhysicsActorBase> ClonedActor{ std::make_unique<PhysicsActor<CollisionSolverType, IntegraterType, ConstraintSolverType, ActorTypeValue>>(*this) };
    return ClonedActor;
}

template <PhysicsCollisionSolverPolicy CollisionSolverType, PhysicsIntegraterPolicy IntegraterType, PhysicsConstraintSolverPolicy ConstraintSolverType, PhysicsActorBase::PhysicsActorType ActorTypeValue>
void PhysicsActor<CollisionSolverType, IntegraterType, ConstraintSolverType, ActorTypeValue>::ApplyActorTypeDefaults() {
    SetActorType(ActorTypeValue);

    if constexpr (ActorTypeValue == PhysicsActorBase::PhysicsActorType::Static) {
        SetFlags(GetFlags() | PhysicsActorBase::PhysicsActorFlags::Static);
        SetMass(0.0F);
    }

    if constexpr (ActorTypeValue == PhysicsActorBase::PhysicsActorType::Kinematic) {
        SetFlags(GetFlags() | PhysicsActorBase::PhysicsActorFlags::Kinematic);
    }
}

PhysicsActorBase::PhysicsActorFlags operator|(PhysicsActorBase::PhysicsActorFlags Left, PhysicsActorBase::PhysicsActorFlags Right);
PhysicsActorBase::PhysicsActorFlags operator&(PhysicsActorBase::PhysicsActorFlags Left, PhysicsActorBase::PhysicsActorFlags Right);
