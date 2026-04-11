#include "Camera.h"

#include <glm/ext/matrix_transform.hpp>

#include <utility>

Camera::Camera()
    : mClearRed{ 0.1F },
      mClearGreen{ 0.2F },
      mClearBlue{ 0.3F },
      mClearAlpha{ 1.0F },
      mTransform{} {
}

Camera::~Camera() {
}

Camera::Camera(const Camera& Other)
    : mClearRed{ Other.mClearRed },
      mClearGreen{ Other.mClearGreen },
      mClearBlue{ Other.mClearBlue },
      mClearAlpha{ Other.mClearAlpha },
      mTransform{ Other.mTransform } {
}

Camera& Camera::operator=(const Camera& Other) {
    if (this == &Other) {
        return *this;
    }

    mClearRed = Other.mClearRed;
    mClearGreen = Other.mClearGreen;
    mClearBlue = Other.mClearBlue;
    mClearAlpha = Other.mClearAlpha;
    mTransform = Other.mTransform;

    return *this;
}

Camera::Camera(Camera&& Other) noexcept
    : mClearRed{ Other.mClearRed },
      mClearGreen{ Other.mClearGreen },
      mClearBlue{ Other.mClearBlue },
      mClearAlpha{ Other.mClearAlpha },
      mTransform{ std::move(Other.mTransform) } {
}

Camera& Camera::operator=(Camera&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mClearRed = Other.mClearRed;
    mClearGreen = Other.mClearGreen;
    mClearBlue = Other.mClearBlue;
    mClearAlpha = Other.mClearAlpha;
    mTransform = std::move(Other.mTransform);

    return *this;
}

void Camera::SetClearColor(float Red, float Green, float Blue, float Alpha) {
    mClearRed = Red;
    mClearGreen = Green;
    mClearBlue = Blue;
    mClearAlpha = Alpha;
}

void Camera::GetClearColor(float& OutRed, float& OutGreen, float& OutBlue, float& OutAlpha) const {
    OutRed = mClearRed;
    OutGreen = mClearGreen;
    OutBlue = mClearBlue;
    OutAlpha = mClearAlpha;
}

Transform& Camera::GetTransform() {
    return mTransform;
}

const Transform& Camera::GetTransform() const {
    return mTransform;
}

void Camera::Move(const glm::vec3& LocalOffset) {
    glm::vec3 Rotation{ mTransform.GetRotation() };
    glm::vec3 Forward{ glm::vec3{ 0.0F, 0.0F, -1.0F } };
    Forward = glm::vec3{ glm::rotate(glm::mat4{ 1.0F }, Rotation.y, glm::vec3{ 0.0F, 1.0F, 0.0F }) * glm::vec4{ Forward, 0.0F } };
    Forward = glm::normalize(Forward);

    glm::vec3 Right{ glm::normalize(glm::cross(Forward, glm::vec3{ 0.0F, 1.0F, 0.0F })) };
    glm::vec3 Up{ glm::normalize(glm::cross(Right, Forward)) };

    glm::vec3 WorldOffset{ Right * LocalOffset.x + Up * LocalOffset.y + Forward * LocalOffset.z };
    glm::vec3 CurrentPosition{ mTransform.GetPosition() };
    mTransform.SetPosition(CurrentPosition + WorldOffset);
}

void Camera::Rotate(const glm::vec3& RotationOffset) {
    glm::vec3 Rotation{ mTransform.GetRotation() };
    Rotation += RotationOffset;
    mTransform.SetRotation(Rotation);
}

glm::mat4 Camera::GetViewMatrix() const {
    glm::vec3 Position{ mTransform.GetPosition() };
    glm::vec3 Rotation{ mTransform.GetRotation() };

    glm::vec3 Forward{ glm::vec3{ 0.0F, 0.0F, -1.0F } };
    Forward = glm::vec3{ glm::rotate(glm::mat4{ 1.0F }, Rotation.x, glm::vec3{ 1.0F, 0.0F, 0.0F }) * glm::vec4{ Forward, 0.0F } };
    Forward = glm::vec3{ glm::rotate(glm::mat4{ 1.0F }, Rotation.y, glm::vec3{ 0.0F, 1.0F, 0.0F }) * glm::vec4{ Forward, 0.0F } };
    Forward = glm::normalize(Forward);

    glm::vec3 Target{ Position + Forward };
    glm::mat4 ViewMatrix{ glm::lookAt(Position, Target, glm::vec3{ 0.0F, 1.0F, 0.0F }) };
    return ViewMatrix;
}
