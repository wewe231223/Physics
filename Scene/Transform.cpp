#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>

Transform::Transform()
    : mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F } {
}

Transform::~Transform() {
}

Transform::Transform(const Transform& Other)
    : mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale } {
}

Transform& Transform::operator=(const Transform& Other) {
    if (this == &Other) {
        return *this;
    }

    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;

    return *this;
}

Transform::Transform(Transform&& Other) noexcept
    : mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale } {
}

Transform& Transform::operator=(Transform&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;

    return *this;
}

void Transform::SetPosition(const glm::vec3& Position) {
    mPosition = Position;
}

const glm::vec3& Transform::GetPosition() const {
    return mPosition;
}

void Transform::SetRotation(const glm::vec3& Rotation) {
    mRotation = Rotation;
}

const glm::vec3& Transform::GetRotation() const {
    return mRotation;
}

void Transform::SetScale(const glm::vec3& Scale) {
    mScale = Scale;
}

const glm::vec3& Transform::GetScale() const {
    return mScale;
}

glm::mat4 Transform::GetWorldMatrix() const {
    glm::mat4 TranslationMatrix{ glm::translate(glm::mat4{ 1.0F }, mPosition) };
    glm::mat4 RotationXMatrix{ glm::rotate(glm::mat4{ 1.0F }, mRotation.x, glm::vec3{ 1.0F, 0.0F, 0.0F }) };
    glm::mat4 RotationYMatrix{ glm::rotate(glm::mat4{ 1.0F }, mRotation.y, glm::vec3{ 0.0F, 1.0F, 0.0F }) };
    glm::mat4 RotationZMatrix{ glm::rotate(glm::mat4{ 1.0F }, mRotation.z, glm::vec3{ 0.0F, 0.0F, 1.0F }) };
    glm::mat4 ScaleMatrix{ glm::scale(glm::mat4{ 1.0F }, mScale) };
    glm::mat4 WorldMatrix{ TranslationMatrix * RotationZMatrix * RotationYMatrix * RotationXMatrix * ScaleMatrix };
    return WorldMatrix;
}
