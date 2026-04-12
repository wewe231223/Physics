#pragma once

#include <vector>
#include <cstdint>

#include <glm/vec3.hpp>
#include <DirectXCollision.h>

enum class MeshTopology {
    Triangles,
    Lines
};

class Mesh final {
public:
    struct TerrainSampleDesc final {
        std::uint32_t Width{};
        std::uint32_t Height{};
        float CellSpacing{};
        float MaxHeight{};
        bool CenterOrigin{};
        std::vector<float> HeightValues{};
    };

public:
    Mesh();
    ~Mesh();
    Mesh(const Mesh& Other);
    Mesh& operator=(const Mesh& Other);
    Mesh(Mesh&& Other) noexcept;
    Mesh& operator=(Mesh&& Other) noexcept;

public:
    void SetVertices(const std::vector<glm::vec3>& Vertices);
    void SetColors(const std::vector<glm::vec3>& Colors);
    void SetIndices(const std::vector<unsigned int>& Indices);

    const std::vector<glm::vec3>& GetVertices() const;
    const std::vector<glm::vec3>& GetColors() const;
    const std::vector<unsigned int>& GetIndices() const;

    void SetTopology(MeshTopology Topology);
    MeshTopology GetTopology() const;
    const DirectX::BoundingOrientedBox& GetBoundingBox() const;

    void SetTerrainSampleDesc(const TerrainSampleDesc& TerrainSampleDescValue);
    void ClearTerrainSampleDesc();
    bool HasTerrainSampleDesc() const;
    const TerrainSampleDesc& GetTerrainSampleDesc() const;

    void EnsureUploaded();
    void Bind() const;
    void Unbind() const;

private:
    void RebuildBoundingBoxFromVertices();
    void ReleaseGpuResources();

private:
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mColors;
    std::vector<unsigned int> mIndices;
    MeshTopology mTopology;
    DirectX::BoundingOrientedBox mBoundingBox;
    unsigned int mVertexArrayObject;
    unsigned int mVertexBufferObject;
    unsigned int mColorBufferObject;
    unsigned int mElementBufferObject;
    bool mIsUploaded;
    bool mHasTerrainSampleDesc;
    TerrainSampleDesc mTerrainSampleDesc;
};
