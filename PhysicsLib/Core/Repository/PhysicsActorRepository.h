#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "IPhysicsActorRepository.h"

class PhysicsActorRepository final : public IPhysicsActorRepository {
public:
    PhysicsActorRepository();
    ~PhysicsActorRepository() override;
    PhysicsActorRepository(const PhysicsActorRepository& Other);
    PhysicsActorRepository& operator=(const PhysicsActorRepository& Other);
    PhysicsActorRepository(PhysicsActorRepository&& Other) noexcept;
    PhysicsActorRepository& operator=(PhysicsActorRepository&& Other) noexcept;

public:
    std::unique_ptr<IPhysicsActorRepository> Clone() const override;

    PhysicsDynamicActor* CreateDynamicActor(const PhysicsDynamicActor::ActorDesc& Desc) override;
    PhysicsKinematicActor* CreateKinematicActor(const PhysicsKinematicActor::ActorDesc& Desc) override;
    PhysicsTerrainActor* CreateTerrainActor(const PhysicsTerrainActor::ActorDesc& Desc) override;

    void AddActor(std::unique_ptr<PhysicsActor> Actor) override;
    void ClearActors() override;

    PhysicsActor* GetActor(std::size_t Index) override;
    const PhysicsActor* GetActor(std::size_t Index) const override;
    std::size_t GetActorCount() const override;

    std::vector<PhysicsDynamicActor*> CollectDynamicActors() override;
    std::vector<const PhysicsDynamicActor*> CollectDynamicActors() const override;
    std::vector<const PhysicsStaticActor*> CollectStaticActors() const override;

private:
    std::unique_ptr<PhysicsActor> CloneActor(const PhysicsActor& SourceActor) const;

private:
    std::vector<std::unique_ptr<PhysicsActor>> mActors;
};
