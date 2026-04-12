#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct TerrainBuildDesc final {
public:
    TerrainBuildDesc();
    ~TerrainBuildDesc();
    TerrainBuildDesc(const TerrainBuildDesc& Other);
    TerrainBuildDesc& operator=(const TerrainBuildDesc& Other);
    TerrainBuildDesc(TerrainBuildDesc&& Other) noexcept;
    TerrainBuildDesc& operator=(TerrainBuildDesc&& Other) noexcept;

public:
    std::string HeightMapPath{};
    float MaxHeight{ 1.0F };
    float CellSpacing{ 1.0F };
    bool CenterOrigin{ true };
};

struct HeightFieldData final {
public:
    HeightFieldData();
    ~HeightFieldData();
    HeightFieldData(const HeightFieldData& Other);
    HeightFieldData& operator=(const HeightFieldData& Other);
    HeightFieldData(HeightFieldData&& Other) noexcept;
    HeightFieldData& operator=(HeightFieldData&& Other) noexcept;

public:
    std::uint32_t Width{};
    std::uint32_t Height{};
    std::vector<float> HeightValues{};
};
