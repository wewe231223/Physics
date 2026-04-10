#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Transform final {
public:
    Transform();
    ~Transform();
    Transform(const Transform& Other);
    Transform& operator=(const Transform& Other);
    Transform(Transform&& Other) noexcept;
    Transform& operator=(Transform&& Other) noexcept;

public:
    void SetPosition(const glm::vec3& Position);
    const glm::vec3& GetPosition() const;

    void SetRotation(const glm::vec3& Rotation);
    const glm::vec3& GetRotation() const;

    void SetScale(const glm::vec3& Scale);
    const glm::vec3& GetScale() const;

    glm::mat4 GetWorldMatrix() const;

private:
    glm::vec3 mPosition;
    glm::vec3 mRotation;
    glm::vec3 mScale;
};
