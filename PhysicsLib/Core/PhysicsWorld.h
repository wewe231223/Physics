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
    float mAccumulator;
    std::unique_ptr<IPhysicsActorRepository> mActorRepository;
    std::unique_ptr<IPhysicsSpatialQuery> mSpatialQuery;
    std::vector<std::unique_ptr<IPhysicsSimulationLogic>> mSimulationLogics;
    std::vector<PhysicsSimulationEvent> mPublishedEvents;
};
