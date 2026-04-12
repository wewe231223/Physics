#include "TerrainMeshBuilder.h"

#include <algorithm>
#include <stdexcept>

TerrainMeshBuilder::TerrainMeshBuilder() {
}

TerrainMeshBuilder::~TerrainMeshBuilder() {
}

TerrainMeshBuilder::TerrainMeshBuilder(const TerrainMeshBuilder& Other) {
    (void)Other;
}

TerrainMeshBuilder& TerrainMeshBuilder::operator=(const TerrainMeshBuilder& Other) {
    (void)Other;
    return *this;
}

TerrainMeshBuilder::TerrainMeshBuilder(TerrainMeshBuilder&& Other) noexcept {
    (void)Other;
}

TerrainMeshBuilder& TerrainMeshBuilder::operator=(TerrainMeshBuilder&& Other) noexcept {
    (void)Other;
    return *this;
}

Mesh TerrainMeshBuilder::Build(const HeightFieldData& Field, const TerrainBuildDesc& Desc) const {
    ValidateBuildInput(Field, Desc);

    Mesh CreatedMesh{};

    std::uint32_t VertexCountX{ Field.Width };
    std::uint32_t VertexCountY{ Field.Height };
    std::uint32_t QuadCountX{ Field.Width - 1U };
    std::uint32_t QuadCountY{ Field.Height - 1U };

    std::size_t VertexCount{ static_cast<std::size_t>(VertexCountX) * static_cast<std::size_t>(VertexCountY) };
    std::size_t IndexCount{ static_cast<std::size_t>(QuadCountX) * static_cast<std::size_t>(QuadCountY) * 6ULL };

    std::vector<glm::vec3> Vertices{};
    std::vector<glm::vec3> Colors{};
    std::vector<unsigned int> Indices{};

    Vertices.resize(VertexCount);
    Colors.resize(VertexCount);
    Indices.resize(IndexCount);

    float OffsetX{ (static_cast<float>(Field.Width) - 1.0F) * Desc.CellSpacing * 0.5F };
    float OffsetZ{ (static_cast<float>(Field.Height) - 1.0F) * Desc.CellSpacing * 0.5F };

    for (std::uint32_t GridY{}; GridY < VertexCountY; ++GridY) {
        for (std::uint32_t GridX{}; GridX < VertexCountX; ++GridX) {
            std::uint32_t VertexIndex{ CalculateIndex(Field.Width, GridX, GridY) };
            float PositionX{ static_cast<float>(GridX) * Desc.CellSpacing };
            float PositionZ{ static_cast<float>(GridY) * Desc.CellSpacing };
            if (Desc.CenterOrigin == true) {
                PositionX -= OffsetX;
                PositionZ -= OffsetZ;
            }

            float Height01{ std::clamp(Field.HeightValues[VertexIndex], 0.0F, 1.0F) };
            float PositionY{ Height01 * Desc.MaxHeight };

            Vertices[VertexIndex] = glm::vec3{ PositionX, PositionY, PositionZ };
            Colors[VertexIndex] = glm::vec3{ 0.10F + Height01 * 0.25F, 0.30F + Height01 * 0.55F, 0.10F + Height01 * 0.20F };
        }
    }

    std::size_t IndexWriteCursor{};
    for (std::uint32_t GridY{}; GridY < QuadCountY; ++GridY) {
        for (std::uint32_t GridX{}; GridX < QuadCountX; ++GridX) {
            std::uint32_t Index0{ CalculateIndex(Field.Width, GridX, GridY) };
            std::uint32_t Index1{ CalculateIndex(Field.Width, GridX + 1U, GridY) };
            std::uint32_t Index2{ CalculateIndex(Field.Width, GridX, GridY + 1U) };
            std::uint32_t Index3{ CalculateIndex(Field.Width, GridX + 1U, GridY + 1U) };

            Indices[IndexWriteCursor] = Index0;
            ++IndexWriteCursor;
            Indices[IndexWriteCursor] = Index2;
            ++IndexWriteCursor;
            Indices[IndexWriteCursor] = Index1;
            ++IndexWriteCursor;

            Indices[IndexWriteCursor] = Index1;
            ++IndexWriteCursor;
            Indices[IndexWriteCursor] = Index2;
            ++IndexWriteCursor;
            Indices[IndexWriteCursor] = Index3;
            ++IndexWriteCursor;
        }
    }

    CreatedMesh.SetVertices(Vertices);
    CreatedMesh.SetColors(Colors);
    CreatedMesh.SetIndices(Indices);
    CreatedMesh.SetTopology(MeshTopology::Triangles);

    return CreatedMesh;
}

void TerrainMeshBuilder::ValidateBuildInput(const HeightFieldData& Field, const TerrainBuildDesc& Desc) const {
    if (Desc.MaxHeight <= 0.0F) {
        throw std::runtime_error{ "MaxHeight must be greater than zero." };
    }

    if (Desc.CellSpacing <= 0.0F) {
        throw std::runtime_error{ "CellSpacing must be greater than zero." };
    }

    if (Field.Width < 2U || Field.Height < 2U) {
        throw std::runtime_error{ "Height field size must be at least 2x2." };
    }

    std::size_t ExpectedSize{ static_cast<std::size_t>(Field.Width) * static_cast<std::size_t>(Field.Height) };
    if (Field.HeightValues.size() != ExpectedSize) {
        throw std::runtime_error{ "Height field buffer size mismatch." };
    }
}

std::uint32_t TerrainMeshBuilder::CalculateIndex(std::uint32_t Width, std::uint32_t X, std::uint32_t Y) const {
    std::uint32_t Index{ (Y * Width) + X };
    return Index;
}
