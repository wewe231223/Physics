#pragma once

#include <memory>
#include <vector>

#include "IPhysicsSpatialQuery.h"

class BruteForcePhysicsSpatialQuery final : public IPhysicsSpatialQuery {
public:
    BruteForcePhysicsSpatialQuery();
    ~BruteForcePhysicsSpatialQuery() override;
    BruteForcePhysicsSpatialQuery(const BruteForcePhysicsSpatialQuery& Other);
    BruteForcePhysicsSpatialQuery& operator=(const BruteForcePhysicsSpatialQuery& Other);
    BruteForcePhysicsSpatialQuery(BruteForcePhysicsSpatialQuery&& Other) noexcept;
    BruteForcePhysicsSpatialQuery& operator=(BruteForcePhysicsSpatialQuery&& Other) noexcept;

public:
    std::unique_ptr<IPhysicsSpatialQuery> Clone() const override;
    std::vector<PhysicsDynamicCollisionPairCandidate> QueryDynamicCollisionPairs(IPhysicsActorRepository& ActorRepository) const override;
};
