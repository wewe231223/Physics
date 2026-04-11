#include "Mesh.h"

#include "glad/glad.h"

#include <algorithm>
#include <utility>

namespace {
DirectX::BoundingOrientedBox MakeDefaultBoundingOrientedBox() {
    DirectX::BoundingOrientedBox Box{};
    Box.Center = DirectX::XMFLOAT3{ 0.0F, 0.0F, 0.0F };
    Box.Extents = DirectX::XMFLOAT3{ 0.5F, 0.5F, 0.5F };
    Box.Orientation = DirectX::XMFLOAT4{ 0.0F, 0.0F, 0.0F, 1.0F };
    return Box;
}
}

Mesh::Mesh()
    : mVertices{},
      mIndices{},
      mTopology{ MeshTopology::Triangles },
      mBoundingBox{ MakeDefaultBoundingOrientedBox() },
      mVertexArrayObject{},
      mVertexBufferObject{},
      mElementBufferObject{},
      mIsUploaded{} {
}

Mesh::~Mesh() {
    ReleaseGpuResources();
}

Mesh::Mesh(const Mesh& Other)
    : mVertices{ Other.mVertices },
      mIndices{ Other.mIndices },
      mTopology{ Other.mTopology },
      mBoundingBox{ Other.mBoundingBox },
      mVertexArrayObject{},
      mVertexBufferObject{},
      mElementBufferObject{},
      mIsUploaded{} {
}

Mesh& Mesh::operator=(const Mesh& Other) {
    if (this == &Other) {
        return *this;
    }

    ReleaseGpuResources();

    mVertices = Other.mVertices;
    mIndices = Other.mIndices;
    mTopology = Other.mTopology;
    mBoundingBox = Other.mBoundingBox;
    mVertexArrayObject = 0U;
    mVertexBufferObject = 0U;
    mElementBufferObject = 0U;
    mIsUploaded = false;

    return *this;
}

Mesh::Mesh(Mesh&& Other) noexcept
    : mVertices{ std::move(Other.mVertices) },
      mIndices{ std::move(Other.mIndices) },
      mTopology{ Other.mTopology },
      mBoundingBox{ Other.mBoundingBox },
      mVertexArrayObject{ Other.mVertexArrayObject },
      mVertexBufferObject{ Other.mVertexBufferObject },
      mElementBufferObject{ Other.mElementBufferObject },
      mIsUploaded{ Other.mIsUploaded } {
    Other.mTopology = MeshTopology::Triangles;
    Other.mBoundingBox = MakeDefaultBoundingOrientedBox();
    Other.mVertexArrayObject = 0U;
    Other.mVertexBufferObject = 0U;
    Other.mElementBufferObject = 0U;
    Other.mIsUploaded = false;
}

Mesh& Mesh::operator=(Mesh&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    ReleaseGpuResources();

    mVertices = std::move(Other.mVertices);
    mIndices = std::move(Other.mIndices);
    mTopology = Other.mTopology;
    mBoundingBox = Other.mBoundingBox;
    mVertexArrayObject = Other.mVertexArrayObject;
    mVertexBufferObject = Other.mVertexBufferObject;
    mElementBufferObject = Other.mElementBufferObject;
    mIsUploaded = Other.mIsUploaded;

    Other.mTopology = MeshTopology::Triangles;
    Other.mBoundingBox = MakeDefaultBoundingOrientedBox();
    Other.mVertexArrayObject = 0U;
    Other.mVertexBufferObject = 0U;
    Other.mElementBufferObject = 0U;
    Other.mIsUploaded = false;

    return *this;
}

void Mesh::SetVertices(const std::vector<glm::vec3>& Vertices) {
    mVertices = Vertices;
    RebuildBoundingBoxFromVertices();
    mIsUploaded = false;
}

void Mesh::SetIndices(const std::vector<unsigned int>& Indices) {
    mIndices = Indices;
    mIsUploaded = false;
}

const std::vector<glm::vec3>& Mesh::GetVertices() const {
    return mVertices;
}

const std::vector<unsigned int>& Mesh::GetIndices() const {
    return mIndices;
}

void Mesh::SetTopology(MeshTopology Topology) {
    mTopology = Topology;
}

MeshTopology Mesh::GetTopology() const {
    return mTopology;
}

const DirectX::BoundingOrientedBox& Mesh::GetBoundingBox() const {
    return mBoundingBox;
}

void Mesh::EnsureUploaded() {
    if (mIsUploaded) {
        return;
    }

    if (mVertices.empty()) {
        return;
    }

    ReleaseGpuResources();

    glGenVertexArrays(1, &mVertexArrayObject);
    glGenBuffers(1, &mVertexBufferObject);
    glGenBuffers(1, &mElementBufferObject);

    glBindVertexArray(mVertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(mVertices.size() * sizeof(glm::vec3)), mVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(mIndices.size() * sizeof(unsigned int)), mIndices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(sizeof(glm::vec3)), nullptr);

    glBindVertexArray(0);

    mIsUploaded = true;
}

void Mesh::Bind() const {
    glBindVertexArray(mVertexArrayObject);
}

void Mesh::Unbind() const {
    glBindVertexArray(0);
}

void Mesh::RebuildBoundingBoxFromVertices() {
    if (mVertices.empty()) {
        mBoundingBox = MakeDefaultBoundingOrientedBox();
        return;
    }

    glm::vec3 MinimumVertex{ mVertices.front() };
    glm::vec3 MaximumVertex{ mVertices.front() };

    for (const glm::vec3& CurrentVertex : mVertices) {
        MinimumVertex.x = std::min(MinimumVertex.x, CurrentVertex.x);
        MinimumVertex.y = std::min(MinimumVertex.y, CurrentVertex.y);
        MinimumVertex.z = std::min(MinimumVertex.z, CurrentVertex.z);
        MaximumVertex.x = std::max(MaximumVertex.x, CurrentVertex.x);
        MaximumVertex.y = std::max(MaximumVertex.y, CurrentVertex.y);
        MaximumVertex.z = std::max(MaximumVertex.z, CurrentVertex.z);
    }

    glm::vec3 Center{ (MinimumVertex + MaximumVertex) * 0.5F };
    glm::vec3 Extents{ (MaximumVertex - MinimumVertex) * 0.5F };

    mBoundingBox.Center = DirectX::XMFLOAT3{ Center.x, Center.y, Center.z };
    mBoundingBox.Extents = DirectX::XMFLOAT3{ Extents.x, Extents.y, Extents.z };
    mBoundingBox.Orientation = DirectX::XMFLOAT4{ 0.0F, 0.0F, 0.0F, 1.0F };
}

void Mesh::ReleaseGpuResources() {
    if (mElementBufferObject != 0U) {
        glDeleteBuffers(1, &mElementBufferObject);
        mElementBufferObject = 0U;
    }

    if (mVertexBufferObject != 0U) {
        glDeleteBuffers(1, &mVertexBufferObject);
        mVertexBufferObject = 0U;
    }

    if (mVertexArrayObject != 0U) {
        glDeleteVertexArrays(1, &mVertexArrayObject);
        mVertexArrayObject = 0U;
    }

    mIsUploaded = false;
}
