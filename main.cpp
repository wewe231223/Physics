#include <iostream>
#include <locale>
#include <memory>
#include <utility>

#include "Renderer/Renderer.h"
#include "PhysicsLib/Core/PhysicsWorld.h"
#include "Scene/MeshFactory.h"

#ifdef _DEBUG
#pragma comment(lib, "debug/glfw3.lib")
#pragma comment(lib, "out/Debug/PhysicsLib.lib")
#else
#pragma comment(lib, "release/glfw3.lib")
#pragma comment(lib, "out/Release/PhysicsLib.lib")
#endif

int main() {
	std::locale::global(std::locale("ko_KR.UTF-8"));
	std::wcout.imbue(std::locale("ko_KR.UTF-8"));
	std::cout.imbue(std::locale("ko_KR.UTF-8"));

	Scene MainScene{};

	PhysicsWorld::WorldSettings WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{ 0.0F, -9.8F, 0.0F } };
	PhysicsWorld MainPhysicsWorld{ WorldSettings };

	Camera& MainCamera{ MainScene.GetMainCamera() };
	MainCamera.SetClearColor(0.1F, 0.2F, 0.3F, 1.0F);
	MainCamera.GetTransform().SetPosition(glm::vec3{ 0.0F, 50.0F, 8.0F });
	MainCamera.GetTransform().SetRotation(glm::vec3{ -0.35F, 0.0F, 0.0F });

	std::size_t GridIndex{ MainScene.CreatePrimitiveGameObject("Grid", PrimitiveMeshType::Grid) };
	std::size_t CubeIndex{ MainScene.CreatePrimitiveGameObject("Cube", PrimitiveMeshType::Cube) };
	std::size_t StackedCubeIndex{ MainScene.CreatePrimitiveGameObject("StackedCube", PrimitiveMeshType::Cube) };
	std::size_t SphereIndex{ MainScene.CreatePrimitiveGameObject("Sphere", PrimitiveMeshType::Sphere) };
	std::size_t TriangularPyramidIndex{ MainScene.CreatePrimitiveGameObject("TriangularPyramid", PrimitiveMeshType::TriangularPyramid) };
	std::size_t SquarePyramidIndex{ MainScene.CreatePrimitiveGameObject("SquarePyramid", PrimitiveMeshType::SquarePyramid) };

	GameObject* GridObject{ MainScene.GetGameObject(GridIndex) };
	if (GridObject != nullptr) {
		GridObject->GetTransform().SetPosition(glm::vec3{ 0.0F, 0.0F, 0.0F });
	}

	float DropStartHeight{ 50.0F };

	GameObject* CubeObject{ MainScene.GetGameObject(CubeIndex) };
	if (CubeObject != nullptr) {
		CubeObject->GetTransform().SetPosition(glm::vec3{ -2.0F, DropStartHeight, 0.0F });
	}

	GameObject* StackedCubeObject{ MainScene.GetGameObject(StackedCubeIndex) };
	if (StackedCubeObject != nullptr) {
		StackedCubeObject->GetTransform().SetPosition(glm::vec3{ -2.0F, DropStartHeight + 2.0F, 0.0F });
	}

	GameObject* SphereObject{ MainScene.GetGameObject(SphereIndex) };
	if (SphereObject != nullptr) {
		SphereObject->GetTransform().SetPosition(glm::vec3{ 2.0F, DropStartHeight, 0.0F });
	}

	GameObject* TriangularPyramidObject{ MainScene.GetGameObject(TriangularPyramidIndex) };
	if (TriangularPyramidObject != nullptr) {
		TriangularPyramidObject->GetTransform().SetPosition(glm::vec3{ 0.0F, DropStartHeight, -2.0F });
	}

	GameObject* SquarePyramidObject{ MainScene.GetGameObject(SquarePyramidIndex) };
	if (SquarePyramidObject != nullptr) {
		SquarePyramidObject->GetTransform().SetPosition(glm::vec3{ 0.0F, DropStartHeight, 2.0F });
	}

	std::shared_ptr<Mesh> TerrainMesh{ std::make_shared<Mesh>(MeshFactory::CreateTerrainFromHeightMapPng("Resources/Heightmap_01_Mountains.png", 256.0F, 1.F)) };
	if (!TerrainMesh->GetVertices().empty()) {
		GameObject TerrainObject{ "TerrainPhysics" };
		TerrainObject.SetMesh(TerrainMesh);
		TerrainObject.GetTransform().SetPosition(glm::vec3{ 0.0F, 0.0F, 0.0F });
		MainScene.AddGameObject(std::move(TerrainObject));
	}

	MainPhysicsWorld.ClearActors();
	std::size_t PhysicsObjectCount{ MainScene.GetGameObjectCount() };

	for (std::size_t ObjectIndex{ 0U }; ObjectIndex < PhysicsObjectCount; ++ObjectIndex) {
		GameObject* CurrentObject{ MainScene.GetGameObject(ObjectIndex) };
		if (CurrentObject == nullptr) {
			continue;
		}

		if (CurrentObject->IsTerrainObject()) {
			PhysicsTerrainActor::ActorDesc ActorDesc{ CurrentObject->GetPhysicsTerrainActorDesc() };
			PhysicsTerrainActor* PhysicsActorPointer{ MainPhysicsWorld.CreateTerrainActor(ActorDesc) };
			PhysicsActorPointer->SetName(CurrentObject->GetName());
			PhysicsActorPointer->SetIsActive(CurrentObject->GetIsActive());
			CurrentObject->SetPhysicsActor(PhysicsActorPointer);
			continue;
		}

		PhysicsDynamicActor::ActorDesc ActorDesc{ CurrentObject->GetPhysicsDynamicActorDesc() };
		PhysicsDynamicActor* PhysicsActorPointer{ MainPhysicsWorld.CreateDynamicActor(ActorDesc) };
		CurrentObject->SetPhysicsActor(PhysicsActorPointer);
	}

	std::shared_ptr<Mesh> BoundingBoxMesh{ std::make_shared<Mesh>(MeshFactory::CreateBoundingBox()) };
	for (std::size_t ObjectIndex{ 0U }; ObjectIndex < PhysicsObjectCount; ++ObjectIndex) {
		GameObject* SourceObject{ MainScene.GetGameObject(ObjectIndex) };
		if (SourceObject == nullptr) {
			continue;
		}

		if (SourceObject->GetPhysicsActor() == nullptr) {
			continue;
		}

		if (SourceObject->GetPhysicsActor()->GetActorType() == PhysicsActor::PhysicsActorType::Static) {
			continue;
		}

		SourceObject->SetBoundingBoxMesh(BoundingBoxMesh);
		SourceObject->SetBoundingBoxVisible(true);
		SourceObject->UpdateBoundingBoxWorldMatrix();
	}

	Renderer MainRenderer{};

	if (!MainRenderer.Initialize()) {
		return -1;
	}

	while (!MainRenderer.ShouldClose()) {
		MainRenderer.ProcessInput(MainScene);
		MainPhysicsWorld.Update(WorldSettings.FixedTimeStep);

		for (std::size_t ObjectIndex{ 0U }; ObjectIndex < PhysicsObjectCount; ++ObjectIndex) {
			GameObject* CurrentObject{ MainScene.GetGameObject(ObjectIndex) };
			if (CurrentObject == nullptr) {
				continue;
			}

			CurrentObject->PullTransformFromPhysicsActor();
		}

		MainScene.Update();
		MainRenderer.RenderFrame(MainScene);
		MainRenderer.Present();
		MainRenderer.PollEvents();
	}

	MainRenderer.Shutdown();

	return 0;
}
