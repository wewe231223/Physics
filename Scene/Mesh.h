#pragma once

#include <vector>

#include <glm/vec3.hpp>

enum class MeshTopology {
    Triangles,
    Lines
};

class Mesh final {
public:
    Mesh();
    ~Mesh();
    Mesh(const Mesh& Other);
    Mesh& operator=(const Mesh& Other);
    Mesh(Mesh&& Other) noexcept;
    Mesh& operator=(Mesh&& Other) noexcept;

public:
    void SetVertices(const std::vector<glm::vec3>& Vertices);
    void SetIndices(const std::vector<unsigned int>& Indices);

    const std::vector<glm::vec3>& GetVertices() const;
    const std::vector<unsigned int>& GetIndices() const;

    void SetTopology(MeshTopology Topology);
    MeshTopology GetTopology() const;

    void EnsureUploaded();
    void Bind() const;
    void Unbind() const;

private:
    void ReleaseGpuResources();

private:
    std::vector<glm::vec3> mVertices;
    std::vector<unsigned int> mIndices;
    MeshTopology mTopology;
    unsigned int mVertexArrayObject;
    unsigned int mVertexBufferObject;
    unsigned int mElementBufferObject;
    bool mIsUploaded;
};
