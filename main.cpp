#include <chrono>
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
#include "PhysicsLib/Core/PhysicsDynamicActor.h"
#include "PhysicsLib/Core/PhysicsTerrainActor.h"
#include "PhysicsLib/Core/PhysicsWorld.h"
#include "Scene/MeshFactory.h"

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

void PlaceDynamicObjectsOnTerrain(Scene& TargetScene, std::size_t TerrainObjectIndex, const std::vector<std::size_t>& ObjectIndices) {
    GameObject* TerrainObject{ TargetScene.GetGameObject(TerrainObjectIndex) };
    if (TerrainObject == nullptr) {
        return;
    }

    const PhysicsActor* TerrainActorBase{ TerrainObject->GetPhysicsActor() };
    if (TerrainActorBase == nullptr) {
        return;
    }

    if (TerrainActorBase->GetActorType() != PhysicsActor::PhysicsActorType::Static) {
        return;
    }

    const PhysicsTerrainActor* TerrainActor{ static_cast<const PhysicsTerrainActor*>(TerrainActorBase) };
    std::size_t ObjectCount{ ObjectIndices.size() };
    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < ObjectCount; ++ObjectIndex) {
        GameObject* CurrentObject{ TargetScene.GetGameObject(ObjectIndices[ObjectIndex]) };
        if (CurrentObject == nullptr) {
            continue;
        }

        PhysicsActor* ActorBase{ CurrentObject->GetPhysicsActor() };
        if (ActorBase == nullptr) {
            continue;
        }

        if (ActorBase->GetActorType() != PhysicsActor::PhysicsActorType::Dynamic) {
            continue;
        }

        PhysicsDynamicActor* DynamicActor{ static_cast<PhysicsDynamicActor*>(ActorBase) };
        DirectX::SimpleMath::Vector3 CurrentPosition{ DynamicActor->GetPosition() };
        float SurfaceHeight{};
        bool HasSurfaceHeight{ TerrainActor->TryGetSurfaceHeightAtWorldPosition(CurrentPosition.x, CurrentPosition.z, SurfaceHeight) };
        if (!HasSurfaceHeight) {
            continue;
        }

        float HalfHeight{ DynamicActor->GetWorldBoundingBox().Extents.y };
        DirectX::SimpleMath::Vector3 PlacedPosition{ CurrentPosition.x, SurfaceHeight + HalfHeight + 0.05F, CurrentPosition.z };
        DynamicActor->SetPosition(PlacedPosition);
        DynamicActor->SetVelocity(DirectX::SimpleMath::Vector3{});
        DynamicActor->SetIsSleeping(false);
        CurrentObject->PullTransformFromPhysicsActor();
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

    TargetScene.BuildPhysicsActors();
    TargetScene.ConfigureBoundingBoxes(BoundingBoxMesh);

    std::vector<std::size_t> GroundObjectIndices{ GroundCubeIndex, GroundSphereIndex };
    PlaceDynamicObjectsOnTerrain(TargetScene, TerrainObjectIndex, GroundObjectIndices);
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
    }

    TargetScene.BuildPhysicsActors();
    TargetScene.ConfigureBoundingBoxes(BoundingBoxMesh);

    ProjectileObject = TargetScene.GetGameObject(ProjectileObjectIndex);
    if (ProjectileObject != nullptr) {
        PhysicsActor* ProjectileActorBase{ ProjectileObject->GetPhysicsActor() };
        if (ProjectileActorBase != nullptr && ProjectileActorBase->GetActorType() == PhysicsActor::PhysicsActorType::Dynamic) {
            PhysicsDynamicActor* ProjectileDynamicActor{ static_cast<PhysicsDynamicActor*>(ProjectileActorBase) };
            DirectX::SimpleMath::Vector3 LaunchDirection{ 1.0F, 0.08F, 0.0F };
            LaunchDirection.Normalize();
            float LaunchImpulseMagnitude{ 800.0F };
            ProjectileDynamicActor->SetMass(12.0F);
            ProjectileDynamicActor->AddImpulse(LaunchDirection * LaunchImpulseMagnitude);
            ProjectileDynamicActor->SetIsSleeping(false);
            ProjectileObject->PullTransformFromPhysicsActor();
            ProjectileObject->UpdateWorldMatrix();
        }
    }
}

void ResetSceneAtIndex(std::vector<Scene>& Scenes, std::size_t SceneIndex, const PhysicsWorld::WorldSettings& WorldSettings, const std::shared_ptr<Mesh>& BoundingBoxMesh) {
    if (SceneIndex >= Scenes.size()) {
        return;
    }

    Scene RecreatedScene{ WorldSettings };
    if (SceneIndex == 0U) {
        ConfigureSceneOne(RecreatedScene, BoundingBoxMesh);
    } else {
        ConfigureSceneTwo(RecreatedScene, BoundingBoxMesh);
    }

    Scenes[SceneIndex] = std::move(RecreatedScene);
}
}

int main() {
    std::locale::global(std::locale("ko_KR.UTF-8"));
    std::wcout.imbue(std::locale("ko_KR.UTF-8"));
    std::cout.imbue(std::locale("ko_KR.UTF-8"));

    PhysicsWorld::WorldSettings WorldSettings{ CreateDefaultWorldSettings() };
    std::shared_ptr<Mesh> BoundingBoxMesh{ CreateBoundingBoxMeshShared() };
    std::vector<Scene> Scenes{};
    Scenes.reserve(2U);
    Scenes.emplace_back(WorldSettings);
    Scenes.emplace_back(WorldSettings);

    ConfigureSceneOne(Scenes[0U], BoundingBoxMesh);
    ConfigureSceneTwo(Scenes[1U], BoundingBoxMesh);

    Renderer MainRenderer{};
    if (!MainRenderer.Initialize()) {
        return -1;
    }

    std::size_t ActiveSceneIndex{};
    std::chrono::steady_clock::time_point PreviousFrameStartTime{ std::chrono::steady_clock::now() };
    while (!MainRenderer.ShouldClose()) {
        std::chrono::steady_clock::time_point CurrentFrameStartTime{ std::chrono::steady_clock::now() };
        std::chrono::duration<double> FrameElapsedTime{ CurrentFrameStartTime - PreviousFrameStartTime };
        PreviousFrameStartTime = CurrentFrameStartTime;
        double CurrentFramesPerSecond{ FrameElapsedTime.count() > 0.0 ? 1.0 / FrameElapsedTime.count() : 0.0 };

        if (ActiveSceneIndex >= Scenes.size()) {
            ActiveSceneIndex = 0U;
        }

        Scene& InputScene{ Scenes[ActiveSceneIndex] };
        bool ShouldRestartActiveScene{};
        MainRenderer.ProcessInput(InputScene, ActiveSceneIndex, ShouldRestartActiveScene);

        if (ActiveSceneIndex >= Scenes.size()) {
            ActiveSceneIndex = 0U;
        }

        if (ShouldRestartActiveScene) {
            ResetSceneAtIndex(Scenes, ActiveSceneIndex, WorldSettings, BoundingBoxMesh);
        }

        Scene& ActiveScene{ Scenes[ActiveSceneIndex] };
        ActiveScene.UpdatePhysics(WorldSettings.FixedTimeStep);
        PhysicsWorld& ActivePhysicsWorld{ ActiveScene.GetPhysicsWorld() };
        double PhysicsLastStepElapsedMilliseconds{ ActivePhysicsWorld.GetLastStepElapsedMilliseconds() };
        std::ostringstream WindowTitleStream{};
        WindowTitleStream << "FPS: " << std::fixed << std::setprecision(1) << CurrentFramesPerSecond << " | PhysicsStepTime: " << std::setprecision(3) << PhysicsLastStepElapsedMilliseconds << " ms";
        MainRenderer.SetWindowTitle(WindowTitleStream.str());
        ActiveScene.Update();
        MainRenderer.RenderFrame(ActiveScene);
        MainRenderer.Present();
        MainRenderer.PollEvents();
    }

    MainRenderer.Shutdown();

    return 0;
}
