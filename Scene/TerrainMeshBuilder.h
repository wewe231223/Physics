#pragma once

#include "Mesh.h"
#include "TerrainMeshTypes.h"

class TerrainMeshBuilder final {
public:
    TerrainMeshBuilder();
    ~TerrainMeshBuilder();
    TerrainMeshBuilder(const TerrainMeshBuilder& Other);
    TerrainMeshBuilder& operator=(const TerrainMeshBuilder& Other);
    TerrainMeshBuilder(TerrainMeshBuilder&& Other) noexcept;
    TerrainMeshBuilder& operator=(TerrainMeshBuilder&& Other) noexcept;

public:
    Mesh Build(const HeightFieldData& Field, const TerrainBuildDesc& Desc) const;

private:
    void ValidateBuildInput(const HeightFieldData& Field, const TerrainBuildDesc& Desc) const;
    std::uint32_t CalculateIndex(std::uint32_t Width, std::uint32_t X, std::uint32_t Y) const;
};
