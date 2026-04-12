#include "TerrainHeightMapLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace {
float Clamp01(float Value) {
    float ClampedValue{ std::clamp(Value, 0.0F, 1.0F) };
    return ClampedValue;
}

float ComputeLuminance01From8Bit(const stbi_uc* PixelPointer, int ChannelCount) {
    float Inverse255{ 1.0F / 255.0F };
    if (ChannelCount == 1 || ChannelCount == 2) {
        float Height01{ static_cast<float>(PixelPointer[0]) * Inverse255 };
        return Clamp01(Height01);
    }

    float R{ static_cast<float>(PixelPointer[0]) * Inverse255 };
    float G{ static_cast<float>(PixelPointer[1]) * Inverse255 };
    float B{ static_cast<float>(PixelPointer[2]) * Inverse255 };
    float Height01{ (0.2126F * R) + (0.7152F * G) + (0.0722F * B) };
    return Clamp01(Height01);
}

float ComputeLuminance01From16Bit(const stbi_us* PixelPointer, int ChannelCount) {
    float Inverse65535{ 1.0F / 65535.0F };
    if (ChannelCount == 1 || ChannelCount == 2) {
        float Height01{ static_cast<float>(PixelPointer[0]) * Inverse65535 };
        return Clamp01(Height01);
    }

    float R{ static_cast<float>(PixelPointer[0]) * Inverse65535 };
    float G{ static_cast<float>(PixelPointer[1]) * Inverse65535 };
    float B{ static_cast<float>(PixelPointer[2]) * Inverse65535 };
    float Height01{ (0.2126F * R) + (0.7152F * G) + (0.0722F * B) };
    return Clamp01(Height01);
}

std::size_t CalculatePixelCount(std::uint32_t Width, std::uint32_t Height) {
    std::size_t PixelCount{ static_cast<std::size_t>(Width) * static_cast<std::size_t>(Height) };
    return PixelCount;
}

HeightFieldData LoadHeightField01From8BitImage(const std::string& FilePath) {
    int WidthInt{};
    int HeightInt{};
    int ChannelCount{};
    stbi_uc* ImageData{ stbi_load(FilePath.c_str(), &WidthInt, &HeightInt, &ChannelCount, 0) };
    if (ImageData == nullptr) {
        throw std::runtime_error{ "Height map decode failed." };
    }

    HeightFieldData Field{};
    Field.Width = static_cast<std::uint32_t>(WidthInt);
    Field.Height = static_cast<std::uint32_t>(HeightInt);
    if (Field.Width < 2 || Field.Height < 2) {
        stbi_image_free(ImageData);
        throw std::runtime_error{ "Height map size must be at least 2x2." };
    }

    if (ChannelCount <= 0) {
        stbi_image_free(ImageData);
        throw std::runtime_error{ "Invalid channel count in height map." };
    }

    std::size_t PixelCount{ CalculatePixelCount(Field.Width, Field.Height) };
    std::size_t Stride{ static_cast<std::size_t>(ChannelCount) };
    Field.HeightValues.resize(PixelCount);

    for (std::size_t PixelIndex{}; PixelIndex < PixelCount; ++PixelIndex) {
        const stbi_uc* PixelPointer{ ImageData + (PixelIndex * Stride) };
        Field.HeightValues[PixelIndex] = ComputeLuminance01From8Bit(PixelPointer, ChannelCount);
    }

    stbi_image_free(ImageData);
    return Field;
}

HeightFieldData LoadHeightField01From16BitImage(const std::string& FilePath) {
    int WidthInt{};
    int HeightInt{};
    int ChannelCount{};
    stbi_us* ImageData{ stbi_load_16(FilePath.c_str(), &WidthInt, &HeightInt, &ChannelCount, 0) };
    if (ImageData == nullptr) {
        throw std::runtime_error{ "Height map decode failed." };
    }

    HeightFieldData Field{};
    Field.Width = static_cast<std::uint32_t>(WidthInt);
    Field.Height = static_cast<std::uint32_t>(HeightInt);
    if (Field.Width < 2 || Field.Height < 2) {
        stbi_image_free(ImageData);
        throw std::runtime_error{ "Height map size must be at least 2x2." };
    }

    if (ChannelCount <= 0) {
        stbi_image_free(ImageData);
        throw std::runtime_error{ "Invalid channel count in height map." };
    }

    std::size_t PixelCount{ CalculatePixelCount(Field.Width, Field.Height) };
    std::size_t Stride{ static_cast<std::size_t>(ChannelCount) };
    Field.HeightValues.resize(PixelCount);

    for (std::size_t PixelIndex{}; PixelIndex < PixelCount; ++PixelIndex) {
        const stbi_us* PixelPointer{ ImageData + (PixelIndex * Stride) };
        Field.HeightValues[PixelIndex] = ComputeLuminance01From16Bit(PixelPointer, ChannelCount);
    }

    stbi_image_free(ImageData);
    return Field;
}
}

TerrainHeightMapLoader::TerrainHeightMapLoader() {
}

TerrainHeightMapLoader::~TerrainHeightMapLoader() {
}

TerrainHeightMapLoader::TerrainHeightMapLoader(const TerrainHeightMapLoader& Other) {
    (void)Other;
}

TerrainHeightMapLoader& TerrainHeightMapLoader::operator=(const TerrainHeightMapLoader& Other) {
    (void)Other;
    return *this;
}

TerrainHeightMapLoader::TerrainHeightMapLoader(TerrainHeightMapLoader&& Other) noexcept {
    (void)Other;
}

TerrainHeightMapLoader& TerrainHeightMapLoader::operator=(TerrainHeightMapLoader&& Other) noexcept {
    (void)Other;
    return *this;
}

HeightFieldData TerrainHeightMapLoader::LoadHeightField(const std::string& FilePath) const {
    if (FilePath.empty() == true) {
        throw std::runtime_error{ "Height map path is empty." };
    }

    int Is16Bit{ stbi_is_16_bit(FilePath.c_str()) };
    if (Is16Bit != 0) {
        return LoadHeightField01From16BitImage(FilePath);
    }

    return LoadHeightField01From8BitImage(FilePath);
}
