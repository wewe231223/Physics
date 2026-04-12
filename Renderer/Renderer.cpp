#include "Renderer.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <utility>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height) {
    (void)Window;
    glViewport(0, 0, Width, Height);
}

Renderer::Renderer()
    : mWindow{},
      mWidth{ 1280 },
      mHeight{ 720 },
      mTitle{ "Physics" },
      mIsInitialized{},
      mShaderProgram{} {
}

Renderer::~Renderer() {
    Shutdown();
}

Renderer::Renderer(const Renderer& Other)
    : mWindow{},
      mWidth{ Other.mWidth },
      mHeight{ Other.mHeight },
      mTitle{ Other.mTitle },
      mIsInitialized{},
      mShaderProgram{} {
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
    mShaderProgram = 0U;

    return *this;
}

Renderer::Renderer(Renderer&& Other) noexcept
    : mWindow{ Other.mWindow },
      mWidth{ Other.mWidth },
      mHeight{ Other.mHeight },
      mTitle{ std::move(Other.mTitle) },
      mIsInitialized{ Other.mIsInitialized },
      mShaderProgram{ Other.mShaderProgram } {
    Other.mWindow = nullptr;
    Other.mWidth = 0;
    Other.mHeight = 0;
    Other.mTitle = "";
    Other.mIsInitialized = false;
    Other.mShaderProgram = 0U;
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
    mShaderProgram = Other.mShaderProgram;

    Other.mWindow = nullptr;
    Other.mWidth = 0;
    Other.mHeight = 0;
    Other.mTitle = "";
    Other.mIsInitialized = false;
    Other.mShaderProgram = 0U;

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

    if (!CreateShaderProgram()) {
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
        glfwTerminate();
        return false;
    }

    glViewport(0, 0, mWidth, mHeight);
    glfwSetFramebufferSizeCallback(mWindow, FramebufferSizeCallback);
    glEnable(GL_DEPTH_TEST);

    mIsInitialized = true;

    return true;
}

bool Renderer::ShouldClose() const {
    if (!mIsInitialized) {
        return true;
    }

    return glfwWindowShouldClose(mWindow) == GLFW_TRUE;
}

void Renderer::ProcessInput(Scene& CurrentScene) const {
    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
    }

    Camera& MainCamera{ CurrentScene.GetMainCamera() };
    float MoveSpeed{ 0.7F };
    float RotationSpeed{ 0.01F };

    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS) {
        MainCamera.Move(glm::vec3{ 0.0F, 0.0F, MoveSpeed });
    }

    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS) {
        MainCamera.Move(glm::vec3{ 0.0F, 0.0F, -MoveSpeed });
    }

    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS) {
        MainCamera.Move(glm::vec3{ -MoveSpeed, 0.0F, 0.0F });
    }

    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS) {
        MainCamera.Move(glm::vec3{ MoveSpeed, 0.0F, 0.0F });
    }

    if (glfwGetKey(mWindow, GLFW_KEY_Q) == GLFW_PRESS) {
        MainCamera.Move(glm::vec3{ 0.0F, MoveSpeed, 0.0F });
    }

    if (glfwGetKey(mWindow, GLFW_KEY_E) == GLFW_PRESS) {
        MainCamera.Move(glm::vec3{ 0.0F, -MoveSpeed, 0.0F });
    }

    if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS) {
        MainCamera.Rotate(glm::vec3{ RotationSpeed, 0.0F, 0.0F });
    }

    if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
        MainCamera.Rotate(glm::vec3{ -RotationSpeed, 0.0F, 0.0F });
    }

    if (glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
        MainCamera.Rotate(glm::vec3{ 0.0F, RotationSpeed, 0.0F });
    }

    if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        MainCamera.Rotate(glm::vec3{ 0.0F, -RotationSpeed, 0.0F });
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float AspectRatio{ static_cast<float>(mWidth) / static_cast<float>(mHeight) };
    glm::mat4 ProjectionMatrix{ glm::perspective(glm::radians(60.0F), AspectRatio, 0.1F, 100.0F) };
    glm::mat4 ViewMatrix{ MainCamera.GetViewMatrix() };

    glUseProgram(mShaderProgram);

    int ModelLocation{ glGetUniformLocation(mShaderProgram, "UModel") };
    int ViewLocation{ glGetUniformLocation(mShaderProgram, "UView") };
    int ProjectionLocation{ glGetUniformLocation(mShaderProgram, "UProjection") };
    glUniformMatrix4fv(ViewLocation, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
    glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

    std::size_t GameObjectCount{ CurrentScene.GetGameObjectCount() };
    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
        const GameObject* CurrentObject{ CurrentScene.GetGameObject(ObjectIndex) };

        if (CurrentObject == nullptr) {
            continue;
        }

        if (!CurrentObject->GetIsActive()) {
            continue;
        }

        const std::shared_ptr<Mesh>& CurrentMesh{ CurrentObject->GetMesh() };
        if (CurrentMesh == nullptr) {
            continue;
        }

        CurrentMesh->EnsureUploaded();

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        const glm::mat4& ModelMatrix{ CurrentObject->GetWorldMatrix() };
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(ModelMatrix));

        CurrentMesh->Bind();

        unsigned int DrawMode{ static_cast<unsigned int>(CurrentMesh->GetTopology() == MeshTopology::Lines ? GL_LINES : GL_TRIANGLES) };
        GLsizei IndexCount{ static_cast<GLsizei>(CurrentMesh->GetIndices().size()) };
        glDrawElements(DrawMode, IndexCount, GL_UNSIGNED_INT, nullptr);

        CurrentMesh->Unbind();
    }
}

void Renderer::Present() const {
    glfwSwapBuffers(mWindow);
}

void Renderer::PollEvents() const {
    glfwPollEvents();
}

void Renderer::Shutdown() {
    DestroyShaderProgram();

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

bool Renderer::CreateShaderProgram() {
    std::string VertexShaderSource{ "#version 330 core\nlayout (location = 0) in vec3 InPosition;\nlayout (location = 1) in vec3 InColor;\nout vec3 VertexColor;\nuniform mat4 UModel;\nuniform mat4 UView;\nuniform mat4 UProjection;\nvoid main()\n{\n    gl_Position = UProjection * UView * UModel * vec4(InPosition, 1.0);\n    VertexColor = InColor;\n}\n" };
    std::string FragmentShaderSource{ "#version 330 core\nin vec3 VertexColor;\nout vec4 OutColor;\nvoid main()\n{\n    OutColor = vec4(VertexColor, 1.0);\n}\n" };

    unsigned int VertexShader{ CreateShader(GL_VERTEX_SHADER, VertexShaderSource) };
    unsigned int FragmentShader{ CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSource) };

    if (VertexShader == 0U || FragmentShader == 0U) {
        if (VertexShader != 0U) {
            glDeleteShader(VertexShader);
        }

        if (FragmentShader != 0U) {
            glDeleteShader(FragmentShader);
        }

        return false;
    }

    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, VertexShader);
    glAttachShader(mShaderProgram, FragmentShader);
    glLinkProgram(mShaderProgram);

    int LinkResult{};
    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &LinkResult);

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    if (LinkResult == 0) {
        int LogLength{};
        glGetProgramiv(mShaderProgram, GL_INFO_LOG_LENGTH, &LogLength);
        std::string ErrorLog(static_cast<std::size_t>(LogLength), '\0');
        glGetProgramInfoLog(mShaderProgram, LogLength, nullptr, ErrorLog.data());
        std::cerr << ErrorLog << std::endl;

        glDeleteProgram(mShaderProgram);
        mShaderProgram = 0U;

        return false;
    }

    return true;
}

void Renderer::DestroyShaderProgram() {
    if (mShaderProgram != 0U) {
        glDeleteProgram(mShaderProgram);
        mShaderProgram = 0U;
    }
}

unsigned int Renderer::CreateShader(unsigned int ShaderType, const std::string& ShaderSource) const {
    unsigned int CreatedShader{ glCreateShader(ShaderType) };
    const char* SourcePointer{ ShaderSource.c_str() };
    glShaderSource(CreatedShader, 1, &SourcePointer, nullptr);
    glCompileShader(CreatedShader);

    int CompileResult{};
    glGetShaderiv(CreatedShader, GL_COMPILE_STATUS, &CompileResult);
    if (CompileResult == 0) {
        int LogLength{};
        glGetShaderiv(CreatedShader, GL_INFO_LOG_LENGTH, &LogLength);
        std::string ErrorLog(static_cast<std::size_t>(LogLength), '\0');
        glGetShaderInfoLog(CreatedShader, LogLength, nullptr, ErrorLog.data());
        std::cerr << ErrorLog << std::endl;
        glDeleteShader(CreatedShader);
        return 0U;
    }

    return CreatedShader;
}
