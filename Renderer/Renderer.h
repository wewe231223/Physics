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
    void Run(const Scene& CurrentScene);
    void Shutdown();

private:
    bool InitializeGlfw();
    bool CreateWindow();
    bool InitializeGlad();
    void ProcessInput() const;
    void RenderFrame(const Scene& CurrentScene) const;

private:
    GLFWwindow* mWindow;
    int mWidth;
    int mHeight;
    std::string mTitle;
    bool mIsInitialized;
};
