#pragma once

#include <SimpleMath/SimpleMath.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "PhysicsActor.h"

class PhysicsTerrainActor final : public PhysicsActor {
public:
    struct ActorDesc {
        std::string Name{};
        bool IsActive{};
        PhysicsActorFlags Flags{};
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

    explicit PhysicsTerrainActor(std::string Name);
    explicit PhysicsTerrainActor(const ActorDesc& Desc);

public:
    void SetPosition(const DirectX::SimpleMath::Vector3& Position);
    const DirectX::SimpleMath::Vector3& GetPosition() const;

    void SetRotation(const DirectX::SimpleMath::Vector3& Rotation);
    const DirectX::SimpleMath::Vector3& GetRotation() const;

    void SetScale(const DirectX::SimpleMath::Vector3& Scale);
    const DirectX::SimpleMath::Vector3& GetScale() const;

    void SetHalfExtentX(float HalfExtentX);
    float GetHalfExtentX() const;

    void SetHalfExtentZ(float HalfExtentZ);
    float GetHalfExtentZ() const;

    void SetHeightFieldWidth(std::uint32_t HeightFieldWidth);
    std::uint32_t GetHeightFieldWidth() const;
    void SetHeightFieldHeight(std::uint32_t HeightFieldHeight);
    std::uint32_t GetHeightFieldHeight() const;
    void SetHeightFieldCellSpacing(float HeightFieldCellSpacing);
    float GetHeightFieldCellSpacing() const;
    void SetHeightFieldMaxHeight(float HeightFieldMaxHeight);
    float GetHeightFieldMaxHeight() const;
    void SetHeightFieldCenterOrigin(bool HeightFieldCenterOrigin);
    bool GetHeightFieldCenterOrigin() const;
    void SetHeightFieldValues(const std::vector<float>& HeightFieldValues);
    const std::vector<float>& GetHeightFieldValues() const;

    bool TryGetSurfaceHeightAtWorldPosition(float WorldX, float WorldZ, float& OutWorldHeight) const;

private:
    bool TryGetSurfaceHeightAtLocalPosition(float LocalX, float LocalZ, float& OutLocalHeight) const;
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
