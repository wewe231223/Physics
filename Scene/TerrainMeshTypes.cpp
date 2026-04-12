#include "TerrainMeshTypes.h"

#include <utility>

TerrainBuildDesc::TerrainBuildDesc() {
}

TerrainBuildDesc::~TerrainBuildDesc() {
}

TerrainBuildDesc::TerrainBuildDesc(const TerrainBuildDesc& Other)
    : HeightMapPath{ Other.HeightMapPath },
      MaxHeight{ Other.MaxHeight },
      CellSpacing{ Other.CellSpacing },
      CenterOrigin{ Other.CenterOrigin } {
}

TerrainBuildDesc& TerrainBuildDesc::operator=(const TerrainBuildDesc& Other) {
    if (this == &Other) {
        return *this;
    }

    HeightMapPath = Other.HeightMapPath;
    MaxHeight = Other.MaxHeight;
    CellSpacing = Other.CellSpacing;
    CenterOrigin = Other.CenterOrigin;

    return *this;
}

TerrainBuildDesc::TerrainBuildDesc(TerrainBuildDesc&& Other) noexcept
    : HeightMapPath{ std::move(Other.HeightMapPath) },
      MaxHeight{ Other.MaxHeight },
      CellSpacing{ Other.CellSpacing },
      CenterOrigin{ Other.CenterOrigin } {
}

TerrainBuildDesc& TerrainBuildDesc::operator=(TerrainBuildDesc&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    HeightMapPath = std::move(Other.HeightMapPath);
    MaxHeight = Other.MaxHeight;
    CellSpacing = Other.CellSpacing;
    CenterOrigin = Other.CenterOrigin;

    return *this;
}

HeightFieldData::HeightFieldData() {
}

HeightFieldData::~HeightFieldData() {
}

HeightFieldData::HeightFieldData(const HeightFieldData& Other)
    : Width{ Other.Width },
      Height{ Other.Height },
      HeightValues{ Other.HeightValues } {
}

HeightFieldData& HeightFieldData::operator=(const HeightFieldData& Other) {
    if (this == &Other) {
        return *this;
    }

    Width = Other.Width;
    Height = Other.Height;
    HeightValues = Other.HeightValues;

    return *this;
}

HeightFieldData::HeightFieldData(HeightFieldData&& Other) noexcept
    : Width{ Other.Width },
      Height{ Other.Height },
      HeightValues{ std::move(Other.HeightValues) } {
}

HeightFieldData& HeightFieldData::operator=(HeightFieldData&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    Width = Other.Width;
    Height = Other.Height;
    HeightValues = std::move(Other.HeightValues);

    return *this;
}
