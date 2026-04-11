#pragma once

#include "Transform.h"

#include <glm/mat4x4.hpp>

class Camera final {
public:
    Camera();
    ~Camera();
    Camera(const Camera& Other);
    Camera& operator=(const Camera& Other);
    Camera(Camera&& Other) noexcept;
    Camera& operator=(Camera&& Other) noexcept;

public:
    void SetClearColor(float Red, float Green, float Blue, float Alpha);
    void GetClearColor(float& OutRed, float& OutGreen, float& OutBlue, float& OutAlpha) const;

    Transform& GetTransform();
    const Transform& GetTransform() const;

    void Move(const glm::vec3& LocalOffset);
    void Rotate(const glm::vec3& RotationOffset);

    glm::mat4 GetViewMatrix() const;

private:
    float mClearRed;
    float mClearGreen;
    float mClearBlue;
    float mClearAlpha;
    Transform mTransform;
};
