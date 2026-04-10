#include "Camera.h"

Camera::Camera()
    : mClearRed{ 0.1F },
      mClearGreen{ 0.2F },
      mClearBlue{ 0.3F },
      mClearAlpha{ 1.0F } {
}

Camera::~Camera() {
}

Camera::Camera(const Camera& Other)
    : mClearRed{ Other.mClearRed },
      mClearGreen{ Other.mClearGreen },
      mClearBlue{ Other.mClearBlue },
      mClearAlpha{ Other.mClearAlpha } {
}

Camera& Camera::operator=(const Camera& Other) {
    if (this == &Other) {
        return *this;
    }

    mClearRed = Other.mClearRed;
    mClearGreen = Other.mClearGreen;
    mClearBlue = Other.mClearBlue;
    mClearAlpha = Other.mClearAlpha;

    return *this;
}

Camera::Camera(Camera&& Other) noexcept
    : mClearRed{ Other.mClearRed },
      mClearGreen{ Other.mClearGreen },
      mClearBlue{ Other.mClearBlue },
      mClearAlpha{ Other.mClearAlpha } {
}

Camera& Camera::operator=(Camera&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mClearRed = Other.mClearRed;
    mClearGreen = Other.mClearGreen;
    mClearBlue = Other.mClearBlue;
    mClearAlpha = Other.mClearAlpha;

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
