#pragma once

#include <SimpleMath/SimpleMath.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace RendererMathConversion {
    DirectX::SimpleMath::Vector3 ToSimpleMathVector3(const glm::vec3& Value);
    DirectX::SimpleMath::Vector4 ToSimpleMathVector4(const glm::vec4& Value);
    DirectX::SimpleMath::Matrix ToSimpleMathMatrix(const glm::mat4& Value);

    glm::vec3 ToGlmVector3(const DirectX::SimpleMath::Vector3& Value);
    glm::vec4 ToGlmVector4(const DirectX::SimpleMath::Vector4& Value);
    glm::mat4 ToGlmMatrix(const DirectX::SimpleMath::Matrix& Value);
}
