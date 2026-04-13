#pragma once

#include <SimpleMath/SimpleMath.h>
#include <DirectXCollision.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "PhysicsStaticActor.h"

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
    bool ResolveDynamicCollision(const DirectX::BoundingOrientedBox& PredictedWorldBoundingBox, float DynamicInverseMass, float DynamicFriction, float DynamicRestitution, DirectX::SimpleMath::Vector3& CorrectedPosition, DirectX::SimpleMath::Vector3& CorrectedVelocity) const override;

private:
    bool TryGetSurfaceHeightAtLocalPosition(float LocalX, float LocalZ, float& OutLocalHeight) const;
    bool TryGetSurfaceNormalAtLocalPosition(float LocalX, float LocalZ, DirectX::SimpleMath::Vector3& OutLocalNormal) const;
    std::size_t CalculateHeightFieldIndex(std::uint32_t X, std::uint32_t Z) const;

private:
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector3 mRotation;
    DirectX::SimpleMath::Vector3 mScale;
    float mHalfExtentX;
    float mHalfExtentZ;
    std::uint32_t mHeightFieldWidth;
    std::uint32_t mHeightFieldHeight;
    float mHeightFieldCellSpacing;
    float mHeightFieldMaxHeight;
    bool mHeightFieldCenterOrigin;
    std::vector<float> mHeightFieldValues;
};
