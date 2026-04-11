#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include <SimpleMath/SimpleMath.h>

#include "PhysicsActor.h"
#include "PhysicsDynamicActor.h"
#include "PhysicsTerrainActor.h"

class PhysicsWorld final {
public:
    struct WorldSettings {
        float FixedTimeStep;
        DirectX::SimpleMath::Vector3 Gravity;
    };

public:
    PhysicsWorld();
    ~PhysicsWorld();
    PhysicsWorld(const PhysicsWorld& Other);
    PhysicsWorld& operator=(const PhysicsWorld& Other);
    PhysicsWorld(PhysicsWorld&& Other) noexcept;
    PhysicsWorld& operator=(PhysicsWorld&& Other) noexcept;

    explicit PhysicsWorld(const WorldSettings& Settings);

public:
    void Initialize(const WorldSettings& Settings);

    PhysicsDynamicActor* CreateDynamicActor(const PhysicsDynamicActor::ActorDesc& Desc);
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

private:
    void IntegrateActor(PhysicsDynamicActor& Actor, float DeltaTime) const;
    bool FindTerrainHitPoint(const PhysicsDynamicActor& Actor, DirectX::SimpleMath::Vector3& HitPoint) const;

private:
    WorldSettings mSettings;
    float mAccumulator;
    std::vector<std::unique_ptr<PhysicsActor>> mActors;
};
