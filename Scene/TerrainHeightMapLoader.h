#pragma once

#include "TerrainMeshTypes.h"

class TerrainHeightMapLoader final {
public:
    TerrainHeightMapLoader();
    ~TerrainHeightMapLoader();
    TerrainHeightMapLoader(const TerrainHeightMapLoader& Other);
    TerrainHeightMapLoader& operator=(const TerrainHeightMapLoader& Other);
    TerrainHeightMapLoader(TerrainHeightMapLoader&& Other) noexcept;
    TerrainHeightMapLoader& operator=(TerrainHeightMapLoader&& Other) noexcept;

public:
    HeightFieldData LoadHeightField(const std::string& FilePath) const;
};
