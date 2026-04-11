#pragma once

#include "../Scene/Scene.h"

#include <string>

struct GLFWwindow;

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
    void ProcessInput(Scene& CurrentScene) const;
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
};
