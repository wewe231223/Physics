#include "Renderer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <utility>

void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height) {
    (void)Window;
    glViewport(0, 0, Width, Height);
}

Renderer::Renderer()
    : mWindow{},
      mWidth{ 1280 },
      mHeight{ 720 },
      mTitle{ "Physics" },
      mIsInitialized{} {
}

Renderer::~Renderer() {
    Shutdown();
}

Renderer::Renderer(const Renderer& Other)
    : mWindow{},
      mWidth{ Other.mWidth },
      mHeight{ Other.mHeight },
      mTitle{ Other.mTitle },
      mIsInitialized{} {
}

Renderer& Renderer::operator=(const Renderer& Other) {
    if (this == &Other) {
        return *this;
    }

    Shutdown();

    mWindow = nullptr;
    mWidth = Other.mWidth;
    mHeight = Other.mHeight;
    mTitle = Other.mTitle;
    mIsInitialized = false;

    return *this;
}

Renderer::Renderer(Renderer&& Other) noexcept
    : mWindow{ Other.mWindow },
      mWidth{ Other.mWidth },
      mHeight{ Other.mHeight },
      mTitle{ std::move(Other.mTitle) },
      mIsInitialized{ Other.mIsInitialized } {
    Other.mWindow = nullptr;
    Other.mWidth = 0;
    Other.mHeight = 0;
    Other.mTitle = "";
    Other.mIsInitialized = false;
}

Renderer& Renderer::operator=(Renderer&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    Shutdown();

    mWindow = Other.mWindow;
    mWidth = Other.mWidth;
    mHeight = Other.mHeight;
    mTitle = std::move(Other.mTitle);
    mIsInitialized = Other.mIsInitialized;

    Other.mWindow = nullptr;
    Other.mWidth = 0;
    Other.mHeight = 0;
    Other.mTitle = "";
    Other.mIsInitialized = false;

    return *this;
}

bool Renderer::Initialize() {
    if (mIsInitialized) {
        return true;
    }

    if (!InitializeGlfw()) {
        return false;
    }

    if (!CreateWindow()) {
        glfwTerminate();
        return false;
    }

    if (!InitializeGlad()) {
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
        glfwTerminate();
        return false;
    }

    glViewport(0, 0, mWidth, mHeight);
    glfwSetFramebufferSizeCallback(mWindow, FramebufferSizeCallback);

    mIsInitialized = true;

    return true;
}

void Renderer::Run(const Scene& CurrentScene) {
    if (!mIsInitialized) {
        return;
    }

    while (glfwWindowShouldClose(mWindow) == GLFW_FALSE) {
        ProcessInput();
        RenderFrame(CurrentScene);
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
}

void Renderer::Shutdown() {
    if (mWindow != nullptr) {
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
    }

    if (mIsInitialized) {
        glfwTerminate();
        mIsInitialized = false;
    }
}

bool Renderer::InitializeGlfw() {
    if (glfwInit() == GLFW_FALSE) {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return true;
}

bool Renderer::CreateWindow() {
    mWindow = glfwCreateWindow(mWidth, mHeight, mTitle.c_str(), nullptr, nullptr);

    if (mWindow == nullptr) {
        return false;
    }

    glfwMakeContextCurrent(mWindow);

    return true;
}

bool Renderer::InitializeGlad() {
    int IsLoaded{ gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) };

    if (IsLoaded == 0) {
        return false;
    }

    return true;
}

void Renderer::ProcessInput() const {
    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
    }
}

void Renderer::RenderFrame(const Scene& CurrentScene) const {
    const Camera& MainCamera{ CurrentScene.GetMainCamera() };

    float ClearRed{};
    float ClearGreen{};
    float ClearBlue{};
    float ClearAlpha{};
    MainCamera.GetClearColor(ClearRed, ClearGreen, ClearBlue, ClearAlpha);

    glClearColor(ClearRed, ClearGreen, ClearBlue, ClearAlpha);
    glClear(GL_COLOR_BUFFER_BIT);
}
