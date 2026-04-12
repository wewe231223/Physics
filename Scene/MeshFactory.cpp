#include "MeshFactory.h"

#include <cmath>

#include <glm/ext/scalar_constants.hpp>

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
    CreatedMesh.SetIndices(Indices);
    CreatedMesh.SetTopology(MeshTopology::Lines);
    return CreatedMesh;
}
