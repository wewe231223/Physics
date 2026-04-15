#pragma once

#include "Camera.h"
#include "GameObject.h"
#include "PhysicsExchangeTypes.h"
#include "PhysicsLib/Actors/PhysicsDynamicActor.h"
#include "PhysicsLib/Actors/PhysicsTerrainActor.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

enum class PrimitiveMeshType {
    Cube,
    Sphere,
    TriangularPyramid,
    SquarePyramid,
    Grid
};

class Scene final {
public:
    struct PhysicsActorSpawnInfo final {
        ActorId mActorId{ InvalidActorId };
        std::string mName{};
        bool mIsActive{ true };
        PhysicsActorBase::PhysicsActorType mActorType{ PhysicsActorBase::PhysicsActorType::Dynamic };
        PhysicsDynamicActor::ActorDesc mDynamicActorDesc{};
        PhysicsTerrainActor::ActorDesc mTerrainActorDesc{};
        bool mHasInitialImpulse{};
        DirectX::SimpleMath::Vector3 mInitialImpulse{};
    };

public:
    Scene();
    ~Scene();
    Scene(const Scene& Other);
    Scene& operator=(const Scene& Other);
    Scene(Scene&& Other) noexcept;
    Scene& operator=(Scene&& Other) noexcept;

public:
    Camera& GetMainCamera();
    const Camera& GetMainCamera() const;

    std::size_t AddGameObject(const GameObject& Object);
    std::size_t AddGameObject(GameObject&& Object);
    std::size_t CreatePrimitiveGameObject(std::string Name, PrimitiveMeshType PrimitiveType);

    GameObject* GetGameObject(std::size_t Index);
    const GameObject* GetGameObject(std::size_t Index) const;
    std::size_t GetGameObjectCount() const;

    const std::vector<PhysicsActorSpawnInfo>& GetPhysicsActorSpawnInfos() const;
    std::size_t GetPhysicsActorCount() const;

    void BuildPhysicsActors();
    void ConfigureBoundingBoxes(const std::shared_ptr<Mesh>& BoundingBoxMesh);
    void ApplyPhysicsSnapshot(const PhysicsSnapshot& Snapshot);
    void Update();

private:
    std::shared_ptr<Mesh> GetPrimitiveMesh(PrimitiveMeshType PrimitiveType);

private:
    Camera mMainCamera;
    std::vector<GameObject> mGameObjects;
    std::shared_ptr<Mesh> mCubeMesh;
    std::shared_ptr<Mesh> mSphereMesh;
    std::shared_ptr<Mesh> mTriangularPyramidMesh;
    std::shared_ptr<Mesh> mSquarePyramidMesh;
    std::shared_ptr<Mesh> mGridMesh;
    std::vector<PhysicsActorSpawnInfo> mPhysicsActorSpawnInfos;
    std::vector<std::size_t> mActorIdToGameObjectIndex;
};


