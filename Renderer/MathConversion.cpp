#include "MathConversion.h"

namespace RendererMathConversion {
    DirectX::SimpleMath::Vector3 ToSimpleMathVector3(const glm::vec3& Value) {
        DirectX::SimpleMath::Vector3 ConvertedValue{ Value.x, Value.y, Value.z };
        return ConvertedValue;
    }

    DirectX::SimpleMath::Vector4 ToSimpleMathVector4(const glm::vec4& Value) {
        DirectX::SimpleMath::Vector4 ConvertedValue{ Value.x, Value.y, Value.z, Value.w };
        return ConvertedValue;
    }

    DirectX::SimpleMath::Matrix ToSimpleMathMatrix(const glm::mat4& Value) {
        DirectX::SimpleMath::Matrix ConvertedValue{
            Value[0][0], Value[0][1], Value[0][2], Value[0][3],
            Value[1][0], Value[1][1], Value[1][2], Value[1][3],
            Value[2][0], Value[2][1], Value[2][2], Value[2][3],
            Value[3][0], Value[3][1], Value[3][2], Value[3][3]
        };
        return ConvertedValue;
    }

    glm::vec3 ToGlmVector3(const DirectX::SimpleMath::Vector3& Value) {
        glm::vec3 ConvertedValue{ Value.x, Value.y, Value.z };
        return ConvertedValue;
    }

    glm::vec4 ToGlmVector4(const DirectX::SimpleMath::Vector4& Value) {
        glm::vec4 ConvertedValue{ Value.x, Value.y, Value.z, Value.w };
        return ConvertedValue;
    }

    glm::mat4 ToGlmMatrix(const DirectX::SimpleMath::Matrix& Value) {
        glm::mat4 ConvertedValue{
            Value._11, Value._12, Value._13, Value._14,
            Value._21, Value._22, Value._23, Value._24,
            Value._31, Value._32, Value._33, Value._34,
            Value._41, Value._42, Value._43, Value._44
        };
        return ConvertedValue;
    }
}
