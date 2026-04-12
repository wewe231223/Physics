#include "GameObject.h"

#include <utility>

GameObject::GameObject()
    : mName{ "GameObject" },
      mIsActive{ true },
      mTransform{},
      mMesh{},
      mWorldMatrix{ 1.0F },
      mPhysicsActor{} {
}

GameObject::~GameObject() {
}

GameObject::GameObject(const GameObject& Other)
    : mName{ Other.mName },
      mIsActive{ Other.mIsActive },
      mTransform{ Other.mTransform },
      mMesh{ Other.mMesh },
      mWorldMatrix{ Other.mWorldMatrix },
      mPhysicsActor{} {
}

GameObject& GameObject::operator=(const GameObject& Other) {
    if (this == &Other) {
        return *this;
    }

    mName = Other.mName;
    mIsActive = Other.mIsActive;
    mTransform = Other.mTransform;
    mMesh = Other.mMesh;
    mWorldMatrix = Other.mWorldMatrix;
    mPhysicsActor = nullptr;

    return *this;
}

GameObject::GameObject(GameObject&& Other) noexcept
    : mName{ std::move(Other.mName) },
      mIsActive{ Other.mIsActive },
      mTransform{ std::move(Other.mTransform) },
      mMesh{ std::move(Other.mMesh) },
      mWorldMatrix{ Other.mWorldMatrix },
      mPhysicsActor{ Other.mPhysicsActor } {
    Other.mName = "";
    Other.mIsActive = false;
    Other.mWorldMatrix = glm::mat4{ 1.0F };
    Other.mPhysicsActor = nullptr;
}

GameObject& GameObject::operator=(GameObject&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mName = std::move(Other.mName);
    mIsActive = Other.mIsActive;
    mTransform = std::move(Other.mTransform);
    mMesh = std::move(Other.mMesh);
    mWorldMatrix = Other.mWorldMatrix;
    mPhysicsActor = Other.mPhysicsActor;

    Other.mName = "";
    Other.mIsActive = false;
    Other.mWorldMatrix = glm::mat4{ 1.0F };
    Other.mPhysicsActor = nullptr;

    return *this;
}

GameObject::GameObject(std::string Name)
    : mName{ std::move(Name) },
      mIsActive{ true },
      mTransform{},
      mMesh{},
      mWorldMatrix{ 1.0F },
      mPhysicsActor{} {
}

void GameObject::SetName(std::string Name) {
    mName = std::move(Name);
}

const std::string& GameObject::GetName() const {
    return mName;
}

void GameObject::SetIsActive(bool IsActive) {
    mIsActive = IsActive;
}

bool GameObject::GetIsActive() const {
    return mIsActive;
}

Transform& GameObject::GetTransform() {
    return mTransform;
}

const Transform& GameObject::GetTransform() const {
    return mTransform;
}

void GameObject::SetMesh(const std::shared_ptr<Mesh>& MeshData) {
    mMesh = MeshData;
}

const std::shared_ptr<Mesh>& GameObject::GetMesh() const {
    return mMesh;
}

void GameObject::UpdateWorldMatrix() {
    mWorldMatrix = mTransform.GetWorldMatrix();
}

const glm::mat4& GameObject::GetWorldMatrix() const {
    return mWorldMatrix;
}

bool GameObject::IsTerrainObject() const {
    bool IsTerrain{ mMesh == nullptr || mName == "Grid" };
    return IsTerrain;
}

PhysicsDynamicActor::ActorDesc GameObject::GetPhysicsDynamicActorDesc() const {
    glm::vec3 Position{ mTransform.GetPosition() };
    glm::vec3 Rotation{ mTransform.GetRotation() };
    glm::vec3 Scale{ mTransform.GetScale() };

    DirectX::BoundingOrientedBox ActorBoundingBox{};
    if (mMesh == nullptr) {
        ActorBoundingBox.Center = DirectX::XMFLOAT3{ 0.0F, 0.0F, 0.0F };
        ActorBoundingBox.Extents = DirectX::XMFLOAT3{ 0.5F, 0.5F, 0.5F };
        ActorBoundingBox.Orientation = DirectX::XMFLOAT4{ 0.0F, 0.0F, 0.0F, 1.0F };
    } else {
        ActorBoundingBox = mMesh->GetBoundingBox();
    }

    PhysicsDynamicActor::ActorDesc ActorDesc{
        mName,
        mIsActive,
        1.0F,
        PhysicsActor::PhysicsActorFlags::None,
        ActorBoundingBox,
        DirectX::SimpleMath::Vector3{ Position.x, Position.y, Position.z },
        DirectX::SimpleMath::Vector3{ Rotation.x, Rotation.y, Rotation.z },
        DirectX::SimpleMath::Vector3{ Scale.x, Scale.y, Scale.z },
        DirectX::SimpleMath::Vector3{}
    };

    return ActorDesc;
}

PhysicsTerrainActor::ActorDesc GameObject::GetPhysicsTerrainActorDesc() const {
    glm::vec3 Position{ mTransform.GetPosition() };
    glm::vec3 Rotation{ mTransform.GetRotation() };
    glm::vec3 Scale{ mTransform.GetScale() };

    float HalfExtentX{ 0.5F };
    float HalfExtentZ{ 0.5F };

    if (mMesh != nullptr) {
        DirectX::BoundingOrientedBox LocalBoundingBox{ mMesh->GetBoundingBox() };
        HalfExtentX = LocalBoundingBox.Extents.x;
        HalfExtentZ = LocalBoundingBox.Extents.z;
    }

    PhysicsTerrainActor::ActorDesc ActorDesc{
        mName,
        mIsActive,
        PhysicsActor::PhysicsActorFlags::Static,
        DirectX::SimpleMath::Vector3{ Position.x, Position.y, Position.z },
        DirectX::SimpleMath::Vector3{ Rotation.x, Rotation.y, Rotation.z },
        DirectX::SimpleMath::Vector3{ Scale.x, Scale.y, Scale.z },
        HalfExtentX,
        HalfExtentZ
    };

    return ActorDesc;
}

void GameObject::SetPhysicsActor(PhysicsActor* PhysicsActorPointer) {
    mPhysicsActor = PhysicsActorPointer;
}

PhysicsActor* GameObject::GetPhysicsActor() {
    return mPhysicsActor;
}

const PhysicsActor* GameObject::GetPhysicsActor() const {
    return mPhysicsActor;
}

void GameObject::PullTransformFromPhysicsActor() {
    if (mPhysicsActor == nullptr) {
        return;
    }

    if (mPhysicsActor->GetActorType() == PhysicsActor::PhysicsActorType::Terrain) {
        const PhysicsTerrainActor* TerrainActor{ static_cast<const PhysicsTerrainActor*>(mPhysicsActor) };
        DirectX::SimpleMath::Vector3 Position{ TerrainActor->GetPosition() };
        DirectX::SimpleMath::Vector3 Rotation{ TerrainActor->GetRotation() };
        DirectX::SimpleMath::Vector3 Scale{ TerrainActor->GetScale() };

        mTransform.SetPosition(glm::vec3{ Position.x, Position.y, Position.z });
        mTransform.SetRotation(glm::vec3{ Rotation.x, Rotation.y, Rotation.z });
        mTransform.SetScale(glm::vec3{ Scale.x, Scale.y, Scale.z });
        return;
    }

    const PhysicsDynamicActor* DynamicActor{ static_cast<const PhysicsDynamicActor*>(mPhysicsActor) };
    DirectX::SimpleMath::Vector3 Position{ DynamicActor->GetPosition() };
    DirectX::SimpleMath::Vector3 Rotation{ DynamicActor->GetRotation() };
    DirectX::SimpleMath::Vector3 Scale{ DynamicActor->GetScale() };

    mTransform.SetPosition(glm::vec3{ Position.x, Position.y, Position.z });
    mTransform.SetRotation(glm::vec3{ Rotation.x, Rotation.y, Rotation.z });
    mTransform.SetScale(glm::vec3{ Scale.x, Scale.y, Scale.z });
}
