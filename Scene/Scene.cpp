#include "Scene.h"

#include "MeshFactory.h"

#include <utility>

Scene::Scene()
    : mMainCamera{},
      mGameObjects{},
      mCubeMesh{},
      mSphereMesh{},
      mTriangularPyramidMesh{},
      mSquarePyramidMesh{},
      mGridMesh{},
      mPhysicsActorSpawnInfos{},
      mActorIdToGameObjectIndex{} {
}

Scene::~Scene() {
}

Scene::Scene(const Scene& Other)
    : mMainCamera{ Other.mMainCamera },
      mGameObjects{ Other.mGameObjects },
      mCubeMesh{ Other.mCubeMesh },
      mSphereMesh{ Other.mSphereMesh },
      mTriangularPyramidMesh{ Other.mTriangularPyramidMesh },
      mSquarePyramidMesh{ Other.mSquarePyramidMesh },
      mGridMesh{ Other.mGridMesh },
      mPhysicsActorSpawnInfos{ Other.mPhysicsActorSpawnInfos },
      mActorIdToGameObjectIndex{ Other.mActorIdToGameObjectIndex } {
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
    mPhysicsActorSpawnInfos = Other.mPhysicsActorSpawnInfos;
    mActorIdToGameObjectIndex = Other.mActorIdToGameObjectIndex;

    return *this;
}

Scene::Scene(Scene&& Other) noexcept
    : mMainCamera{ std::move(Other.mMainCamera) },
      mGameObjects{ std::move(Other.mGameObjects) },
      mCubeMesh{ std::move(Other.mCubeMesh) },
      mSphereMesh{ std::move(Other.mSphereMesh) },
      mTriangularPyramidMesh{ std::move(Other.mTriangularPyramidMesh) },
      mSquarePyramidMesh{ std::move(Other.mSquarePyramidMesh) },
      mGridMesh{ std::move(Other.mGridMesh) },
      mPhysicsActorSpawnInfos{ std::move(Other.mPhysicsActorSpawnInfos) },
      mActorIdToGameObjectIndex{ std::move(Other.mActorIdToGameObjectIndex) } {
}

Scene& Scene::operator=(Scene&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mMainCamera = std::move(Other.mMainCamera);
    mGameObjects = std::move(Other.mGameObjects);
    mCubeMesh = std::move(Other.mCubeMesh);
    mSphereMesh = std::move(Other.mSphereMesh);
    mTriangularPyramidMesh = std::move(Other.mTriangularPyramidMesh);
    mSquarePyramidMesh = std::move(Other.mSquarePyramidMesh);
    mGridMesh = std::move(Other.mGridMesh);
    mPhysicsActorSpawnInfos = std::move(Other.mPhysicsActorSpawnInfos);
    mActorIdToGameObjectIndex = std::move(Other.mActorIdToGameObjectIndex);

    return *this;
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

const std::vector<Scene::PhysicsActorSpawnInfo>& Scene::GetPhysicsActorSpawnInfos() const {
    return mPhysicsActorSpawnInfos;
}

std::size_t Scene::GetPhysicsActorCount() const {
    std::size_t ActorCount{ mPhysicsActorSpawnInfos.size() };
    return ActorCount;
}

ActorId Scene::GetFirstKinematicActorId() const {
    std::size_t GameObjectCount{ mGameObjects.size() };
    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
        const GameObject& CurrentObject{ mGameObjects[ObjectIndex] };
        if (CurrentObject.GetPhysicsActorType() != PhysicsActorBase::PhysicsActorType::Kinematic) {
            continue;
        }

        if (!CurrentObject.HasActorId()) {
            continue;
        }

        return CurrentObject.GetActorId();
    }

    return InvalidActorId;
}

void Scene::BuildPhysicsActors() {
    mPhysicsActorSpawnInfos.clear();
    mActorIdToGameObjectIndex.clear();

    std::size_t GameObjectCount{ mGameObjects.size() };
    mPhysicsActorSpawnInfos.reserve(GameObjectCount);
    mActorIdToGameObjectIndex.reserve(GameObjectCount);

    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
        GameObject& CurrentObject{ mGameObjects[ObjectIndex] };
        CurrentObject.SetActorId(InvalidActorId);

        PhysicsActorSpawnInfo SpawnInfo{};
        SpawnInfo.mActorId = static_cast<ActorId>(mPhysicsActorSpawnInfos.size());
        SpawnInfo.mName = CurrentObject.GetName();
        SpawnInfo.mIsActive = CurrentObject.GetIsActive();
        if (CurrentObject.IsTerrainObject()) {
            SpawnInfo.mActorType = PhysicsActorBase::PhysicsActorType::Static;
            SpawnInfo.mTerrainActorDesc = CurrentObject.GetPhysicsTerrainActorDesc();
        } else if (CurrentObject.GetPhysicsActorType() == PhysicsActorBase::PhysicsActorType::Kinematic) {
            SpawnInfo.mActorType = PhysicsActorBase::PhysicsActorType::Kinematic;
            SpawnInfo.mDynamicActorDesc = CurrentObject.GetPhysicsDynamicActorDesc();
        } else {
            SpawnInfo.mActorType = PhysicsActorBase::PhysicsActorType::Dynamic;
            SpawnInfo.mDynamicActorDesc = CurrentObject.GetPhysicsDynamicActorDesc();
            SpawnInfo.mHasInitialImpulse = CurrentObject.HasInitialImpulse();
            SpawnInfo.mInitialImpulse = CurrentObject.GetInitialImpulse();
        }

        CurrentObject.SetActorId(SpawnInfo.mActorId);
        mPhysicsActorSpawnInfos.push_back(SpawnInfo);
        mActorIdToGameObjectIndex.push_back(ObjectIndex);
    }
}

void Scene::ConfigureBoundingBoxes(const std::shared_ptr<Mesh>& BoundingBoxMesh) {
    std::size_t GameObjectCount{ mGameObjects.size() };
    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
        GameObject& CurrentObject{ mGameObjects[ObjectIndex] };
        if (!CurrentObject.HasActorId() || CurrentObject.IsTerrainObject()) {
            CurrentObject.SetBoundingBoxVisible(false);
            CurrentObject.ClearBoundingBoxWorldMatrix();
            continue;
        }

        CurrentObject.SetBoundingBoxMesh(BoundingBoxMesh);
        CurrentObject.SetBoundingBoxVisible(true);
        CurrentObject.ClearBoundingBoxWorldMatrix();
    }
}

void Scene::ApplyPhysicsSnapshot(const PhysicsSnapshot& Snapshot) {
    std::size_t SnapshotActorCount{ Snapshot.mActorCount };
    std::size_t AvailableActorCount{ Snapshot.mActors.size() };
    if (SnapshotActorCount > AvailableActorCount) {
        SnapshotActorCount = AvailableActorCount;
    }

    for (std::size_t ActorIndex{ 0U }; ActorIndex < SnapshotActorCount; ++ActorIndex) {
        const PhysicsActorSnapshot& SnapshotActor{ Snapshot.mActors[ActorIndex] };
        ActorId CurrentActorId{ SnapshotActor.mActorId };
        if (CurrentActorId == InvalidActorId) {
            continue;
        }

        std::size_t ActorIdIndex{ static_cast<std::size_t>(CurrentActorId) };
        if (ActorIdIndex >= mActorIdToGameObjectIndex.size()) {
            continue;
        }

        std::size_t ObjectIndex{ mActorIdToGameObjectIndex[ActorIdIndex] };
        if (ObjectIndex >= mGameObjects.size()) {
            continue;
        }

        GameObject& CurrentObject{ mGameObjects[ObjectIndex] };
        CurrentObject.SetIsActive(SnapshotActor.mIsActive);
        CurrentObject.ApplyPhysicsState(SnapshotActor.mPosition, SnapshotActor.mRotation, SnapshotActor.mScale);

        if (CurrentObject.GetBoundingBoxVisible() && SnapshotActor.mActorType != PhysicsActorBase::PhysicsActorType::Static) {
            CurrentObject.SetBoundingBoxFromPhysicsState(SnapshotActor.mWorldBoundingBox, SnapshotActor.mRotation);
        } else {
            CurrentObject.ClearBoundingBoxWorldMatrix();
        }

        CurrentObject.UpdateWorldMatrix();
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

