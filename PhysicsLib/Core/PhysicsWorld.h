#pragma once

#include <cstddef>
#include <deque>
#include <SimpleMath/SimpleMath.h>
#include "PhysicsActor.h"

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

    PhysicsActor* CreateActor(const PhysicsActor::ActorDesc& Desc);
    void AddActor(const PhysicsActor& Actor);
    void AddActor(PhysicsActor&& Actor);
    void ClearActors();

    PhysicsActor* GetActor(std::size_t Index);
    const PhysicsActor* GetActor(std::size_t Index) const;
    std::size_t GetActorCount() const;

    const WorldSettings& GetSettings() const;
    float GetAccumulator() const;

    void StepSimulation();
    void Update(float DeltaTime);

private:
    void IntegrateActor(PhysicsActor& Actor, float DeltaTime) const;

private:
    WorldSettings mSettings;
    float mAccumulator;
    std::deque<PhysicsActor> mActors;
};
