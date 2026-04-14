#include "Scene.h"

#include "MeshFactory.h"

#include <utility>

Scene::Scene()
    : mMainCamera{},
      mGameObjects{},
      mPhysicsWorld{},
      mCubeMesh{},
      mSphereMesh{},
      mTriangularPyramidMesh{},
      mSquarePyramidMesh{},
      mGridMesh{} {
}

Scene::~Scene() {
}

Scene::Scene(const Scene& Other)
    : mMainCamera{ Other.mMainCamera },
      mGameObjects{ Other.mGameObjects },
      mPhysicsWorld{ Other.mPhysicsWorld.GetSettings() },
      mCubeMesh{ Other.mCubeMesh },
      mSphereMesh{ Other.mSphereMesh },
      mTriangularPyramidMesh{ Other.mTriangularPyramidMesh },
      mSquarePyramidMesh{ Other.mSquarePyramidMesh },
      mGridMesh{ Other.mGridMesh } {
    BuildPhysicsActors();
}

Scene& Scene::operator=(const Scene& Other) {
    if (this == &Other) {
        return *this;
    }

    mMainCamera = Other.mMainCamera;
    mGameObjects = Other.mGameObjects;
    mCubeMesh = Other.mCubeMesh;
    mSphereMesh = Other.mSphereMesh;
    mTriangularPyramidMesh = Other.mTriangularPyramidMesh;
    mSquarePyramidMesh = Other.mSquarePyramidMesh;
    mGridMesh = Other.mGridMesh;
    mPhysicsWorld.Initialize(Other.mPhysicsWorld.GetSettings());
    BuildPhysicsActors();

    return *this;
}

Scene::Scene(Scene&& Other) noexcept
    : mMainCamera{ std::move(Other.mMainCamera) },
      mGameObjects{ std::move(Other.mGameObjects) },
      mPhysicsWorld{ std::move(Other.mPhysicsWorld) },
      mCubeMesh{ std::move(Other.mCubeMesh) },
      mSphereMesh{ std::move(Other.mSphereMesh) },
      mTriangularPyramidMesh{ std::move(Other.mTriangularPyramidMesh) },
      mSquarePyramidMesh{ std::move(Other.mSquarePyramidMesh) },
      mGridMesh{ std::move(Other.mGridMesh) } {
}

Scene& Scene::operator=(Scene&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mMainCamera = std::move(Other.mMainCamera);
    mGameObjects = std::move(Other.mGameObjects);
    mPhysicsWorld = std::move(Other.mPhysicsWorld);
    mCubeMesh = std::move(Other.mCubeMesh);
    mSphereMesh = std::move(Other.mSphereMesh);
    mTriangularPyramidMesh = std::move(Other.mTriangularPyramidMesh);
    mSquarePyramidMesh = std::move(Other.mSquarePyramidMesh);
    mGridMesh = std::move(Other.mGridMesh);

    return *this;
}

Scene::Scene(const PhysicsWorld::WorldSettings& WorldSettings)
    : mMainCamera{},
      mGameObjects{},
      mPhysicsWorld{ WorldSettings },
      mCubeMesh{},
      mSphereMesh{},
      mTriangularPyramidMesh{},
      mSquarePyramidMesh{},
      mGridMesh{} {
}

Camera& Scene::GetMainCamera() {
    return mMainCamera;
}

const Camera& Scene::GetMainCamera() const {
    return mMainCamera;
}

std::size_t Scene::AddGameObject(const GameObject& Object) {
    mGameObjects.push_back(Object);

    std::size_t CreatedIndex{ mGameObjects.size() - 1U };
    mGameObjects[CreatedIndex].UpdateWorldMatrix();

    return CreatedIndex;
}

std::size_t Scene::AddGameObject(GameObject&& Object) {
    mGameObjects.push_back(std::move(Object));

    std::size_t CreatedIndex{ mGameObjects.size() - 1U };
    mGameObjects[CreatedIndex].UpdateWorldMatrix();

    return CreatedIndex;
}

std::size_t Scene::CreatePrimitiveGameObject(std::string Name, PrimitiveMeshType PrimitiveType) {
    GameObject NewObject{ std::move(Name) };
    std::shared_ptr<Mesh> SharedMesh{ GetPrimitiveMesh(PrimitiveType) };

    NewObject.SetMesh(SharedMesh);
    NewObject.UpdateWorldMatrix();
    mGameObjects.push_back(std::move(NewObject));

    std::size_t CreatedIndex{ mGameObjects.size() - 1U };
    return CreatedIndex;
}

GameObject* Scene::GetGameObject(std::size_t Index) {
    if (Index >= mGameObjects.size()) {
        return nullptr;
    }

    return &mGameObjects[Index];
}

const GameObject* Scene::GetGameObject(std::size_t Index) const {
    if (Index >= mGameObjects.size()) {
        return nullptr;
    }

    return &mGameObjects[Index];
}

std::size_t Scene::GetGameObjectCount() const {
    std::size_t Count{ mGameObjects.size() };
    return Count;
}

PhysicsWorld& Scene::GetPhysicsWorld() {
    return mPhysicsWorld;
}

const PhysicsWorld& Scene::GetPhysicsWorld() const {
    return mPhysicsWorld;
}

void Scene::BuildPhysicsActors() {
    mPhysicsWorld.ClearActors();

    std::size_t GameObjectCount{ mGameObjects.size() };
    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
        GameObject& CurrentObject{ mGameObjects[ObjectIndex] };
        CurrentObject.SetPhysicsActor(nullptr);

        if (CurrentObject.IsTerrainObject()) {
            PhysicsTerrainActor::ActorDesc ActorDesc{ CurrentObject.GetPhysicsTerrainActorDesc() };
            PhysicsTerrainActor* PhysicsActorPointer{ mPhysicsWorld.CreateTerrainActor(ActorDesc) };
            PhysicsActorPointer->SetName(CurrentObject.GetName());
            PhysicsActorPointer->SetIsActive(CurrentObject.GetIsActive());
            CurrentObject.SetPhysicsActor(PhysicsActorPointer);
            continue;
        }

        PhysicsDynamicActor::ActorDesc ActorDesc{ CurrentObject.GetPhysicsDynamicActorDesc() };
        PhysicsDynamicActor* PhysicsActorPointer{ mPhysicsWorld.CreateDynamicActor(ActorDesc) };
        CurrentObject.SetPhysicsActor(PhysicsActorPointer);
    }
}

void Scene::ConfigureBoundingBoxes(const std::shared_ptr<Mesh>& BoundingBoxMesh) {
    std::size_t GameObjectCount{ mGameObjects.size() };
    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
        GameObject& CurrentObject{ mGameObjects[ObjectIndex] };
        PhysicsActor* PhysicsActorPointer{ CurrentObject.GetPhysicsActor() };
        if (PhysicsActorPointer == nullptr) {
            CurrentObject.SetBoundingBoxVisible(false);
            continue;
        }

        if (PhysicsActorPointer->GetActorType() == PhysicsActor::PhysicsActorType::Static) {
            CurrentObject.SetBoundingBoxVisible(false);
            continue;
        }

        CurrentObject.SetBoundingBoxMesh(BoundingBoxMesh);
        CurrentObject.SetBoundingBoxVisible(true);
        CurrentObject.UpdateBoundingBoxWorldMatrix();
    }
}

void Scene::UpdatePhysics(float DeltaTime) {
    mPhysicsWorld.Update(DeltaTime);

    std::size_t GameObjectCount{ mGameObjects.size() };
    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
        GameObject& CurrentObject{ mGameObjects[ObjectIndex] };
        PhysicsActor* PhysicsActorPointer{ CurrentObject.GetPhysicsActor() };
        if (PhysicsActorPointer == nullptr) {
            continue;
        }

        DirectX::SimpleMath::Vector3 InterpolatedPosition{};
        DirectX::SimpleMath::Vector3 InterpolatedRotation{};
        DirectX::SimpleMath::Vector3 InterpolatedScale{};
        bool HasInterpolatedState{ mPhysicsWorld.TryGetInterpolatedActorTransform(*PhysicsActorPointer, InterpolatedPosition, InterpolatedRotation, InterpolatedScale) };
        if (!HasInterpolatedState) {
            CurrentObject.PullTransformFromPhysicsActor();
            continue;
        }

        CurrentObject.GetTransform().SetPosition(glm::vec3{ InterpolatedPosition.x, InterpolatedPosition.y, InterpolatedPosition.z });
        CurrentObject.GetTransform().SetRotation(glm::vec3{ InterpolatedRotation.x, InterpolatedRotation.y, InterpolatedRotation.z });
        CurrentObject.GetTransform().SetScale(glm::vec3{ InterpolatedScale.x, InterpolatedScale.y, InterpolatedScale.z });
        CurrentObject.UpdateBoundingBoxWorldMatrix();
    }
}

void Scene::Update() {
    std::size_t GameObjectCount{ mGameObjects.size() };

    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
        mGameObjects[ObjectIndex].UpdateWorldMatrix();
    }
}

std::shared_ptr<Mesh> Scene::GetPrimitiveMesh(PrimitiveMeshType PrimitiveType) {
    if (PrimitiveType == PrimitiveMeshType::Cube) {
        if (mCubeMesh == nullptr) {
            mCubeMesh = std::make_shared<Mesh>(MeshFactory::CreateCube(1.0F));
        }

        return mCubeMesh;
    }

    if (PrimitiveType == PrimitiveMeshType::Sphere) {
        if (mSphereMesh == nullptr) {
            mSphereMesh = std::make_shared<Mesh>(MeshFactory::CreateSphere(0.5F, 24U, 16U));
        }

        return mSphereMesh;
    }

    if (PrimitiveType == PrimitiveMeshType::TriangularPyramid) {
        if (mTriangularPyramidMesh == nullptr) {
            mTriangularPyramidMesh = std::make_shared<Mesh>(MeshFactory::CreateTriangularPyramid(1.0F));
        }

        return mTriangularPyramidMesh;
    }

    if (PrimitiveType == PrimitiveMeshType::SquarePyramid) {
        if (mSquarePyramidMesh == nullptr) {
            mSquarePyramidMesh = std::make_shared<Mesh>(MeshFactory::CreateSquarePyramid(1.0F));
        }

        return mSquarePyramidMesh;
    }

    if (mGridMesh == nullptr) {
        mGridMesh = std::make_shared<Mesh>(MeshFactory::CreateGrid(10.0F, 20U));
    }

    return mGridMesh;
}
