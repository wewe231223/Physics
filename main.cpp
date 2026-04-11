#include <iostream>
#include <locale>
#include "Renderer/Renderer.h"
#include "PhysicsLib/Core/PhysicsWorld.h"


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

	PhysicsWorld::WorldSettings WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{ 0.0F, -0.F, 0.0F } };
	PhysicsWorld MainPhysicsWorld{ WorldSettings };

	Camera& MainCamera{ MainScene.GetMainCamera() };
	MainCamera.SetClearColor(0.1F, 0.2F, 0.3F, 1.0F);
	MainCamera.GetTransform().SetPosition(glm::vec3{ 0.0F, 3.0F, 8.0F });
	MainCamera.GetTransform().SetRotation(glm::vec3{ -0.35F, 0.0F, 0.0F });

	std::size_t GridIndex{ MainScene.CreatePrimitiveGameObject("Grid", PrimitiveMeshType::Grid) };
	std::size_t CubeIndex{ MainScene.CreatePrimitiveGameObject("Cube", PrimitiveMeshType::Cube) };
	std::size_t SphereIndex{ MainScene.CreatePrimitiveGameObject("Sphere", PrimitiveMeshType::Sphere) };
	std::size_t TriangularPyramidIndex{ MainScene.CreatePrimitiveGameObject("TriangularPyramid", PrimitiveMeshType::TriangularPyramid) };
	std::size_t SquarePyramidIndex{ MainScene.CreatePrimitiveGameObject("SquarePyramid", PrimitiveMeshType::SquarePyramid) };

	GameObject* GridObject{ MainScene.GetGameObject(GridIndex) };
	if (GridObject != nullptr) {
		GridObject->GetTransform().SetPosition(glm::vec3{ 0.0F, 0.0F, 0.0F });
	}

	GameObject* CubeObject{ MainScene.GetGameObject(CubeIndex) };
	if (CubeObject != nullptr) {
		CubeObject->GetTransform().SetPosition(glm::vec3{ -2.0F, 0.5F, 0.0F });
	}

	GameObject* SphereObject{ MainScene.GetGameObject(SphereIndex) };
	if (SphereObject != nullptr) {
		SphereObject->GetTransform().SetPosition(glm::vec3{ 2.0F, 0.5F, 0.0F });
	}

	GameObject* TriangularPyramidObject{ MainScene.GetGameObject(TriangularPyramidIndex) };
	if (TriangularPyramidObject != nullptr) {
		TriangularPyramidObject->GetTransform().SetPosition(glm::vec3{ 0.0F, 0.5F, -2.0F });
	}

	GameObject* SquarePyramidObject{ MainScene.GetGameObject(SquarePyramidIndex) };
	if (SquarePyramidObject != nullptr) {
		SquarePyramidObject->GetTransform().SetPosition(glm::vec3{ 0.0F, 0.5F, 2.0F });
	}

	MainPhysicsWorld.ClearActors();
	std::size_t GameObjectCount{ MainScene.GetGameObjectCount() };

	for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
		GameObject* CurrentObject{ MainScene.GetGameObject(ObjectIndex) };
		if (CurrentObject == nullptr) {
			continue;
		}

		if (CurrentObject->IsTerrainObject()) {
			PhysicsTerrainActor::ActorDesc ActorDesc{ CurrentObject->GetPhysicsTerrainActorDesc() };
			PhysicsTerrainActor* PhysicsActorPointer{ MainPhysicsWorld.CreateTerrainActor(ActorDesc) };
			CurrentObject->SetPhysicsActor(PhysicsActorPointer);
			continue;
		}

		PhysicsDynamicActor::ActorDesc ActorDesc{ CurrentObject->GetPhysicsDynamicActorDesc() };
		PhysicsDynamicActor* PhysicsActorPointer{ MainPhysicsWorld.CreateDynamicActor(ActorDesc) };
		CurrentObject->SetPhysicsActor(PhysicsActorPointer);
	}

	Renderer MainRenderer{};

	if (!MainRenderer.Initialize()) {
		return -1;
	}

	while (!MainRenderer.ShouldClose()) {
		MainRenderer.ProcessInput(MainScene);
		MainPhysicsWorld.Update(WorldSettings.FixedTimeStep);

		for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
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
