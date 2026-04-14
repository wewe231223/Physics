#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include <SimpleMath/SimpleMath.h>

#include "Mediator/IPhysicsWorldMediator.h"
#include "PhysicsActor.h"
#include "PhysicsDynamicActor.h"
#include "PhysicsKinematicActor.h"
#include "Types/PhysicsSimulationTypes.h"
#include "PhysicsTerrainActor.h"

class IPhysicsActorRepository;
class IPhysicsSimulationLogic;
class IPhysicsSpatialQuery;

class PhysicsFrameAccumulator final {
public:
    struct ActorState {
        const PhysicsActor* mActorPointer;
        PhysicsActor::PhysicsActorType mActorType;
        DirectX::SimpleMath::Vector3 mPosition;
        DirectX::SimpleMath::Vector3 mRotation;
        DirectX::SimpleMath::Vector3 mScale;
    };

public:
    PhysicsFrameAccumulator();
    ~PhysicsFrameAccumulator();
    PhysicsFrameAccumulator(const PhysicsFrameAccumulator& Other);
    PhysicsFrameAccumulator& operator=(const PhysicsFrameAccumulator& Other);
    PhysicsFrameAccumulator(PhysicsFrameAccumulator&& Other) noexcept;
    PhysicsFrameAccumulator& operator=(PhysicsFrameAccumulator&& Other) noexcept;

    explicit PhysicsFrameAccumulator(float FixedTimeStep);

public:
    void Initialize(float FixedTimeStep);
    void AddDeltaTime(float DeltaTime);
    bool TryConsumeFixedStep();
    float GetAccumulatedTime() const;
    float GetInterpolationAlpha() const;

    void SynchronizeStatePair(const IPhysicsActorRepository& ActorRepository);
    void CapturePreviousState(const IPhysicsActorRepository& ActorRepository);
    void CaptureCurrentState(const IPhysicsActorRepository& ActorRepository);
    bool TryGetInterpolatedState(const PhysicsActor& Actor, DirectX::SimpleMath::Vector3& OutPosition, DirectX::SimpleMath::Vector3& OutRotation, DirectX::SimpleMath::Vector3& OutScale) const;

private:
    void CaptureState(std::vector<ActorState>& OutStates, const IPhysicsActorRepository& ActorRepository) const;
    bool TryGetActorState(const std::vector<ActorState>& States, const PhysicsActor& Actor, ActorState& OutActorState) const;

private:
    float mFixedTimeStep;
    float mAccumulatedTime;
    std::vector<ActorState> mPreviousStates;
    std::vector<ActorState> mCurrentStates;
};

class PhysicsWorld final : public IPhysicsWorldMediator {
public:
    struct WorldSettings {
        float FixedTimeStep;
        DirectX::SimpleMath::Vector3 Gravity;
    };

public:
    PhysicsWorld();
    ~PhysicsWorld() override;
    PhysicsWorld(const PhysicsWorld& Other);
    PhysicsWorld& operator=(const PhysicsWorld& Other);
    PhysicsWorld(PhysicsWorld&& Other) noexcept;
    PhysicsWorld& operator=(PhysicsWorld&& Other) noexcept;

    explicit PhysicsWorld(const WorldSettings& Settings);

public:
    void Initialize(const WorldSettings& Settings);

    PhysicsDynamicActor* CreateDynamicActor(const PhysicsDynamicActor::ActorDesc& Desc);
    PhysicsKinematicActor* CreateKinematicActor(const PhysicsKinematicActor::ActorDesc& Desc);
    PhysicsTerrainActor* CreateTerrainActor(const PhysicsTerrainActor::ActorDesc& Desc);
    void AddActor(std::unique_ptr<PhysicsActor> Actor);
    void ClearActors();

    PhysicsActor* GetActor(std::size_t Index);
    const PhysicsActor* GetActor(std::size_t Index) const;
    std::size_t GetActorCount() const;

    const WorldSettings& GetSettings() const;
    float GetAccumulator() const;
    float GetInterpolationAlpha() const;
    std::size_t GetLastUpdateStepCount() const;
    double GetLastUpdateStepElapsedMilliseconds() const;
    double GetLastStepElapsedMilliseconds() const;
    bool TryGetInterpolatedActorTransform(const PhysicsActor& Actor, DirectX::SimpleMath::Vector3& OutPosition, DirectX::SimpleMath::Vector3& OutRotation, DirectX::SimpleMath::Vector3& OutScale) const;

    void StepSimulation();
    void Update(float DeltaTime);

    const DirectX::SimpleMath::Vector3& GetGravity() const override;
    IPhysicsActorRepository& GetActorRepository() override;
    const IPhysicsActorRepository& GetActorRepository() const override;
    IPhysicsSpatialQuery& GetSpatialQuery() override;
    const IPhysicsSpatialQuery& GetSpatialQuery() const override;
    void PublishEvent(PhysicsSimulationEventType EventType, const PhysicsActor* FirstActor, const PhysicsActor* SecondActor) override;
    void ClearPublishedEvents() override;
    const std::vector<PhysicsSimulationEvent>& GetPublishedEvents() const override;

private:
    void InitializeDependencies();
    void InitializeSimulationLogics();

private:
    WorldSettings mSettings;
    PhysicsFrameAccumulator mFrameAccumulator;
    std::size_t mLastUpdateStepCount;
    double mLastUpdateStepElapsedMilliseconds;
    double mLastStepElapsedMilliseconds;
    std::unique_ptr<IPhysicsActorRepository> mActorRepository;
    std::unique_ptr<IPhysicsSpatialQuery> mSpatialQuery;
    std::vector<std::unique_ptr<IPhysicsSimulationLogic>> mSimulationLogics;
    std::vector<PhysicsSimulationEvent> mPublishedEvents;
};
