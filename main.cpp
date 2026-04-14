#include <chrono>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <locale>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Renderer/Renderer.h"
#include "Scene/MeshFactory.h"
#include "Scene/PhysicsRuntime.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#ifdef _DEBUG
#pragma comment(lib, "debug/glfw3.lib")
#pragma comment(lib, "out/Debug/PhysicsLib.lib")
#else
#pragma comment(lib, "release/glfw3.lib")
#pragma comment(lib, "out/Release/PhysicsLib.lib")
#endif

namespace {
PhysicsWorld::WorldSettings CreateDefaultWorldSettings() {
    PhysicsWorld::WorldSettings WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{ 0.0F, -9.8F, 0.0F } };
    return WorldSettings;
}

std::shared_ptr<Mesh> CreateBoundingBoxMeshShared() {
    std::shared_ptr<Mesh> BoundingBoxMesh{ std::make_shared<Mesh>(MeshFactory::CreateBoundingBox()) };
    return BoundingBoxMesh;
}

std::shared_ptr<Mesh> CreateFlatTerrainMeshShared(float HalfExtent, std::uint32_t Width, std::uint32_t Height) {
    std::uint32_t SafeWidth{ Width < 2U ? 2U : Width };
    std::uint32_t SafeHeight{ Height < 2U ? 2U : Height };
    float CellSpacing{ (HalfExtent * 2.0F) / static_cast<float>(SafeWidth - 1U) };
    float OffsetX{ (static_cast<float>(SafeWidth) - 1.0F) * CellSpacing * 0.5F };
    float OffsetZ{ (static_cast<float>(SafeHeight) - 1.0F) * CellSpacing * 0.5F };
    std::size_t VertexCount{ static_cast<std::size_t>(SafeWidth) * static_cast<std::size_t>(SafeHeight) };
    std::size_t IndexCount{ static_cast<std::size_t>(SafeWidth - 1U) * static_cast<std::size_t>(SafeHeight - 1U) * 6ULL };
    std::vector<glm::vec3> Vertices{};
    std::vector<glm::vec3> Colors{};
    std::vector<unsigned int> Indices{};
    Vertices.resize(VertexCount);
    Colors.resize(VertexCount, glm::vec3{ 0.20F, 0.60F, 0.20F });
    Indices.resize(IndexCount);

    for (std::uint32_t GridZ{ 0U }; GridZ < SafeHeight; ++GridZ) {
        for (std::uint32_t GridX{ 0U }; GridX < SafeWidth; ++GridX) {
            std::size_t VertexIndex{ static_cast<std::size_t>(GridZ) * static_cast<std::size_t>(SafeWidth) + static_cast<std::size_t>(GridX) };
            float PositionX{ static_cast<float>(GridX) * CellSpacing - OffsetX };
            float PositionZ{ static_cast<float>(GridZ) * CellSpacing - OffsetZ };
            Vertices[VertexIndex] = glm::vec3{ PositionX, 0.0F, PositionZ };
        }
    }

    std::size_t WriteIndex{};
    for (std::uint32_t GridZ{ 0U }; GridZ < SafeHeight - 1U; ++GridZ) {
        for (std::uint32_t GridX{ 0U }; GridX < SafeWidth - 1U; ++GridX) {
            unsigned int Index0{ GridZ * SafeWidth + GridX };
            unsigned int Index1{ GridZ * SafeWidth + GridX + 1U };
            unsigned int Index2{ (GridZ + 1U) * SafeWidth + GridX };
            unsigned int Index3{ (GridZ + 1U) * SafeWidth + GridX + 1U };
            Indices[WriteIndex] = Index0;
            ++WriteIndex;
            Indices[WriteIndex] = Index2;
            ++WriteIndex;
            Indices[WriteIndex] = Index1;
            ++WriteIndex;
            Indices[WriteIndex] = Index1;
            ++WriteIndex;
            Indices[WriteIndex] = Index2;
            ++WriteIndex;
            Indices[WriteIndex] = Index3;
            ++WriteIndex;
        }
    }

    Mesh FlatTerrainMesh{};
    FlatTerrainMesh.SetVertices(Vertices);
    FlatTerrainMesh.SetColors(Colors);
    FlatTerrainMesh.SetIndices(Indices);
    FlatTerrainMesh.SetTopology(MeshTopology::Triangles);

    Mesh::TerrainSampleDesc TerrainSampleDesc{};
    TerrainSampleDesc.Width = SafeWidth;
    TerrainSampleDesc.Height = SafeHeight;
    TerrainSampleDesc.CellSpacing = CellSpacing;
    TerrainSampleDesc.MaxHeight = 1.0F;
    TerrainSampleDesc.CenterOrigin = true;
    TerrainSampleDesc.HeightValues.resize(VertexCount, 0.0F);
    FlatTerrainMesh.SetTerrainSampleDesc(TerrainSampleDesc);

    std::shared_ptr<Mesh> SharedTerrainMesh{ std::make_shared<Mesh>(std::move(FlatTerrainMesh)) };
    return SharedTerrainMesh;
}

bool TryGetTerrainHeightFromObject(const GameObject& TerrainObject, float WorldX, float WorldZ, float& OutWorldHeight) {
    const std::shared_ptr<Mesh>& TerrainMesh{ TerrainObject.GetMesh() };
    if (TerrainMesh == nullptr || !TerrainMesh->HasTerrainSampleDesc()) {
        return false;
    }

    const Mesh::TerrainSampleDesc& TerrainSampleDesc{ TerrainMesh->GetTerrainSampleDesc() };
    if (TerrainSampleDesc.Width < 2U || TerrainSampleDesc.Height < 2U || TerrainSampleDesc.HeightValues.empty()) {
        return false;
    }

    if (TerrainSampleDesc.CellSpacing <= 0.0F || TerrainSampleDesc.MaxHeight <= 0.0F) {
        return false;
    }

    const Transform& TerrainTransform{ TerrainObject.GetTransform() };
    glm::vec3 TerrainPosition{ TerrainTransform.GetPosition() };
    glm::vec3 TerrainRotation{ TerrainTransform.GetRotation() };
    glm::vec3 TerrainScale{ TerrainTransform.GetScale() };
    glm::mat4 TranslationMatrix{ glm::translate(glm::mat4{ 1.0F }, TerrainPosition) };
    glm::mat4 RotationXMatrix{ glm::rotate(glm::mat4{ 1.0F }, TerrainRotation.x, glm::vec3{ 1.0F, 0.0F, 0.0F }) };
    glm::mat4 RotationYMatrix{ glm::rotate(glm::mat4{ 1.0F }, TerrainRotation.y, glm::vec3{ 0.0F, 1.0F, 0.0F }) };
    glm::mat4 RotationZMatrix{ glm::rotate(glm::mat4{ 1.0F }, TerrainRotation.z, glm::vec3{ 0.0F, 0.0F, 1.0F }) };
    glm::mat4 ScaleMatrix{ glm::scale(glm::mat4{ 1.0F }, TerrainScale) };
    glm::mat4 WorldMatrix{ TranslationMatrix * RotationZMatrix * RotationYMatrix * RotationXMatrix * ScaleMatrix };
    glm::mat4 InverseWorldMatrix{ glm::inverse(WorldMatrix) };
    glm::vec4 LocalPoint4{ InverseWorldMatrix * glm::vec4{ WorldX, 0.0F, WorldZ, 1.0F } };

    float LocalX{ LocalPoint4.x };
    float LocalZ{ LocalPoint4.z };
    float OffsetX{};
    float OffsetZ{};
    if (TerrainSampleDesc.CenterOrigin) {
        OffsetX = (static_cast<float>(TerrainSampleDesc.Width) - 1.0F) * TerrainSampleDesc.CellSpacing * 0.5F;
        OffsetZ = (static_cast<float>(TerrainSampleDesc.Height) - 1.0F) * TerrainSampleDesc.CellSpacing * 0.5F;
    }

    float GridX{ (LocalX + OffsetX) / TerrainSampleDesc.CellSpacing };
    float GridZ{ (LocalZ + OffsetZ) / TerrainSampleDesc.CellSpacing };
    if (GridX < 0.0F || GridZ < 0.0F) {
        return false;
    }

    float MaxGridX{ static_cast<float>(TerrainSampleDesc.Width - 1U) };
    float MaxGridZ{ static_cast<float>(TerrainSampleDesc.Height - 1U) };
    if (GridX > MaxGridX || GridZ > MaxGridZ) {
        return false;
    }

    std::uint32_t X0{ static_cast<std::uint32_t>(std::floor(GridX)) };
    std::uint32_t Z0{ static_cast<std::uint32_t>(std::floor(GridZ)) };
    std::uint32_t X1{ std::min(X0 + 1U, TerrainSampleDesc.Width - 1U) };
    std::uint32_t Z1{ std::min(Z0 + 1U, TerrainSampleDesc.Height - 1U) };
    float FractionX{ GridX - static_cast<float>(X0) };
    float FractionZ{ GridZ - static_cast<float>(Z0) };

    std::size_t Index00{ static_cast<std::size_t>(Z0) * static_cast<std::size_t>(TerrainSampleDesc.Width) + static_cast<std::size_t>(X0) };
    std::size_t Index10{ static_cast<std::size_t>(Z0) * static_cast<std::size_t>(TerrainSampleDesc.Width) + static_cast<std::size_t>(X1) };
    std::size_t Index01{ static_cast<std::size_t>(Z1) * static_cast<std::size_t>(TerrainSampleDesc.Width) + static_cast<std::size_t>(X0) };
    std::size_t Index11{ static_cast<std::size_t>(Z1) * static_cast<std::size_t>(TerrainSampleDesc.Width) + static_cast<std::size_t>(X1) };
    if (Index00 >= TerrainSampleDesc.HeightValues.size() || Index10 >= TerrainSampleDesc.HeightValues.size() || Index01 >= TerrainSampleDesc.HeightValues.size() || Index11 >= TerrainSampleDesc.HeightValues.size()) {
        return false;
    }

    float Height00{ TerrainSampleDesc.HeightValues[Index00] * TerrainSampleDesc.MaxHeight };
    float Height10{ TerrainSampleDesc.HeightValues[Index10] * TerrainSampleDesc.MaxHeight };
    float Height01{ TerrainSampleDesc.HeightValues[Index01] * TerrainSampleDesc.MaxHeight };
    float Height11{ TerrainSampleDesc.HeightValues[Index11] * TerrainSampleDesc.MaxHeight };
    float InterpolatedHeight0{ Height00 + ((Height10 - Height00) * FractionX) };
    float InterpolatedHeight1{ Height01 + ((Height11 - Height01) * FractionX) };
    float LocalHeight{ InterpolatedHeight0 + ((InterpolatedHeight1 - InterpolatedHeight0) * FractionZ) };

    glm::vec4 WorldPoint4{ WorldMatrix * glm::vec4{ LocalX, LocalHeight, LocalZ, 1.0F } };
    OutWorldHeight = WorldPoint4.y;
    return true;
}

void PlaceDynamicObjectsOnTerrain(Scene& TargetScene, std::size_t TerrainObjectIndex, const std::vector<std::size_t>& ObjectIndices) {
    GameObject* TerrainObject{ TargetScene.GetGameObject(TerrainObjectIndex) };
    if (TerrainObject == nullptr) {
        return;
    }

    std::size_t ObjectCount{ ObjectIndices.size() };
    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < ObjectCount; ++ObjectIndex) {
        GameObject* CurrentObject{ TargetScene.GetGameObject(ObjectIndices[ObjectIndex]) };
        if (CurrentObject == nullptr || CurrentObject->IsTerrainObject()) {
            continue;
        }

        glm::vec3 CurrentPosition{ CurrentObject->GetTransform().GetPosition() };
        float SurfaceHeight{};
        bool HasSurfaceHeight{ TryGetTerrainHeightFromObject(*TerrainObject, CurrentPosition.x, CurrentPosition.z, SurfaceHeight) };
        if (!HasSurfaceHeight) {
            continue;
        }

        float HalfHeight{ 0.5F };
        const std::shared_ptr<Mesh>& CurrentMesh{ CurrentObject->GetMesh() };
        if (CurrentMesh != nullptr) {
            DirectX::BoundingOrientedBox BoundingBox{ CurrentMesh->GetBoundingBox() };
            glm::vec3 CurrentScale{ CurrentObject->GetTransform().GetScale() };
            HalfHeight = std::abs(BoundingBox.Extents.y * CurrentScale.y);
        }

        CurrentObject->GetTransform().SetPosition(glm::vec3{ CurrentPosition.x, SurfaceHeight + HalfHeight + 0.05F, CurrentPosition.z });
        CurrentObject->UpdateWorldMatrix();
    }
}

void ConfigureSceneOne(Scene& TargetScene, const std::shared_ptr<Mesh>& BoundingBoxMesh) {
    Camera& MainCamera{ TargetScene.GetMainCamera() };
    MainCamera.SetClearColor(0.1F, 0.2F, 0.3F, 1.0F);
    MainCamera.GetTransform().SetPosition(glm::vec3{ 0.0F, 50.0F, 8.0F });
    MainCamera.GetTransform().SetRotation(glm::vec3{ -0.35F, 0.0F, 0.0F });

    std::shared_ptr<Mesh> TerrainMesh{ std::make_shared<Mesh>(MeshFactory::CreateTerrainFromHeightMapPng("Resources/Heightmap_01_Mountains.png", 256.0F, 1.0F)) };
    std::size_t TerrainObjectIndex{};
    if (!TerrainMesh->GetVertices().empty()) {
        GameObject TerrainObject{ "TerrainPhysics" };
        TerrainObject.SetMesh(TerrainMesh);
        TerrainObject.GetTransform().SetPosition(glm::vec3{ 0.0F, 0.0F, 0.0F });
        TerrainObjectIndex = TargetScene.AddGameObject(std::move(TerrainObject));
    }

    float DropStartHeight{ 50.0F };
    std::size_t DroppedCubeIndex{ TargetScene.CreatePrimitiveGameObject("DroppedCube", PrimitiveMeshType::Cube) };
    std::size_t DroppedSphereIndex{ TargetScene.CreatePrimitiveGameObject("DroppedSphere", PrimitiveMeshType::Sphere) };
    std::size_t DroppedTriangularPyramidIndex{ TargetScene.CreatePrimitiveGameObject("DroppedTriangularPyramid", PrimitiveMeshType::TriangularPyramid) };
    std::size_t DroppedSquarePyramidIndex{ TargetScene.CreatePrimitiveGameObject("DroppedSquarePyramid", PrimitiveMeshType::SquarePyramid) };
    std::size_t GroundCubeIndex{ TargetScene.CreatePrimitiveGameObject("GroundCube", PrimitiveMeshType::Cube) };
    std::size_t GroundSphereIndex{ TargetScene.CreatePrimitiveGameObject("GroundSphere", PrimitiveMeshType::Sphere) };

    GameObject* DroppedCubeObject{ TargetScene.GetGameObject(DroppedCubeIndex) };
    if (DroppedCubeObject != nullptr) {
        DroppedCubeObject->GetTransform().SetPosition(glm::vec3{ -2.0F, DropStartHeight, 0.0F });
    }

    GameObject* DroppedSphereObject{ TargetScene.GetGameObject(DroppedSphereIndex) };
    if (DroppedSphereObject != nullptr) {
        DroppedSphereObject->GetTransform().SetPosition(glm::vec3{ 2.0F, DropStartHeight + 2.0F, 0.0F });
    }

    GameObject* DroppedTriangularPyramidObject{ TargetScene.GetGameObject(DroppedTriangularPyramidIndex) };
    if (DroppedTriangularPyramidObject != nullptr) {
        DroppedTriangularPyramidObject->GetTransform().SetPosition(glm::vec3{ 0.0F, DropStartHeight + 4.0F, -2.0F });
    }

    GameObject* DroppedSquarePyramidObject{ TargetScene.GetGameObject(DroppedSquarePyramidIndex) };
    if (DroppedSquarePyramidObject != nullptr) {
        DroppedSquarePyramidObject->GetTransform().SetPosition(glm::vec3{ 0.0F, DropStartHeight + 6.0F, 2.0F });
    }

    GameObject* GroundCubeObject{ TargetScene.GetGameObject(GroundCubeIndex) };
    if (GroundCubeObject != nullptr) {
        GroundCubeObject->GetTransform().SetPosition(glm::vec3{ -6.0F, 0.0F, 2.0F });
    }

    GameObject* GroundSphereObject{ TargetScene.GetGameObject(GroundSphereIndex) };
    if (GroundSphereObject != nullptr) {
        GroundSphereObject->GetTransform().SetPosition(glm::vec3{ 4.0F, 0.0F, -4.0F });
    }

    std::vector<std::size_t> GroundObjectIndices{ GroundCubeIndex, GroundSphereIndex };
    PlaceDynamicObjectsOnTerrain(TargetScene, TerrainObjectIndex, GroundObjectIndices);

    TargetScene.BuildPhysicsActors();
    TargetScene.ConfigureBoundingBoxes(BoundingBoxMesh);
}

void ConfigureSceneTwo(Scene& TargetScene, const std::shared_ptr<Mesh>& BoundingBoxMesh) {
    Camera& MainCamera{ TargetScene.GetMainCamera() };
    MainCamera.SetClearColor(0.08F, 0.08F, 0.12F, 1.0F);
    MainCamera.GetTransform().SetPosition(glm::vec3{ -10.0F, 12.0F, 24.0F });
    MainCamera.GetTransform().SetRotation(glm::vec3{ -0.32F, -0.58F, 0.0F });

    std::shared_ptr<Mesh> FlatTerrainMesh{ CreateFlatTerrainMeshShared(24.0F, 72U, 72U) };
    GameObject FlatTerrainObject{ "FlatTerrain" };
    FlatTerrainObject.SetMesh(FlatTerrainMesh);
    FlatTerrainObject.GetTransform().SetPosition(glm::vec3{ 0.0F, 0.0F, 0.0F });
    TargetScene.AddGameObject(std::move(FlatTerrainObject));

    for (std::size_t LayerIndex{ 0U }; LayerIndex < 6U; ++LayerIndex) {
        for (std::size_t RowIndex{ 0U }; RowIndex < 3U; ++RowIndex) {
            for (std::size_t ColumnIndex{ 0U }; ColumnIndex < 5U; ++ColumnIndex) {
                std::string ObjectName{ "StackCube_" + std::to_string(LayerIndex) + "_" + std::to_string(RowIndex) + "_" + std::to_string(ColumnIndex) };
                std::size_t CreatedObjectIndex{ TargetScene.CreatePrimitiveGameObject(std::move(ObjectName), PrimitiveMeshType::Cube) };
                GameObject* CreatedObject{ TargetScene.GetGameObject(CreatedObjectIndex) };
                if (CreatedObject == nullptr) {
                    continue;
                }

                float PositionX{ 5.0F + static_cast<float>(ColumnIndex) * 1.05F };
                float PositionY{ 0.5F + static_cast<float>(LayerIndex) * 1.02F };
                float PositionZ{ -2.2F + static_cast<float>(RowIndex) * 1.1F };
                CreatedObject->GetTransform().SetPosition(glm::vec3{ PositionX, PositionY, PositionZ });
            }
        }
    }

    std::size_t ProjectileObjectIndex{ TargetScene.CreatePrimitiveGameObject("ProjectileSphere", PrimitiveMeshType::Sphere) };
    GameObject* ProjectileObject{ TargetScene.GetGameObject(ProjectileObjectIndex) };
    if (ProjectileObject != nullptr) {
        ProjectileObject->GetTransform().SetPosition(glm::vec3{ -18.0F, 3.0F, 0.0F });
        ProjectileObject->GetTransform().SetScale(glm::vec3{ 2.0F, 2.0F, 2.0F });

        DirectX::SimpleMath::Vector3 LaunchDirection{ 1.0F, 0.08F, 0.0F };
        LaunchDirection.Normalize();
        float LaunchImpulseMagnitude{ 800.0F };
        ProjectileObject->SetPhysicsMass(12.0F);
        ProjectileObject->SetInitialImpulse(LaunchDirection * LaunchImpulseMagnitude);
    }

    TargetScene.BuildPhysicsActors();
    TargetScene.ConfigureBoundingBoxes(BoundingBoxMesh);
}
}

int main() {
    std::locale::global(std::locale("ko_KR.UTF-8"));
    std::wcout.imbue(std::locale("ko_KR.UTF-8"));
    std::cout.imbue(std::locale("ko_KR.UTF-8"));

    PhysicsWorld::WorldSettings WorldSettings{ CreateDefaultWorldSettings() };
    std::shared_ptr<Mesh> BoundingBoxMesh{ CreateBoundingBoxMeshShared() };
    std::vector<Scene> SceneTemplates{};
    SceneTemplates.reserve(2U);
    SceneTemplates.emplace_back();
    SceneTemplates.emplace_back();
    ConfigureSceneOne(SceneTemplates[0U], BoundingBoxMesh);
    ConfigureSceneTwo(SceneTemplates[1U], BoundingBoxMesh);

    std::vector<Scene> RenderScenes{ SceneTemplates };

    Renderer MainRenderer{};
    if (!MainRenderer.Initialize()) {
        return -1;
    }

    PhysicsRuntime::RuntimeSettings RuntimeSettings{};
    RuntimeSettings.mWorldSettings = WorldSettings;
    RuntimeSettings.mMaxSubSteps = 4U;
    std::uint32_t CurrentWorldVersion{ 1U };
    PhysicsRuntime Runtime{};
    bool IsPhysicsRuntimeInitialized{ Runtime.Initialize(&SceneTemplates, RuntimeSettings, 0U, CurrentWorldVersion) };
    if (!IsPhysicsRuntimeInitialized) {
        MainRenderer.Shutdown();
        return -1;
    }

    std::size_t DisplayedSceneIndex{};
    std::size_t RequestedSceneIndex{};
    std::uint32_t RequestedWorldVersion{ CurrentWorldVersion };
    bool HasPendingSceneSwitch{};
    std::chrono::steady_clock::time_point PreviousFrameStartTime{ std::chrono::steady_clock::now() };
    while (!MainRenderer.ShouldClose()) {
        std::chrono::steady_clock::time_point CurrentFrameStartTime{ std::chrono::steady_clock::now() };
        std::chrono::duration<double> FrameElapsedTime{ CurrentFrameStartTime - PreviousFrameStartTime };
        PreviousFrameStartTime = CurrentFrameStartTime;
        double CurrentFramesPerSecond{ FrameElapsedTime.count() > 0.0 ? 1.0 / FrameElapsedTime.count() : 0.0 };

        if (DisplayedSceneIndex >= RenderScenes.size()) {
            DisplayedSceneIndex = 0U;
        }

        std::size_t InputSceneIndex{ DisplayedSceneIndex };
        bool ShouldRestartActiveScene{};
        MainRenderer.ProcessInput(RenderScenes[DisplayedSceneIndex], InputSceneIndex, ShouldRestartActiveScene);

        if (InputSceneIndex >= RenderScenes.size()) {
            InputSceneIndex = 0U;
        }

        bool ShouldSendResetCommand{ ShouldRestartActiveScene || InputSceneIndex != DisplayedSceneIndex };
        if (ShouldSendResetCommand) {
            if (InputSceneIndex < RenderScenes.size()) {
                RenderScenes[InputSceneIndex] = SceneTemplates[InputSceneIndex];
            }

            ++CurrentWorldVersion;
            RequestedSceneIndex = InputSceneIndex;
            RequestedWorldVersion = CurrentWorldVersion;
            HasPendingSceneSwitch = true;
            Runtime.EnqueueResetScene(RequestedSceneIndex, RequestedWorldVersion);
        }

        std::uint32_t ReadableSnapshotIndex{ Runtime.GetReadableSnapshotIndex() };
        const PhysicsSnapshot& CurrentSnapshot{ Runtime.GetSnapshot(ReadableSnapshotIndex) };
        if (CurrentSnapshot.mSceneIndex < RenderScenes.size()) {
            RenderScenes[CurrentSnapshot.mSceneIndex].ApplyPhysicsSnapshot(CurrentSnapshot);

            if (!HasPendingSceneSwitch) {
                DisplayedSceneIndex = CurrentSnapshot.mSceneIndex;
            } else if (CurrentSnapshot.mSceneIndex == RequestedSceneIndex && CurrentSnapshot.mWorldVersion == RequestedWorldVersion) {
                DisplayedSceneIndex = RequestedSceneIndex;
                HasPendingSceneSwitch = false;
            }
        }

        if (DisplayedSceneIndex >= RenderScenes.size()) {
            DisplayedSceneIndex = 0U;
        }

        Scene& ActiveScene{ RenderScenes[DisplayedSceneIndex] };
        double PhysicsLastStepElapsedMilliseconds{ CurrentSnapshot.mLastStepElapsedMilliseconds };
        std::ostringstream WindowTitleStream{};
        WindowTitleStream << "FPS: " << std::fixed << std::setprecision(1) << CurrentFramesPerSecond << " | PhysicsStepTime: " << std::setprecision(3) << PhysicsLastStepElapsedMilliseconds << " ms";
        MainRenderer.SetWindowTitle(WindowTitleStream.str());
        ActiveScene.Update();
        MainRenderer.RenderFrame(ActiveScene);
        MainRenderer.Present();
        MainRenderer.PollEvents();
    }

    Runtime.Shutdown();
    MainRenderer.Shutdown();

    return 0;
}


