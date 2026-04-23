#pragma once

#include "Scene/Scene.h"

#include <cstddef>
#include <string>

#include <glm/vec3.hpp>

struct GLFWwindow;

#undef CreateWindow

class Renderer final {
public:
    Renderer();
    ~Renderer();
    Renderer(const Renderer& Other);
    Renderer& operator=(const Renderer& Other);
    Renderer(Renderer&& Other) noexcept;
    Renderer& operator=(Renderer&& Other) noexcept;

public:
    bool Initialize();
    bool ShouldClose() const;
    void ProcessInput(Scene& CurrentScene, std::size_t& ActiveSceneIndex, bool& ShouldRestartActiveScene, glm::vec3& KinematicMoveDirection);
    bool GetIsKinematicControlMode() const;
    void SetWindowTitle(const std::string& Title);
    void RenderFrame(const Scene& CurrentScene) const;
    void Present() const;
    void PollEvents() const;
    void Shutdown();

private:
    bool InitializeGlfw();
    bool CreateWindow();
    bool InitializeGlad();
    bool CreateShaderProgram();
    void DestroyShaderProgram();
    unsigned int CreateShader(unsigned int ShaderType, const std::string& ShaderSource) const;

private:
    GLFWwindow* mWindow;
    int mWidth;
    int mHeight;
    std::string mTitle;
    bool mIsInitialized;
    unsigned int mShaderProgram;
    bool mWasKey1Pressed;
    bool mWasKey2Pressed;
    bool mWasKey3Pressed;
    bool mWasKeyF2Pressed;
    bool mIsKinematicControlMode;
};
