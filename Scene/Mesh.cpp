#include "Mesh.h"

#include "glad/glad.h"

#include <utility>

Mesh::Mesh()
    : mVertices{},
      mIndices{},
      mTopology{ MeshTopology::Triangles },
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
      mVertexArrayObject{ Other.mVertexArrayObject },
      mVertexBufferObject{ Other.mVertexBufferObject },
      mElementBufferObject{ Other.mElementBufferObject },
      mIsUploaded{ Other.mIsUploaded } {
    Other.mTopology = MeshTopology::Triangles;
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
    mVertexArrayObject = Other.mVertexArrayObject;
    mVertexBufferObject = Other.mVertexBufferObject;
    mElementBufferObject = Other.mElementBufferObject;
    mIsUploaded = Other.mIsUploaded;

    Other.mTopology = MeshTopology::Triangles;
    Other.mVertexArrayObject = 0U;
    Other.mVertexBufferObject = 0U;
    Other.mElementBufferObject = 0U;
    Other.mIsUploaded = false;

    return *this;
}

void Mesh::SetVertices(const std::vector<glm::vec3>& Vertices) {
    mVertices = Vertices;
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
