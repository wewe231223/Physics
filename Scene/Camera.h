#pragma once

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

private:
    float mClearRed;
    float mClearGreen;
    float mClearBlue;
    float mClearAlpha;
};
