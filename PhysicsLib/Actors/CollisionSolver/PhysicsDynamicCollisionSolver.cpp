#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <utility>

#include "PhysicsLib/Actors/CollisionSolver/PhysicsDynamicCollisionSolver.h"
#include "PhysicsLib/Actors/PhysicsActorBase.h"

#undef max
#undef min

#include "PhysicsLib/Actors/PhysicsDynamicCollisionInternal.inl"

PhysicsDynamicCollisionSolver::PhysicsDynamicCollisionSolver() {
}

PhysicsDynamicCollisionSolver::~PhysicsDynamicCollisionSolver() {
}

PhysicsDynamicCollisionSolver::PhysicsDynamicCollisionSolver(const PhysicsDynamicCollisionSolver& Other)
    : IPhysicsCollisionSolver{ Other } {
}

PhysicsDynamicCollisionSolver& PhysicsDynamicCollisionSolver::operator=(const PhysicsDynamicCollisionSolver& Other) {
    if (this == &Other) {
        return *this;
    }

    IPhysicsCollisionSolver::operator=(Other);
    return *this;
}

PhysicsDynamicCollisionSolver::PhysicsDynamicCollisionSolver(PhysicsDynamicCollisionSolver&& Other) noexcept
    : IPhysicsCollisionSolver{ std::move(Other) } {
}

PhysicsDynamicCollisionSolver& PhysicsDynamicCollisionSolver::operator=(PhysicsDynamicCollisionSolver&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    IPhysicsCollisionSolver::operator=(std::move(Other));
    return *this;
}

void PhysicsDynamicCollisionSolver::BeginFrame(std::size_t PairCandidateCount) {
    BeginDynamicCollisionPairCacheFrame(PairCandidateCount);
}

void PhysicsDynamicCollisionSolver::EndFrame() {
    EndDynamicCollisionPairCacheFrame();
}

bool PhysicsDynamicCollisionSolver::ResolveCollision(PhysicsActorBase& SelfActor, PhysicsActorBase& OtherActor, float DeltaTime) const {
    if (SelfActor.GetActorType() != PhysicsActorBase::PhysicsActorType::Dynamic || OtherActor.GetActorType() != PhysicsActorBase::PhysicsActorType::Dynamic) {
        return false;
    }

    if (!SelfActor.GetIsActive() || !OtherActor.GetIsActive()) {
        return false;
    }

    if (SelfActor.GetInverseMass() <= 0.0F || OtherActor.GetInverseMass() <= 0.0F) {
        return false;
    }

    DirectX::BoundingOrientedBox SelfBounds{ SelfActor.GetWorldBoundingBox() };
    DirectX::BoundingOrientedBox OtherBounds{ OtherActor.GetWorldBoundingBox() };
    if (!SelfBounds.Intersects(OtherBounds)) {
        return false;
    }

    DynamicObb SelfObb{ CreateDynamicObb(SelfBounds) };
    DynamicObb OtherObb{ CreateDynamicObb(OtherBounds) };
    DynamicPairCacheKey PairCacheKey{ CreateDynamicPairCacheKey(SelfActor, OtherActor) };
    auto CacheIterator{ DynamicPersistentPairCache.find(PairCacheKey) };
    bool HasPersistentCache{ CacheIterator != DynamicPersistentPairCache.end() };
    bool HasPreviousFramePersistentCache{ HasPersistentCache && (CacheIterator->second.mLastFrameIndex + 1U == DynamicPersistentPairCacheFrameIndex) };
    if (CacheIterator != DynamicPersistentPairCache.end()) {
        DirectX::SimpleMath::Vector3 CachedAxis{};
        if (TryGetAxisFromPersistentCache(CacheIterator->second, SelfObb, OtherObb, CachedAxis)) {
            float CachedAxisOverlap{ CalculateAxisOverlap(SelfObb, OtherObb, CachedAxis) };
            if (CachedAxisOverlap <= 0.0F) {
                DynamicPersistentPairCache.erase(CacheIterator);
                return false;
            }
        }
    }

    DynamicContactManifold ContactManifold{};
    bool HasBuiltManifoldFromCache{};
    if (HasPreviousFramePersistentCache) {
        HasBuiltManifoldFromCache = TryBuildContactManifoldFromPersistentCache(CacheIterator->second, SelfObb, OtherObb, ContactManifold);
    }

    if (!HasBuiltManifoldFromCache) {
        DynamicSatResult SatResult{};
        if (!ComputeObbSatResult(SelfObb, OtherObb, SatResult)) {
            if (CacheIterator != DynamicPersistentPairCache.end()) {
                DynamicPersistentPairCache.erase(CacheIterator);
            }

            return false;
        }

        if (!BuildContactManifoldFromSatResult(SelfObb, OtherObb, SatResult, ContactManifold)) {
            if (CacheIterator != DynamicPersistentPairCache.end()) {
                DynamicPersistentPairCache.erase(CacheIterator);
            }

            return false;
        }

        if (HasPreviousFramePersistentCache) {
            SeedManifoldWithPersistentCache(CacheIterator->second, ContactManifold, true);
        }
    }

    if (ContactManifold.mContactCount == 0U) {
        if (CacheIterator != DynamicPersistentPairCache.end()) {
            DynamicPersistentPairCache.erase(CacheIterator);
        }

        return false;
    }

    SolveContactManifoldWithPgs(ContactManifold, SelfActor, OtherActor, DeltaTime);
    ApplyPositionCorrectionFromManifold(ContactManifold, SelfActor, OtherActor);
    SelfActor.SetIsSleeping(false);
    OtherActor.SetIsSleeping(false);

    DynamicObb CorrectedSelfObb{ CreateDynamicObb(SelfActor.GetWorldBoundingBox()) };
    DynamicObb CorrectedOtherObb{ CreateDynamicObb(OtherActor.GetWorldBoundingBox()) };
    DynamicPersistentManifoldCache& PersistentCache{ DynamicPersistentPairCache[PairCacheKey] };
    UpdatePersistentCacheFromManifold(CorrectedSelfObb, CorrectedOtherObb, ContactManifold, PersistentCache);
    return true;
}

bool PhysicsDynamicCollisionSolver::ResolveDynamicCollision(const PhysicsActorBase& SelfActor, PhysicsActorBase& DynamicActor, float DeltaTime) const {
    (void)SelfActor;
    (void)DynamicActor;
    (void)DeltaTime;
    return false;
}
