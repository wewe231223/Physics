#include "MeshFactory.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cmath>

#include <glm/ext/scalar_constants.hpp>

namespace {
std::vector<glm::vec3> CreateSolidColors(std::size_t VertexCount, const glm::vec3& Color) {
    std::vector<glm::vec3> Colors{};
    Colors.resize(VertexCount, Color);
    return Colors;
}

Mesh CreateFlatTerrainMesh(int Width, int Height, float CellSpacing) {
    Mesh CreatedMesh{};

    int SafeWidth{ Width < 2 ? 2 : Width };
    int SafeHeight{ Height < 2 ? 2 : Height };
    float HalfWidth{ (static_cast<float>(SafeWidth) - 1.0F) * CellSpacing * 0.5F };
    float HalfDepth{ (static_cast<float>(SafeHeight) - 1.0F) * CellSpacing * 0.5F };
    std::vector<glm::vec3> Vertices{};
    std::vector<glm::vec3> Colors{};
    std::vector<unsigned int> Indices{};
    Vertices.reserve(static_cast<std::size_t>(SafeWidth) * static_cast<std::size_t>(SafeHeight));
    Colors.reserve(static_cast<std::size_t>(SafeWidth) * static_cast<std::size_t>(SafeHeight));

    for (int ZIndex{ 0 }; ZIndex < SafeHeight; ++ZIndex) {
        for (int XIndex{ 0 }; XIndex < SafeWidth; ++XIndex) {
            float PositionX{ static_cast<float>(XIndex) * CellSpacing - HalfWidth };
            float PositionZ{ static_cast<float>(ZIndex) * CellSpacing - HalfDepth };
            Vertices.push_back(glm::vec3{ PositionX, 0.0F, PositionZ });
            Colors.push_back(glm::vec3{ 0.20F, 0.65F, 0.25F });
        }
    }

    int LastX{ SafeWidth - 1 };
    int LastZ{ SafeHeight - 1 };
    Indices.reserve(static_cast<std::size_t>(LastX) * static_cast<std::size_t>(LastZ) * 6U);

    for (int ZIndex{ 0 }; ZIndex < LastZ; ++ZIndex) {
        for (int XIndex{ 0 }; XIndex < LastX; ++XIndex) {
            unsigned int TopLeft{ static_cast<unsigned int>(ZIndex * SafeWidth + XIndex) };
            unsigned int TopRight{ TopLeft + 1U };
            unsigned int BottomLeft{ static_cast<unsigned int>((ZIndex + 1) * SafeWidth + XIndex) };
            unsigned int BottomRight{ BottomLeft + 1U };

            Indices.push_back(TopLeft);
            Indices.push_back(BottomLeft);
            Indices.push_back(TopRight);

            Indices.push_back(TopRight);
            Indices.push_back(BottomLeft);
            Indices.push_back(BottomRight);
        }
    }

    CreatedMesh.SetVertices(Vertices);
    CreatedMesh.SetColors(Colors);
    CreatedMesh.SetIndices(Indices);
    CreatedMesh.SetTopology(MeshTopology::Triangles);

    return CreatedMesh;
}
}

Mesh MeshFactory::CreateCube(float Size) {
    Mesh CreatedMesh{};

    float HalfSize{ Size * 0.5F };
    std::vector<glm::vec3> Vertices{
        glm::vec3{ -HalfSize, -HalfSize, -HalfSize }, glm::vec3{ HalfSize, -HalfSize, -HalfSize }, glm::vec3{ HalfSize, HalfSize, -HalfSize }, glm::vec3{ -HalfSize, HalfSize, -HalfSize },
        glm::vec3{ -HalfSize, -HalfSize, HalfSize }, glm::vec3{ HalfSize, -HalfSize, HalfSize }, glm::vec3{ HalfSize, HalfSize, HalfSize }, glm::vec3{ -HalfSize, HalfSize, HalfSize }
    };

    std::vector<unsigned int> Indices{
        0U, 1U, 2U, 2U, 3U, 0U,
        4U, 5U, 6U, 6U, 7U, 4U,
        0U, 4U, 7U, 7U, 3U, 0U,
        1U, 5U, 6U, 6U, 2U, 1U,
        3U, 2U, 6U, 6U, 7U, 3U,
        0U, 1U, 5U, 5U, 4U, 0U
    };

    CreatedMesh.SetVertices(Vertices);
    CreatedMesh.SetColors(CreateSolidColors(Vertices.size(), glm::vec3{ 0.85F, 0.25F, 0.25F }));
    CreatedMesh.SetIndices(Indices);
    CreatedMesh.SetTopology(MeshTopology::Triangles);

    return CreatedMesh;
}

Mesh MeshFactory::CreateSphere(float Radius, unsigned int SectorCount, unsigned int StackCount) {
    Mesh CreatedMesh{};

    unsigned int SafeSectorCount{ SectorCount < 3U ? 3U : SectorCount };
    unsigned int SafeStackCount{ StackCount < 2U ? 2U : StackCount };
    std::vector<glm::vec3> Vertices{};
    std::vector<unsigned int> Indices{};

    for (unsigned int StackIndex{ 0U }; StackIndex <= SafeStackCount; ++StackIndex) {
        float StackRatio{ static_cast<float>(StackIndex) / static_cast<float>(SafeStackCount) };
        float Phi{ glm::pi<float>() * StackRatio };
        float Y{ std::cos(Phi) * Radius };
        float RadiusAtY{ std::sin(Phi) * Radius };

        for (unsigned int SectorIndex{ 0U }; SectorIndex <= SafeSectorCount; ++SectorIndex) {
            float SectorRatio{ static_cast<float>(SectorIndex) / static_cast<float>(SafeSectorCount) };
            float Theta{ 2.0F * glm::pi<float>() * SectorRatio };
            float X{ RadiusAtY * std::cos(Theta) };
            float Z{ RadiusAtY * std::sin(Theta) };
            Vertices.push_back(glm::vec3{ X, Y, Z });
        }
    }

    unsigned int RingSize{ SafeSectorCount + 1U };
    for (unsigned int StackIndex{ 0U }; StackIndex < SafeStackCount; ++StackIndex) {
        for (unsigned int SectorIndex{ 0U }; SectorIndex < SafeSectorCount; ++SectorIndex) {
            unsigned int Current{ StackIndex * RingSize + SectorIndex };
            unsigned int Next{ Current + RingSize };

            Indices.push_back(Current);
            Indices.push_back(Next);
            Indices.push_back(Current + 1U);

            Indices.push_back(Current + 1U);
            Indices.push_back(Next);
            Indices.push_back(Next + 1U);
        }
    }

    CreatedMesh.SetVertices(Vertices);
    CreatedMesh.SetColors(CreateSolidColors(Vertices.size(), glm::vec3{ 0.25F, 0.45F, 0.90F }));
    CreatedMesh.SetIndices(Indices);
    CreatedMesh.SetTopology(MeshTopology::Triangles);

    return CreatedMesh;
}

Mesh MeshFactory::CreateTriangularPyramid(float Size) {
    Mesh CreatedMesh{};

    float HalfSize{ Size * 0.5F };
    float BaseY{ -HalfSize };
    float TopY{ HalfSize };
    float HalfWidth{ HalfSize };
    float DepthOffset{ Size * 0.288675F };
    float ForwardOffset{ Size * 0.57735F };
    std::vector<glm::vec3> Vertices{
        glm::vec3{ -HalfWidth, BaseY, -DepthOffset }, glm::vec3{ HalfWidth, BaseY, -DepthOffset }, glm::vec3{ 0.0F, BaseY, ForwardOffset }, glm::vec3{ 0.0F, TopY, 0.0F }
    };

    std::vector<unsigned int> Indices{
        0U, 2U, 1U,
        0U, 1U, 3U,
        1U, 2U, 3U,
        2U, 0U, 3U
    };

    CreatedMesh.SetVertices(Vertices);
    CreatedMesh.SetColors(CreateSolidColors(Vertices.size(), glm::vec3{ 0.25F, 0.80F, 0.35F }));
    CreatedMesh.SetIndices(Indices);
    CreatedMesh.SetTopology(MeshTopology::Triangles);

    return CreatedMesh;
}

Mesh MeshFactory::CreateSquarePyramid(float Size) {
    Mesh CreatedMesh{};

    float HalfSize{ Size * 0.5F };
    std::vector<glm::vec3> Vertices{
        glm::vec3{ -HalfSize, -HalfSize, -HalfSize }, glm::vec3{ HalfSize, -HalfSize, -HalfSize }, glm::vec3{ HalfSize, -HalfSize, HalfSize }, glm::vec3{ -HalfSize, -HalfSize, HalfSize }, glm::vec3{ 0.0F, HalfSize, 0.0F }
    };

    std::vector<unsigned int> Indices{
        0U, 1U, 2U, 2U, 3U, 0U,
        0U, 1U, 4U,
        1U, 2U, 4U,
        2U, 3U, 4U,
        3U, 0U, 4U
    };

    CreatedMesh.SetVertices(Vertices);
    CreatedMesh.SetColors(CreateSolidColors(Vertices.size(), glm::vec3{ 0.65F, 0.35F, 0.85F }));
    CreatedMesh.SetIndices(Indices);
    CreatedMesh.SetTopology(MeshTopology::Triangles);

    return CreatedMesh;
}

Mesh MeshFactory::CreateGrid(float HalfExtent, unsigned int DivisionCount) {
    Mesh CreatedMesh{};

    unsigned int SafeDivisionCount{ DivisionCount < 1U ? 1U : DivisionCount };
    float CellCount{ static_cast<float>(SafeDivisionCount) };
    float Step{ (HalfExtent * 2.0F) / CellCount };
    std::vector<glm::vec3> Vertices{};
    std::vector<unsigned int> Indices{};

    for (unsigned int DivisionIndex{ 0U }; DivisionIndex <= SafeDivisionCount; ++DivisionIndex) {
        float Offset{ -HalfExtent + Step * static_cast<float>(DivisionIndex) };

        unsigned int StartIndexX{ static_cast<unsigned int>(Vertices.size()) };
        Vertices.push_back(glm::vec3{ -HalfExtent, 0.0F, Offset });
        Vertices.push_back(glm::vec3{ HalfExtent, 0.0F, Offset });
        Indices.push_back(StartIndexX);
        Indices.push_back(StartIndexX + 1U);

        unsigned int StartIndexZ{ static_cast<unsigned int>(Vertices.size()) };
        Vertices.push_back(glm::vec3{ Offset, 0.0F, -HalfExtent });
        Vertices.push_back(glm::vec3{ Offset, 0.0F, HalfExtent });
        Indices.push_back(StartIndexZ);
        Indices.push_back(StartIndexZ + 1U);
    }

    CreatedMesh.SetVertices(Vertices);
    CreatedMesh.SetColors(CreateSolidColors(Vertices.size(), glm::vec3{ 0.50F, 0.50F, 0.50F }));
    CreatedMesh.SetIndices(Indices);
    CreatedMesh.SetTopology(MeshTopology::Lines);

    return CreatedMesh;
}

Mesh MeshFactory::CreateBoundingBox() {
    Mesh CreatedMesh{};
    std::vector<glm::vec3> Vertices{
        glm::vec3{ -0.5F, -0.5F, -0.5F }, glm::vec3{ 0.5F, -0.5F, -0.5F }, glm::vec3{ 0.5F, 0.5F, -0.5F }, glm::vec3{ -0.5F, 0.5F, -0.5F },
        glm::vec3{ -0.5F, -0.5F, 0.5F }, glm::vec3{ 0.5F, -0.5F, 0.5F }, glm::vec3{ 0.5F, 0.5F, 0.5F }, glm::vec3{ -0.5F, 0.5F, 0.5F }
    };
    std::vector<unsigned int> Indices{
        0U, 1U, 1U, 2U, 2U, 3U, 3U, 0U,
        4U, 5U, 5U, 6U, 6U, 7U, 7U, 4U,
        0U, 4U, 1U, 5U, 2U, 6U, 3U, 7U
    };

    CreatedMesh.SetVertices(Vertices);
    CreatedMesh.SetColors(CreateSolidColors(Vertices.size(), glm::vec3{ 0.95F, 0.95F, 0.95F }));
    CreatedMesh.SetIndices(Indices);
    CreatedMesh.SetTopology(MeshTopology::Lines);
    return CreatedMesh;
}

Mesh MeshFactory::CreateTerrainFromHeightMapPng(const std::string& FilePath, float MaxHeight, float CellSpacing) {
    int ImageWidth{};
    int ImageHeight{};
    int ImageChannels{};
    unsigned char* ImageData{ stbi_load(FilePath.c_str(), &ImageWidth, &ImageHeight, &ImageChannels, 1) };

    if (ImageData == nullptr) {
        Mesh FallbackMesh{ CreateFlatTerrainMesh(64, 48, CellSpacing) };
        return FallbackMesh;
    }

    if (ImageWidth < 2 || ImageHeight < 2) {
        stbi_image_free(ImageData);
        Mesh FallbackMesh{ CreateFlatTerrainMesh(64, 48, CellSpacing) };
        return FallbackMesh;
    }

    float HalfWidth{ (static_cast<float>(ImageWidth) - 1.0F) * CellSpacing * 0.5F };
    float HalfDepth{ (static_cast<float>(ImageHeight) - 1.0F) * CellSpacing * 0.5F };
    std::vector<glm::vec3> Vertices{};
    std::vector<glm::vec3> Colors{};
    std::vector<unsigned int> Indices{};
    Vertices.reserve(static_cast<std::size_t>(ImageWidth) * static_cast<std::size_t>(ImageHeight));
    Colors.reserve(static_cast<std::size_t>(ImageWidth) * static_cast<std::size_t>(ImageHeight));

    for (int ZIndex{ 0 }; ZIndex < ImageHeight; ++ZIndex) {
        for (int XIndex{ 0 }; XIndex < ImageWidth; ++XIndex) {
            int PixelIndex{ ZIndex * ImageWidth + XIndex };
            float NormalizedHeight{ static_cast<float>(ImageData[PixelIndex]) / 255.0F };
            float HeightValue{ NormalizedHeight * MaxHeight };
            float PositionX{ static_cast<float>(XIndex) * CellSpacing - HalfWidth };
            float PositionZ{ static_cast<float>(ZIndex) * CellSpacing - HalfDepth };
            Vertices.push_back(glm::vec3{ PositionX, HeightValue, PositionZ });
            Colors.push_back(glm::vec3{ 0.10F + NormalizedHeight * 0.25F, 0.30F + NormalizedHeight * 0.55F, 0.10F + NormalizedHeight * 0.20F });
        }
    }

    int LastX{ ImageWidth - 1 };
    int LastZ{ ImageHeight - 1 };
    Indices.reserve(static_cast<std::size_t>(LastX) * static_cast<std::size_t>(LastZ) * 6U);

    for (int ZIndex{ 0 }; ZIndex < LastZ; ++ZIndex) {
        for (int XIndex{ 0 }; XIndex < LastX; ++XIndex) {
            unsigned int TopLeft{ static_cast<unsigned int>(ZIndex * ImageWidth + XIndex) };
            unsigned int TopRight{ TopLeft + 1U };
            unsigned int BottomLeft{ static_cast<unsigned int>((ZIndex + 1) * ImageWidth + XIndex) };
            unsigned int BottomRight{ BottomLeft + 1U };

            Indices.push_back(TopLeft);
            Indices.push_back(BottomLeft);
            Indices.push_back(TopRight);

            Indices.push_back(TopRight);
            Indices.push_back(BottomLeft);
            Indices.push_back(BottomRight);
        }
    }

    stbi_image_free(ImageData);

    Mesh CreatedMesh{};
    CreatedMesh.SetVertices(Vertices);
    CreatedMesh.SetColors(Colors);
    CreatedMesh.SetIndices(Indices);
    CreatedMesh.SetTopology(MeshTopology::Triangles);

    return CreatedMesh;
}
