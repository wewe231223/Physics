#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "../PhysicsActor.h"
#include "../PhysicsDynamicActor.h"
#include "../PhysicsKinematicActor.h"
#include "../PhysicsStaticActor.h"
#include "../PhysicsTerrainActor.h"

class IPhysicsActorRepository {
public:
    IPhysicsActorRepository();
    virtual ~IPhysicsActorRepository();
    IPhysicsActorRepository(const IPhysicsActorRepository& Other);
    IPhysicsActorRepository& operator=(const IPhysicsActorRepository& Other);
    IPhysicsActorRepository(IPhysicsActorRepository&& Other) noexcept;
    IPhysicsActorRepository& operator=(IPhysicsActorRepository&& Other) noexcept;

public:
    virtual std::unique_ptr<IPhysicsActorRepository> Clone() const = 0;

    virtual PhysicsDynamicActor* CreateDynamicActor(const PhysicsDynamicActor::ActorDesc& Desc) = 0;
    virtual PhysicsKinematicActor* CreateKinematicActor(const PhysicsKinematicActor::ActorDesc& Desc) = 0;
    virtual PhysicsTerrainActor* CreateTerrainActor(const PhysicsTerrainActor::ActorDesc& Desc) = 0;

    virtual void AddActor(std::unique_ptr<PhysicsActor> Actor) = 0;
    virtual void ClearActors() = 0;

    virtual PhysicsActor* GetActor(std::size_t Index) = 0;
    virtual const PhysicsActor* GetActor(std::size_t Index) const = 0;
    virtual std::size_t GetActorCount() const = 0;

    virtual std::vector<PhysicsDynamicActor*> CollectDynamicActors() = 0;
    virtual std::vector<const PhysicsDynamicActor*> CollectDynamicActors() const = 0;
    virtual std::vector<const PhysicsStaticActor*> CollectStaticActors() const = 0;
};
