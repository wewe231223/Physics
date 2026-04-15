#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include <SimpleMath/SimpleMath.h>

#include "PhysicsLib/Actors/PhysicsStaticActor.h"

class PhysicsTerrainActor final : public PhysicsStaticActor {
public:
    struct ActorDesc {
        DirectX::SimpleMath::Vector3 Position{};
        DirectX::SimpleMath::Vector3 Rotation{};
        DirectX::SimpleMath::Vector3 Scale{};
        float HalfExtentX{};
        float HalfExtentZ{};
        std::uint32_t HeightFieldWidth{};
        std::uint32_t HeightFieldHeight{};
        float HeightFieldCellSpacing{};
        float HeightFieldMaxHeight{};
        bool HeightFieldCenterOrigin{};
        std::vector<float> HeightFieldValues{};
    };

public:
    PhysicsTerrainActor();
    ~PhysicsTerrainActor() override;
    PhysicsTerrainActor(const PhysicsTerrainActor& Other);
    PhysicsTerrainActor& operator=(const PhysicsTerrainActor& Other);
    PhysicsTerrainActor(PhysicsTerrainActor&& Other) noexcept;
    PhysicsTerrainActor& operator=(PhysicsTerrainActor&& Other) noexcept;

    explicit PhysicsTerrainActor(const ActorDesc& Desc);

public:
    void SetActorDesc(const ActorDesc& Desc);
    ActorDesc GetActorDesc() const;

    bool TryGetSurfaceHeightAtWorldPosition(float WorldX, float WorldZ, float& OutWorldHeight) const;
    bool ResolveDynamicCollision(PhysicsActorBase& DynamicActor, float DeltaTime) const override;
    std::unique_ptr<PhysicsActorBase> Clone() const override;

private:
    bool TryGetSurfaceHeightAtLocalPosition(float LocalX, float LocalZ, float& OutLocalHeight) const;
    bool TryGetSurfaceNormalAtLocalPosition(float LocalX, float LocalZ, DirectX::SimpleMath::Vector3& OutLocalNormal) const;
    std::size_t CalculateHeightFieldIndex(std::uint32_t X, std::uint32_t Z) const;

private:
    float mHalfExtentX;
    float mHalfExtentZ;
    std::uint32_t mHeightFieldWidth;
    std::uint32_t mHeightFieldHeight;
    float mHeightFieldCellSpacing;
    float mHeightFieldMaxHeight;
    bool mHeightFieldCenterOrigin;
    std::vector<float> mHeightFieldValues;
};
